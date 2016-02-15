#include "nan.h"

#include "AuthListenerWrapper.h"
#include "AuthListenerImpl.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

static v8::Persistent<v8::FunctionTemplate> authlistener_constructor;

v8::Handle<v8::Value> AuthListenerWrapper::NewInstance() {
    NanScope();

    v8::Local<v8::Object> obj;
    v8::Local<v8::FunctionTemplate> con = NanNew<v8::FunctionTemplate>(authlistener_constructor);
    obj = con->GetFunction()->NewInstance(0, NULL);
    return obj;
}

NAN_METHOD(AuthListenerConstructor) {
  NanScope();

  if(args.Length() < 1){
    return NanThrowError("NAN_METHOD(AuthListenerConstructor) AuthListener requires a callback for Announced(busName, version, port, objectDescriptionArg, authDataArg).");
  }

  v8::Local<v8::Object> obj;
  v8::Local<v8::FunctionTemplate> con = NanNew<v8::FunctionTemplate>(authlistener_constructor);

  v8::Handle<v8::Value> argv[] = {
    args[0]
  };
  obj = con->GetFunction()->NewInstance(1, argv);
  NanReturnValue(obj);
}

AuthListenerWrapper::AuthListenerWrapper(NanCallback* requestCredentials, NanCallback* authenticationComplete)
  :listener(new AuthListenerImpl(requestCredentials, authenticationComplete)){
}

AuthListenerWrapper::~AuthListenerWrapper(){
}

void AuthListenerWrapper::Init () {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(AuthListenerWrapper::New);
  NanAssignPersistent(authlistener_constructor, tpl);
  tpl->SetClassName(NanNew<v8::String>("AuthListener"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
}

NAN_METHOD(AuthListenerWrapper::New) {
  NanScope();
  if(args.Length() < 1){
    return NanThrowError("NAN_METHOD(AuthListenerWrapper::New) AuthListener requires a callback for RequestCredentials(const char* authMechanism, const char* authPeer, uint16_t authCount, const char* userId, uint16_t credMask, Credentials& creds) and AuthenticationComplete(const char* authMechanism, const char* authPeer, bool success).");
  }
  v8::Local<v8::Function> requestCredentials = args[0].As<v8::Function>();
  NanCallback *requestCredentialsCall = new NanCallback(requestCredentials);
  v8::Local<v8::Function> authenticationComplete = args[1].As<v8::Function>();
  NanCallback *authenticationCompleteCall = new NanCallback(authenticationComplete);

  AuthListenerWrapper* obj = new AuthListenerWrapper(requestCredentialsCall, authenticationCompleteCall);
  obj->Wrap(args.This());

  NanReturnValue(args.This());
}
