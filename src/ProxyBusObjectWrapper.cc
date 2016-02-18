#include "nan.h"

#include "InterfaceWrapper.h"
#include "util.h"
#include "BusConnection.h"
#include "ProxyBusObjectWrapper.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>


static Nan::Persistent<v8::FunctionTemplate> proxybusobject_constructor;

NAN_METHOD(ProxyBusObjectConstructor) {
  if(info.Length() < 4){
    return Nan::ThrowError("NAN_METHOD(ProxyBusObjectConstructor) ProxyBusObject requires busAttachment, busName, path and sessionId.");
  }

  v8::Local<v8::Object> obj;
  v8::Local<v8::FunctionTemplate> con = Nan::New<v8::FunctionTemplate>(proxybusobject_constructor);

  v8::Local<v8::Value> argv[] = {
    info[0],
    info[1],
    info[2],
    info[3]
  };
  obj = con->GetFunction()->NewInstance(4, argv);
  info.GetReturnValue().Set(obj);
}

ProxyBusObjectWrapper::ProxyBusObjectWrapper(ajn::BusAttachment* busAttachment, const char* busName, const char* path, ajn::SessionId sessionId)
  :proxyBusObject(new ajn::ProxyBusObject(*busAttachment, busName, path, sessionId)){
}

ProxyBusObjectWrapper::~ProxyBusObjectWrapper(){
}

void ProxyBusObjectWrapper::Init () {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(ProxyBusObjectWrapper::New);
  proxybusobject_constructor.Reset(tpl);
  tpl->SetClassName(Nan::New<v8::String>("ProxyBusObject").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "getInterfaceNames", ProxyBusObjectWrapper::GetInterfaceNames);
  Nan::SetPrototypeMethod(tpl, "getInterface", ProxyBusObjectWrapper::GetInterface);
  Nan::SetPrototypeMethod(tpl, "methodCall", ProxyBusObjectWrapper::MethodCall);
  Nan::SetPrototypeMethod(tpl, "secureConnectionAsync", ProxyBusObjectWrapper::SecureConnectionAsync);
  Nan::SetPrototypeMethod(tpl, "isSecure", ProxyBusObjectWrapper::IsSecure);
}

