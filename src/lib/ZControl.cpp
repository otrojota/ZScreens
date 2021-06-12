#include "ZControl.h"
#include "ZCompoundControl.h"
#include "ZApp.h"

ZLinkedList *ZControl::factoriesLL() {
  static ZLinkedList *ll = new ZLinkedList();
  return ll;
}

int ZControl::registerFactory(const char *type, ZControlFactory factory) {
  ZControlFactoryDeclaration *data = new ZControlFactoryDeclaration();
  data->controlType = type;
  data->factory = factory;  
  factoriesLL()->add(data);
}
void ZControl::showFactories() {
  if (!factoriesLL()->length()) {
    Serial.println("No ZControls Factories");
  } else {
    Serial.println("ZControl Factories:");
    for (int i=0; i<factoriesLL()->length(); i++) {
      ZControlFactoryDeclaration *data = (ZControlFactoryDeclaration *)factoriesLL()->get(i);
      Serial.print("  -- "); Serial.println(data->controlType);
    }
  }
}

ZControl *ZControl::create(const char *id, JsonObject def) {
  const char *type = def["type"];
  int i = 0, n = factoriesLL()->length();
  ZControlFactoryDeclaration *found = NULL;
  while (i < n) {
    ZControlFactoryDeclaration *data = (ZControlFactoryDeclaration *)factoriesLL()->get(i);
    if (!strcmp(data->controlType, type)) {
      found = data;
      break;
    }
    i++;
  }
  if (!found) {
    Serial.print("ZControl type '"); Serial.print(type); Serial.println("' Not found");
    while(1) sleep(100);
  }  
  return (found->factory)(id, def);
}

void ZControl::parseFontUsage(JsonObject def, const char *jsonAttributeName, ZFontUsage &fontUsage) {
  fontUsage.name[0] = 0;
  fontUsage.number = -1;    
  fontUsage.textSize = 1;
  fontUsage.isValid = false;
  if (!def.containsKey(jsonAttributeName)) return;
  JsonObject fDef = def[jsonAttributeName];
  if (fDef.containsKey("name")) {strcpy(fontUsage.name, (const char *)fDef["name"]); fontUsage.isValid = true;}
  else if (fDef.containsKey("number")) {fontUsage.number = fDef["number"]; fontUsage.isValid = true;}
  else {
    Serial.println("Invalid font usage. Expected name or number in object");
    while(1) sleep(100);
  }
  if (fDef.containsKey("size")) fontUsage.textSize = fDef["size"];
}

int ZControl::parseTextAlign(const char *ta) {
  if (!strcmp(ta, "TL")) return TL_DATUM;
  if (!strcmp(ta, "TC")) return TC_DATUM;
  if (!strcmp(ta, "TR")) return TR_DATUM;
  if (!strcmp(ta, "ML")) return ML_DATUM;
  if (!strcmp(ta, "CL")) return CL_DATUM;
  if (!strcmp(ta, "MC")) return MC_DATUM;
  if (!strcmp(ta, "CC")) return CC_DATUM;
  if (!strcmp(ta, "MR")) return MR_DATUM;
  if (!strcmp(ta, "CR")) return CR_DATUM;
  if (!strcmp(ta, "BL")) return BL_DATUM;
  if (!strcmp(ta, "BC")) return BC_DATUM;
  if (!strcmp(ta, "BR")) return TR_DATUM;

  Serial.println("Invalid textAlign");
  while(1) sleep(100);
}

ZControl::ZControl(const char *_id, int _x, int _y, int _w, int _h) {
  strcpy(id, _id);
  x = _x; y = _y; w = _w; h = _h;
  valid = false;
  visible = true;
  container = NULL;
  theme = NULL;
  tindex = -1;
}
ZControl::ZControl(const char *_id, JsonObject def):ZControl(_id, (int)def["pos"][0], (int)def["pos"][1], (int)def["pos"][2], (int)def["pos"][3]) {
  if (def.containsKey("theme")) theme = def["theme"];  
  if (def.containsKey("tindex")) tindex = def["tindex"];
}


