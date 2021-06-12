#ifndef ZControl_h
#define ZControl_h

#include <TFT_eSPI.h>
#include <Arduino.h>
#include "ZTypes.h"
#include "ZLinkedList.h"

struct ZControlFactoryDeclaration: ZLLData {
  const char* controlType;
  ZControlFactory factory;
};
class ZControl {
  private:
    bool valid;
    bool visible;
    static ZLinkedList *factoriesLL();
  protected:
    const char *theme;
    ZTheme getTheme();
  public:
    static int registerFactory(const char *type, ZControlFactory factory);
    static void showFactories();
    static ZControl *create(const char *_id, JsonObject def);
    static void parseFontUsage(JsonObject def, const char *jsonAttributeName, ZFontUsage &fontUsage);
    static int parseTextAlign(const char *ta);
    char id[33];
    int x,y,w,h;
    int tindex;
    ZApp *app;
    ZCompoundControl *container;
    ZControl(const char *_id, int _x, int _y, int _w, int _h);
    ZControl(const char *_id, JsonObject def);

    virtual bool triggerEvent(const char *eventName, ZEventData eventData);
    virtual void init(ZApp *_app, ZCompoundControl *_container);
    virtual int zx(); virtual int zy(); virtual int zw(); virtual int zh();
    virtual int zAbsX(); virtual int zAbsY();
    virtual void draw();
    virtual bool isFinal();
    bool isValid();
    virtual bool isModified();
    virtual void invalidate();
    ZScreen *getScreen();
    virtual ZControl *getControl(char *id);
    virtual void clear();
    ZColorScheme getColorSchemeForType(enum ZControlType type);
    ZControlType getControlTypeFromStyleName(char *styleNMame);
    virtual ZControl *getControlAt(int x, int y);
    virtual void touchDown(int x, int y);
    virtual void touchUp(int x, int y);
    virtual void touchStartMove(int x, int y);
    virtual void touchMove(int x0, int y0, int x, int y);
    virtual void touchEndMove(int x0, int y0, int x, int y);
    virtual void tabInfo(bool left, bool ok, bool right);
    void show();
    void hide();
    void setVisible(bool v);
    bool isVisible();
    bool isFocusable();
    bool hasFocus();
};

#endif