#include "nan.h"

#include "BusConnection.h"
#include "AboutDataWrapper.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

static v8::Persistent<v8::FunctionTemplate> aboutdata_constructor;

v8::Handle<v8::Value> AboutDataWrapper::NewInstance() {
    NanScope();

    v8::Local<v8::Object> obj;
    v8::Local<v8::FunctionTemplate> con = NanNew<v8::FunctionTemplate>(aboutdata_constructor);
    obj = con->GetFunction()->NewInstance(0, NULL);
    return obj;
}

NAN_METHOD(AboutDataConstructor) {
  NanScope();
  
  if(args.Length() < 1){
    return NanThrowError("NAN_METHOD(AboutDataConstructor) AboutData language tag string");
  }
  
  v8::Local<v8::Object> obj;
  v8::Local<v8::FunctionTemplate> con = NanNew<v8::FunctionTemplate>(aboutdata_constructor);

  v8::Handle<v8::Value> argv[] = {
    args[0]
  };
  obj = con->GetFunction()->NewInstance(1, argv);
  NanReturnValue(obj);
}

AboutDataWrapper::AboutDataWrapper(const char* languageTag)
  :aboutdata(new ajn::AboutData(languageTag)){
}

AboutDataWrapper::~AboutDataWrapper(){
}

void AboutDataWrapper::Init () {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(AboutDataWrapper::New);
  NanAssignPersistent(aboutdata_constructor, tpl);
  tpl->SetClassName(NanNew<v8::String>("AboutData"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(tpl, "isValid", AboutDataWrapper::IsValid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setAppId", AboutDataWrapper::SetAppId);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setDeviceName", AboutDataWrapper::SetDeviceName);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setDeviceId", AboutDataWrapper::SetDeviceId);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setAppName", AboutDataWrapper::SetAppName);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setManufacturer", AboutDataWrapper::SetManufacturer);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setModelNumber", AboutDataWrapper::SetModelNumber);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setDescription", AboutDataWrapper::SetDescription);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setDateOfManufacture", AboutDataWrapper::SetDateOfManufacture);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setSoftwareVersion", AboutDataWrapper::SetSoftwareVersion);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setHardwareVersion", AboutDataWrapper::SetHardwareVersion);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setSupportUrl", AboutDataWrapper::SetSupportUrl);

}

NAN_METHOD(AboutDataWrapper::New) {
  NanScope();
  if(args.Length() < 1){
    return NanThrowError("NAN_METHOD(AboutDataWrapper::New) AboutData requires a language tag string.");
  }

  char* languageTag = strdup(*NanUtf8String(args[0]));
  
  AboutDataWrapper* obj = new AboutDataWrapper(languageTag);
  obj->Wrap(args.This());

  NanReturnValue(args.This());
}

NAN_METHOD(AboutDataWrapper::IsValid) {
  NanScope();

  AboutDataWrapper* wrapper = node::ObjectWrap::Unwrap<AboutDataWrapper>(args.This());
  
  bool valid = wrapper->aboutdata->IsValid(strdup(*NanUtf8String(args[0])));
  NanReturnValue(NanNew<v8::Boolean>(valid));
}

