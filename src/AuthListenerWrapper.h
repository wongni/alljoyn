#ifndef LD_AUTHLISTENERWRAPPER_H
#define LD_AUTHLISTENERWRAPPER_H

#include <nan.h>
#include <uv.h>
#include <alljoyn/BusObject.h>
#include <AuthListener.h>
#include <TransportMask.h>
#include <alljoyn/AllJoynStd.h>
#include "AuthListenerImpl.h"

NAN_METHOD(AuthListenerConstructor);

class AuthListenerWrapper : public Nan::ObjectWrap {
  private:
    static NAN_METHOD(New);

  public:
  	AuthListenerWrapper(Nan::Callback* requestCredentials, Nan::Callback* authenticationComplete);
  	~AuthListenerWrapper();
    static void Init ();
    static v8::Handle<v8::Value> NewInstance();

    AuthListenerImpl *listener;
};

#endif
