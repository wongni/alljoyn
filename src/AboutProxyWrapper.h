#ifndef LD_ABOUTPROXYWRAPPER_H
#define LD_ABOUTPROXYWRAPPER_H

#include <nan.h>
#include <uv.h>
#include <alljoyn/BusObject.h>
#include <AboutProxy.h>
#include <TransportMask.h>
#include <alljoyn/AllJoynStd.h>

NAN_METHOD(AboutProxyConstructor);

class AboutProxyWrapper : public Nan::ObjectWrap {
  private:
    static NAN_METHOD(New);
    static NAN_METHOD(GetObjectDescription);
    static NAN_METHOD(GetAboutData);
    static NAN_METHOD(GetSessionId);
    static NAN_METHOD(GetUniqueName);
    static NAN_METHOD(GetVersion);

  public:
    AboutProxyWrapper(ajn::BusAttachment* busAttachment, const char* busName, ajn::SessionId sessionId);
    ~AboutProxyWrapper();
    static void Init ();
    static v8::Handle<v8::Value> NewInstance();

    ajn::AboutProxy *proxy;
};

#endif