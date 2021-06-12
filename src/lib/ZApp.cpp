#include "ZApp.h"
#include "ZCompoundControl.h"
#include "ZScreen.h"

extern DynamicJsonDocument zScreenDefJson;

ZLinkedList *ZApp::screenDefinitionsLL() {
    static ZLinkedList *ll = new ZLinkedList();
    return ll;
}

void ZApp::registerScreen(const char *id, const char *def) {
    ZScreenDefinition *d = new ZScreenDefinition();
    d->id = id;
    d->def = def;
    screenDefinitionsLL()->add(d);
}

ZScreen *ZApp::createScreen(const char *id) {
    int n = screenDefinitionsLL()->length();
    ZScreenDefinition *found = NULL;    
    int i = 0;
    while (i < n) {
        ZScreenDefinition *d = (ZScreenDefinition *)screenDefinitionsLL()->get(i);
        if (!strcmp(d->id, id)) {
            found = d;
            break;
        }
        i++;
    }
    if (!found) {
        Serial.print("Screen '"); Serial.print(id); Serial.println("' Not found");
        while(1) {sleep(100);}
    }
    zScreenDefJson.clear();
    auto err = deserializeJson(zScreenDefJson, found->def);
    if (err) {
        Serial.print("Error parsing json for screen '"); Serial.print(id); Serial.print("'. JsonError:");
        Serial.println(err.c_str());
        while(1) {sleep(100);}
    }
    ZControl **_controls = ZCompoundControl::parseControls(zScreenDefJson["controls"]);
    ZScreen *sc = new ZScreen(id, _controls);
    return sc;
}

ZLinkedList *ZApp::themesLL() {
    static ZLinkedList *ll = new ZLinkedList();
    return ll;
}

uint16_t ZApp::color565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}
int ZApp::extractColor(JsonArray array) {
    return color565(array[0], array[1], array[2]);
}