void ZControl::init(ZApp *_app, ZCompoundControl *_container) {
  app = _app;
  container = _container;  
}
// Todo: Add container scrolling (xOffset, yOffset) and some other (future) transformations
int ZControl::zx() {return x;}
int ZControl::zy() {return y;}
int ZControl::zw() {return w;}
int ZControl::zh() {return h;}
int ZControl::zAbsX() {return container?container->zAbsX() + zx():zx();}
int ZControl::zAbsY() {return container?container->zAbsY() + zy():zy();}
void ZControl::draw() {
  valid = true;
}
bool ZControl::isValid() {return valid;}
bool ZControl::isFinal() {return true;}
bool ZControl::isModified() {return !isValid();}
void ZControl::invalidate() {
  valid = false;
  if (container) container->setModified();
}
ZScreen *ZControl::getScreen() {
  ZControl *c = this;
  while(c->container) c = c->container;
  return (ZScreen *)c;
}
ZTheme ZControl::getTheme() {
  if (theme) return ZApp::getTheme(theme);
  if (container) return container->getTheme();
  return app->theme;
}
ZControl *ZControl::getControl(char *_id) {
  if (!strcmp(_id, id)) return this;
  return NULL;
}
void ZControl::clear() {
  app->tft->fillRect(zx(), zy(), zw(), zh(), getTheme().mainBackground);
}

ZControlType ZControl::getControlTypeFromStyleName(char *styleNMame) {
  if (!strcmp("primary", styleNMame)) return Z_PRIMARY;
  if (!strcmp("secondary", styleNMame)) return Z_SECONDARY;
  if (!strcmp("success", styleNMame)) return Z_SUCCESS;
  if (!strcmp("danger", styleNMame)) return Z_DANGER;
  if (!strcmp("warning", styleNMame)) return Z_WARNING;
  if (!strcmp("info", styleNMame)) return Z_INFO;
  if (!strcmp("light", styleNMame)) return Z_LIGHT;
  if (!strcmp("dark", styleNMame)) return Z_DARK;
  return Z_ERROR;
}
ZColorScheme ZControl::getColorSchemeForType(enum ZControlType type) {
  int bk, fg, bd;
  ZTheme theme = getTheme();
  switch(type) {
    case Z_PRIMARY:
      return {.background = theme.primaryBackground, .foreground = theme.primaryForeground, .border = theme.primaryBorder};      
    case Z_SECONDARY:
      return {.background = theme.secondaryBackground, .foreground = theme.secondaryForeground, .border = theme.secondaryBorder};
      break;      
    case Z_SUCCESS:
      return {.background = theme.successBackground, .foreground = theme.successForeground, .border = theme.successBorder};
      break;      
    case Z_DANGER:
      return {.background = theme.dangerBackground, .foreground = theme.dangerForeground, .border = theme.dangerBorder};
      break;      
    case Z_WARNING:
      return {.background = theme.warningBackground, .foreground = theme.warningForeground, .border = theme.warningBorder};
      break;      
    case Z_INFO:
      return {.background = theme.infoBackground, .foreground = theme.infoForeground, .border = theme.infoBorder};
      break;      
    case Z_LIGHT:
      return {.background = theme.lightBackground, .foreground = theme.lightForeground, .border = theme.lightBorder};
      break;      
    case Z_DARK:
      return {.background = theme.darkBackground, .foreground = theme.darkForeground, .border = theme.darkBorder};
      break;      
  }
}

ZControl *ZControl::getControlAt(int x, int y) {
  if (x >= this->zAbsX() && x <= (this->zAbsX() + this->zw()) && y >= this->zAbsY() && y <= (this->zAbsY() + this->zh())) return this;
  return NULL;
}

bool ZControl::triggerEvent(const char *eventName, ZEventData eventData) {
  return container->triggerEvent(this, eventName, eventData);
}

void ZControl::touchDown(int x, int y) {}
void ZControl::touchUp(int x, int y) {}
void ZControl::touchStartMove(int x, int y) {}
void ZControl::touchMove(int x0, int y0, int x, int y) {}
void ZControl::touchEndMove(int x0, int y0, int x, int y) {}

void ZControl::show() {visible = true; invalidate();}
void ZControl::hide() {visible = false; invalidate();}
void ZControl::setVisible(bool v) {visible = v; invalidate();} 
bool ZControl::isVisible() {return visible;}
bool ZControl::isFocusable() {return tindex >0 && visible;}
bool ZControl::hasFocus() {return container && tindex > 0 && container->activeTindex == tindex;}

void ZControl::tabInfo(bool left, bool ok, bool right) {
  if (left && container) container->doTabLeft();
  if (right && container) container->doTabRight();
}