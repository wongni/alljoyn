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
  v8::Local<v8::Object> obj;
  v8::Local<v8::FunctionTemplate> con = NanNew<v8::FunctionTemplate>(listener_constructor);

  obj = con->GetFunction()->NewInstance();
  NanReturnValue(obj);
}

AboutListenerWrapper::AboutListenerWrapper()
  :listener(new AboutListenerImpl()){
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
  AboutListenerWrapper* obj = new AboutListenerWrapper();
  obj->Wrap(args.This());

  NanReturnValue(args.This());
}