int ZApp::registerTheme(const char *id, char *jsonTheme) {
    ZThemeDefinition *d = new ZThemeDefinition();
    d->id = id;
    zScreenDefJson.clear();
    auto err = deserializeJson(zScreenDefJson, jsonTheme);
    if (err) {
        Serial.print("Error parsing json for them '"); Serial.print(id); Serial.print("'. JsonError:");
        Serial.println(err.c_str());
        while(1) {sleep(100);}
    }
    if (zScreenDefJson.containsKey("extends")) {
        d->theme = getTheme(zScreenDefJson["extends"]);
    } else {
        d->theme = {};                
        d->theme.mainBackground = TFT_WHITE;        
        d->theme.mainRadius = 4;
        d->theme.mainSelected = TFT_SKYBLUE;
        d->theme.mainFocus = TFT_NAVY;
        d->theme.primaryBackground = color565(49,108,244);
        d->theme.primaryForeground = TFT_WHITE;
        d->theme.primaryBorder = color565(49,108,244);
        d->theme.secondaryBackground = color565(110,117,124);
        d->theme.secondaryForeground = TFT_WHITE;
        d->theme.secondaryBorder = color565(110,117,124);
        d->theme.successBackground = color565(64,133,88);
        d->theme.successForeground = TFT_WHITE;
        d->theme.successBorder = color565(64,133,88);
        d->theme.dangerBackground = color565(203,68,75);
        d->theme.dangerForeground = TFT_WHITE;
        d->theme.dangerBorder = color565(203,68,75);
        d->theme.warningBackground = color565(246,195,68);
        d->theme.warningForeground = TFT_BLACK;
        d->theme.warningBorder = color565(246,195,68);
        d->theme.infoBackground = color565(92,199,236);
        d->theme.infoForeground = TFT_BLACK;
        d->theme.infoBorder = color565(92,199,236);
        d->theme.lightBackground = color565(230,230,230);
        d->theme.lightForeground = TFT_BLACK;
        d->theme.lightBorder = color565(230,230,230);
        d->theme.darkBackground = color565(34,37,41);
        d->theme.darkForeground = TFT_WHITE;
        d->theme.darkBorder = color565(34,37,41);                        
        d->theme.yCenterDelta = 0;
        d->theme.margin = 8;
        d->theme.lineHeight = 26;
        d->theme.fontNormal.number = 2;
        d->theme.fontNormal.isValid = true;
    }
    ZTheme t = d->theme;
    if (zScreenDefJson.containsKey("main")) {
        JsonObject s = zScreenDefJson["main"];
        if (s.containsKey("background")) t.mainBackground = extractColor(s["background"]);
        if (s.containsKey("selected")) t.mainSelected = extractColor(s["selected"]);
        if (s.containsKey("focus")) t.mainFocus = extractColor(s["focus"]);
        if (s.containsKey("radius")) t.mainRadius = s["radius"];
        if (s.containsKey("margin")) t.margin = s["margin"];
        if (s.containsKey("lineHeight")) t.lineHeight = s["lineHeight"];
    }
    if (zScreenDefJson.containsKey("yCenterDelta")) t.yCenterDelta = zScreenDefJson["yCenterDelta"];
    if (zScreenDefJson.containsKey("primary")) {
        JsonObject s = zScreenDefJson["primary"];
        if (s.containsKey("background")) t.primaryBorder = t.primaryBackground = extractColor(s["background"]);
        if (s.containsKey("foreground")) t.primaryForeground = extractColor(s["foreground"]);
        if (s.containsKey("border")) t.primaryBorder = extractColor(s["border"]);
    }
    if (zScreenDefJson.containsKey("secondary")) {
        JsonObject s = zScreenDefJson["secondary"];
        if (s.containsKey("background")) t.secondaryBorder = t.secondaryBackground = extractColor(s["background"]);
        if (s.containsKey("foreground")) t.secondaryForeground = extractColor(s["foreground"]);
        if (s.containsKey("border")) t.secondaryBorder = extractColor(s["border"]);
    }
    if (zScreenDefJson.containsKey("success")) {
        JsonObject s = zScreenDefJson["success"];
        if (s.containsKey("background")) t.successBorder = t.successBackground = extractColor(s["background"]);
        if (s.containsKey("foreground")) t.successForeground = extractColor(s["foreground"]);
        if (s.containsKey("border")) t.successBorder = extractColor(s["border"]);
    }
    if (zScreenDefJson.containsKey("danger")) {
        JsonObject s = zScreenDefJson["danger"];
        if (s.containsKey("background")) t.dangerBorder = t.dangerBackground = extractColor(s["background"]);
        if (s.containsKey("foreground")) t.dangerForeground = extractColor(s["foreground"]);
        if (s.containsKey("border")) t.dangerBorder = extractColor(s["border"]);
    }
    if (zScreenDefJson.containsKey("warning")) {
        JsonObject s = zScreenDefJson["warning"];
        if (s.containsKey("background")) t.warningBorder = t.warningBackground = extractColor(s["background"]);
        if (s.containsKey("foreground")) t.warningForeground = extractColor(s["foreground"]);
        if (s.containsKey("border")) t.warningBorder = extractColor(s["border"]);
    }
    if (zScreenDefJson.containsKey("info")) {
        JsonObject s = zScreenDefJson["info"];
        if (s.containsKey("background")) t.infoBorder = t.infoBackground = extractColor(s["background"]);
        if (s.containsKey("foreground")) t.infoForeground = extractColor(s["foreground"]);
        if (s.containsKey("border")) t.infoBorder = extractColor(s["border"]);
    }
    if (zScreenDefJson.containsKey("light")) {
        JsonObject s = zScreenDefJson["light"];
        if (s.containsKey("background")) t.lightBorder = t.lightBackground = extractColor(s["background"]);
        if (s.containsKey("foreground")) t.lightForeground = extractColor(s["foreground"]);
        if (s.containsKey("border")) t.lightBorder = extractColor(s["border"]);
    }
    if (zScreenDefJson.containsKey("dark")) {
        JsonObject s = zScreenDefJson["dark"];
        if (s.containsKey("background")) t.darkBorder = t.darkBackground = extractColor(s["background"]);
        if (s.containsKey("foreground")) t.darkForeground = extractColor(s["foreground"]);
        if (s.containsKey("border")) t.darkBorder = extractColor(s["border"]);
    }    
    if (zScreenDefJson.containsKey("fonts")) {
        JsonObject fonts = zScreenDefJson["fonts"];
        if (fonts.containsKey("normal")) {
            ZControl::parseFontUsage(fonts, "normal", t.fontNormal);            
        }
    }
    
    d->theme = t;
    themesLL()->add(d);
    return themesLL()->length();
}

