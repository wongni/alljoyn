#include "nan.h"

#include "BusConnection.h"
#include "AboutProxyWrapper.h"
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
  
  if(args.Length() < 2){
    return NanThrowError("NAN_METHOD(AboutProxyConstructor) AboutProxy requires busAttachment and busName. sessionId is optional.");
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
  :proxy(new ajn::AboutProxy(*busAttachment, busName, sessionId)){
}

AboutProxyWrapper::~AboutProxyWrapper(){
}

void AboutProxyWrapper::Init () {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(AboutProxyWrapper::New);
  NanAssignPersistent(aboutproxy_constructor, tpl);
  tpl->SetClassName(NanNew<v8::String>("AboutProxy"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(tpl, "getObjectDescription", AboutProxyWrapper::GetObjectDescription);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getSessionId", AboutProxyWrapper::GetSessionId);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getUniqueName", AboutProxyWrapper::GetUniqueName);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getVersion", AboutProxyWrapper::GetVersion);

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

NAN_METHOD(AboutProxyWrapper::GetSessionId) {
  NanScope();
  AboutProxyWrapper* wrapper = node::ObjectWrap::Unwrap<AboutProxyWrapper>(args.This());
  ajn::SessionId sessionId = wrapper->proxy->GetSessionId();
  NanReturnValue(NanNew<v8::Integer>(static_cast<int>(sessionId)));
}

NAN_METHOD(AboutProxyWrapper::GetUniqueName) {
  NanScope();
  AboutProxyWrapper* wrapper = node::ObjectWrap::Unwrap<AboutProxyWrapper>(args.This());
  qcc::String uniqueName = wrapper->proxy->GetUniqueName();
  NanReturnValue(NanNew<v8::String>(uniqueName.c_str()));
}

NAN_METHOD(AboutProxyWrapper::GetVersion) {
  NanScope();
  AboutProxyWrapper* wrapper = node::ObjectWrap::Unwrap<AboutProxyWrapper>(args.This());

  uint16_t ver;
  QStatus status = wrapper->proxy->GetVersion(ver);
  if (status == ER_OK)
    NanReturnValue(NanNew<v8::Integer>(static_cast<int>(ver)));
  else
    NanReturnValue(NanNew<v8::String>(std::string(QCC_StatusText(status))));
}

NAN_METHOD(AboutProxyWrapper::GetObjectDescription) {
  NanScope();
  
  AboutProxyWrapper* wrapper = node::ObjectWrap::Unwrap<AboutProxyWrapper>(args.This());
  ajn::MsgArg objectDescriptionArg;
  QStatus status = wrapper->proxy->GetObjectDescription(objectDescriptionArg);
  
  // this objectDescription will head back to Node.js after we 
  // populate it using the AllJoyn helper methods
  v8::Local<v8::Object> objectDescription = v8::Object::New();

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
      interfaceNames->Set(j, NanNew<v8::String>(intfs[j]));
    }
    objectDescription->Set(NanNew<v8::String>(paths[i]), interfaceNames);
    delete [] intfs;
  }
  
  if (status == ER_OK)
    NanReturnValue(objectDescription);
  else
    NanReturnValue(NanNew<v8::String>(std::string(QCC_StatusText(status))));
}
