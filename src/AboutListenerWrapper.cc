#include "nan.h"

#include "AboutListenerWrapper.h"
#include "AboutListenerImpl.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

static Nan::Persistent<v8::FunctionTemplate> aboutlistener_constructor;

NAN_METHOD(AboutListenerConstructor) {
  if(info.Length() < 1){
    return Nan::ThrowError("NAN_METHOD(AboutListenerConstructor) AboutListener requires a callback for Announced(busName, version, port, objectDescriptionArg, aboutDataArg).");
  }

  v8::Local<v8::Object> obj;
  v8::Local<v8::FunctionTemplate> con = Nan::New<v8::FunctionTemplate>(aboutlistener_constructor);

  v8::Local<v8::Value> argv[] = {
    info[0]
  };
  obj = con->GetFunction()->NewInstance(1, argv);
  info.GetReturnValue().Set(obj);
}

AboutListenerWrapper::AboutListenerWrapper(Nan::Callback* announced)
  :listener(new AboutListenerImpl(announced)){
}

AboutListenerWrapper::~AboutListenerWrapper(){
}

void AboutListenerWrapper::Init () {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(AboutListenerWrapper::New);
  aboutlistener_constructor.Reset(tpl);
  tpl->SetClassName(Nan::New<v8::String>("AboutListener").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
}

NAN_METHOD(AboutListenerWrapper::New) {
  if(info.Length() < 1){
    return Nan::ThrowError("NAN_METHOD(AboutListenerWrapper::New) AboutListener requires a callback for Announced(busName, version, port, objectDescriptionArg, aboutDataArg).");
  }
  v8::Local<v8::Function> announced = info[0].As<v8::Function>();
  Nan::Callback *announcedCall = new Nan::Callback(announced);

  AboutListenerWrapper* obj = new AboutListenerWrapper(announcedCall);
  obj->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}