NAN_METHOD(ProxyBusObjectWrapper::New) {
  if(info.Length() < 4){
    return Nan::ThrowError("NAN_METHOD(ProxyBusObjectWrapper::New) ProxyBusObject requires a BusAttachment, a busName, a path and a Session ID.");
  }

  BusConnection* busWrapper = Nan::ObjectWrap::Unwrap<BusConnection>(info[0].As<v8::Object>());
  char* busName = strdup(*Nan::Utf8String(info[1]));
  char* path = strdup(*Nan::Utf8String(info[2]));

  ProxyBusObjectWrapper* obj = new ProxyBusObjectWrapper(busWrapper->bus, busName, path, info[3]->Int32Value());
  obj->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(ProxyBusObjectWrapper::GetInterfaceNames) {
  ProxyBusObjectWrapper* wrapper = Nan::ObjectWrap::Unwrap<ProxyBusObjectWrapper>(info.This());
  // QStatus status = wrapper->proxyBusObject->IntrospectRemoteObject();
  wrapper->proxyBusObject->IntrospectRemoteObject();

  size_t intf_num = wrapper->proxyBusObject->GetInterfaces();
  const ajn::InterfaceDescription** intfs = new const ajn::InterfaceDescription*[intf_num];
  wrapper->proxyBusObject->GetInterfaces(intfs, intf_num);
  v8::Local<v8::Array> interfaces = v8::Array::New(intf_num);
  for (size_t i = 0; i < intf_num; ++i) {
    Nan::Set(interfaces, i, Nan::New<v8::String>(std::string(intfs[i]->GetName())).ToLocalChecked());
  }
  info.GetReturnValue().Set(interfaces);
}

NAN_METHOD(ProxyBusObjectWrapper::GetInterface) {
  if (info.Length() == 0 || !info[0]->IsString())
    return Nan::ThrowError("GetInterface requires a name string argument");
  if (info.Length() == 1)
    return Nan::ThrowError("GetInterface requires a new InterfaceDescription argument");

  char* name = strdup(*Nan::Utf8String(info[0]));

  ajn::InterfaceDescription* interface = NULL;

  ProxyBusObjectWrapper* proxyBusObjectWrapper = Nan::ObjectWrap::Unwrap<ProxyBusObjectWrapper>(info.This());
  interface = const_cast<ajn::InterfaceDescription*>(proxyBusObjectWrapper->proxyBusObject->GetInterface(name));
  InterfaceWrapper* interfaceWrapper = Nan::ObjectWrap::Unwrap<InterfaceWrapper>(info[1].As<v8::Object>());
  interfaceWrapper->interface = interface;

  info.GetReturnValue().SetUndefined();
}

NAN_METHOD(ProxyBusObjectWrapper::MethodCall) {
  if(info.Length() < 5  || !info[3]->IsArray() || !info[4]->IsArray()){
    return Nan::ThrowError("NAN_METHOD(ProxyBusObjectWrapper::MethodCall) MethodCall requires a bus attachment, an interface name, a method name and an array of input arguments.");
  }

  BusConnection* busWrapper = Nan::ObjectWrap::Unwrap<BusConnection>(info[0].As<v8::Object>());
  char* interfaceName = strdup(*Nan::Utf8String(info[1]));
  char* methodName = strdup(*Nan::Utf8String(info[2]));
  v8::Local<v8::Array> v8InArguments = v8::Local<v8::Array>::Cast(info[3]);
  v8::Local<v8::Array> v8OutArguments = v8::Local<v8::Array>::Cast(info[4]);

  ajn::MsgArg* msgArgs = new ajn::MsgArg[v8InArguments->Length()];
  for (size_t i = 0; i < v8InArguments->Length(); ++i) {
    v8::Local<v8::Object> v8InArgument = v8::Local<v8::Object>::Cast(v8InArguments->Get(i));
    v8::Local<v8::Value> argValue = v8InArgument->Get(Nan::New<v8::String>("value").ToLocalChecked());
    char* argSignature = strdup(*Nan::Utf8String(v8InArgument->Get(Nan::New<v8::String>("signature").ToLocalChecked())));
    printf("argSignature=%s\n", argSignature);

    ajn::MsgArg* ajnArg = new ajn::MsgArg(argSignature);
    switch(ajnArg->typeId){
      case ajn::ALLJOYN_STRING:
      //ajnArg, Nan::New<v8::String>("s").ToLocalChecked(), Nan::To<v8::String>(argValue).ToLocalChecked());
      ajnArg->Set("s", strdup(*v8::String::Utf8Value(argValue)));
      break;
      case ajn::ALLJOYN_UINT32:
      //Nan::Set(ajnArg, Nan::New<v8::String>("u").ToLocalChecked(), Nan::New<v8::Integer>(argValue->Uint32Value()));
      ajnArg->Set("u", argValue->Uint32Value());
      break;
      default:
      break;
    }
    msgArgs[i].Set("*", ajnArg);
  }

  ajn::Message replyMsg(*busWrapper->bus);

  ProxyBusObjectWrapper* proxyBusObjectWrapper = Nan::ObjectWrap::Unwrap<ProxyBusObjectWrapper>(info.This());
  QStatus status = proxyBusObjectWrapper->proxyBusObject->MethodCall(interfaceName, methodName, msgArgs, v8InArguments->Length(), replyMsg);

  size_t numArgs;
  const ajn::MsgArg* replyArgs;

  if (status == ER_OK) {
    replyMsg->GetArgs(numArgs, replyArgs);
    if (numArgs == 0) {
      info.GetReturnValue().SetUndefined();
    } else {
      v8::Local<v8::Object> v8ReplyObj = Nan::New<v8::Object>();
      for (size_t i = 0; i < numArgs; i++) {
        // TODO: should return an array
        v8::Local<v8::Object> v8OutArgument = v8::Local<v8::Object>::Cast(v8OutArguments->Get(i));
        v8::Local<v8::Value> outArgName = v8OutArgument->Get(Nan::New<v8::String>("name").ToLocalChecked());

        // Nan::Set(v8ReplyObj,outArgName, Nan::New<v8::String>(std::string("foo")));
        switch(replyArgs[i].typeId){
          case ajn::ALLJOYN_STRING:
          Nan::Set(v8ReplyObj, outArgName, Nan::New<v8::String>(replyArgs[i].v_string.str).ToLocalChecked());
          break;
          case ajn::ALLJOYN_UINT32:
          Nan::Set(v8ReplyObj, outArgName, Nan::New<v8::Integer>(replyArgs[i].v_uint32));
          break;
          default:
          break;
        }

      }
      info.GetReturnValue().Set(v8ReplyObj);
    }
  } else {
    info.GetReturnValue().Set(Nan::New<v8::String>(std::string(QCC_StatusText(status))).ToLocalChecked());
  }
}

NAN_METHOD(ProxyBusObjectWrapper::SecureConnectionAsync) {
  ProxyBusObjectWrapper* wrapper = Nan::ObjectWrap::Unwrap<ProxyBusObjectWrapper>(info.This());
  QStatus status = wrapper->proxyBusObject->SecureConnectionAsync(info[0]->BooleanValue());
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(ProxyBusObjectWrapper::IsSecure) {
  ProxyBusObjectWrapper* wrapper = Nan::ObjectWrap::Unwrap<ProxyBusObjectWrapper>(info.This());
  bool isSecure = wrapper->proxyBusObject->IsSecure();
  info.GetReturnValue().Set(Nan::New<v8::Boolean>(isSecure));
}
