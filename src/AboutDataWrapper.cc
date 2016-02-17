#include "nan.h"

#include "BusConnection.h"
#include "AboutDataWrapper.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

static Nan::Persistent<v8::FunctionTemplate> aboutdata_constructor;

NAN_METHOD(AboutDataConstructor) {


  if(info.Length() < 1){
    return Nan::ThrowError("NAN_METHOD(AboutDataConstructor) AboutData language tag string");
  }

  v8::Local<v8::Object> obj;
  v8::Local<v8::FunctionTemplate> con = Nan::New<v8::FunctionTemplate>(aboutdata_constructor);

  v8::Local<v8::Value> argv[] = {
    info[0]
  };
  obj = con->GetFunction()->NewInstance(1, argv);
  info.GetReturnValue().Set(obj);
}

AboutDataWrapper::AboutDataWrapper(const char* languageTag)
  :aboutdata(new ajn::AboutData(languageTag)){
}

AboutDataWrapper::~AboutDataWrapper(){
}

void AboutDataWrapper::Init () {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(AboutDataWrapper::New);
  aboutdata_constructor.Reset(tpl);
  tpl->SetClassName(Nan::New<v8::String>("AboutData").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "isValid", AboutDataWrapper::IsValid);
  Nan::SetPrototypeMethod(tpl, "setAppId", AboutDataWrapper::SetAppId);
  Nan::SetPrototypeMethod(tpl, "setDeviceName", AboutDataWrapper::SetDeviceName);
  Nan::SetPrototypeMethod(tpl, "setDeviceId", AboutDataWrapper::SetDeviceId);
  Nan::SetPrototypeMethod(tpl, "setAppName", AboutDataWrapper::SetAppName);
  Nan::SetPrototypeMethod(tpl, "setManufacturer", AboutDataWrapper::SetManufacturer);
  Nan::SetPrototypeMethod(tpl, "setModelNumber", AboutDataWrapper::SetModelNumber);
  Nan::SetPrototypeMethod(tpl, "setDescription", AboutDataWrapper::SetDescription);
  Nan::SetPrototypeMethod(tpl, "setDateOfManufacture", AboutDataWrapper::SetDateOfManufacture);
  Nan::SetPrototypeMethod(tpl, "setSoftwareVersion", AboutDataWrapper::SetSoftwareVersion);
  Nan::SetPrototypeMethod(tpl, "setHardwareVersion", AboutDataWrapper::SetHardwareVersion);
  Nan::SetPrototypeMethod(tpl, "setSupportUrl", AboutDataWrapper::SetSupportUrl);
}

