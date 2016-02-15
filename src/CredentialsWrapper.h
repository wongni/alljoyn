#ifndef LD_CREDENTIALSWRAPPER_H
#define LD_CREDENTIALSWRAPPER_H

#include <nan.h>
#include <uv.h>
#include <AuthListener.h>
#include <TransportMask.h>
#include <alljoyn/AllJoynStd.h>

NAN_METHOD(CredentialsConstructor);

class CredentialsWrapper : public node::ObjectWrap {
  private:

    static NAN_METHOD(New);
    static NAN_METHOD(IsSet);
    static NAN_METHOD(SetPassword);
    static NAN_METHOD(SetUserName);
    static NAN_METHOD(SetCertChain);
    static NAN_METHOD(SetPrivateKey);
    static NAN_METHOD(SetLogonEntry);
    static NAN_METHOD(SetExpiration);
    static NAN_METHOD(GetPassword);
    static NAN_METHOD(GetUserName);
    static NAN_METHOD(GetCertChain);
    static NAN_METHOD(GetPrivateKey);
    static NAN_METHOD(GetLogonEntry);
    static NAN_METHOD(GetExpiration);

  public:
    CredentialsWrapper();
    ~CredentialsWrapper();
    static void Init ();
    static v8::Handle<v8::Value> NewInstance();

    ajn::AuthListener::Credentials *credentials;
};

#endif
