#ifndef LD_PROXYBUSOBJECTWRAPPER_H
#define LD_PROXYBUSOBJECTWRAPPER_H

#include <nan.h>
#include <uv.h>
#include <alljoyn/BusObject.h>
#include <ProxyBusObject.h>
#include <TransportMask.h>
#include <alljoyn/AllJoynStd.h>

NAN_METHOD(ProxyBusObjectConstructor);

class ProxyBusObjectWrapper : public node::ObjectWrap {
  private:

    static NAN_METHOD(New);
    static NAN_METHOD(GetInterfaceNames);
    static NAN_METHOD(GetInterface);
    static NAN_METHOD(MethodCall);
    static NAN_METHOD(SecureConnectionAsync);
    static NAN_METHOD(IsSecure);
  public:
    ProxyBusObjectWrapper(ajn::BusAttachment* bus, const char *busName, const char *path, ajn::SessionId sessionId);
    ~ProxyBusObjectWrapper();
    static void Init ();
    static v8::Handle<v8::Value> NewInstance();

    ajn::ProxyBusObject *proxyBusObject;
};

#endif