NAN_METHOD(AboutDataWrapper::SetDeviceName) {
  NanScope();
  if(args.Length() < 1){
    return NanThrowError("NAN_METHOD(AboutDataWrapper::SetDeviceName) SetDeviceName requires a device name string.");
  }
  
  AboutDataWrapper* wrapper = node::ObjectWrap::Unwrap<AboutDataWrapper>(args.This());
  
  QStatus status = wrapper->aboutdata->SetDeviceName(strdup(*NanUtf8String(args[0])));
  NanReturnValue(NanNew<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(AboutDataWrapper::SetAppId) {
  NanScope();
  if(args.Length() < 1){
    return NanThrowError("NAN_METHOD(AboutDataWrapper::SetAppId) SetAppId requires an App Id string which is either a 32-character hex digit string (i.e. 4a354637564945188a48323c158bc02d) or a UUID string as specified in RFC 4122 (i.e. 4a354637-5649-4518-8a48-323c158bc02d).");
  }
  
  AboutDataWrapper* wrapper = node::ObjectWrap::Unwrap<AboutDataWrapper>(args.This());
  
  QStatus status = wrapper->aboutdata->SetAppId(strdup(*NanUtf8String(args[0])));
  NanReturnValue(NanNew<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(AboutDataWrapper::SetDeviceId) {
  NanScope();
  if(args.Length() < 1){
    return NanThrowError("NAN_METHOD(AboutDataWrapper::SetDeviceId) SetDeviceId requires an Device Id string.");
  }
  
  AboutDataWrapper* wrapper = node::ObjectWrap::Unwrap<AboutDataWrapper>(args.This());
  
  QStatus status = wrapper->aboutdata->SetDeviceId(strdup(*NanUtf8String(args[0])));
  NanReturnValue(NanNew<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(AboutDataWrapper::SetAppName) {
  NanScope();
  if(args.Length() < 1){
    return NanThrowError("NAN_METHOD(AboutDataWrapper::SetAppName) SetAppName requires an app name string.");
  }
  
  AboutDataWrapper* wrapper = node::ObjectWrap::Unwrap<AboutDataWrapper>(args.This());
  
  QStatus status = wrapper->aboutdata->SetAppName(strdup(*NanUtf8String(args[0])));
  NanReturnValue(NanNew<v8::Integer>(static_cast<int>(status)));
}
  
NAN_METHOD(AboutDataWrapper::SetManufacturer) {
  NanScope();
  if(args.Length() < 1){
    return NanThrowError("NAN_METHOD(AboutDataWrapper::SetManufacturer) SetManufacturer requires a manufacturer's name string.");
  }
  
  AboutDataWrapper* wrapper = node::ObjectWrap::Unwrap<AboutDataWrapper>(args.This());
  
  QStatus status = wrapper->aboutdata->SetManufacturer(strdup(*NanUtf8String(args[0])));
  NanReturnValue(NanNew<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(AboutDataWrapper::SetModelNumber) {
  NanScope();
  if(args.Length() < 1){
    return NanThrowError("NAN_METHOD(AboutDataWrapper::SetModelNumber) SetModelNumber requires an model number string.");
  }
  
  AboutDataWrapper* wrapper = node::ObjectWrap::Unwrap<AboutDataWrapper>(args.This());
  
  QStatus status = wrapper->aboutdata->SetModelNumber(strdup(*NanUtf8String(args[0])));
  NanReturnValue(NanNew<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(AboutDataWrapper::SetDescription) {
  NanScope();
  if(args.Length() < 1){
    return NanThrowError("NAN_METHOD(AboutDataWrapper::SetDescription) SetDescription requires a string description.");
  }
  
  AboutDataWrapper* wrapper = node::ObjectWrap::Unwrap<AboutDataWrapper>(args.This());
  
  QStatus status = wrapper->aboutdata->SetDescription(strdup(*NanUtf8String(args[0])));
  NanReturnValue(NanNew<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(AboutDataWrapper::SetDateOfManufacture) {
  NanScope();
  if(args.Length() < 1){
    return NanThrowError("NAN_METHOD(AboutDataWrapper::SetDateOfManufacture) SetDateOfManufacture requires the date of manufacture using the format YYYY-MM-DD. Known as XML DateTime format.");
  }
  
  AboutDataWrapper* wrapper = node::ObjectWrap::Unwrap<AboutDataWrapper>(args.This());
  
  QStatus status = wrapper->aboutdata->SetDateOfManufacture(strdup(*NanUtf8String(args[0])));
  NanReturnValue(NanNew<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(AboutDataWrapper::SetSoftwareVersion) {
  NanScope();
  if(args.Length() < 1){
    return NanThrowError("NAN_METHOD(AboutDataWrapper::SetSoftwareVersion) SetSoftwareVersion requires a string representing the hardware version.");
  }
  
  AboutDataWrapper* wrapper = node::ObjectWrap::Unwrap<AboutDataWrapper>(args.This());
  
  QStatus status = wrapper->aboutdata->SetSoftwareVersion(strdup(*NanUtf8String(args[0])));
  NanReturnValue(NanNew<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(AboutDataWrapper::SetHardwareVersion) {
  NanScope();
  if(args.Length() < 1){
    return NanThrowError("NAN_METHOD(AboutDataWrapper::SetHardwareVersion) SetHardwareVersion requires a string representing the hardware version.");
  }
  
  AboutDataWrapper* wrapper = node::ObjectWrap::Unwrap<AboutDataWrapper>(args.This());
  
  QStatus status = wrapper->aboutdata->SetHardwareVersion(strdup(*NanUtf8String(args[0])));
  NanReturnValue(NanNew<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(AboutDataWrapper::SetSupportUrl) {
  NanScope();
  if(args.Length() < 1){
    return NanThrowError("NAN_METHOD(AboutDataWrapper::SetSupportUrl) SetSupportUrl requires a string representing the support URL.");
  }
  
  AboutDataWrapper* wrapper = node::ObjectWrap::Unwrap<AboutDataWrapper>(args.This());
  
  QStatus status = wrapper->aboutdata->SetSupportUrl(strdup(*NanUtf8String(args[0])));
  NanReturnValue(NanNew<v8::Integer>(static_cast<int>(status)));
}