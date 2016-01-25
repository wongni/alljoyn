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
  
  char* name = *NanUtf8String(args[0]);
  ajn::InterfaceDescription* interface = NULL;

  ProxyBusObjectWrapper* proxyBusObjectWrapper = node::ObjectWrap::Unwrap<ProxyBusObjectWrapper>(args.This());
  interface = const_cast<ajn::InterfaceDescription*>(proxyBusObjectWrapper->proxyBusObject->GetInterface(name));
  InterfaceWrapper* interfaceWrapper = node::ObjectWrap::Unwrap<InterfaceWrapper>(args[1].As<v8::Object>());
  interfaceWrapper->interface = interface;

  NanReturnUndefined();
}

NAN_METHOD(ProxyBusObjectWrapper::MethodCall) {
  NanScope();
  if(args.Length() < 4  || !args[3]->IsArray()){
    return NanThrowError("NAN_METHOD(ProxyBusObjectWrapper::MethodCall) MethodCall requires a bus attachment, an interface name, a method name and an array of input arguments.");
  }
  
  printf("interfaceName: %s  methodName: %s\n", strdup(*NanUtf8String(args[1])), strdup(*NanUtf8String(args[2])));

  BusConnection* busWrapper = node::ObjectWrap::Unwrap<BusConnection>(args[0].As<v8::Object>());
  char* interfaceName = strdup(*NanUtf8String(args[1]));
  char* methodName = strdup(*NanUtf8String(args[2]));
  v8::Local<v8::Array> v8Arguments = v8::Local<v8::Array>::Cast(args[3]);
  printf("v8Arguments->Length(): %d\n", v8Arguments->Length());
  char* strArg = strdup(*NanUtf8String(v8Arguments->Get(1)));
  printf("v8Arguments->Get(1): %s\n", strArg);

  ajn::MsgArg* msgArgs = new ajn::MsgArg[v8Arguments->Length()];
  // // ajn::MsgArg* msgArgs = new ajn::MsgArg[1];
  // // ajn::MsgArg* msgArgs = new ajn::MsgArg[2];
  ajn::MsgArg* firstArg = new ajn::MsgArg("u", v8Arguments->Get(0)->Uint32Value());
  // // ajn::MsgArg* firstArg = new ajn::MsgArg("u", args[3]->Uint32Value());
  msgArgs->Set("*", firstArg);
  // ajn::MsgArg* secondArg = new ajn::MsgArg("s", strArg);
  // msgArgs->Set("*", secondArg);
  //
  // // ajn::MsgArg* msgArgs = new ajn::MsgArg("u", args[3]->Uint32Value());
  ajn::Message replyMsg(*busWrapper->bus);
  //
  // // TODO: repalce hardcoded 1 with length of array
  ProxyBusObjectWrapper* proxyBusObjectWrapper = node::ObjectWrap::Unwrap<ProxyBusObjectWrapper>(args.This());
  QStatus status = proxyBusObjectWrapper->proxyBusObject->MethodCall(interfaceName, methodName, msgArgs, v8Arguments->Length(), replyMsg);

  // // uint32_t branchNumber = msg->GetArg(0)->v_uint32;
  // static uint32_t u;
  // replyMsg->GetArg(0)->Get("u", &u);
  // static char* strReply;
  // replyMsg->GetArgs("s", &strReply);
  // replyMsg->GetArg(0)->Get("s", &strReply);
  
  // printf("strReply: %s", strReply);
  // printf("strReply: %s", replyMsg->GetArg(0)->v_string.str);
  size_t numArgs;
  const ajn::MsgArg* replyArgs;
  replyMsg->GetArgs(numArgs, replyArgs);
  printf("numArgs: %zd\n", numArgs);
  printf("replyArgs[0].ToString(): %s\n", replyArgs[0].ToString().c_str());
  if (status == ER_OK)
    NanReturnValue(NanNew<v8::Integer>(static_cast<int>(replyArgs[0].v_uint32)));
    // NanReturnValue(NanNew<v8::String>(strReply));
    // NanReturnUndefined();
  else
    NanReturnValue(NanNew<v8::String>(std::string(QCC_StatusText(status))));

}
