#include "ZCompoundControl.h"
#include "ZApp.h"

ZControl **ZCompoundControl::parseControls(JsonObject jsonControls) {
  int n = 0;
  for (JsonPair p:jsonControls) n++;
  ZControl **_controls = (ZControl **)malloc(sizeof(ZControl *) * (n + 1));
  _controls[n] = NULL;
  int i=0; 
  for (JsonPair p:jsonControls) {
    const char *id = p.key().c_str();
    JsonObject controlDef = p.value();
    _controls[i++] = ZControl::create(id, controlDef);
  }
  return _controls;
}

ZCompoundControl::ZCompoundControl(const char *_id, int _x, int _y, int _w, int _h, ZControl **_controls):ZControl(_id, _x,_y, _w,_h) {
  controls = _controls;
  modified = true;
  eventListeners = new ZLinkedList();
}

ZCompoundControl::ZCompoundControl(const char *_id, JsonObject def): ZControl(_id, def) {
  modified = true;
  eventListeners = new ZLinkedList();
  if (def.containsKey("controls")) {
    controls = ZCompoundControl::parseControls(def["controls"]);
  } else {
    controls = {NULL};
  }
}

ZCompoundControl::~ZCompoundControl() {
  delete eventListeners;
  int i=0;
  while(controls[i]) {
    delete controls[i];
    controls[i] = NULL;
    i++;
  }
}

void ZCompoundControl::init(ZApp *_app, ZCompoundControl *_container) {
  ZControl::init(_app, _container);
  activeTindex = -1;
  int i=0;
  for (int i=0; controls[i]; i++) {
    controls[i]->init(app, this);
  }
}

void ZCompoundControl::draw() {
  if (!isValid()) clear();
  for (int i=0; controls[i]; i++) {
    if (controls[i]->isModified()) controls[i]->draw();
  }
  ZControl::draw();
  modified = false;
}

bool ZCompoundControl::isFinal() {return false;}

void ZCompoundControl::invalidate() {
  for (int i=0; controls[i]; i++) controls[i]->invalidate();
  ZControl::invalidate();
  modified = true;
}
void ZCompoundControl::setModified() {
  modified = true;
  if (container) container->setModified();
}
bool ZCompoundControl::isModified() {return modified;}

ZControl *ZCompoundControl::getControl(char *_id) {
  if (!strcmp(_id, id)) return this;
  for (int i=0; controls[i]; i++) {
    ZControl *c = controls[i]->getControl(_id);
    if (c) return c;
  }
  return NULL;
}
ZControl *ZCompoundControl::getControlAt(int x, int y) {
  ZControl *thisTarget = ZControl::getControlAt(x, y);
  if (!thisTarget) return NULL;
  for (int i=0; controls[i]; i++) {
    ZControl *childTarget = controls[i]->getControlAt(x, y);
    if (childTarget) return childTarget;
  }
  return thisTarget;
}
int ZCompoundControl::childrenOffsetX() {return 0;}
int ZCompoundControl::childrenOffsetY() {return 0;}

void ZCompoundControl::on(const char *controlId, const char *eventName, ZEventHandler eventHandler) {
  ZEventHandlerRegistration *d = new ZEventHandlerRegistration();
  d->screenId = id;
  d->controlId = controlId;
  d->eventName = eventName;
  d->handler = eventHandler;
  eventListeners->add(d);
}
bool ZCompoundControl::triggerEvent(ZControl *control, const char *eventName, ZEventData eventData) {
  int n = eventListeners->length();
  for (int i=0; i<n; i++) {
    ZEventHandlerRegistration *d = (ZEventHandlerRegistration *)eventListeners->get(i);
    if (!strcmp(d->controlId, control->id) && !strcmp(eventName, d->eventName)) {
      (d->handler)(getScreen(), control, eventData);
      return true;
    }
  }
  if (container && container->triggerEvent(control, eventName, eventData)) return true;
  return app->triggerEvent(getScreen(), control, eventName, eventData);
}

int ZCompoundControl::findNextTindex(int fromTindex) {
  int i=0, selected = -1;
  while (controls[i]) {
    if (controls[i]->isFocusable()) {
      int ti = controls[i]->tindex;
      if (ti > 0 && ti > fromTindex) {
        if (selected == -1 || ti < selected) selected = ti;
      }
    }
    i++;
  }
  return selected;
}
int ZCompoundControl::findPrevTindex(int fromTindex) {
  int i=0, selected = -1;
  while (controls[i]) {
    if (controls[i]->isFocusable()) {
      int ti = controls[i]->tindex;
      if (ti > 0 && ti < fromTindex) {
        if (selected == -1 || ti > selected) selected = ti;
      }
    }
    i++;
  }
  return selected;
}
ZControl *ZCompoundControl::getFocusedFinalControl() {
  if (activeTindex <= 0) activeTindex = findNextTindex(activeTindex);
  if (activeTindex <= 0) return NULL;
  int i=0;
  while (controls[i]) {
    if (controls[i]->tindex == activeTindex) {
      if (controls[i]->isFinal()) return controls[i];
      return ((ZCompoundControl *)controls[i])->getFocusedFinalControl();
    }
    i++;
  }
  return NULL;
}

void ZCompoundControl::doTabLeft() {
  int idx = findPrevTindex(activeTindex);
  if (idx < 0) {
    if (container) {
      container->doTabLeft();
    } else {
      idx = findPrevTindex(65000); // Last control
    }
  }
  activeTindex = idx;
}
void ZCompoundControl::doTabRight() {
  int idx = findNextTindex(activeTindex);
  if (idx < 0) {
    if (container) {
      container->doTabRight();
    } else {
      idx = findNextTindex(-1); // First control
    }
  }
  activeTindex = idx;
}