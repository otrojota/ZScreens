#ifndef ZPanel_h
#define ZPanel_h

#include "ZScreens.h"
#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>

class ZPanel: public ZCompoundControl {
  private:
    enum ZControlType panelType;
  public: 
    ZPanel(const char *_id, JsonObject def);
    virtual void clear();
    virtual void draw();
};

//// ******* Implementation in header. Only when #included

ZPanel::ZPanel(const char *_id, JsonObject def): ZCompoundControl(_id, def) {
  panelType = def.containsKey("style")?getControlTypeFromStyleName((char *)((const char *)def["style"])):Z_SECONDARY;
  if (panelType == Z_ERROR) {
    Serial.print("Panel Style '"); Serial.print((char *)((const char *)def["style"])); Serial.println(" Not recognized");
    while(1) sleep(100);
  }
}
void ZPanel::clear() {
  ZTheme theme = getTheme();
  app->tft->fillRoundRect(0, 0, zw(), zh(), theme.mainRadius, theme.mainBackground);
}
void ZPanel::draw() {  
  int vpX = app->tft->getViewportX();
  int vpY = app->tft->getViewportY();
  int vpW = app->tft->getViewportWidth();
  int vpH = app->tft->getViewportHeight();
  app->tft->setViewport(zx(), zy(), zw(), zh());
  if (!isValid()) {
    clear();
    ZTheme theme = getTheme();
    struct ZColorScheme colors = getColorSchemeForType(panelType);
    app->tft->drawRoundRect(0,0, w, h, theme.mainRadius, colors.background);
  }
  for (int i=0; controls[i]; i++) {
    if (controls[i]->isModified()) controls[i]->draw();
  }
  ZControl::draw();
  app->tft->setViewport(vpX, vpY, vpW, vpH);
  modified = false;
}

ZControl *ZPanelFactory(const char *id, JsonObject def) {
  return new ZPanel(id, def);
}
static_block {
  ZControl::registerFactory("ZPanel", ZPanelFactory);
}

#endif