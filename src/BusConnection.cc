#include <nan.h>

#include "BusConnection.h"
#include "InterfaceWrapper.h"
#include "AboutListenerWrapper.h"
#include "AuthListenerWrapper.h"
#include "BusListenerWrapper.h"
#include "SessionPortListenerWrapper.h"
#include "BusObjectWrapper.h"
#include "SignalHandlerImpl.h"
#include <alljoyn/BusAttachment.h>
#include <alljoyn/ProxyBusObject.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/InterfaceDescription.h>
#include <MessageReceiver.h>
#include <alljoyn/DBusStd.h>
#include <alljoyn/AllJoynStd.h>

static Nan::Persistent<v8::FunctionTemplate> bus_constructor;

v8::Local<v8::Value> BusConnection::NewInstance(v8::Local<v8::String> &appName) {
    v8::Local<v8::Object> obj;
    v8::Local<v8::FunctionTemplate> con = Nan::New<v8::FunctionTemplate>(bus_constructor);
    v8::Handle<v8::Value> argv[] = { appName };
    obj = con->GetFunction()->NewInstance(1, argv);
    return obj;
}

NAN_METHOD(BusAttachmentWrapper) {
    v8::Local<v8::String> appName = info[0].As<v8::String>();
    info.GetReturnValue().Set(BusConnection::NewInstance(appName));
}

BusConnection::BusConnection(const char* shortName, bool allowRemoteMessages, int maxConcurrent){
  bus = new ajn::BusAttachment(shortName, allowRemoteMessages, maxConcurrent);
}

BusConnection::~BusConnection(){
}

