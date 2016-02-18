#include "nan.h"

#include "BusConnection.h"
#include "AboutProxyWrapper.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

static Nan::Persistent<v8::FunctionTemplate> aboutproxy_constructor;

NAN_METHOD(AboutProxyConstructor) {
  if(info.Length() < 2){
    return Nan::ThrowError("NAN_METHOD(AboutProxyConstructor) AboutProxy requires busAttachment and busName. sessionId is optional.");
  }

  v8::Local<v8::Object> obj;
  v8::Local<v8::FunctionTemplate> con = Nan::New<v8::FunctionTemplate>(aboutproxy_constructor);

  v8::Local<v8::Value> argv[] = {
    info[0],
    info[1],
    info[2]
  };
  obj = con->GetFunction()->NewInstance(3, argv);
  info.GetReturnValue().Set(obj);
}

AboutProxyWrapper::AboutProxyWrapper(ajn::BusAttachment* busAttachment, const char* busName, ajn::SessionId sessionId)
  :proxy(new ajn::AboutProxy(*busAttachment, busName, sessionId)){
}

AboutProxyWrapper::~AboutProxyWrapper(){
}

void AboutProxyWrapper::Init () {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(AboutProxyWrapper::New);
  aboutproxy_constructor.Reset(tpl);
  tpl->SetClassName(Nan::New<v8::String>("AboutProxy").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "getObjectDescription", AboutProxyWrapper::GetObjectDescription);
  Nan::SetPrototypeMethod(tpl, "getAboutData", AboutProxyWrapper::GetAboutData);
  Nan::SetPrototypeMethod(tpl, "getSessionId", AboutProxyWrapper::GetSessionId);
  Nan::SetPrototypeMethod(tpl, "getUniqueName", AboutProxyWrapper::GetUniqueName);
  Nan::SetPrototypeMethod(tpl, "getVersion", AboutProxyWrapper::GetVersion);
}

NAN_METHOD(AboutProxyWrapper::New) {
  if(info.Length() < 2){
    return Nan::ThrowError("NAN_METHOD(AboutProxyWrapper::New) AboutProxy requires a BusAttachment and a busName. Session ID is optional.");
  }

  BusConnection* busWrapper = Nan::ObjectWrap::Unwrap<BusConnection>(info[0].As<v8::Object>());

  char* busName = strdup(*Nan::Utf8String(info[1]));

  AboutProxyWrapper* obj = new AboutProxyWrapper(busWrapper->bus, busName, info[2]->Int32Value());
  obj->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(AboutProxyWrapper::GetSessionId) {
  AboutProxyWrapper* wrapper = Nan::ObjectWrap::Unwrap<AboutProxyWrapper>(info.This());
  ajn::SessionId sessionId = wrapper->proxy->GetSessionId();
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(sessionId)));
}

NAN_METHOD(AboutProxyWrapper::GetUniqueName) {
  AboutProxyWrapper* wrapper = Nan::ObjectWrap::Unwrap<AboutProxyWrapper>(info.This());
  qcc::String uniqueName = wrapper->proxy->GetUniqueName();
  info.GetReturnValue().Set(Nan::New<v8::String>(uniqueName.c_str()).ToLocalChecked());
}

NAN_METHOD(AboutProxyWrapper::GetVersion) {
  AboutProxyWrapper* wrapper = Nan::ObjectWrap::Unwrap<AboutProxyWrapper>(info.This());

  uint16_t ver;
  QStatus status = wrapper->proxy->GetVersion(ver);
  if (status == ER_OK)
    info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(ver)));
  else
    info.GetReturnValue().Set(Nan::New<v8::String>(std::string(QCC_StatusText(status))).ToLocalChecked());
}

