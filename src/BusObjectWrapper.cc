#include "nan.h"

#include "BusObjectWrapper.h"
#include "InterfaceWrapper.h"
#include "util.h"
#include <string.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

static Nan::Persistent<v8::FunctionTemplate> busobject_constructor;

NAN_METHOD(BusObjectConstructor) {

  if(info.Length() == 0 || !info[0]->IsString()){
    return Nan::ThrowError("BusObject requires a path string.");
  }

  v8::Local<v8::Object> obj;
  v8::Local<v8::FunctionTemplate> con = Nan::New<v8::FunctionTemplate>(busobject_constructor);

  v8::Local<v8::Value> argv[] = {
    info[0]
  };
  obj = con->GetFunction()->NewInstance(1, argv);
  info.GetReturnValue().Set(obj);
}

BusObjectWrapper::BusObjectWrapper(const char* path):object(new BusObjectImpl(path)){}

void BusObjectWrapper::Init () {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(BusObjectWrapper::New);
  busobject_constructor.Reset(tpl);
  tpl->SetClassName(Nan::New<v8::String>("BusObject").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  Nan::SetPrototypeMethod(tpl, "addInterface", BusObjectWrapper::AddInterfaceInternal);
  Nan::SetPrototypeMethod(tpl, "signal", BusObjectWrapper::Signal);
}

NAN_METHOD(BusObjectWrapper::New) {

  if(info.Length() < 1 || !info[0]->IsString()){
    return Nan::ThrowError("BusObject requires a path string.");
  }
  char* path = strdup(*Nan::Utf8String(info[0]));
  BusObjectWrapper* obj = new BusObjectWrapper(path);
  obj->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(BusObjectWrapper::AddInterfaceInternal) {

  if(info.Length() < 1){
    return Nan::ThrowError("BusObject.AddInterface requires an Interface.");
  }
  BusObjectWrapper* obj = node::ObjectWrap::Unwrap<BusObjectWrapper>(info.This());
  InterfaceWrapper* interWrapper = node::ObjectWrap::Unwrap<InterfaceWrapper>(info[0].As<v8::Object>());
  QStatus status = obj->object->AddInter(interWrapper->interface);
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusObjectWrapper::Signal) {

  if(info.Length() < 3){
    return Nan::ThrowError("BusObject.Signal requires a (nullable) destination, SessionId, Interface, member name, and message info.");
  }
  BusObjectWrapper* obj = node::ObjectWrap::Unwrap<BusObjectWrapper>(info.This());
  InterfaceWrapper* interface = node::ObjectWrap::Unwrap<InterfaceWrapper>(info[2].As<v8::Object>());
  const ajn::InterfaceDescription::Member* signalMember = interface->interface->GetMember(*Nan::Utf8String(info[3]));
  const char* destination = NULL;
  if(!info[0]->IsNull() && info[0]->IsString()){
    destination = strdup(*Nan::Utf8String(info[0]));
  }
  ajn::MsgArg* msgArgs = objToMsgArg(info[4]);
  QStatus status = ER_OK;
  if(info.Length() == 4){
    status = obj->object->Signal(destination, info[1]->Int32Value(), *signalMember, NULL, 0, 0, 0);
  }else if(info.Length() == 5){
    status = obj->object->Signal(destination, info[1]->Int32Value(), *signalMember, msgArgs, 1, 0, 0);
  }else if(info.Length() > 5){
    //TODO handle multi-arg messages
    status = obj->object->Signal(destination, info[1]->Int32Value(), *signalMember, msgArgs, 1, 0, 0);
  }else{
    return Nan::ThrowError("BusObject.Signal requires a SessionId, Interface, member name, and (optionally) destination, message info.");
  }
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

// this is a subclass of BusinessObject that allows access to
// protected functions like AddInterface.
BusObjectImpl::BusObjectImpl(const char* path):ajn::BusObject(path){
}

QStatus BusObjectImpl::AddInter(ajn::InterfaceDescription* interface){
    return AddInterface(*interface);
}

BusObjectImpl::~BusObjectImpl(){
}
