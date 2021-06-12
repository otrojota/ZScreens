#ifndef ZHLine_h
#define ZHLine_h

#include "ZScreens.h"

class ZHLine: public ZControl {
    private:
        int width;
        enum ZControlType style;
    public:
        ZHLine(const char *_id, JsonObject def);
        virtual void draw();
};

//// ******* Implementation in header. Only when #included

ZHLine::ZHLine(const char *_id, JsonObject def): ZControl(_id, def) {
    style = def.containsKey("style")?getControlTypeFromStyleName((char *)((const char *)def["style"])):Z_PRIMARY;
    width = def.containsKey("width")?def["width"]:1;
}
void ZHLine::draw() {
    clear();
    if (!isVisible()) {
      ZControl::draw();
      return;  
    }
    struct ZColorScheme colors = getColorSchemeForType(style);
    int y = zy();
    for (int i=0; i<width; i++) {
        app->tft->drawFastHLine(zx(), y+i, zw(), colors.background);
    }
    ZControl::draw();
}


ZControl *ZHLineFactory(const char *id, JsonObject def) {
    return new ZHLine(id, def);
}
static_block {
    ZControl::registerFactory("ZHLine", ZHLineFactory);
}


#endif