void BusConnection::Init () {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(BusConnection::New);
  bus_constructor.Reset(tpl);
  tpl->SetClassName(Nan::New<v8::String>("BusAttachment").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  Nan::SetPrototypeMethod(tpl, "start", BusConnection::Start);
  Nan::SetPrototypeMethod(tpl, "stop", BusConnection::Stop);
  Nan::SetPrototypeMethod(tpl, "join", BusConnection::Join);
  Nan::SetPrototypeMethod(tpl, "connect", BusConnection::Connect);
  Nan::SetPrototypeMethod(tpl, "disconnect", BusConnection::Disconnect);
  Nan::SetPrototypeMethod(tpl, "createInterface", BusConnection::CreateInterface);
  Nan::SetPrototypeMethod(tpl, "getInterface", BusConnection::GetInterface);
  Nan::SetPrototypeMethod(tpl, "registerAboutListener", BusConnection::RegisterAboutListener);
  Nan::SetPrototypeMethod(tpl, "enablePeerSecurity", BusConnection::EnablePeerSecurity);
  Nan::SetPrototypeMethod(tpl, "registerBusListener", BusConnection::RegisterBusListener);
  Nan::SetPrototypeMethod(tpl, "unregisterBusListener", BusConnection::UnregisterBusListener);
  Nan::SetPrototypeMethod(tpl, "registerBusObject", BusConnection::RegisterBusObject);
  Nan::SetPrototypeMethod(tpl, "findAdvertisedName", BusConnection::FindAdvertisedName);
  Nan::SetPrototypeMethod(tpl, "joinSession", BusConnection::JoinSession);
  Nan::SetPrototypeMethod(tpl, "bindSessionPort", BusConnection::BindSessionPort);
  Nan::SetPrototypeMethod(tpl, "requestName", BusConnection::RequestName);
  Nan::SetPrototypeMethod(tpl, "advertiseName", BusConnection::AdvertiseName);
  Nan::SetPrototypeMethod(tpl, "registerSignalHandler", BusConnection::RegisterSignalHandler);
  Nan::SetPrototypeMethod(tpl, "getConnectSpec", BusConnection::GetConnectSpec);
  Nan::SetPrototypeMethod(tpl, "ping", BusConnection::Ping);
  Nan::SetPrototypeMethod(tpl, "whoImplements", BusConnection::WhoImplements);
  Nan::SetPrototypeMethod(tpl, "enableConcurrentCallbacks", BusConnection::EnableConcurrentCallbacks);
  Nan::SetPrototypeMethod(tpl, "getUniqueName", BusConnection::GetUniqueName);
  Nan::SetPrototypeMethod(tpl, "createInterfacesFromXml", BusConnection::CreateInterfacesFromXml);
}

NAN_METHOD(BusConnection::New) {
  if (info.Length() == 0 || !info[0]->IsString())
    return Nan::ThrowError("constructor requires an applicationName string argument");

  BusConnection* obj = new BusConnection(strdup(*Nan::Utf8String(info[0])), true, 4);
  obj->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(BusConnection::Start) {
  BusConnection* connection = Nan::ObjectWrap::Unwrap<BusConnection>(info.This());
  QStatus status = connection->bus->Start();
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::Stop) {
  BusConnection* connection = Nan::ObjectWrap::Unwrap<BusConnection>(info.This());
  QStatus status = connection->bus->Stop();
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::Join) {
  BusConnection* connection = Nan::ObjectWrap::Unwrap<BusConnection>(info.This());
  QStatus status = connection->bus->Join();
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::Connect) {
  BusConnection* connection = Nan::ObjectWrap::Unwrap<BusConnection>(info.This());
  if (info.Length() != 0 && !info[0]->IsString())
    return Nan::ThrowError("connect must be called with a string connectSpec");

  QStatus status = info.Length() == 0 ? connection->bus->Connect()
    : connection->bus->Connect(*Nan::Utf8String(info[0]));
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::Disconnect) {
  BusConnection* connection = Nan::ObjectWrap::Unwrap<BusConnection>(info.This());
  QStatus status = connection->bus->Disconnect();
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::CreateInterface) {
  if (info.Length() == 0 || !info[0]->IsString())
    return Nan::ThrowError("CreateInterface requires a name string argument");
  if (info.Length() == 1)
    return Nan::ThrowError("CreateInterface requires a new InterfaceDescription argument");

  char* name = strdup(*Nan::Utf8String(info[0]));

  ajn::InterfaceDescription* interface = NULL;

  BusConnection* connection = Nan::ObjectWrap::Unwrap<BusConnection>(info.This());
  QStatus status = connection->bus->CreateInterface(name, interface);
  InterfaceWrapper* wrapper = Nan::ObjectWrap::Unwrap<InterfaceWrapper>(info[1].As<v8::Object>());
  wrapper->interface = interface;
  if (ER_OK == status) {
    //callback
  } else {
      printf("Failed to create interface \"%s\" (%s)\n", name, QCC_StatusText(status));
  }

  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::GetInterface) {
  if (info.Length() == 0 || !info[0]->IsString())
    return Nan::ThrowError("GetInterface requires a name string argument");
  if (info.Length() == 1)
    return Nan::ThrowError("GetInterface requires a new InterfaceDescription argument");

  char* name = strdup(*Nan::Utf8String(info[0]));
  ajn::InterfaceDescription* interface = NULL;

  BusConnection* connection = Nan::ObjectWrap::Unwrap<BusConnection>(info.This());
  interface = const_cast<ajn::InterfaceDescription*>(connection->bus->GetInterface(name));
  InterfaceWrapper* wrapper = Nan::ObjectWrap::Unwrap<InterfaceWrapper>(info[1].As<v8::Object>());
  wrapper->interface = interface;

  info.GetReturnValue().SetUndefined();
}

NAN_METHOD(BusConnection::RegisterAboutListener) {

  if (info.Length() == 0)
    return Nan::ThrowError("RegisterAboutListener requires an AboutListener argument");

  BusConnection* connection = Nan::ObjectWrap::Unwrap<BusConnection>(info.This());
  AboutListenerWrapper* wrapper = Nan::ObjectWrap::Unwrap<AboutListenerWrapper>(info[0].As<v8::Object>());

  connection->bus->RegisterAboutListener(*(wrapper->listener));

  info.GetReturnValue().SetUndefined();
}

NAN_METHOD(BusConnection::EnablePeerSecurity) {

  if (info.Length() < 4)
    return Nan::ThrowError("EnablePeerSecurity requires authMechanisms, authListener, keyStoreFileName and isShared arguments");

  BusConnection* connection = Nan::ObjectWrap::Unwrap<BusConnection>(info.This());
  AuthListenerWrapper* wrapper = Nan::ObjectWrap::Unwrap<AuthListenerWrapper>(info[1].As<v8::Object>());

  QStatus status = connection->bus->EnablePeerSecurity(
    strdup(*Nan::Utf8String(info[0])),
    wrapper->listener,
    strdup(*Nan::Utf8String(info[2])),
    info[3]->BooleanValue()
  );

  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::RegisterBusListener) {
  if (info.Length() == 0)
    return Nan::ThrowError("RegisterBusListener requires a BusListener argument");

  BusConnection* connection = Nan::ObjectWrap::Unwrap<BusConnection>(info.This());
  BusListenerWrapper* wrapper = Nan::ObjectWrap::Unwrap<BusListenerWrapper>(info[0].As<v8::Object>());
  connection->bus->RegisterBusListener(*(wrapper->listener));

  info.GetReturnValue().SetUndefined();
}

NAN_METHOD(BusConnection::UnregisterBusListener) {

  if (info.Length() == 0)
    return Nan::ThrowError("UnregisterBusListener requires a BusListener argument");

  BusConnection* connection = Nan::ObjectWrap::Unwrap<BusConnection>(info.This());
  BusListenerWrapper* wrapper = Nan::ObjectWrap::Unwrap<BusListenerWrapper>(info[0].As<v8::Object>());
  connection->bus->UnregisterBusListener(*(wrapper->listener));

  info.GetReturnValue().SetUndefined();
}


NAN_METHOD(BusConnection::RegisterBusObject) {
  if (info.Length() == 0)
    return Nan::ThrowError("RegisterBusObject requires a BusObject argument");

  BusConnection* connection = Nan::ObjectWrap::Unwrap<BusConnection>(info.This());
  BusObjectWrapper* wrapper = Nan::ObjectWrap::Unwrap<BusObjectWrapper>(info[0].As<v8::Object>());

  QStatus status = connection->bus->RegisterBusObject(*(wrapper->object));

  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::FindAdvertisedName) {
  if (info.Length() == 0 || !info[0]->IsString())
    return Nan::ThrowError("FindAdvertisedName requires a namePrefix string argument");

  BusConnection* connection = Nan::ObjectWrap::Unwrap<BusConnection>(info.This());
  QStatus status = connection->bus->FindAdvertisedName(strdup(*Nan::Utf8String(info[0])));
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::JoinSession) {
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsNumber())
    return Nan::ThrowError("JoinSession requires a sessionHost name, sessionPort number, and (optional) SessionListener callback");

  BusConnection* connection = Nan::ObjectWrap::Unwrap<BusConnection>(info.This());
  ajn::SessionId sessionId = static_cast<ajn::SessionPort>(info[1]->Int32Value());
  ajn::SessionOpts opts(ajn::SessionOpts::TRAFFIC_MESSAGES, true, ajn::SessionOpts::PROXIMITY_ANY, ajn::TRANSPORT_ANY);
  // if(info.Length() == 3 && info[2]->IsObject() && !info[2]->IsNull()){
  //   SessionPortListenerWrapper* wrapper = Nan::ObjectWrap::Unwrap<SessionPortListenerWrapper>(info[2].As<v8::Object>());
  //   QStatus status = connection->bus->JoinSession(*Nan::Utf8String(info[0]), info[1]->IntegerValue(), *(wrapper->listener), info[1]->IntegerValue(), opts);
  // }else{
  connection->bus->JoinSession(strdup(*Nan::Utf8String(info[0])), static_cast<ajn::SessionPort>(info[1]->Int32Value()), NULL, sessionId, opts);
  // }

  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(sessionId)));
}

