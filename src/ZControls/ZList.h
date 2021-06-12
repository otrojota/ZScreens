#ifndef ZList_h
#define ZList_h

#include "ZScreens.h"
#include "../lib/ZLinkedList.h"

struct ZListRow: ZLLData {
  char *leftIcon;
  char *rightIcon;
  char *id;
  char *label;
  ZListRow(const char *_id, const char *_label, const char *_leftIcon, const char *_rightIcon) {
    id = (char *)malloc(1 + strlen(_id));
    strcpy(id, _id);
    label = (char *)malloc(1 + strlen(_label));
    strcpy(label, _label);    
    if (_leftIcon) {
      leftIcon = (char *)malloc(1 + strlen(_leftIcon));
      strcpy(leftIcon, _leftIcon);
    } else {
      leftIcon = NULL;
    }
    if (_rightIcon) {
      rightIcon = (char *)malloc(1 + strlen(_rightIcon));
      strcpy(rightIcon, _rightIcon);
    } else {
      rightIcon = NULL;
    }
  }
  ~ZListRow() {
    free(id); free(label);
    if (leftIcon) free(leftIcon);
    if (rightIcon) free(rightIcon);
  }
};


class ZList: public ZControl {
  private:
    enum ZControlType normalStyle, selectedStyle;
    ZFontUsage font;
    bool useLeftIcon;
    bool useRightIcon;
    ZLinkedList *rows;
    int lineHeight;
    int selectedIndex;
    int pressedIndex;
    int yScroll, maxYScroll, yScroll0;
    unsigned long lastOkMillis;
    void ensureVisible(int idx);
    ZScrollDirection selectedScrollControl;    
    void paintScrollControls();
  public:
    ZList(const char *_id, JsonObject def);
    ~ZList();
    virtual void clear();
    virtual void draw();
    virtual void touchDown(int x, int y);
    virtual void touchUp(int x, int y);
    virtual void touchStartMove(int x, int y);
    void touchMove(int x0, int y0, int x, int y);
    int getSelectedIndex();
    int setSelectedIndex(int idx);
    char *getSelectedId();
    void setSelectedId(char *id);
    char *getSelectedLabel();
    ZListRow *getSelectedRow();
    void addRow(const char *id, const char *label);
    void addRow(const char *id, const char *label, const char *leftIcon, const char *rightIcon);
    void removeRow(int idx);
    int getNRows();
    ZListRow *getRow(int idx);
    virtual void tabInfo(bool left, bool ok, bool right);
};

//// ******* Implementation in header. Only when #included

#include "ZIcons/16x16/arrow_up_16x16.h"
#include "ZIcons/16x16/arrow_down_16x16.h"