NAN_METHOD(AboutDataWrapper::New) {

  if(info.Length() < 1){
    return Nan::ThrowError("NAN_METHOD(AboutDataWrapper::New) AboutData requires a language tag string.");
  }

  char* languageTag = strdup(*Nan::Utf8String(info[0]));

  AboutDataWrapper* obj = new AboutDataWrapper(languageTag);
  obj->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(AboutDataWrapper::IsValid) {


  AboutDataWrapper* wrapper = node::ObjectWrap::Unwrap<AboutDataWrapper>(info.This());

  bool valid = wrapper->aboutdata->IsValid(strdup(*Nan::Utf8String(info[0])));
  info.GetReturnValue().Set(Nan::New<v8::Boolean>(valid));
}

NAN_METHOD(AboutDataWrapper::SetDeviceName) {

  if(info.Length() < 1){
    return Nan::ThrowError("NAN_METHOD(AboutDataWrapper::SetDeviceName) SetDeviceName requires a device name string.");
  }

  AboutDataWrapper* wrapper = node::ObjectWrap::Unwrap<AboutDataWrapper>(info.This());

  QStatus status = wrapper->aboutdata->SetDeviceName(strdup(*Nan::Utf8String(info[0])));
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(AboutDataWrapper::SetAppId) {

  if(info.Length() < 1){
    return Nan::ThrowError("NAN_METHOD(AboutDataWrapper::SetAppId) SetAppId requires an App Id string which is either a 32-character hex digit string (i.e. 4a354637564945188a48323c158bc02d) or a UUID string as specified in RFC 4122 (i.e. 4a354637-5649-4518-8a48-323c158bc02d).");
  }

  AboutDataWrapper* wrapper = node::ObjectWrap::Unwrap<AboutDataWrapper>(info.This());

  QStatus status = wrapper->aboutdata->SetAppId(strdup(*Nan::Utf8String(info[0])));
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(AboutDataWrapper::SetDeviceId) {

  if(info.Length() < 1){
    return Nan::ThrowError("NAN_METHOD(AboutDataWrapper::SetDeviceId) SetDeviceId requires an Device Id string.");
  }

  AboutDataWrapper* wrapper = node::ObjectWrap::Unwrap<AboutDataWrapper>(info.This());

  QStatus status = wrapper->aboutdata->SetDeviceId(strdup(*Nan::Utf8String(info[0])));
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(AboutDataWrapper::SetAppName) {

  if(info.Length() < 1){
    return Nan::ThrowError("NAN_METHOD(AboutDataWrapper::SetAppName) SetAppName requires an app name string.");
  }

  AboutDataWrapper* wrapper = node::ObjectWrap::Unwrap<AboutDataWrapper>(info.This());

  QStatus status = wrapper->aboutdata->SetAppName(strdup(*Nan::Utf8String(info[0])));
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(AboutDataWrapper::SetManufacturer) {

  if(info.Length() < 1){
    return Nan::ThrowError("NAN_METHOD(AboutDataWrapper::SetManufacturer) SetManufacturer requires a manufacturer's name string.");
  }

  AboutDataWrapper* wrapper = node::ObjectWrap::Unwrap<AboutDataWrapper>(info.This());

  QStatus status = wrapper->aboutdata->SetManufacturer(strdup(*Nan::Utf8String(info[0])));
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(AboutDataWrapper::SetModelNumber) {

  if(info.Length() < 1){
    return Nan::ThrowError("NAN_METHOD(AboutDataWrapper::SetModelNumber) SetModelNumber requires an model number string.");
  }

  AboutDataWrapper* wrapper = node::ObjectWrap::Unwrap<AboutDataWrapper>(info.This());

  QStatus status = wrapper->aboutdata->SetModelNumber(strdup(*Nan::Utf8String(info[0])));
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(AboutDataWrapper::SetDescription) {

  if(info.Length() < 1){
    return Nan::ThrowError("NAN_METHOD(AboutDataWrapper::SetDescription) SetDescription requires a string description.");
  }

  AboutDataWrapper* wrapper = node::ObjectWrap::Unwrap<AboutDataWrapper>(info.This());

  QStatus status = wrapper->aboutdata->SetDescription(strdup(*Nan::Utf8String(info[0])));
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(AboutDataWrapper::SetDateOfManufacture) {

  if(info.Length() < 1){
    return Nan::ThrowError("NAN_METHOD(AboutDataWrapper::SetDateOfManufacture) SetDateOfManufacture requires the date of manufacture using the format YYYY-MM-DD. Known as XML DateTime format.");
  }

  AboutDataWrapper* wrapper = node::ObjectWrap::Unwrap<AboutDataWrapper>(info.This());

  QStatus status = wrapper->aboutdata->SetDateOfManufacture(strdup(*Nan::Utf8String(info[0])));
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(AboutDataWrapper::SetSoftwareVersion) {

  if(info.Length() < 1){
    return Nan::ThrowError("NAN_METHOD(AboutDataWrapper::SetSoftwareVersion) SetSoftwareVersion requires a string representing the hardware version.");
  }

  AboutDataWrapper* wrapper = node::ObjectWrap::Unwrap<AboutDataWrapper>(info.This());

  QStatus status = wrapper->aboutdata->SetSoftwareVersion(strdup(*Nan::Utf8String(info[0])));
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(AboutDataWrapper::SetHardwareVersion) {

  if(info.Length() < 1){
    return Nan::ThrowError("NAN_METHOD(AboutDataWrapper::SetHardwareVersion) SetHardwareVersion requires a string representing the hardware version.");
  }

  AboutDataWrapper* wrapper = node::ObjectWrap::Unwrap<AboutDataWrapper>(info.This());

  QStatus status = wrapper->aboutdata->SetHardwareVersion(strdup(*Nan::Utf8String(info[0])));
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(AboutDataWrapper::SetSupportUrl) {

  if(info.Length() < 1){
    return Nan::ThrowError("NAN_METHOD(AboutDataWrapper::SetSupportUrl) SetSupportUrl requires a string representing the support URL.");
  }

  AboutDataWrapper* wrapper = node::ObjectWrap::Unwrap<AboutDataWrapper>(info.This());

  QStatus status = wrapper->aboutdata->SetSupportUrl(strdup(*Nan::Utf8String(info[0])));
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}
