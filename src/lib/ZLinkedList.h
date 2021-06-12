#ifndef ZLinkedList_h
#define ZLinkedList_h

#include <Arduino.h>

struct ZLLData {}; // Abstract

class ZLLElement {
  friend class ZLinkedList;
  protected:
    ZLLElement *prev;
    ZLLElement *next;
    ZLLData *data;
    ZLLElement(ZLLData *_data);  
    ~ZLLElement();
};

class ZLinkedList {
  private:
    int n;
    ZLLElement *root;
  public:
    ZLinkedList();
    ~ZLinkedList();
    int length();
    ZLLElement *firstElement();
    ZLLElement *lastElement();
    ZLLElement *getElement(int index);
    void add(ZLLData *data);
    ZLLData *get(int index);
    ZLLData *top();
    void remove(int index);  
};

#endif