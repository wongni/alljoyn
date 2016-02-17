#ifndef LD_ABOUTLISTENERWRAPPER_H
#define LD_ABOUTLISTENERWRAPPER_H

#include <nan.h>
#include <uv.h>
#include <alljoyn/BusObject.h>
#include <AboutListener.h>
#include <TransportMask.h>
#include <alljoyn/AllJoynStd.h>
#include "AboutListenerImpl.h"

NAN_METHOD(AboutListenerConstructor);

class AboutListenerWrapper : public node::ObjectWrap {
  private:

    static NAN_METHOD(New);
  public:
  	AboutListenerWrapper(Nan::Callback* announced);
  	~AboutListenerWrapper();
    static void Init ();
    static v8::Handle<v8::Value> NewInstance();

    AboutListenerImpl *listener;
};

#endif