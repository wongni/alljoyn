#include "nan.h"

#include "BusConnection.h"
#include "AboutObjWrapper.h"
#include "AboutDataWrapper.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

static v8::Persistent<v8::FunctionTemplate> aboutobj_constructor;

v8::Handle<v8::Value> AboutObjWrapper::NewInstance() {
    NanScope();

    v8::Local<v8::Object> obj;
    v8::Local<v8::FunctionTemplate> con = NanNew<v8::FunctionTemplate>(aboutobj_constructor);
    obj = con->GetFunction()->NewInstance(0, NULL);
    return obj;
}

NAN_METHOD(AboutObjConstructor) {
  NanScope();
  
  if(args.Length() < 1){
    return NanThrowError("NAN_METHOD(AboutObjConstructor) AboutObj requires a bus attachment");
  }
  
  v8::Local<v8::Object> obj;
  v8::Local<v8::FunctionTemplate> con = NanNew<v8::FunctionTemplate>(aboutobj_constructor);

  v8::Handle<v8::Value> argv[] = {
    args[0]
  };
  obj = con->GetFunction()->NewInstance(1, argv);
  NanReturnValue(obj);
}

AboutObjWrapper::AboutObjWrapper(ajn::BusAttachment* bus)
  :aboutobj(new ajn::AboutObj(*bus)){
}

AboutObjWrapper::~AboutObjWrapper(){
}

void AboutObjWrapper::Init () {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(AboutObjWrapper::New);
  NanAssignPersistent(aboutobj_constructor, tpl);
  tpl->SetClassName(NanNew<v8::String>("AboutObj"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  
  NODE_SET_PROTOTYPE_METHOD(tpl, "announce", AboutObjWrapper::Announce);
}

NAN_METHOD(AboutObjWrapper::New) {
  NanScope();
  if(args.Length() < 1){
    return NanThrowError("NAN_METHOD(AboutObjWrapper::New) AboutObj requires a bus attachment.");
  }

  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(args[0].As<v8::Object>());
  
  AboutObjWrapper* obj = new AboutObjWrapper(connection->bus);
  obj->Wrap(args.This());

  NanReturnValue(args.This());
}


NAN_METHOD(AboutObjWrapper::Announce) {
  NanScope();
  if(args.Length() < 2){
    return NanThrowError("NAN_METHOD(AboutObjWrapper::Announce) AboutObjWrapper::Announce requires a session port and about data.");
  }

  ajn::SessionPort port = static_cast<ajn::SessionPort>(args[0]->Int32Value());
  AboutDataWrapper* wrapper = node::ObjectWrap::Unwrap<AboutDataWrapper>(args[1].As<v8::Object>());

  AboutObjWrapper* objWrapper = node::ObjectWrap::Unwrap<AboutObjWrapper>(args.This());
  QStatus status = objWrapper->aboutobj->Announce(port, *(wrapper->aboutdata));

  NanReturnValue(NanNew<v8::Integer>(static_cast<int>(status)));
}
