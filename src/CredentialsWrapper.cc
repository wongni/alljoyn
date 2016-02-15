#include "nan.h"

#include "CredentialsWrapper.h"
#include <alljoyn/AllJoynStd.h>

static v8::Persistent<v8::FunctionTemplate> credentials_constructor;

v8::Handle<v8::Value> CredentialsWrapper::NewInstance() {
    NanScope();

    v8::Local<v8::Object> obj;
    v8::Local<v8::FunctionTemplate> con = NanNew<v8::FunctionTemplate>(credentials_constructor);
    obj = con->GetFunction()->NewInstance(0, NULL);
    return obj;
}

NAN_METHOD(CredentialsConstructor) {
  NanScope();

  v8::Local<v8::Object> obj;
  v8::Local<v8::FunctionTemplate> con = NanNew<v8::FunctionTemplate>(credentials_constructor);

  obj = con->GetFunction()->NewInstance(0, NULL);
  NanReturnValue(obj);
}

CredentialsWrapper::CredentialsWrapper()
  :credentials(new ajn::AuthListener::Credentials()){
}

CredentialsWrapper::~CredentialsWrapper(){
}

void CredentialsWrapper::Init () {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(CredentialsWrapper::New);
  NanAssignPersistent(credentials_constructor, tpl);
  tpl->SetClassName(NanNew<v8::String>("Credentials"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(tpl, "isSet", CredentialsWrapper::IsSet);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setPassword", CredentialsWrapper::SetPassword);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setUserName", CredentialsWrapper::SetUserName);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setCertChain", CredentialsWrapper::SetCertChain);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setPrivateKey", CredentialsWrapper::SetPrivateKey);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setLogonEntry", CredentialsWrapper::SetLogonEntry);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setExpiration", CredentialsWrapper::SetExpiration);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getPassword", CredentialsWrapper::GetPassword);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getUserName", CredentialsWrapper::GetUserName);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getCertChain", CredentialsWrapper::GetCertChain);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getPrivateKey", CredentialsWrapper::GetPrivateKey);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getLogonEntry", CredentialsWrapper::GetLogonEntry);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getExpiration", CredentialsWrapper::GetExpiration);
}

NAN_METHOD(CredentialsWrapper::New) {
  NanScope();

  CredentialsWrapper* obj = new CredentialsWrapper();
  obj->Wrap(args.This());

  NanReturnValue(args.This());
}

NAN_METHOD(CredentialsWrapper::IsSet) {
  NanScope();
  if(args.Length() < 1){
    return NanThrowError("NAN_METHOD(CredentialsWrapper::IsSet) IsSet requires a flag integer.");
  }

  CredentialsWrapper* wrapper = node::ObjectWrap::Unwrap<CredentialsWrapper>(args.This());

  bool isSet = wrapper->credentials->IsSet(args[0]->Uint32Value());
  NanReturnValue(NanNew<v8::Boolean>(isSet));
}

NAN_METHOD(CredentialsWrapper::SetPassword) {
  NanScope();
  if(args.Length() < 1){
    return NanThrowError("NAN_METHOD(CredentialsWrapper::SetPassword) SetPassword requires a password string.");
  }

  CredentialsWrapper* wrapper = node::ObjectWrap::Unwrap<CredentialsWrapper>(args.This());

  wrapper->credentials->SetPassword(strdup(*NanUtf8String(args[0])));
  NanReturnUndefined();
}

NAN_METHOD(CredentialsWrapper::SetUserName) {
  NanScope();
  if(args.Length() < 1){
    return NanThrowError("NAN_METHOD(CredentialsWrapper::SetUserName) SetUserName requires a user name string.");
  }

  CredentialsWrapper* wrapper = node::ObjectWrap::Unwrap<CredentialsWrapper>(args.This());

  wrapper->credentials->SetUserName(strdup(*NanUtf8String(args[0])));
  NanReturnUndefined();
}