ZList::ZList(const char *_id, JsonObject def): ZControl(_id, def) {
    ZControl::parseFontUsage(def, "font", font);
    if (def.containsKey("useLeftIcon")) useLeftIcon = def["useLeftIcon"];
    else useLeftIcon = false;
    if (def.containsKey("useRightIcon")) useRightIcon = def["useRightIcon"];
    else useRightIcon = false;
    normalStyle = def.containsKey("normalStyle")?getControlTypeFromStyleName((char *)((const char *)def["normalStyle"])):Z_LIGHT;
    if (normalStyle == Z_ERROR) {
        Serial.print("Style '"); Serial.print((char *)((const char *)def["normalStyle"])); Serial.println(" Not recognized");
        while(1) sleep(100);
    }
    selectedStyle = def.containsKey("selectedStyle")?getControlTypeFromStyleName((char *)((const char *)def["selectedStyle"])):Z_PRIMARY;
    if (selectedStyle == Z_ERROR) {
        Serial.print("Style '"); Serial.print((char *)((const char *)def["selectedStyle"])); Serial.println(" Not recognized");
        while(1) sleep(100);
    }
    if (def.containsKey("lineHeight")) lineHeight = def["lineHeight"];
    else lineHeight = 0;
    rows = new ZLinkedList();
    if (def.containsKey("rows")) {
        JsonArray jRows = def["rows"];
        for (JsonObject jRow:jRows) {
            ZListRow *row = new ZListRow(
                jRow["id"], jRow["label"], 
                jRow.containsKey("leftIcon")?jRow["leftIcon"]:(const char *)NULL, 
                jRow.containsKey("rightIcon")?jRow["rightIcon"]:(const char *)NULL
            );
            rows->add(row);
        }
    }

    selectedIndex = -1;
    yScroll = 0;
    pressedIndex =-1;
    selectedScrollControl = Z_SCROLL_NONE;
    lastOkMillis = -1;
}
void ZList::clear() {
    ZTheme theme = getTheme();
    struct ZColorScheme normalColors = getColorSchemeForType(normalStyle);    
    app->tft->fillRoundRect(zx(), zy(), zw(), zh(), theme.mainRadius, normalColors.background);
}
void ZList::draw() {
    clear();
    if (!isVisible()) {
        ZControl::draw();
        return;  
    }
    ZTheme theme = getTheme();
    int vpX = app->tft->getViewportX();
    int vpY = app->tft->getViewportY();
    int vpW = app->tft->getViewportWidth();
    int vpH = app->tft->getViewportHeight();
    int margin = theme.margin;
    app->tft->setViewport(zx() + margin, zy() + margin, zw() - 2*margin - 18, zh() - 2*margin);
    struct ZColorScheme normalColors = getColorSchemeForType(normalStyle);    
    struct ZColorScheme selectedColors = getColorSchemeForType(selectedStyle);    

    int lh = lineHeight?lineHeight:theme.lineHeight;
    int n = rows->length();    
    int i0 = yScroll / lh;
    int i1 = i0 + (zh() / lh) + 1;
    if (i1 >= n) i1 = n - 1;
    int yy = i0 * lh - yScroll;
    ZFontUsage fu = font.isValid?font:theme.fontNormal;
    app->useFont(fu);    
    int x0 = margin + (useLeftIcon?lh + margin:0);  
    int x1 = x0 + zw() - 2 * margin - 18 - (useRightIcon?lh + margin:0);  
    for (int i=i0; i<=i1; i++) {
        ZListRow *row = (ZListRow *)rows->get(i);        
        if (i > i0) {
            app->tft->drawLine(margin, yy, zw() - 2*margin - 18, yy, normalColors.foreground);
        }
        int fg, bg;
        fg = i == selectedIndex?selectedColors.foreground:normalColors.foreground;
        bg = i == selectedIndex?selectedColors.background:normalColors.background;
        if (i == selectedIndex) {
            app->tft->fillRect(0, yy, zw() - 2*margin - 18, lh - 1, selectedColors.background);
        }
        app->tft->setTextColor(fg, bg);
        app->tft->setTextSize(fu.textSize);
        app->tft->setTextWrap(false);
        app->tft->setTextDatum(ML_DATUM);        
        int y0 = (int)(yy + lh / 2.0) + theme.yCenterDelta;
        app->tft->drawString(row->label, x0, y0);
        if (useLeftIcon && row->leftIcon) {
            ZIconInfo icon = ZApp::getIcon(row->leftIcon);
            int ix = margin + lh / 2 - icon.width / 2;
            int iy = yy + lh / 2 - icon.height / 2;
            app->tft->drawXBitmap(ix, iy, (const unsigned char*)icon.data, icon.width, icon.height, fg, bg);
        }
        if (useRightIcon && row->rightIcon) {
            ZIconInfo icon = ZApp::getIcon(row->rightIcon);
            int ix = zw() - 3*margin - 18 - lh / 2 - icon.width / 2;
            int iy = yy + lh / 2 - icon.height / 2;
            app->tft->drawXBitmap(ix, iy, (const unsigned char*)icon.data, icon.width, icon.height, fg, bg);
        }

        if (i == pressedIndex) {
            app->tft->drawRoundRect(x0 - 2, yy + 2, x1 - x0 - 4, lh - 4, 4, normalColors.foreground);
        }

        yy += lh;
    }

    app->tft->setViewport(vpX, vpY, vpW, vpH);
    paintScrollControls();
    app->tft->drawRoundRect(zx(),zy(), zw(), zh(), theme.mainRadius, hasFocus()?selectedColors.background:normalColors.foreground);
    app->tft->drawLine(zx() + zw() - 18, zy() + 1, zx() + zw() - 18, zy() + zh() - 2, normalColors.foreground);
    ZControl::draw();
}

