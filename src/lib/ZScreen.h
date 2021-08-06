#ifndef ZScreen_h
#define ZScreen_h

#include <Arduino.h>
#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>
#include "ZTypes.h"
#include "ZCompoundControl.h"

class ZScreen: public ZCompoundControl {
  friend class ZApp;
  public:
    const char *screenAction;
    ZScreen(const char *_id, ZControl *_controls[]);
    virtual void init(ZApp *_app, const char *_screenAction);
    virtual void close();
    virtual void close(JsonDocument *jsonData);
    virtual void cancel();
    virtual void clear();
};

#endif