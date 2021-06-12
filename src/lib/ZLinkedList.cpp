#include "ZLinkedList.h"

ZLLElement::ZLLElement(ZLLData *_data) {
  data = _data;
  prev = NULL;
  next = NULL;
}
ZLLElement::~ZLLElement() {
  if (data) delete data;
}
ZLinkedList::ZLinkedList() {
  n = 0;
  root = NULL;
}
ZLinkedList::~ZLinkedList() {
  for (int i=0; i<length(); i++) {
    delete get(i);
  }
}
int ZLinkedList::length() {
  return n;
}
ZLLElement *ZLinkedList::firstElement() {return root;}
ZLLElement *ZLinkedList::lastElement() {
  ZLLElement *e = root;
  while(e->next) {e = e->next;}
  return e;
}
ZLLElement *ZLinkedList::getElement(int index) {
  ZLLElement *e = root;
  int i = 0;
  while(i < index) {e = e->next; i++;}
  return e;
}
void ZLinkedList::add(ZLLData *data) {  
  ZLLElement *e = new ZLLElement(data);
  if (!root) {
    root = e;
  } else {
    ZLLElement *last = lastElement();
    last->next = e;
    e->prev = last;
  }
  n++;
}
ZLLData *ZLinkedList::get(int index) {
  return getElement(index)->data;
}
ZLLData *ZLinkedList::top() {
  return get(length() - 1);
}
void ZLinkedList::remove(int index) {
  ZLLElement *e = getElement(index);
  if (!index) root = e->next;
  if (e->prev) e->prev->next = e->next;
  if (e->next) e->next->prev = e->prev;
  delete e;
  n--;
  if (!n) root = NULL;
}