#include "nan.h"

#include "InterfaceWrapper.h"
#include "util.h"
#include "BusConnection.h"
#include "ProxyBusObjectWrapper.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>


static v8::Persistent<v8::FunctionTemplate> proxybusobject_constructor;

v8::Handle<v8::Value> ProxyBusObjectWrapper::NewInstance() {
    NanScope();

    v8::Local<v8::Object> obj;
    v8::Local<v8::FunctionTemplate> con = NanNew<v8::FunctionTemplate>(proxybusobject_constructor);
    obj = con->GetFunction()->NewInstance(0, NULL);
    return obj;
}

NAN_METHOD(ProxyBusObjectConstructor) {
  NanScope();
  
  if(args.Length() < 4){
    return NanThrowError("NAN_METHOD(ProxyBusObjectConstructor) ProxyBusObject requires busAttachment, busName, path and sessionId.");
  }
  
  v8::Local<v8::Object> obj;
  v8::Local<v8::FunctionTemplate> con = NanNew<v8::FunctionTemplate>(proxybusobject_constructor);

  v8::Handle<v8::Value> argv[] = {
    args[0],
    args[1],
    args[2],
    args[3]
  };
  obj = con->GetFunction()->NewInstance(4, argv);
  NanReturnValue(obj);
}

ProxyBusObjectWrapper::ProxyBusObjectWrapper(ajn::BusAttachment* busAttachment, const char* busName, const char* path, ajn::SessionId sessionId)
  :proxyBusObject(new ajn::ProxyBusObject(*busAttachment, busName, path, sessionId)){
}

ProxyBusObjectWrapper::~ProxyBusObjectWrapper(){
}