NAN_METHOD(AboutProxyWrapper::GetObjectDescription) {
  AboutProxyWrapper* wrapper = Nan::ObjectWrap::Unwrap<AboutProxyWrapper>(info.This());
  ajn::MsgArg objectDescriptionArg;
  QStatus status = wrapper->proxy->GetObjectDescription(objectDescriptionArg);

  // this objectDescription will head back to Node.js after we
  // populate it using the AllJoyn helper methods
  v8::Local<v8::Object> objectDescription = Nan::New<v8::Object>();

  // use the AllJoyn helper methods to get the data out of
  // the AllJoyn ObjectDescription and into the Node.js object
  ajn::AboutObjectDescription ajnObjectDescription;
  ajnObjectDescription.CreateFromMsgArg(objectDescriptionArg);
  ajn::AboutObjectDescription aod(objectDescriptionArg);
  size_t path_num = aod.GetPaths(NULL, 0);
  const char** paths = new const char*[path_num];
  aod.GetPaths(paths, path_num);
  for (size_t i = 0; i < path_num; ++i) {
    size_t intf_num = aod.GetInterfaces(paths[i], NULL, 0);
    const char** intfs = new const char*[intf_num];
    aod.GetInterfaces(paths[i], intfs, intf_num);
    v8::Local<v8::Array> interfaceNames = v8::Array::New(intf_num);
    for (size_t j = 0; j < intf_num; ++j) {
      Nan::Set(interfaceNames, j, Nan::New<v8::String>(intfs[j]).ToLocalChecked());
    }
    Nan::Set(objectDescription, Nan::New<v8::String>(paths[i]).ToLocalChecked(), interfaceNames);
    delete [] intfs;
  }

  if (status == ER_OK)
    info.GetReturnValue().Set(objectDescription);
  else
    info.GetReturnValue().Set(Nan::New<v8::String>(std::string(QCC_StatusText(status))).ToLocalChecked());
}


NAN_METHOD(AboutProxyWrapper::GetAboutData) {
  if(info.Length() < 1){
    return Nan::ThrowError("NAN_METHOD(AboutProxyWrapper::GetAboutData) GetAboutData requires a language tag.");
  }

  char* languageTag = strdup(*Nan::Utf8String(info[0]));

  AboutProxyWrapper* wrapper = Nan::ObjectWrap::Unwrap<AboutProxyWrapper>(info.This());
  ajn::MsgArg aboutDataArg;
  QStatus status = wrapper->proxy->GetAboutData(languageTag, aboutDataArg);

  // this aboutData object will head back to Node.js after we
  // populate it using the AllJoyn helper methods
  // const ajn::MsgArg* aboutDataArgIn = holder->aboutDataArg;
  // msgArgToObject(aboutDataArgIn, 0, aboutDataArgOut);
  v8::Local<v8::Object> aboutData = Nan::New<v8::Object>();
  ajn::AboutData ajnAboutData(aboutDataArg);
  size_t count = ajnAboutData.GetFields();
  const char** fields = new const char*[count];
  ajnAboutData.GetFields(fields, count);

  for (size_t i = 0; i < count; ++i) {
    ajn::MsgArg* tmp;
    ajnAboutData.GetField(fields[i], tmp, NULL);
    if (tmp->Signature() == "s") {
      const char* tmp_s;
      tmp->Get("s", &tmp_s);
      Nan::Set(aboutData, Nan::New<v8::String>(fields[i]).ToLocalChecked(), Nan::New<v8::String>(tmp_s).ToLocalChecked());
    } else if (tmp->Signature() == "ay") {
      size_t lay;
      uint8_t* pay;
      tmp->Get("ay", &lay, &pay);
      v8::Local<v8::Array> v8_pay = v8::Array::New(lay);
      for (size_t j = 0; j < lay; ++j) {
        Nan::Set(v8_pay, j, Nan::New<v8::Integer>(pay[j]));
      }
      Nan::Set(aboutData, Nan::New<v8::String>(fields[i]).ToLocalChecked(), v8_pay);
    } else if (tmp->Signature() == "as") {
      // TODO: need to write a test to hit this block
      // it is only called after a session is created
      size_t las;
      ajn::MsgArg* as_arg;
      tmp->Get("as", &las, &as_arg);
      v8::Local<v8::Array> v8_las = v8::Array::New(las);
      for (size_t j = 0; j < las; ++j) {
        const char* tmp_s;
        as_arg[j].Get("s", &tmp_s);
        Nan::Set(v8_las, j, Nan::New<v8::String>(tmp_s).ToLocalChecked());
      }
      Nan::Set(aboutData, Nan::New<v8::String>(fields[i]).ToLocalChecked(), v8_las);
    }
  }
  delete [] fields;

  if (status == ER_OK)
    info.GetReturnValue().Set(aboutData);
  else
    info.GetReturnValue().Set(Nan::New<v8::String>(std::string(QCC_StatusText(status))).ToLocalChecked());
}
