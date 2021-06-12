#ifndef ZTypes_h
#define ZTypes_h

#include <ArduinoJson.h>

class ZControl;
class ZCompoundControl;
class ZScreen;
class ZApp;

struct ZFontUsage {
  char name[33] = "\0                               ";
  int number = -1;         // -1 if not used
  int textSize = 1;
  bool isValid = false;
};

enum ZControlType {Z_ERROR, Z_CUSTOM, Z_PRIMARY, Z_SECONDARY, Z_SUCCESS, Z_DANGER, Z_WARNING, Z_INFO, Z_LIGHT, Z_DARK};
enum ZIconLocation {Z_ICON_LEFT, Z_ICON_TOP, Z_ICON_RIGHT, Z_ICON_BOTTOM};

struct ZColorScheme {
  int background; int foreground; int border;
};

enum ZTouchStatus {Z_UP, Z_DOWN, Z_MOVE};
enum ZEvent {Z_TOUCH_DOWN, Z_TOUCH_UP, Z_TOUCH_START_MOVE, Z_TOUCH_END_MOVE, Z_TOUCH_MOVE};
enum ZScrollDirection {Z_SCROLL_NONE, Z_SCROLL_UP, Z_SCROLL_DOWN};
struct ZEventData {
  // Coordinate Events
  int x0; int y0; int x1; int y1;
  // InterScreen communicatons (onInit, onClose)
  JsonDocument *jsonData = NULL;
  // Lists
  int selectedIndex; 
  char *selectedId;
  char *selectedLabel;
};

typedef void (*ZEventHandler) (ZScreen *screen, ZControl *control, ZEventData eventData);
typedef ZControl* (*ZControlFactory) (const char *_id, JsonObject spec);

struct ZTheme {
  int mainBackground; int mainRadius; int mainSelected; int mainFocus;
  int yCenterDelta;
  int margin;
  int lineHeight;
  int primaryBackground; int primaryForeground; int primaryBorder;
  int secondaryBackground; int secondaryForeground; int secondaryBorder;
  int successBackground; int successForeground; int successBorder;
  int dangerBackground; int dangerForeground; int dangerBorder;
  int warningBackground; int warningForeground; int warningBorder;
  int infoBackground; int infoForeground; int infoBorder;
  int lightBackground; int lightForeground; int lightBorder;
  int darkBackground; int darkForeground; int darkBorder;
  ZFontUsage fontNormal;
};

#pragma once
#ifndef STATIC_BLOCK_HPP_
#define STATIC_BLOCK_HPP_

#ifndef CONCATENATE
#define CONCATENATE(s1, s2)     s1##s2
#define EXPAND_THEN_CONCATENATE(s1, s2) CONCATENATE(s1, s2)
#endif /* CONCATENATE */

#ifndef UNIQUE_IDENTIFIER
#ifdef __COUNTER__
#define UNIQUE_IDENTIFIER(prefix) EXPAND_THEN_CONCATENATE(prefix, __COUNTER__)
#else
#define UNIQUE_IDENTIFIER(prefix) EXPAND_THEN_CONCATENATE(prefix, __LINE__)
#endif /* COUNTER */
#else
#endif /* UNIQUE_IDENTIFIER */

#define static_block STATIC_BLOCK_IMPL1(UNIQUE_IDENTIFIER(_static_block_))

#define STATIC_BLOCK_IMPL1(prefix) \
	STATIC_BLOCK_IMPL2(CONCATENATE(prefix,_fn),CONCATENATE(prefix,_var))

#define STATIC_BLOCK_IMPL2(function_name,var_name) \
static void function_name(); \
static int var_name __attribute((unused)) = (function_name(), 0) ; \
static void function_name()


#endif // STATIC_BLOCK_HPP_

#endif