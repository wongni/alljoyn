#include "nan.h"

#include "BusConnection.h"
#include "AboutProxyWrapper.h"
#include "AboutProxyImpl.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

static v8::Persistent<v8::FunctionTemplate> aboutproxy_constructor;

v8::Handle<v8::Value> AboutProxyWrapper::NewInstance() {
    NanScope();

    v8::Local<v8::Object> obj;
    v8::Local<v8::FunctionTemplate> con = NanNew<v8::FunctionTemplate>(aboutproxy_constructor);
    obj = con->GetFunction()->NewInstance(0, NULL);
    return obj;
}

NAN_METHOD(AboutProxyConstructor) {
  NanScope();
  
  if(args.Length() < 1){
    return NanThrowError("NAN_METHOD(AboutProxyConstructor) AboutProxy requires busAttachment, busName and sessionId.");
  }
  
  v8::Local<v8::Object> obj;
  v8::Local<v8::FunctionTemplate> con = NanNew<v8::FunctionTemplate>(aboutproxy_constructor);

  v8::Handle<v8::Value> argv[] = {
    args[0],
    args[1],
    args[2]
  };
  obj = con->GetFunction()->NewInstance(3, argv);
  NanReturnValue(obj);
}

AboutProxyWrapper::AboutProxyWrapper(ajn::BusAttachment* busAttachment, const char* busName, ajn::SessionId sessionId)
  :proxy(new AboutProxyImpl(busAttachment, busName, sessionId)){
}

AboutProxyWrapper::~AboutProxyWrapper(){
}

void AboutProxyWrapper::Init () {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(AboutProxyWrapper::New);
  NanAssignPersistent(aboutproxy_constructor, tpl);
  tpl->SetClassName(NanNew<v8::String>("AboutProxy"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(tpl, "getObjectDescription", AboutProxyWrapper::GetObjectDescription);

  // NODE_SET_PROTOTYPE_METHOD(tpl, "getAboutData", AboutProxyWrapper::GetAboutData);
  // NODE_SET_PROTOTYPE_METHOD(tpl, "getVersion", AboutProxyWrapper::GetVersion);
}

NAN_METHOD(AboutProxyWrapper::New) {
  NanScope();
  if(args.Length() < 2){
    return NanThrowError("NAN_METHOD(AboutProxyWrapper::New) AboutProxy requires a BusAttachment and a busName. Session ID is optional.");
  }

  BusConnection* busWrapper = node::ObjectWrap::Unwrap<BusConnection>(args[0].As<v8::Object>());

  char* busName = strdup(*NanUtf8String(args[1]));
  
  AboutProxyWrapper* obj = new AboutProxyWrapper(busWrapper->bus, busName, args[2]->Int32Value());
  obj->Wrap(args.This());

  NanReturnValue(args.This());
}

NAN_METHOD(AboutProxyWrapper::GetObjectDescription) {
  NanScope();
  
  // AboutProxyWrapper* wrapper = node::ObjectWrap::Unwrap<AboutProxyWrapper>(args.This());
  // ajn::MsgArg* objectDescription = new ajn::MsgArg();
  // ajn::MsgArg objArg;
  // QStatus status = wrapper->proxy->GetObjectDescription(objArg);
  // NanReturnValue(NanNew<v8::Integer>(static_cast<int>(status)));
  NanReturnValue(NanNew<v8::Integer>(0));
}