void ProxyBusObjectWrapper::Init () {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(ProxyBusObjectWrapper::New);
  NanAssignPersistent(proxybusobject_constructor, tpl);
  tpl->SetClassName(NanNew<v8::String>("ProxyBusObject"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(tpl, "getInterfaceNames", ProxyBusObjectWrapper::GetInterfaceNames);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getInterface", ProxyBusObjectWrapper::GetInterface);
  NODE_SET_PROTOTYPE_METHOD(tpl, "methodCall", ProxyBusObjectWrapper::MethodCall);
}

NAN_METHOD(ProxyBusObjectWrapper::New) {
  NanScope();
  if(args.Length() < 4){
    return NanThrowError("NAN_METHOD(ProxyBusObjectWrapper::New) ProxyBusObject requires a BusAttachment, a busName, a path and a Session ID.");
  }

  BusConnection* busWrapper = node::ObjectWrap::Unwrap<BusConnection>(args[0].As<v8::Object>());
  char* busName = strdup(*NanUtf8String(args[1]));
  char* path = strdup(*NanUtf8String(args[2]));
  
  ProxyBusObjectWrapper* obj = new ProxyBusObjectWrapper(busWrapper->bus, busName, path, args[3]->Int32Value());
  obj->Wrap(args.This());

  NanReturnValue(args.This());
}

NAN_METHOD(ProxyBusObjectWrapper::GetInterfaceNames) {
  NanScope();
  ProxyBusObjectWrapper* wrapper = node::ObjectWrap::Unwrap<ProxyBusObjectWrapper>(args.This());
  // QStatus status = wrapper->proxyBusObject->IntrospectRemoteObject();
  wrapper->proxyBusObject->IntrospectRemoteObject();

  size_t intf_num = wrapper->proxyBusObject->GetInterfaces();
  const ajn::InterfaceDescription** intfs = new const ajn::InterfaceDescription*[intf_num];
  wrapper->proxyBusObject->GetInterfaces(intfs, intf_num);
  v8::Local<v8::Array> interfaces = v8::Array::New(intf_num);
  for (size_t i = 0; i < intf_num; ++i) {
    interfaces->Set(i, NanNew<v8::String>(std::string(intfs[i]->GetName())));
  }
  NanReturnValue(interfaces);
}

NAN_METHOD(ProxyBusObjectWrapper::GetInterface) {
  NanScope();
  if (args.Length() == 0 || !args[0]->IsString())
    return NanThrowError("GetInterface requires a name string argument");
  if (args.Length() == 1)
    return NanThrowError("GetInterface requires a new InterfaceDescription argument");
  
  char* name = strdup(*NanUtf8String(args[0]));

  ajn::InterfaceDescription* interface = NULL;

  ProxyBusObjectWrapper* proxyBusObjectWrapper = node::ObjectWrap::Unwrap<ProxyBusObjectWrapper>(args.This());
  interface = const_cast<ajn::InterfaceDescription*>(proxyBusObjectWrapper->proxyBusObject->GetInterface(name));
  InterfaceWrapper* interfaceWrapper = node::ObjectWrap::Unwrap<InterfaceWrapper>(args[1].As<v8::Object>());
  interfaceWrapper->interface = interface;

  NanReturnUndefined();
}

NAN_METHOD(ProxyBusObjectWrapper::MethodCall) {
  NanScope();
  if(args.Length() < 5  || !args[3]->IsArray() || !args[4]->IsArray()){
    return NanThrowError("NAN_METHOD(ProxyBusObjectWrapper::MethodCall) MethodCall requires a bus attachment, an interface name, a method name and an array of input arguments.");
  }
  
  BusConnection* busWrapper = node::ObjectWrap::Unwrap<BusConnection>(args[0].As<v8::Object>());
  char* interfaceName = strdup(*NanUtf8String(args[1]));
  char* methodName = strdup(*NanUtf8String(args[2]));
  v8::Local<v8::Array> v8InArguments = v8::Local<v8::Array>::Cast(args[3]);
  v8::Local<v8::Array> v8OutArguments = v8::Local<v8::Array>::Cast(args[4]);

  ajn::MsgArg* msgArgs = new ajn::MsgArg[v8InArguments->Length()];
  for (size_t i = 0; i < v8InArguments->Length(); ++i) {
    v8::Local<v8::Object> v8InArgument = v8::Local<v8::Object>::Cast(v8InArguments->Get(i));
    v8::Local<v8::Value> argValue = v8InArgument->Get(NanNew<v8::String>("value"));
    char* argSignature = strdup(*NanUtf8String(v8InArgument->Get(NanNew<v8::String>("signature"))));

    ajn::MsgArg* ajnArg = new ajn::MsgArg(argSignature);
    switch(ajnArg->typeId){
      case ajn::ALLJOYN_STRING:
      ajnArg->Set("s", strdup(*NanUtf8String(argValue)));
      break;
      case ajn::ALLJOYN_UINT32:
      ajnArg->Set("u", argValue->Uint32Value());
      break;
      default:
      break;
    }
    msgArgs[i].Set("*", ajnArg);
  }

  ajn::Message replyMsg(*busWrapper->bus);

  ProxyBusObjectWrapper* proxyBusObjectWrapper = node::ObjectWrap::Unwrap<ProxyBusObjectWrapper>(args.This());
  QStatus status = proxyBusObjectWrapper->proxyBusObject->MethodCall(interfaceName, methodName, msgArgs, v8InArguments->Length(), replyMsg);

  size_t numArgs;
  const ajn::MsgArg* replyArgs;

  if (status == ER_OK) {
    replyMsg->GetArgs(numArgs, replyArgs);
    if (numArgs == 0) {
      NanReturnUndefined();
    } else {
      v8::Local<v8::Object> v8ReplyObj = v8::Object::New();
      for (size_t i = 0; i < numArgs; i++) {
        // TODO: should return an array
        v8::Local<v8::Object> v8OutArgument = v8::Local<v8::Object>::Cast(v8OutArguments->Get(i));
        v8::Local<v8::Value> outArgName = v8OutArgument->Get(NanNew<v8::String>("name"));
        
        // v8ReplyObj->Set(outArgName, NanNew<v8::String>(std::string("foo")));
        switch(replyArgs[i].typeId){
          case ajn::ALLJOYN_STRING:
          v8ReplyObj->Set(outArgName, NanNew<v8::String>(replyArgs[i].v_string.str));
          break;
          case ajn::ALLJOYN_UINT32:
          v8ReplyObj->Set(outArgName, NanNew<v8::Integer>(replyArgs[i].v_uint32));
          break;
          default:
          break;
        }
        
      }
      NanReturnValue(v8ReplyObj);
    }
  } else {
    NanReturnValue(NanNew<v8::String>(std::string(QCC_StatusText(status))));
  }
}
