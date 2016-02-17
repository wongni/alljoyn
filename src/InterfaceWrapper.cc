#include "nan.h"

#include "InterfaceWrapper.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

static Nan::Persistent<v8::FunctionTemplate> interface_constructor;

NAN_METHOD(InterfaceDescriptionWrapper) {
  v8::Local<v8::Object> obj;
  v8::Local<v8::FunctionTemplate> con = Nan::New<v8::FunctionTemplate>(interface_constructor);

  v8::Local<v8::Value> argv[] = {};
  obj = con->GetFunction()->NewInstance(0, argv);
  info.GetReturnValue().Set(obj);
}

InterfaceWrapper::InterfaceWrapper():interface(NULL){}

void InterfaceWrapper::Init () {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(InterfaceWrapper::New);
  interface_constructor.Reset(tpl);
  tpl->SetClassName(Nan::New<v8::String>("InterfaceDescription").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  Nan::SetPrototypeMethod(tpl, "addSignal", InterfaceWrapper::AddSignal);
  Nan::SetPrototypeMethod(tpl, "activate", InterfaceWrapper::Activate);
  Nan::SetPrototypeMethod(tpl, "getMembers", InterfaceWrapper::GetMembers);
  Nan::SetPrototypeMethod(tpl, "introspect", InterfaceWrapper::Introspect);
}

NAN_METHOD(InterfaceWrapper::New) {


  InterfaceWrapper* obj = new InterfaceWrapper();
  obj->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(InterfaceWrapper::AddSignal) {

  int annotation = 0;
  if (info.Length() == 0 || !info[0]->IsString())
    return Nan::ThrowError("AddSignal requires a name string argument");
  if (info.Length() == 1 || !info[1]->IsString())
    return Nan::ThrowError("AddSignal requires an param signature string argument");
  if (info.Length() == 2 || !info[2]->IsString())
    return Nan::ThrowError("AddSignal requires an argument list string argument");
  if(info.Length() >= 4){
    annotation = info[3]->Int32Value();
  }

  InterfaceWrapper* wrapper = node::ObjectWrap::Unwrap<InterfaceWrapper>(info.This());
  QStatus status = wrapper->interface->AddSignal(strdup(*Nan::Utf8String(info[0])), strdup(*Nan::Utf8String(info[1])), strdup(*Nan::Utf8String(info[2])), annotation);
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(InterfaceWrapper::Activate) {

  InterfaceWrapper* wrapper = node::ObjectWrap::Unwrap<InterfaceWrapper>(info.This());
  wrapper->interface->Activate();
  info.GetReturnValue().SetUndefined();
}

NAN_METHOD(InterfaceWrapper::GetMembers) {

  InterfaceWrapper* wrapper = node::ObjectWrap::Unwrap<InterfaceWrapper>(info.This());
  size_t member_num = wrapper->interface->GetMembers();
  const ajn::InterfaceDescription::Member** members = new const ajn::InterfaceDescription::Member*[member_num];
  wrapper->interface->GetMembers(members, member_num);

  v8::Local<v8::Array> v8members = v8::Array::New(member_num);
  for (size_t i = 0; i < member_num; ++i) {
    v8::Local<v8::Object> v8member = Nan::New<v8::Object>();

    Nan::Set(v8member, Nan::New<v8::String>("memberType").ToLocalChecked(), Nan::New<v8::Integer>(static_cast<int>(members[i]->memberType)));
    Nan::Set(v8member, Nan::New<v8::String>("name").ToLocalChecked(), Nan::New<v8::String>(members[i]->name.c_str()).ToLocalChecked());
    Nan::Set(v8member, Nan::New<v8::String>("signature").ToLocalChecked(), Nan::New<v8::String>(members[i]->signature.c_str()).ToLocalChecked());
    Nan::Set(v8member, Nan::New<v8::String>("returnSignature").ToLocalChecked(), Nan::New<v8::String>(members[i]->returnSignature.c_str()).ToLocalChecked());
    Nan::Set(v8member, Nan::New<v8::String>("argNames").ToLocalChecked(), Nan::New<v8::String>(members[i]->argNames.c_str()).ToLocalChecked());
    Nan::Set(v8member, Nan::New<v8::String>("accessPerms").ToLocalChecked(), Nan::New<v8::String>(members[i]->accessPerms.c_str()).ToLocalChecked());
    Nan::Set(v8member, Nan::New<v8::String>("description").ToLocalChecked(), Nan::New<v8::String>(members[i]->description.c_str()).ToLocalChecked());
    Nan::Set(v8member, Nan::New<v8::String>("isSessionlessSignal").ToLocalChecked(), Nan::New<v8::Boolean>(members[i]->isSessionlessSignal));
    Nan::Set(v8members, i, v8member);
  }
  info.GetReturnValue().Set(v8members);
}

NAN_METHOD(InterfaceWrapper::Introspect) {


  InterfaceWrapper* wrapper = node::ObjectWrap::Unwrap<InterfaceWrapper>(info.This());
  info.GetReturnValue().Set(Nan::New<v8::String>(wrapper->interface->Introspect().c_str()).ToLocalChecked());
}