NAN_METHOD(BusConnection::BindSessionPort) {
  if (info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsObject())
    return Nan::ThrowError("BindSessionPort requires a sessionPort number and SessionPortListener callback");

  BusConnection* connection = Nan::ObjectWrap::Unwrap<BusConnection>(info.This());
  SessionPortListenerWrapper* wrapper = Nan::ObjectWrap::Unwrap<SessionPortListenerWrapper>(info[1].As<v8::Object>());
  ajn::SessionPort port = static_cast<ajn::SessionPort>(info[0]->Int32Value());
  ajn::SessionOpts opts(ajn::SessionOpts::TRAFFIC_MESSAGES, true, ajn::SessionOpts::PROXIMITY_ANY, ajn::TRANSPORT_ANY);
  QStatus status = connection->bus->BindSessionPort(port, opts, *(wrapper->listener));

  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::RequestName) {
  if (info.Length() == 0 || !info[0]->IsString())
    return Nan::ThrowError("RequestName requires a requestedName string argument");

  BusConnection* connection = Nan::ObjectWrap::Unwrap<BusConnection>(info.This());
  QStatus status = connection->bus->RequestName(strdup(*Nan::Utf8String(info[0])), DBUS_NAME_FLAG_DO_NOT_QUEUE);
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::AdvertiseName) {
  if (info.Length() == 0 || !info[0]->IsString())
    return Nan::ThrowError("AdvertiseName requires a name string argument");

  BusConnection* connection = Nan::ObjectWrap::Unwrap<BusConnection>(info.This());
  QStatus status = connection->bus->AdvertiseName(strdup(*Nan::Utf8String(info[0])), ajn::TRANSPORT_ANY);
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::RegisterSignalHandler) {
  if (info.Length() < 4 || !info[0]->IsObject() || !info[1]->IsFunction() || !info[2]->IsObject() || !info[3]->IsString())
    return Nan::ThrowError("RegisterSignalHandler requires a receiver BusObject, signalHandler callback, interface, interface member name, and (optional) srcPath.");

  BusConnection* connection = Nan::ObjectWrap::Unwrap<BusConnection>(info.This());
  InterfaceWrapper* interface = Nan::ObjectWrap::Unwrap<InterfaceWrapper>(info[2].As<v8::Object>());
  const ajn::InterfaceDescription::Member* signalMember = interface->interface->GetMember(*Nan::Utf8String(info[3]));

  v8::Local<v8::Function> fn = info[1].As<v8::Function>();
  Nan::Callback *callback = new Nan::Callback(fn);
  SignalHandlerImpl* signalHandler = new SignalHandlerImpl(callback);
  QStatus status = ER_OK;
  if(info.Length() == 5){
    status = connection->bus->RegisterSignalHandler(signalHandler, static_cast<ajn::MessageReceiver::SignalHandler>(&SignalHandlerImpl::Signal), signalMember, strdup(*Nan::Utf8String(info[4])));
  }else{
    status = connection->bus->RegisterSignalHandler(signalHandler, static_cast<ajn::MessageReceiver::SignalHandler>(&SignalHandlerImpl::Signal), signalMember, NULL);
  }

  if (status == ER_OK)
    info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
  else
    info.GetReturnValue().Set(Nan::New<v8::String>(std::string(QCC_StatusText(status))).ToLocalChecked());
}

