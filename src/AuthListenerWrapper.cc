#include "nan.h"

#include "AuthListenerWrapper.h"
#include "AuthListenerImpl.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

static Nan::Persistent<v8::FunctionTemplate> authlistener_constructor;

NAN_METHOD(AuthListenerConstructor) {
  if(info.Length() < 1){
    return Nan::ThrowError("NAN_METHOD(AuthListenerConstructor) AuthListener requires a callback for Announced(busName, version, port, objectDescriptionArg, authDataArg).");
  }

  v8::Local<v8::Object> obj;
  v8::Local<v8::FunctionTemplate> con = Nan::New<v8::FunctionTemplate>(authlistener_constructor);

  v8::Local<v8::Value> argv[] = {
    info[0], info[1]
  };
  obj = con->GetFunction()->NewInstance(2, argv);
  info.GetReturnValue().Set(obj);
}

AuthListenerWrapper::AuthListenerWrapper(Nan::Callback* requestCredentials, Nan::Callback* authenticationComplete)
  :listener(new AuthListenerImpl(requestCredentials, authenticationComplete)){
}

AuthListenerWrapper::~AuthListenerWrapper(){
}

void AuthListenerWrapper::Init () {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(AuthListenerWrapper::New);
  authlistener_constructor.Reset(tpl);
  tpl->SetClassName(Nan::New<v8::String>("AuthListener").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
}

NAN_METHOD(AuthListenerWrapper::New) {
  if(info.Length() < 2){
    return Nan::ThrowError("NAN_METHOD(AuthListenerWrapper::New) AuthListener requires callbacks for RequestCredentials and AuthenticationComplete.");
  }
  v8::Local<v8::Function> requestCredentials = info[0].As<v8::Function>();
  Nan::Callback *requestCredentialsCall = new Nan::Callback(requestCredentials);
  v8::Local<v8::Function> authenticationComplete = info[1].As<v8::Function>();
  Nan::Callback *authenticationCompleteCall = new Nan::Callback(authenticationComplete);

  AuthListenerWrapper* obj = new AuthListenerWrapper(requestCredentialsCall, authenticationCompleteCall);
  obj->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}