ZTheme ZApp::getTheme(const char *id) {
    int n = themesLL()->length();
    for (int i=0; i<n; i++) {
        ZThemeDefinition *d = (ZThemeDefinition *)themesLL()->get(i);
        if (!strcmp(d->id, id)) return d->theme;
    }
    Serial.print("Theme '"); Serial.print(id); Serial.println("' Not found");
    while(1) {sleep(100);}
}

ZLinkedList *ZApp::fontsLL() {
    static ZLinkedList *ll = new ZLinkedList();
    return ll;
}

int ZApp::registerGFXFont(const char *name, const GFXfont *font) {
    // Ignore duplicates
    int n = fontsLL()->length();
    ZFontDeclaration *d = NULL;
    for (int i=0; i<n; i++) {
        ZFontDeclaration *dec = (ZFontDeclaration *)fontsLL()->get(i);
        if (!strcmp(dec->name, name)) {
            d = dec;
            break;
        }
    }
    if (d) return n;
    d = new ZFontDeclaration();
    d->name = name;
    d->data = (void *)font;
    d->type = Z_GFX;
    fontsLL()->add(d);
    return n+1;
}
int ZApp::registerAAFont(const char *name, const uint8_t *font) {
    // Ignore duplicates
    int n = fontsLL()->length();
    ZFontDeclaration *d = NULL;
    for (int i=0; i<n; i++) {
        ZFontDeclaration *dec = (ZFontDeclaration *)fontsLL()->get(i);
        if (!strcmp(dec->name, name)) {
            d = dec;
            break;
        }
    }
    if (d) return n;
    d = new ZFontDeclaration();
    d->name = name;
    d->data = (void *)font;
    d->type = Z_ANTIALIASED;
    
    fontsLL()->add(d);
    return n+1;
}

ZFontDeclaration ZApp::getFont(const char *name) {
    int n = fontsLL()->length();
    for (int i=0; i<n; i++) {
        ZFontDeclaration *d = (ZFontDeclaration *)fontsLL()->get(i);
        if (!strcmp(d->name, name)) return *d;
    }
    Serial.print("Font name '"); Serial.print(name); Serial.println("' not registered");
    while(1) sleep(100);    
}

ZLinkedList *ZApp::eventListenersLL() {
    static ZLinkedList *ll = new ZLinkedList();
    return ll;
}

int ZApp::registerEventHandler(const char *screenId, const char *controlId, const char *eventName, ZEventHandler handler) {
    ZEventHandlerRegistration *d = new ZEventHandlerRegistration();
    d->screenId = screenId;
    d->controlId = controlId;
    d->eventName = eventName;
    d->handler = handler;
    eventListenersLL()->add(d);
    
    return eventListenersLL()->length();
}

ZLinkedList *ZApp::iconsLL() {
    static ZLinkedList *ll = new ZLinkedList();
    return ll;
}

int ZApp::registerXBM(const char *name, const unsigned char *xbm, const int width, const int height) {
    ZIconInfo *d = new ZIconInfo();
    d->type = Z_XBM;
    d->name = name;
    d->data = xbm;
    d->width = width;
    d->height = height;
    iconsLL()->add(d);
    return iconsLL()->length();
}
ZIconInfo ZApp::getIcon(const char *name) {
    int n = iconsLL()->length();
    for (int i=0; i<n; i++) {
        ZIconInfo *d = (ZIconInfo *)iconsLL()->get(i);
        if (!strcmp(d->name, name)) return *d;
    }
    Serial.print("Icon '"); Serial.print(name); Serial.println("' Not found");
    while(1) {sleep(100);}
}

ZApp::ZApp(ZScreen *_home, char *themeId) {
    theme = ZApp::getTheme(themeId);
    touchX0 = -1; 
    touchY0 = -1;
    lastTouchX = -1; 
    lastTouchY = -1;
    deltaMoveSensitivity = 15; // pixels
    deltaNoTouchForUpMillis = 200;
    touchStatus = Z_UP;
    tabLeftStatus = false;
    tabRightStatus = false;
    screens = new ZLinkedList();
    ZScreenInstance *d = new ZScreenInstance();
    d->screen = _home;    
    screens->add(d);
}

