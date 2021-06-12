#ifndef bs5_h
#define bs5_h

#include "ZScreens.h"
#include "ZAAFonts/helvetica_neue_16.h"

char jsonTheme_bs5[] PROGMEM = R"ZZ({
  "main":{"background":[255,255,255], "radius": 4},
  "yCenterDelta":2,
  "primary":{"background":[49,108,244], "foreground":[255,255,255]},
  "secondary":{"background":[110,117,124], "foreground":[255,255,255]},
  "success":{"background":[64,133,88], "foreground":[255,255,255]},
  "danger":{"background":[203,68,75], "foreground":[255,255,255]},
  "warning":{"background":[246,195,68], "foreground":[0,0,0]},
  "info":{"background":[92,199,236], "foreground":[0,0,0]},
  "light":{"background":[230,230,230], "foreground":[0,0,0]},
  "dark":{"background":[34,37,41], "foreground":[255,255,255]},
  "fonts":{"normal":{"name":"helvetica_neue_16"}}
})ZZ";

static_block {
  ZApp::registerTheme("bs5", jsonTheme_bs5);
}

#endif