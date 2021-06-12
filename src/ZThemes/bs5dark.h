#ifndef bs5dark_h
#define bs5dark_h

#include "ZScreens.h"
#include "ZAAFonts/helvetica_neue_16.h"

char jsonTheme_bs5dark[] PROGMEM = R"ZZ({
  "main":{"background":[34,34,34], "radius": 4},
  "yCenterDelta":2,
  "primary":{"background":[63,89,124], "foreground":[255,255,255]},
  "secondary":{"background":[68,68,68], "foreground":[255,255,255]},
  "success":{"background":[84,185,143], "foreground":[255,255,255]},
  "danger":{"background":[214,87,69], "foreground":[255,255,255]},
  "warning":{"background":[230,160,60], "foreground":[255,255,255]},
  "info":{"background":[82,150,214], "foreground":[255,255,255]},
  "light":{"background":[174,181,188], "foreground":[0,0,0]},
  "dark":{"background":[48,48,48], "foreground":[255,255,255]},
  "fonts":{"normal":{"name":"helvetica_neue_16"}}
})ZZ";

static_block {
  ZApp::registerTheme("bs5dark", jsonTheme_bs5dark);
}

#endif