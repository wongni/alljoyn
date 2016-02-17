#include <node.h>
#include <nan.h>
#include "BusConnection.h"
#include "InterfaceWrapper.h"
#include "AboutListenerWrapper.h"
#include "AuthListenerWrapper.h"
#include "ProxyBusObjectWrapper.h"
#include "AboutProxyWrapper.h"
#include "AboutDataWrapper.h"
#include "AboutObjWrapper.h"
#include "BusListenerWrapper.h"
#include "BusObjectWrapper.h"
#include "SessionPortListenerWrapper.h"
#include "NotificationWrapper.h"


using namespace v8;

void init(Handle<Object> target) {
  BusConnection::Init();
  InterfaceWrapper::Init();
  BusListenerWrapper::Init();
  AboutListenerWrapper::Init();
  AuthListenerWrapper::Init();
  AboutProxyWrapper::Init();
  ProxyBusObjectWrapper::Init();
  AboutDataWrapper::Init();
  AboutObjWrapper::Init();
  BusObjectWrapper::Init();
  SessionPortListenerWrapper::Init();
  NotificationWrapper::Init();

  Nan::SetMethod(target, "BusAttachment", BusAttachmentWrapper);
  Nan::SetMethod(target, "InterfaceDescription", InterfaceDescriptionWrapper);
  Nan::SetMethod(target, "BusListener", BusListenerConstructor);
  Nan::SetMethod(target, "BusObject", BusObjectConstructor);
  Nan::SetMethod(target, "SessionPortListener", SessionPortListenerConstructor);
  Nan::SetMethod(target, "NotificationService", NotificationConstructor);
  Nan::SetMethod(target, "AboutListener", AboutListenerConstructor);
  Nan::SetMethod(target, "AuthListener", AuthListenerConstructor);
  Nan::SetMethod(target, "AboutProxy", AboutProxyConstructor);
  Nan::SetMethod(target, "ProxyBusObject", ProxyBusObjectConstructor);
  Nan::SetMethod(target, "AboutData", AboutDataConstructor);
  Nan::SetMethod(target, "AboutObj", AboutObjConstructor);
}

NODE_MODULE(node_alljoyn, init)
