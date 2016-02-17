#include "nan.h"

#include "BusConnection.h"
#include "AboutObjWrapper.h"
#include "AboutDataWrapper.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

static Nan::Persistent<v8::FunctionTemplate> aboutobj_constructor;

NAN_METHOD(AboutObjConstructor) {


  if(info.Length() < 1){
    return Nan::ThrowError("NAN_METHOD(AboutObjConstructor) AboutObj requires a bus attachment");
  }

  v8::Local<v8::Object> obj;
  v8::Local<v8::FunctionTemplate> con = Nan::New<v8::FunctionTemplate>(aboutobj_constructor);

  v8::Local<v8::Value> argv[] = {
    info[0]
  };
  obj = con->GetFunction()->NewInstance(1, argv);
  info.GetReturnValue().Set(obj);
}

AboutObjWrapper::AboutObjWrapper(ajn::BusAttachment* bus)
  :aboutobj(new ajn::AboutObj(*bus)){
}

AboutObjWrapper::~AboutObjWrapper(){
}

void AboutObjWrapper::Init () {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(AboutObjWrapper::New);
  aboutobj_constructor.Reset(tpl);
  tpl->SetClassName(Nan::New<v8::String>("AboutObj").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "announce", AboutObjWrapper::Announce);
}

NAN_METHOD(AboutObjWrapper::New) {

  if(info.Length() < 1){
    return Nan::ThrowError("NAN_METHOD(AboutObjWrapper::New) AboutObj requires a bus attachment.");
  }

  BusConnection* connection = node::ObjectWrap::Unwrap<BusConnection>(info[0].As<v8::Object>());

  AboutObjWrapper* obj = new AboutObjWrapper(connection->bus);
  obj->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}


NAN_METHOD(AboutObjWrapper::Announce) {

  if(info.Length() < 2){
    return Nan::ThrowError("NAN_METHOD(AboutObjWrapper::Announce) AboutObjWrapper::Announce requires a session port and about data.");
  }

  ajn::SessionPort port = static_cast<ajn::SessionPort>(info[0]->Int32Value());
  AboutDataWrapper* wrapper = node::ObjectWrap::Unwrap<AboutDataWrapper>(info[1].As<v8::Object>());

  AboutObjWrapper* objWrapper = node::ObjectWrap::Unwrap<AboutObjWrapper>(info.This());
  QStatus status = objWrapper->aboutobj->Announce(port, *(wrapper->aboutdata));

  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}
