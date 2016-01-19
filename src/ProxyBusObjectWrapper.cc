#include "nan.h"

#include "InterfaceWrapper.h"
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

  NODE_SET_PROTOTYPE_METHOD(tpl, "getInterfaces", ProxyBusObjectWrapper::GetInterfaces);
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

NAN_METHOD(ProxyBusObjectWrapper::GetInterfaces) {
  NanScope();
  ProxyBusObjectWrapper* wrapper = node::ObjectWrap::Unwrap<ProxyBusObjectWrapper>(args.This());
  // QStatus status = wrapper->proxyBusObject->IntrospectRemoteObject();
  wrapper->proxyBusObject->IntrospectRemoteObject();

  size_t intf_num = wrapper->proxyBusObject->GetInterfaces();
  const ajn::InterfaceDescription** intfs = new const ajn::InterfaceDescription*[intf_num];
  wrapper->proxyBusObject->GetInterfaces(intfs, intf_num);
  v8::Local<v8::Array> interfaces = v8::Array::New(intf_num);
  for (size_t i = 0; i < intf_num; ++i) {
    interfaces->Set(i, InterfaceWrapper::NewInstance());
  }
  NanReturnValue(interfaces);
}
