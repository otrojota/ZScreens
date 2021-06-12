#ifndef ZApp_h
#define ZApp_h

#include <TFT_eSPI.h>
#include <Arduino.h>
#include "ZTypes.h"
#include "ZLinkedList.h"

struct ZScreenDefinition: ZLLData {
  const char *id;
  const char *def;
};
struct ZThemeDefinition: ZLLData {
  const char *id;
  ZTheme theme;
};
enum ZFontType {Z_GFX, Z_ANTIALIASED};
struct ZFontDeclaration: ZLLData {
  const char *name;
  const void *data;
  ZFontType type;
};
enum ZIconType {Z_XBM};
struct ZIconInfo: ZLLData {
  const char *name;
  ZIconType type = Z_XBM;
  const void *data; int width; int height;
};
struct ZScreenInstance: ZLLData {
  ZScreen *screen;
};
class ZApp {
  private:
    static ZLinkedList *screenDefinitionsLL();
    static ZLinkedList *themesLL();
    static ZLinkedList *fontsLL();
    static ZLinkedList *eventListenersLL();
    static ZLinkedList *iconsLL();

    ZLinkedList *screens;

    int touchX0, touchY0;
    int lastTouchX, lastTouchY;
    unsigned long lastTouchMillis;
    enum ZTouchStatus touchStatus;
    ZControl *touchedControl;
    void processEvent(enum ZEvent event);
    bool tabLeftStatus, tabRightStatus;
    unsigned long lastTabMillis;

  public:
    static void registerScreen(const char *id, const char *def);
    static ZScreen *createScreen(const char *id);

    static int registerTheme(const char *id, char *jsonTheme);
    static ZTheme getTheme(const char *id);
    static uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
    static int extractColor(JsonArray array);

    static int registerGFXFont(const char *name, const GFXfont *font);
    static int registerAAFont(const char *name, const uint8_t *font);    
    static ZFontDeclaration getFont(const char *name);

    static int registerEventHandler(const char *screenId, const char *controlId, const char *eventName, ZEventHandler handler);
    static int nXBMs;
    static int registerXBM(const char *name, const unsigned char *xbm, const int width, const int height);
    static ZIconInfo getIcon(const char *name);

    int deltaMoveSensitivity;
    unsigned long deltaNoTouchForUpMillis;
    TFT_eSPI *tft;
    ZTheme theme;
    ZApp(ZScreen *_home, char *themeId);
    ZApp(char *homeScreenId, char *themeId);

    void init(TFT_eSPI *_tft);
    ZScreen *currentScreen();
    void useFont(ZFontUsage f);
    void unuseFont(ZFontUsage f);

    bool triggerEvent(ZScreen *screen, ZControl *control, const char *eventName, ZEventData eventData);
    void touchInfo(int touchX, int touchY);
    void tabInfo(bool left, bool ok, bool right);
    void loop();

    void push(const char *screenId);
    void push(const char *screenId, JsonDocument *jsonData);
    void push(const char *screenId, const char *screenAction);
    void push(const char *screenId, JsonDocument *jsonData, const char *screenAction);
    void open(const char *screenId);
    void open(const char *screenId, JsonDocument *jsonData);

    void closeCurrent(JsonDocument *jsonData);
    void cancelCurrent();
};

#endif