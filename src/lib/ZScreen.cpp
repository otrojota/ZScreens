#include "ZScreen.h"
#include "ZApp.h"

ZScreen::ZScreen(const char *_id, ZControl *_controls[]):ZCompoundControl(_id, -1, -1, -1, -1, _controls) {  
}
void ZScreen::init(ZApp *_app, const char *_screenAction) {
  ZCompoundControl::init(_app, NULL);
  x = 0; y = 0;
  w = app->tft->width(); h = app->tft->height(); 
  screenAction = _screenAction;
}
void ZScreen::close() {
  close(NULL);
}
void ZScreen::close(JsonDocument *jsonData) {
  app->closeCurrent(jsonData);
}
void ZScreen::cancel() {
  app->cancelCurrent();
}
void ZScreen::clear() {
  app->tft->fillScreen(getTheme().mainBackground);
}