NAN_METHOD(BusConnection::GetConnectSpec){

  BusConnection* connection = Nan::ObjectWrap::Unwrap<BusConnection>(info.This());
  qcc::String connectSpec = connection->bus->GetConnectSpec();
  info.GetReturnValue().Set(Nan::New<v8::String>(connectSpec.c_str()).ToLocalChecked());
}

NAN_METHOD(BusConnection::Ping) {

  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsNumber())
    return Nan::ThrowError("Ping requires a well-known name and timeout in milliseconds.");

  char* name = strdup(*Nan::Utf8String(info[0]));

  BusConnection* connection = Nan::ObjectWrap::Unwrap<BusConnection>(info.This());
  QStatus status = connection->bus->Ping(name, info[1]->Int32Value());

  if (status == ER_OK)
    info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
  else
    info.GetReturnValue().Set(Nan::New<v8::String>(std::string(QCC_StatusText(status))).ToLocalChecked());
}

NAN_METHOD(BusConnection::GetUniqueName){

  BusConnection* connection = Nan::ObjectWrap::Unwrap<BusConnection>(info.This());
  qcc::String uniqueName = connection->bus->GetUniqueName();
  info.GetReturnValue().Set(Nan::New<v8::String>(uniqueName.c_str()).ToLocalChecked());
}

NAN_METHOD(BusConnection::EnableConcurrentCallbacks){

  BusConnection* connection = Nan::ObjectWrap::Unwrap<BusConnection>(info.This());
  connection->bus->EnableConcurrentCallbacks();
  info.GetReturnValue().Set(Nan::New<v8::Integer>(0));
}

NAN_METHOD(BusConnection::WhoImplements) {

  if (info.Length() < 1 || !info[0]->IsArray())
    return Nan::ThrowError("WhoImplements requires an array of interface names.");

  // TODO: there must be better ways to go from
  // JavaScript array to V8 array to a C++ char**

  v8::Local<v8::Array> v8InterfaceNames = v8::Local<v8::Array>::Cast(info[0]);

  const char** interfaceNames;
  interfaceNames = new const char*[v8InterfaceNames->Length()];

  uint32_t i;
  for (i = 0; i < v8InterfaceNames->Length(); ++i)
    interfaceNames[i] = strdup(*Nan::Utf8String(v8InterfaceNames->Get(i)));

  size_t numberInterfaceNames = sizeof(interfaceNames) / sizeof(interfaceNames[0]);

  BusConnection* connection = Nan::ObjectWrap::Unwrap<BusConnection>(info.This());
  QStatus status = connection->bus->WhoImplements(interfaceNames, numberInterfaceNames);
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(BusConnection::CreateInterfacesFromXml) {

  if (info.Length() < 1 || !info[0]->IsString())
    return Nan::ThrowError("CreateInterfacesFromXml requires a an XML string: <node><interface name='com.example.about.feature.interface.sample'><method name='Echo'><arg name='out_arg' type='s' direction='in' /><arg name='return_arg' type='s' direction='out' /></method></interface></node>");

  BusConnection* connection = Nan::ObjectWrap::Unwrap<BusConnection>(info.This());
  QStatus status = connection->bus->CreateInterfacesFromXml(strdup(*Nan::Utf8String(info[0])));
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}
