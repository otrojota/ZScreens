#ifndef ZIcon_h
#define ZIcon_h

#include "ZScreens.h"

class ZIcon: public ZControl {
  private:
    bool pressed, invert;
    enum ZControlType style;
    ZIconInfo icon;
  public:
    ZIcon(const char *_id, JsonObject def);
    virtual void draw();
    bool getInvert();
    void setInvert(bool inv);
    virtual void touchDown(int x, int y);
    virtual void touchUp(int x, int y);
    virtual void touchEndMove(int x0, int y0, int x, int y);
};

//// ******* Implementation in header. Only when #included

ZIcon::ZIcon(const char *_id, JsonObject def): ZControl(_id, def) {
  if (def.containsKey("icon")) {
    icon = ZApp::getIcon(def["icon"]);
  } else {
    Serial.print("No icon (xbm) in ZIcon declaration for '"); Serial.print(id); Serial.println("'");
    while(1) sleep(100);
  }
  style = def.containsKey("style")?getControlTypeFromStyleName((char *)((const char *)def["style"])):Z_PRIMARY;
  if (style == Z_ERROR) {
    Serial.print("ZIcon Style '"); Serial.print((char *)((const char *)def["style"])); Serial.println(" Not recognized");
    while(1) sleep(100);
  }
  pressed = false;
  if (def["invert"]) invert = def["invert"];
  else invert = false;
}
void ZIcon::draw() {
  clear();
  if (!isVisible()) {
    ZControl::draw();
    return;  
  }
  struct ZColorScheme colors = getColorSchemeForType(style);
  int fg = colors.foreground, bg = colors.background;
  if (invert) {
    int swap = fg;
    fg = bg;
    bg = swap;
  }
  if (hasFocus()) {
    ZTheme theme = getTheme();
    fg = theme.mainSelected;
    bg = theme.mainFocus;
  }
  if (icon.type == Z_XBM) {
    app->tft->drawXBitmap(zx(), zy(), (const unsigned char*)icon.data, icon.width, icon.height, fg, bg);
  }

  if (pressed) {
    app->tft->drawRoundRect(zx() + 2, zy() + 2, zw() - 4, zh() - 4, 4, colors.foreground);
  }
  
  ZControl::draw();
}

bool ZIcon::getInvert() {return invert;}
void ZIcon::setInvert(bool inv) {
  invert = inv;
  invalidate();
}


void ZIcon::touchDown(int x, int y) {
  pressed = true;
  invalidate();
}
void ZIcon::touchUp(int x, int y) {
  ZEventData eventData = {};
  triggerEvent("click", eventData);
  pressed = false;
  invalidate();
}
void ZIcon::touchEndMove(int x0, int y0, int x, int y) {
  ZEventData eventData = {};
  triggerEvent("click", eventData);
  pressed = false;
  invalidate();
}

ZControl *ZIconFactory(const char *id, JsonObject def) {
  return new ZIcon(id, def);
}
static_block {
  ZControl::registerFactory("ZIcon", ZIconFactory);
}

#endif
