#include <node.h>
#include <nan.h>
#include "BusConnection.h"
#include "InterfaceWrapper.h"
#include "AboutListenerWrapper.h"
#include "AboutProxyWrapper.h"
#include "AboutDataWrapper.h"
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
  AboutProxyWrapper::Init();
  AboutDataWrapper::Init();
  BusObjectWrapper::Init();
  SessionPortListenerWrapper::Init();
  NotificationWrapper::Init();

  Local<Function> busWrap = FunctionTemplate::New(BusAttachmentWrapper)->GetFunction();
  target->Set(NanNew<String>("BusAttachment"), busWrap);
  Local<Function> interfaceWrap = FunctionTemplate::New(InterfaceDescriptionWrapper)->GetFunction();
  target->Set(NanNew<String>("InterfaceDescription"), interfaceWrap);

  Local<Function> aboutListenerConstructor = FunctionTemplate::New(AboutListenerConstructor)->GetFunction();
  target->Set(NanNew<String>("AboutListener"), aboutListenerConstructor);

  Local<Function> aboutProxyConstructor = FunctionTemplate::New(AboutProxyConstructor)->GetFunction();
  target->Set(NanNew<String>("AboutProxy"), aboutProxyConstructor);

  Local<Function> aboutDataConstructor = FunctionTemplate::New(AboutDataConstructor)->GetFunction();
  target->Set(NanNew<String>("AboutData"), aboutDataConstructor);

  Local<Function> busListenerConstructor = FunctionTemplate::New(BusListenerConstructor)->GetFunction();
  target->Set(NanNew<String>("BusListener"), busListenerConstructor);
  Local<Function> objectConstructor = FunctionTemplate::New(BusObjectConstructor)->GetFunction();
  target->Set(NanNew<String>("BusObject"), objectConstructor);
  Local<Function> sessionPortListenerConstructor = FunctionTemplate::New(SessionPortListenerConstructor)->GetFunction();
  target->Set(NanNew<String>("SessionPortListener"), sessionPortListenerConstructor);
  Local<Function> notificationConstructor = FunctionTemplate::New(NotificationConstructor)->GetFunction();
  target->Set(NanNew<String>("NotificationService"), notificationConstructor);
}

NODE_MODULE(node_alljoyn, init)