void ZList::paintScrollControls() {
    if (!rows->length()) return;
    struct ZColorScheme primaryColors = getColorSchemeForType(Z_PRIMARY);
    if (hasFocus()) {        
        if (selectedScrollControl == Z_SCROLL_NONE) selectedScrollControl = Z_SCROLL_UP;
        ZTheme theme = getTheme();
        int margin = theme.margin;
        ZIconInfo iconUp = ZApp::getIcon("arrow_up_16x16");
        int x = zx() + zw() - iconUp.width - 1;
        int y = zy() + 1;
        int fg, bg;
        if (selectedScrollControl == Z_SCROLL_UP) {
            app->tft->drawXBitmap(x, y, (const unsigned char*)iconUp.data, iconUp.width, iconUp.height, theme.mainSelected, theme.mainFocus);
        } else {
            app->tft->drawXBitmap(x, y, (const unsigned char*)iconUp.data, iconUp.width, iconUp.height, primaryColors.background);
        }
        ZIconInfo iconDown = ZApp::getIcon("arrow_down_16x16");
        y = zy() + zh() - iconDown.height - 1;
        if (selectedScrollControl == Z_SCROLL_DOWN) {
            app->tft->drawXBitmap(x, y, (const unsigned char*)iconDown.data, iconDown.width, iconDown.height, theme.mainSelected, bg = theme.mainFocus);
        } else {
            app->tft->drawXBitmap(x, y, (const unsigned char*)iconDown.data, iconDown.width, iconDown.height, primaryColors.background);
        }
    }
    int y0 = zy() + 18, y1 = zy() + zh() - 18;
    if (selectedIndex >= 0) {
        float d = ((float)y1 - (float)y0) / (float)rows->length();
        int yBar = y0 + (int)(d/2.0 + selectedIndex * d);
        app->tft->fillRect(zx() + zw() - 17, yBar - 1, 16, 3, primaryColors.background);
    }
}