ZApp::ZApp(char *homeScreenId, char *themeId): ZApp(ZApp::createScreen(homeScreenId), themeId) {    
}

void ZApp::init(TFT_eSPI *_tft) {
    tft = _tft;
    ZScreen *current = currentScreen();
    current->init(this, NULL);
    ZEventData eventData = {};
    current->triggerEvent(current, "init", eventData);
}

ZScreen *ZApp::currentScreen() {
    ZScreenInstance *d = (ZScreenInstance *)screens->get(screens->length() - 1);
    return d->screen;
}

void ZApp::useFont(ZFontUsage f) {
    if (f.number < 0) {
        ZFontDeclaration d = ZApp::getFont(f.name);
        if (d.type == Z_ANTIALIASED) {
            tft->loadFont((const uint8_t *)d.data);
        } else {
            tft->setFreeFont((GFXfont *)d.data);
        }
    }    else {
        tft->setTextFont(f.number);
    }
}
void ZApp::unuseFont(ZFontUsage f) {
    if (f.number < 0) {
        ZFontDeclaration d = ZApp::getFont(f.name);
        if (d.type == Z_ANTIALIASED) {
            tft->unloadFont();
        }
    }
}

bool ZApp::triggerEvent(ZScreen *screen, ZControl *control, const char *eventName, ZEventData eventData) {
    int n = eventListenersLL()->length();
    for (int i=0; i<n; i++) {
        ZEventHandlerRegistration *d = (ZEventHandlerRegistration *)eventListenersLL()->get(i);
        if (!strcmp(screen->id, d->screenId) && !strcmp(d->controlId, control->id) && !strcmp(eventName, d->eventName)) {
            (d->handler)(screen, control, eventData);
            return true;
        }
    }
}

void ZApp::loop() {
        ZScreen *current = currentScreen();
        if (current->isModified()) current->draw();
}

void ZApp::touchInfo(int touchX, int touchY) {
    if (touchX >= 0 && touchY >= 0) {
        delay(10);
        lastTouchMillis = millis();
        if (touchStatus == Z_UP) {
            touchStatus = Z_DOWN;
            touchX0= touchX; touchY0 = touchY;
            processEvent(Z_TOUCH_DOWN);            
        } else if (touchStatus == Z_DOWN) {
            if (abs(touchX - touchX0) > deltaMoveSensitivity || abs(touchY - touchY0) > deltaMoveSensitivity) {
                touchStatus = Z_MOVE;
                lastTouchX= touchX; lastTouchY = touchY;
                processEvent(Z_TOUCH_START_MOVE);
            }
        } else if (touchStatus == Z_MOVE) {
            if (touchX != lastTouchX || touchY != lastTouchY) {
                lastTouchX= touchX; lastTouchY = touchY;
                processEvent(Z_TOUCH_MOVE);
            }
        }
    } else {     
        // Ignore if less than delta millis from lastTouch (touching is not continuous, there can be gaps)
        if ((millis() - lastTouchMillis) > deltaNoTouchForUpMillis) {
            if (touchStatus == Z_DOWN) {
                touchStatus = Z_UP;
                processEvent(Z_TOUCH_UP);
            } else if (touchStatus == Z_MOVE) {
                touchStatus = Z_UP;
                processEvent(Z_TOUCH_END_MOVE);
            }
        }
    }
}

void ZApp::tabInfo(bool left, bool ok, bool right) {
    bool processedLeft = false, processedRight = false;
    if (tabLeftStatus) {
        if (left) {
            // Pressed for more than 400 ms => new press
            if (millis() - lastTabMillis > 400) {
                lastTabMillis = millis();
                processedLeft = true;
            }
        } else {
            tabLeftStatus = false;
        }
    } else {
        if (left) {
            lastTabMillis = millis();
            tabLeftStatus = true;
            processedLeft = true;
        }
    }
    if (tabRightStatus) {
        if (right) {
            // Pressed for more than 400 ms => new press
            if (millis() - lastTabMillis > 400) {
                lastTabMillis = millis();
                processedRight = true;
            }
        } else {
            tabRightStatus = false;
        }
    } else {
        if (right) {
            lastTabMillis = millis();
            tabRightStatus = true;
            processedRight = true;
        }
    }

    ZScreen *current = currentScreen();
    ZControl *oldFocused = current->getFocusedFinalControl();
    if (!oldFocused) return;
    if (!oldFocused->isFocusable()) {
        if (!oldFocused->container) return;
        oldFocused->container->doTabRight();
        ZControl *oldFocused = current->getFocusedFinalControl();
        if (!oldFocused) return;
    }
    oldFocused->tabInfo(processedLeft, ok, processedRight);
    ZControl *newFocused = current->getFocusedFinalControl();
    if (newFocused) {
        if (strcmp(oldFocused->id, newFocused->id)) {
            oldFocused->invalidate(); //Serial.print("Unfocus:"); Serial.println(oldFocused->id);
            newFocused->invalidate(); //Serial.print("Focus:"); Serial.println(newFocused->id);
        }
    } else {
        oldFocused->invalidate();     //Serial.print("Unfocus:"); Serial.println(oldFocused->id);
    }
}

