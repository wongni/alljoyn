#ifndef LD_ABOUTPROXYWRAPPER_H
#define LD_ABOUTPROXYWRAPPER_H

#include <nan.h>
#include <uv.h>
#include <alljoyn/BusObject.h>
#include <AboutProxy.h>
#include <TransportMask.h>
#include <alljoyn/AllJoynStd.h>
#include "AboutProxyImpl.h"

NAN_METHOD(AboutProxyConstructor);

class AboutProxyWrapper : public node::ObjectWrap {
  private:

    static NAN_METHOD(New);
  public:
    AboutProxyWrapper(ajn::BusAttachment* busAttachment, const char* busName, ajn::SessionId sessionId);
    ~AboutProxyWrapper();
    static void Init ();
    static v8::Handle<v8::Value> NewInstance();

    AboutProxyImpl *proxy;
};

#endif