void ZList::touchDown(int x, int y) {
    // Calculate pressedIndex
    int lh = lineHeight?lineHeight:getTheme().lineHeight;
    int localY = y - zAbsY();
    int rowIndex = (localY + yScroll) / lh;
    if (rowIndex >= 0 && rowIndex < rows->length()) {
        pressedIndex = rowIndex;
    } else {
        pressedIndex = -1;
    }
    invalidate();    
}
void ZList::touchUp(int x, int y) {    
    if (pressedIndex < 0) return;
    selectedIndex = pressedIndex;
    ZEventData eventData = {};
    eventData.selectedIndex = selectedIndex;
    eventData.selectedId = ((ZListRow *)rows->get(selectedIndex))->id;
    eventData.selectedLabel = ((ZListRow *)rows->get(selectedIndex))->label;
    triggerEvent("change", eventData);
    pressedIndex = -1;
    invalidate();    
}
void ZList::touchStartMove(int x, int y) {
    int lh = lineHeight?lineHeight:getTheme().lineHeight;
    pressedIndex = -1;
    maxYScroll = rows->length() * lh - zh() + 2 * getTheme().margin;
    if (maxYScroll < 0) maxYScroll = 0;    
    yScroll0 = yScroll;
    invalidate();
}
void ZList::touchMove(int x0, int y0, int x, int y) {
    int oldYScroll = yScroll;
    yScroll = y0 - y + yScroll0;
    if (yScroll > maxYScroll) yScroll = maxYScroll;
    if (yScroll < 0) yScroll = 0;
    if (oldYScroll != yScroll) invalidate();
}
void ZList::ensureVisible(int idx) {
    int lh = lineHeight?lineHeight:getTheme().lineHeight;
    if (idx < 0) return;
    int margin = getTheme().margin;
    int firstVisible = yScroll / lh;
    if (firstVisible < 0) firstVisible = 0;
    int lastVisible = firstVisible + (int)((zh() - 2 * margin) / lh);
    if (lastVisible > (rows->length() - 1)) lastVisible = rows->length() - 1;
    if (idx < firstVisible) yScroll = lh * idx;
    if (idx > lastVisible) yScroll = lh * idx - zh() + 2*margin + lh;
    if (yScroll < 0) yScroll = 0;
    int maxYScroll = rows->length() * lh - zh() + 2 * margin;
    if (maxYScroll < 0) maxYScroll = 0;
    if (yScroll > maxYScroll) yScroll = maxYScroll;
}
int ZList::getSelectedIndex() {return selectedIndex;}
int ZList::setSelectedIndex(int idx) {
    if (idx < rows->length()) selectedIndex = idx;
    else selectedIndex = -1;
    ensureVisible(selectedIndex);
    invalidate();
}
char *ZList::getSelectedId() {
    if (selectedIndex >= 0 && selectedIndex < rows->length()) return ((ZListRow *)rows->get(selectedIndex))->id;
    return NULL;
}
void ZList::setSelectedId(char *id) {
    int idx = -1, n = rows->length();
    for (int i=0; i<n; i++) {
        ZListRow *d = (ZListRow *)rows->get(i);
        if (!strcmp(d->id, id)) {
            idx = i; 
            break;
        }
    }
    selectedIndex = idx;    
    ensureVisible(selectedIndex);
    invalidate();
}
char *ZList::getSelectedLabel() {
    if (selectedIndex >= 0 && selectedIndex < rows->length()) return ((ZListRow *)rows->get(selectedIndex))->id;
    return NULL;
}
ZListRow *ZList::getSelectedRow() {
    if (selectedIndex >= 0 && selectedIndex < rows->length()) return ((ZListRow *)rows->get(selectedIndex));
    return NULL;
}
void ZList::addRow(const char *id, const char *label) {
    addRow(id, label, NULL, NULL);
}
void ZList::addRow(const char *id, const char *label, const char *leftIcon, const char *rightIcon) {
    ZListRow *d = new ZListRow(id, label, leftIcon, rightIcon);
    rows->add(d);
    invalidate();
}
void ZList::removeRow(int idx) {
    rows->remove(idx);
    if (selectedIndex >= rows->length()) selectedIndex--;
    ensureVisible(selectedIndex);
    ZEventData eventData = {};
    eventData.selectedIndex = selectedIndex;
    eventData.selectedId = selectedIndex >= 0?((ZListRow *)rows->get(selectedIndex))->id:NULL;
    eventData.selectedLabel = selectedIndex >= 0?((ZListRow *)rows->get(selectedIndex))->label:NULL;
    triggerEvent("change", eventData);
    invalidate();
}
int ZList::getNRows() {
    return rows->length();
}
ZListRow *ZList::getRow(int idx) {
    if (idx >= 0 && idx < rows->length()) return ((ZListRow *)rows->get(idx));
    return NULL;
}
void ZList::tabInfo(bool left, bool ok, bool right) {    
    if (!rows->length()) {
        ZControl::tabInfo(left, ok, right);
        return;
    }
    if (left && selectedScrollControl == Z_SCROLL_DOWN) {
        selectedScrollControl = Z_SCROLL_UP;
        invalidate();
        return;
    }
    if (right && selectedScrollControl == Z_SCROLL_UP) {
        selectedScrollControl = Z_SCROLL_DOWN;
        invalidate();
        return;
    }
    if (ok && (millis() - lastOkMillis) > 300) {
        lastOkMillis = millis();
        if (selectedScrollControl == Z_SCROLL_UP) {
            if (--selectedIndex < 0) selectedIndex = rows->length() - 1;
        } else if (selectedScrollControl == Z_SCROLL_DOWN) {
            if (++selectedIndex >= rows->length()) selectedIndex = 0;
        }
        ensureVisible(selectedIndex);
        ZEventData eventData = {};
        eventData.selectedIndex = selectedIndex;
        eventData.selectedId = ((ZListRow *)rows->get(selectedIndex))->id;
        eventData.selectedLabel = ((ZListRow *)rows->get(selectedIndex))->label;        
        triggerEvent("change", eventData);
        invalidate();
        return;
    }
    if (!ok) lastOkMillis = -1;
    ZControl::tabInfo(left, ok, right);
}

ZControl *ZListFactory(const char *id, JsonObject def) {
    return new ZList(id, def);
}
static_block {
    ZControl::registerFactory("ZList", ZListFactory);
}


#endif
