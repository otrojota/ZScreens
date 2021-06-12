#ifndef ZLabel_h
#define ZLabel_h

#include "ZScreens.h"

class ZLabel: public ZControl {
  private:
    bool pressed;
    ZFontUsage font;
    int textAlign;
  public:
    String text;
    enum ZControlType labelType;
    ZLabel(const char *_id, JsonObject def);
    virtual void draw();
    virtual void touchDown(int x, int y);
    virtual void touchUp(int x, int y);
    virtual void touchEndMove(int x0, int y0, int x, int y);
    String getText();
    void setText(String txt);
    void setText(char *txt);
    void setText(const char *txt);
};

//// ******* Implementation in header. Only when #included

ZLabel::ZLabel(const char *_id, JsonObject def): ZControl(_id, def) {
  if (def.containsKey("text")) {
    text = String((const char *)def["text"]);
  } else {
    text = "";
  }
  ZControl::parseFontUsage(def, "font", font);
  labelType = def.containsKey("style")?getControlTypeFromStyleName((char *)((const char *)def["style"])):Z_PRIMARY;
  if (labelType == Z_ERROR) {
    Serial.print("Button Style '"); Serial.print((char *)((const char *)def["style"])); Serial.println(" Not recognized");
    while(1) sleep(100);
  }
  if (def.containsKey("textAlign")) {
    textAlign = ZControl::parseTextAlign(def["textAlign"]);
  } else {
    textAlign = MC_DATUM;
  }
  pressed = false;
}
void ZLabel::draw() {
  clear();
  if (!isVisible()) {
    ZControl::draw();
    return;  
  }
  struct ZColorScheme colors = getColorSchemeForType(labelType);
  ZTheme theme = getTheme();
  ZFontUsage fu = font.isValid?font:theme.fontNormal;
  app->useFont(fu);
  app->tft->setTextColor(colors.background, theme.mainBackground);
  app->tft->setTextWrap(false);
  app->tft->setTextDatum(textAlign);
  int x0, y0;
  switch(textAlign) {
    case TL_DATUM:
    case ML_DATUM:
    case BL_DATUM:
      x0 = zx();
      break;
    case TC_DATUM:
    case MC_DATUM:
    case BC_DATUM:
      x0 = (int)(zx() + zw() / 2.0);
      break;
    case TR_DATUM:
    case MR_DATUM:
    case BR_DATUM:
      x0 = zx() + zw();
      break;
  }
  switch(textAlign) {
    case TL_DATUM:
    case TC_DATUM:
    case TR_DATUM:
      y0 = zy();
      break;
    case ML_DATUM:
    case MC_DATUM:
    case MR_DATUM:
      y0 = (int)(zy() + zh() / 2.0);
      break;
    case BL_DATUM:
    case BC_DATUM:
    case BR_DATUM:
      y0 = zy() - zh();
      break;
  }
  /*
  int x0 = (int)(zx() + zw() / 2.0);
  int y0 = (int)((float)zy() + (float)zh() / 2.0);
  */
  app->tft->drawString(text, x0, y0);
  app->unuseFont(fu);
  if (pressed) {
    app->tft->drawRoundRect(zx() + 2, zy() + 2, zw() - 4, zh() - 4, 4, colors.background);
  }
  
  ZControl::draw();
}

void ZLabel::touchDown(int x, int y) {
  pressed = true;
  invalidate();
}
void ZLabel::touchUp(int x, int y) {
  ZEventData eventData = {};
  triggerEvent("click", eventData);
  pressed = false;
  invalidate();
}
void ZLabel::touchEndMove(int x0, int y0, int x, int y) {
  ZEventData eventData = {};
  triggerEvent("click", eventData);
  pressed = false;
  invalidate();
}

String ZLabel::getText() {return text;}
void ZLabel::setText(char *txt) {
  text = String(txt);
  invalidate();
}
void ZLabel::setText(const char *txt) {
  setText((char *)txt);
}
void ZLabel::setText(String txt) {
  setText(txt.c_str());
}

ZControl *ZLabelFactory(const char *id, JsonObject def) {
  return new ZLabel(id, def);
}
static_block {
  ZControl::registerFactory("ZLabel", ZLabelFactory);
}

#endif
