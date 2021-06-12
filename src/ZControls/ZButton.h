#ifndef ZButton_h
#define ZButton_h

#include "ZScreens.h"

class ZButton: public ZControl {
  private:
    bool pressed;
    bool outline;
    ZFontUsage font;
    bool hasIcon;
    ZIconInfo icon;
    ZIconLocation iconLocation;
  public:
    String label;
    enum ZControlType buttonType;
    ZButton(const char *_id, JsonObject def);
    virtual void draw();
    virtual void touchDown(int x, int y);
    virtual void touchUp(int x, int y);
    virtual void touchEndMove(int x0, int y0, int x, int y);
    virtual void tabInfo(bool left, bool ok, bool right);
    bool getOutline();
    void setOutline(bool ol);
    String getLabel();
    void setLabel(String lbl);
};

//// ******* Implementation in header. Only when #included

ZButton::ZButton(const char *_id, JsonObject def): ZControl(_id, def) {
  if (def.containsKey("label")) {
    label = String((const char *)def["label"]);
  } else {
    label = "";
  }
  ZControl::parseFontUsage(def, "font", font);
  buttonType = def.containsKey("style")?getControlTypeFromStyleName((char *)((const char *)def["style"])):Z_PRIMARY;
  if (buttonType == Z_ERROR) {
    Serial.print("Button Style '"); Serial.print((char *)((const char *)def["style"])); Serial.println(" Not recognized");
    while(1) sleep(100);
  }
  if (def.containsKey("iconRight")) {
    hasIcon = true;
    icon = ZApp::getIcon(def["iconRight"]);
    iconLocation = Z_ICON_RIGHT;
  } else if (def.containsKey("iconLeft")) {
    hasIcon = true;
    icon = ZApp::getIcon(def["iconLeft"]);
    iconLocation = Z_ICON_LEFT;
  } else if (def.containsKey("iconTop")) {
    hasIcon = true;
    icon = ZApp::getIcon(def["iconTop"]);
    iconLocation = Z_ICON_TOP;
  } else if (def.containsKey("iconBottom")) {
    hasIcon = true;
    icon = ZApp::getIcon(def["iconBottom"]);
    iconLocation = Z_ICON_BOTTOM;
  } else {
    hasIcon = false; 
  }
  pressed = false;
  outline = false;
  if (def.containsKey("outline")) outline = def["outline"];
}
void ZButton::draw() {
  clear();
  if (!isVisible()) {
    ZControl::draw();
    return;  
  }
  ZTheme theme = getTheme();
  struct ZColorScheme colors = getColorSchemeForType(buttonType);
  if (!outline) {
    app->tft->fillRoundRect(zx(), zy(), zw(), zh(), theme.mainRadius, colors.background);  
  }
  app->tft->drawRoundRect(zx(), zy(), zw(), zh(), theme.mainRadius, colors.border);
  if (hasFocus()) {
    app->tft->fillRoundRect(zx() + 3, zy() + 3, zw() - 6, zh() - 6, 4, theme.mainFocus);
  }

  ZFontUsage fu = font.isValid?font:theme.fontNormal;
  int fg, bg;
  app->useFont(fu);
  if (hasFocus()) {
    fg = theme.mainSelected;
    bg = theme.mainFocus;    
  } else {
    fg = outline?colors.border:colors.foreground;
    bg = outline?theme.mainBackground:colors.background;
  }
  app->tft->setTextColor(fg, bg);
  app->tft->setTextSize(fu.textSize);
  app->tft->setTextWrap(false);
  int x0, y0, ix, iy;
  if (!hasIcon) {
    x0 = (int)(zx() + zw() / 2.0);
    y0 = (int)((float)zy() + (float)zh() / 2.0) + theme.yCenterDelta;
    app->tft->setTextDatum(MC_DATUM);
  } else if (iconLocation == Z_ICON_LEFT) {
    ix = (int)(zx() + theme.margin);
    iy = (int)((float)zy() + (float)zh() / 2.0 - icon.height / 2.0);
    x0 = (int)(zx() + zw() - theme.margin);
    y0 = (int)((float)zy() + (float)zh() / 2.0) + theme.yCenterDelta;
    app->tft->setTextDatum(MR_DATUM);
  } else if (iconLocation == Z_ICON_RIGHT) {
    ix = (int)(zx() + zw() - theme.margin - icon.width);
    iy = (int)((float)zy() + (float)zh() / 2.0 - icon.height / 2.0);
    x0 = (int)(zx() + theme.margin);
    y0 = (int)((float)zy() + (float)zh() / 2.0) + theme.yCenterDelta;
    app->tft->setTextDatum(ML_DATUM);
  } else if (iconLocation == Z_ICON_TOP) {
    ix = (int)(zx() + zw() / 2.0 - icon.width / 2.0);
    iy = (int)((float)zy() + theme.margin);
    int th = zh() - 3 * theme.margin - icon.height; // Text area height
    x0 = (int)(zx() + zw() / 2.0);
    y0 = (int)((float)zy() + 2*theme.margin + icon.height + th / 2.0) + theme.yCenterDelta;
    app->tft->setTextDatum(MC_DATUM);
  } else if (iconLocation == Z_ICON_BOTTOM) {
    ix = (int)(zx() + zw() / 2.0 - icon.width / 2.0);
    iy = (int)((float)zy() + (float)zh() - theme.margin - icon.height);
    int th = zh() - 3 * theme.margin - icon.height; // Text area height
    x0 = (int)(zx() + zw() / 2.0);
    y0 = (int)((float)zy() + theme.margin + th / 2.0) + theme.yCenterDelta;
    app->tft->setTextDatum(MC_DATUM);
  }  
  app->tft->drawString(label, x0, y0);
  app->unuseFont(fu);
  if (hasIcon) {
    app->tft->drawXBitmap(ix, iy, (const unsigned char*)icon.data, icon.width, icon.height, fg);
  }
  if (pressed) {
    app->tft->drawRoundRect(zx() + 2, zy() + 2, zw() - 4, zh() - 4, 4, colors.foreground);
  }  
  ZControl::draw();
}

void ZButton::touchDown(int x, int y) {
  pressed = true;
  invalidate();
}
void ZButton::touchUp(int x, int y) {
  ZEventData eventData = {};
  triggerEvent("click", eventData);
  pressed = false;
  invalidate();
}
void ZButton::touchEndMove(int x0, int y0, int x, int y) {
  ZEventData eventData = {};
  triggerEvent("click", eventData);
  pressed = false;
  invalidate();
}

void ZButton::tabInfo(bool left, bool ok, bool right) {
  if (ok) {
    if (!pressed) {
      pressed = true;
      invalidate();
    }
  } else {
    if (pressed) {
      ZEventData eventData = {};
      triggerEvent("click", eventData);
      pressed = false;
      invalidate();
    }
  }
  ZControl::tabInfo(left, ok, right);
}

bool ZButton::getOutline() {return outline;}
void ZButton::setOutline(bool ol) {
  outline = ol;
  invalidate();
}

String ZButton::getLabel() {return label;}
void ZButton::setLabel(String lbl) {label = String(lbl.c_str()); invalidate();}

ZControl *ZButtonFactory(const char *id, JsonObject def) {
  return new ZButton(id, def);
}
static_block {
  ZControl::registerFactory("ZButton", ZButtonFactory);
}

#endif