NAN_METHOD(CredentialsWrapper::SetCertChain) {
  NanScope();
  if(args.Length() < 1){
    return NanThrowError("NAN_METHOD(CredentialsWrapper::SetCertChain) SetCertChain requires a cert chain string.");
  }

  CredentialsWrapper* wrapper = node::ObjectWrap::Unwrap<CredentialsWrapper>(args.This());

  wrapper->credentials->SetCertChain(strdup(*NanUtf8String(args[0])));
  NanReturnUndefined();
}

NAN_METHOD(CredentialsWrapper::SetPrivateKey) {
  NanScope();
  if(args.Length() < 1){
    return NanThrowError("NAN_METHOD(CredentialsWrapper::SetPrivateKey) SetPrivateKey requires a private key string.");
  }

  CredentialsWrapper* wrapper = node::ObjectWrap::Unwrap<CredentialsWrapper>(args.This());

  wrapper->credentials->SetPrivateKey(strdup(*NanUtf8String(args[0])));
  NanReturnUndefined();
}

NAN_METHOD(CredentialsWrapper::SetLogonEntry) {
  NanScope();
  if(args.Length() < 1){
    return NanThrowError("NAN_METHOD(CredentialsWrapper::SetLogonEntry) SetLogonEntry requires a logon entry string.");
  }

  CredentialsWrapper* wrapper = node::ObjectWrap::Unwrap<CredentialsWrapper>(args.This());

  wrapper->credentials->SetLogonEntry(strdup(*NanUtf8String(args[0])));
  NanReturnUndefined();
}

NAN_METHOD(CredentialsWrapper::SetExpiration) {
  NanScope();
  if(args.Length() < 1){
    return NanThrowError("NAN_METHOD(CredentialsWrapper::SetExpiration) SetExpiration requires a expiration integer.");
  }

  CredentialsWrapper* wrapper = node::ObjectWrap::Unwrap<CredentialsWrapper>(args.This());

  wrapper->credentials->SetExpiration(args[0]->Uint32Value());
  NanReturnUndefined();
}

NAN_METHOD(CredentialsWrapper::GetPassword) {
  NanScope();

  CredentialsWrapper* wrapper = node::ObjectWrap::Unwrap<CredentialsWrapper>(args.This());

  NanReturnValue(NanNew<v8::String>(wrapper->credentials->GetPassword().c_str()));
}

NAN_METHOD(CredentialsWrapper::GetUserName) {
  NanScope();

  CredentialsWrapper* wrapper = node::ObjectWrap::Unwrap<CredentialsWrapper>(args.This());

  NanReturnValue(NanNew<v8::String>(wrapper->credentials->GetUserName().c_str()));
}

NAN_METHOD(CredentialsWrapper::GetCertChain) {
  NanScope();

  CredentialsWrapper* wrapper = node::ObjectWrap::Unwrap<CredentialsWrapper>(args.This());

  NanReturnValue(NanNew<v8::String>(wrapper->credentials->GetCertChain().c_str()));
}

NAN_METHOD(CredentialsWrapper::GetPrivateKey) {
  NanScope();

  CredentialsWrapper* wrapper = node::ObjectWrap::Unwrap<CredentialsWrapper>(args.This());

  NanReturnValue(NanNew<v8::String>(wrapper->credentials->GetPrivateKey().c_str()));
}

NAN_METHOD(CredentialsWrapper::GetLogonEntry) {
  NanScope();

  CredentialsWrapper* wrapper = node::ObjectWrap::Unwrap<CredentialsWrapper>(args.This());

  NanReturnValue(NanNew<v8::String>(wrapper->credentials->GetLogonEntry().c_str()));
}

NAN_METHOD(CredentialsWrapper::GetExpiration) {
  NanScope();

  CredentialsWrapper* wrapper = node::ObjectWrap::Unwrap<CredentialsWrapper>(args.This());

  NanReturnValue(NanNew<v8::Integer>(wrapper->credentials->GetExpiration()));
}