void ZApp::processEvent(enum ZEvent event) {
    switch(event) {
        case Z_TOUCH_DOWN:
            touchedControl = currentScreen()->getControlAt(touchX0, touchY0);
            touchedControl->touchDown(touchX0, touchY0);
            break;
        case Z_TOUCH_UP:
            if (touchedControl) {
                touchedControl->touchUp(touchX0, touchY0);
                touchedControl = NULL;
            }
            break;
        case Z_TOUCH_START_MOVE:
            touchedControl->touchStartMove(touchX0, touchY0);
            break;
        case Z_TOUCH_MOVE:
            touchedControl->touchMove(touchX0, touchY0, lastTouchX, lastTouchY);
            break;
        case Z_TOUCH_END_MOVE:
            touchedControl->touchEndMove(touchX0, touchY0, lastTouchX, lastTouchY);
            touchedControl = NULL;
            break;
        default:
            Serial.print("Event not Handled:");
            Serial.println(event);
    }
}

void ZApp::push(const char *screenId) {
    push(screenId, NULL, NULL);
}
void ZApp::push(const char *screenId, JsonDocument *jsonData) {
    push(screenId, jsonData, NULL);
}
void ZApp::push(const char *screenId, const char *screenAction) {
    push(screenId, NULL, screenAction);
}
void ZApp::push(const char *screenId, JsonDocument *jsonData, const char *screenAction) {
    ZScreenInstance *d = new ZScreenInstance();
    d->screen = ZApp::createScreen(screenId);
    screens->add(d);
    d->screen->init(this, screenAction);
    ZEventData eventData = {};
    eventData.jsonData = jsonData;
    d->screen->triggerEvent(d->screen, "init", eventData);
}
void ZApp::open(const char *screenId) {
    open(screenId, NULL);
}
void ZApp::open(const char *screenId, JsonDocument *jsonData) {
    ZScreenInstance *top = (ZScreenInstance *)screens->top();
    ZEventData closeEventData = {};
    top->screen->triggerEvent(top->screen, "finish", closeEventData);
    screens->remove(screens->length() - 1);

    ZScreenInstance *d = new ZScreenInstance();
    d->screen = ZApp::createScreen(screenId);    
    screens->add(d);
    d->screen->init(this, "open");
    ZEventData eventData = {};
    eventData.jsonData = jsonData;
    d->screen->triggerEvent(d->screen, "init", eventData);
}

void ZApp::closeCurrent(JsonDocument *jsonData) {
    ZScreenInstance *top = (ZScreenInstance *)screens->top();
    ZEventData eventData = {};
    top->screen->triggerEvent(top->screen, "finish", eventData);
    screens->remove(screens->length() - 1);
    ZScreenInstance *newTop = (ZScreenInstance *)screens->top();
    eventData.jsonData = jsonData;
    newTop->screen->triggerEvent(top->screen, "close", eventData);
    delete top->screen;
    newTop->screen->invalidate();
}
void ZApp::cancelCurrent() {
    ZScreenInstance *top = (ZScreenInstance *)screens->top();
    ZEventData eventData = {};
    top->screen->triggerEvent(top->screen, "finish", eventData);
    screens->remove(screens->length() - 1);
    ((ZScreenInstance *)screens->top())->screen->triggerEvent(top->screen, "cancel", eventData);
    delete top->screen;
    ((ZScreenInstance *)screens->top())->screen->invalidate();
}