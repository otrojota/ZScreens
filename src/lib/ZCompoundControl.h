#ifndef ZCompoundControl_h
#define ZCompoundControl_h

#include <TFT_eSPI.h>
#include <Arduino.h>
#include "ZTypes.h"
#include "ZControl.h"

struct ZEventHandlerRegistration: ZLLData {
  const char *screenId;
  const char *controlId;
  const char *eventName;
  ZEventHandler handler;
};
class ZCompoundControl: public ZControl {
  private:
    ZLinkedList *eventListeners;
  public:
    static ZControl** parseControls(JsonObject jsonControls);
    ZControl **controls; // NULL terminated array
    bool modified;
    ZCompoundControl(const char *_id, int _x, int _y, int _w, int _h, ZControl **_controls);
    ZCompoundControl(const char *_id, JsonObject def);
    ~ZCompoundControl();
    virtual void init(ZApp *_app, ZCompoundControl *_container);
    virtual bool isFinal();
    virtual void draw();
    virtual void invalidate();
    virtual void setModified();
    virtual bool isModified();
    virtual ZControl *getControl(char *id);
    virtual ZControl *getControlAt(int x, int y);
    virtual int childrenOffsetX();
    virtual int childrenOffsetY();
    virtual void on(const char *controlId, const char *eventName, ZEventHandler eventHandler);
    bool triggerEvent(ZControl *control, const char *eventName, ZEventData eventData);
    virtual ZControl *getFocusedFinalControl();
    int activeTindex;
    int findNextTindex(int fromTindex);
    int findPrevTindex(int fromTindex);
    virtual void doTabLeft();
    virtual void doTabRight();
};

#endif