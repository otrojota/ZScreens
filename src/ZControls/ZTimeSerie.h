#ifndef ZTimeSerie_h
#define ZTimeSerie_h

#include "ZScreens.h"
#include <RTClib.h>
#include "../ZAAFonts/noto_sans_bold_15.h"
#include "../ZAAFonts/helvetica_neue_16.h"

struct TimeSeriaData: ZLLData{
    unsigned long time;
    float value;
};

class ZTimeSerie: public ZControl {
    private:
        ZLinkedList *rows;
        float min, max;
        unsigned long fixedT0, fixedT1;
        int decimalPlaces;
        String title, subtitle;
    public:
        ZTimeSerie(const char *_id, JsonObject def);
        virtual void draw();
        void clearRows();
        void addRow(unsigned long time, float value);
        void setFixedTime(unsigned long start, unsigned long end);
        void setDecimalPlaces(int dp);
        String fmtTime(unsigned long time);
};

//// ******* Implementation in header. Only when #included

ZTimeSerie::ZTimeSerie(const char *_id, JsonObject def): ZControl(_id, def) {    
    rows = new ZLinkedList();
    fixedT0 = 0;
    fixedT1 = 0;
    decimalPlaces = 2;
    if (def.containsKey("decimalPlaces")) decimalPlaces = def["decimalPlaces"];
    title = "";
    if (def.containsKey("title")) title = def["title"].as<String>();
    subtitle = "";
    if (def.containsKey("subtitle")) subtitle = def["subtitle"].as<String>();
}

void ZTimeSerie::draw() {
    clear();
    if (!isVisible()) {
      ZControl::draw();
      return;  
    }
    ZTheme theme = getTheme();    
    unsigned long t0 = fixedT0, t1 = fixedT1;
    if (!t0 || !t1) {
        if (!rows->length()) {
            ZControl::draw();
            return;
        }
        t0 = ((TimeSeriaData *)rows->get(0))->time;
        t1 = ((TimeSeriaData *)rows->get(rows->length() - 1))->time;
        if (t0 == t1) {
            ZControl::draw();
            return;
        }
    }
    ZFontUsage font;
    strcpy(font.name, "noto_sans_bold_15");
    font.isValid = true;
    app->useFont(font);

    app->tft->setTextColor(theme.darkBackground);
    app->tft->setTextSize(1);
    app->tft->setTextWrap(false);
    app->tft->setTextDatum(BL_DATUM);
    app->tft->drawString(fmtTime(t0), zx(), zy() + zh());
    app->tft->setTextDatum(BR_DATUM);
    app->tft->drawString(fmtTime(t1), zx() + zw(), zy() + zh());

    app->tft->drawRect(zx(), zy(), zw(), zh() - 20, theme.darkBackground);  

    if (!rows->length()) {
        Serial.println("No rows");
        app->unuseFont(font);
        ZControl::draw();
        return;
    }
    Serial.print("Si hay rows:"); Serial.println(rows->length());

    float v0 = min, v1 = max;
    if (v0 == v1) {
        v0 -= 1.0; v1 += 1.0;
    }
    int y1 = zy() + zh() - 20 - 10;
    int x0 = zx(), x1 = zx() + zw();
    int _x, _y, x, y;
    for (int i=0; i<rows->length(); i++) {
        TimeSeriaData *row = ((TimeSeriaData *)rows->get(i));
        x = x0 + (int)(zw() * (row->time - t0) / (t1 - t0));
        y = y1 - (int)((zh() - 20 - 10) * (row->value - min) / (max - min));
        if (i > 0) app->tft->drawLine(_x, _y, x, y, theme.primaryBackground);
        _x = x; _y = y;
    }
    app->tft->setTextDatum(MR_DATUM);
    app->tft->drawLine(zx() + zw() - 5, zy() + 10, zx() + zw(), zy() + 10, theme.primaryBackground);
    app->tft->drawString(String(max, decimalPlaces), zx() + zw() - 10, zy() + 10);
    app->tft->drawLine(zx() + zw() - 5, zy() + zh() - 30, zx() + zw(), zy() + zh() - 30, theme.primaryBackground);
    app->tft->drawString(String(min, decimalPlaces), zx() + zw() - 10, zy() + zh() - 30);
    app->unuseFont(font);

    y=5;
    if (title.length()) {
        strcpy(font.name, "helvetica_neue_16");
        app->useFont(font);
        app->tft->setTextColor(theme.darkBackground);
        app->tft->setTextSize(1);
        app->tft->setTextWrap(false);
        app->tft->setTextDatum(TL_DATUM);
        app->tft->drawString(title, zx() + 5, zy() + y);
        app->unuseFont(font);
        y += 20;
    }
    if (subtitle.length()) {
        strcpy(font.name, "noto_sans_bold_15");
        app->useFont(font);
        app->tft->setTextColor(theme.darkBackground);
        app->tft->setTextSize(1);
        app->tft->setTextWrap(false);
        app->tft->setTextDatum(TL_DATUM);
        app->tft->drawString(subtitle, zx() + 5, zy() + y);
        app->unuseFont(font);
    }

    ZControl::draw();
}

void ZTimeSerie::clearRows() {
    if (rows) delete rows;
    rows = new ZLinkedList();
}

void ZTimeSerie::addRow(unsigned long time, float value) {
    bool first = (rows->length() == 0);
    if (first || value < min) min = value;
    if (first || value > max) max = value;
    TimeSeriaData *d = new TimeSeriaData();
    d->time = time;
    d->value = value;
    rows->add(d);
    invalidate();
}

void ZTimeSerie::setFixedTime(unsigned long start, unsigned long end) {
    fixedT0 = start;
    fixedT1 = end;
    invalidate();
}

void ZTimeSerie::setDecimalPlaces(int dp) {
    decimalPlaces = dp;
    invalidate();
}

String months[] = {"jan", "feb", "mar", "apr", "may", "jun", "jul", "ago", "sep", "oct", "nov", "dec"};
String ZTimeSerie::fmtTime(unsigned long time) {
    DateTime d(time);
    return months[d.month() - 1] + "-" + (d.day() < 10?"0":"") + (String)d.day() + " "
         + (d.hour() < 10?"0":"") + (String)d.hour() + ":" + (d.minute() < 10?"0":"") + (String)d.minute();
}

// **** Registration

ZControl *ZTimeSerieFactory(const char *id, JsonObject def) {
    return new ZTimeSerie(id, def);
}
static_block {
    ZControl::registerFactory("ZTimeSerie", ZTimeSerieFactory);
}


#endif
