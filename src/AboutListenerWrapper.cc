#include "nan.h"

#include "AboutListenerWrapper.h"
#include "AboutListenerImpl.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

static v8::Persistent<v8::FunctionTemplate> listener_constructor;

v8::Handle<v8::Value> AboutListenerWrapper::NewInstance() {
    NanScope();

    v8::Local<v8::Object> obj;
    v8::Local<v8::FunctionTemplate> con = NanNew<v8::FunctionTemplate>(listener_constructor);
    obj = con->GetFunction()->NewInstance(0, NULL);
    return obj;
}

NAN_METHOD(AboutListenerConstructor) {
  NanScope();
  
  if(args.Length() < 1){
    return NanThrowError("NAN_METHOD(AboutListenerConstructor) AboutListener requires a callback for Announced(busName, version, port, objectDescriptionArg, aboutDataArg).");
  }
  
  v8::Local<v8::Object> obj;
  v8::Local<v8::FunctionTemplate> con = NanNew<v8::FunctionTemplate>(listener_constructor);

  v8::Handle<v8::Value> argv[] = {
    args[0]
  };
  obj = con->GetFunction()->NewInstance(1, argv);
  NanReturnValue(obj);
}

AboutListenerWrapper::AboutListenerWrapper(NanCallback* announced)
  :listener(new AboutListenerImpl(announced)){
}

AboutListenerWrapper::~AboutListenerWrapper(){
}

void AboutListenerWrapper::Init () {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(AboutListenerWrapper::New);
  NanAssignPersistent(listener_constructor, tpl);
  tpl->SetClassName(NanNew<v8::String>("AboutListener"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
}

NAN_METHOD(AboutListenerWrapper::New) {
  NanScope();
  if(args.Length() < 1){
    return NanThrowError("NAN_METHOD(AboutListenerWrapper::New) AboutListener requires a callback for Announced(busName, version, port, objectDescriptionArg, aboutDataArg).");
  }
  v8::Local<v8::Function> announced = args[0].As<v8::Function>();
  NanCallback *announcedCall = new NanCallback(announced);

  AboutListenerWrapper* obj = new AboutListenerWrapper(announcedCall);
  obj->Wrap(args.This());

  NanReturnValue(args.This());
}

