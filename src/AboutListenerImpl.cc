#include "nan.h"

#include "util.h"
#include "AboutListenerImpl.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

AboutListenerImpl::AboutListenerImpl(NanCallback* announcedCallback){
  loop = uv_default_loop();
  announced.callback = announcedCallback;
  uv_async_init(loop, &announced_async, announced_callback);
  uv_rwlock_init(&calllock);
}

AboutListenerImpl::~AboutListenerImpl(){
}

void AboutListenerImpl::announced_callback(uv_async_t *handle, int status) {
    CallbackHolder* holder = (CallbackHolder*) handle->data;

    v8::Local<v8::Object> objectDescriptionArgOut = v8::Object::New();
    const ajn::MsgArg* objectDescriptionArgIn = holder->objectDescriptionArg;
    msgArgToObject(objectDescriptionArgIn, 0, objectDescriptionArgOut);

    v8::Local<v8::Object> aboutDataArgOut = v8::Object::New();
    const ajn::MsgArg* aboutDataArgIn = holder->aboutDataArg;
    msgArgToObject(aboutDataArgIn, 0, aboutDataArgOut);

    v8::Handle<v8::Value> argv[] = {
      NanNew<v8::String>(holder->busName),
      NanNew<v8::Integer>(holder->version),
      NanNew<v8::Integer>(holder->port),
      objectDescriptionArgOut,
      aboutDataArgOut
    };
    holder->callback->Call(5, argv);
}

void AboutListenerImpl::Announced(const char * busName, uint16_t version, ajn::SessionPort port, const ajn::MsgArg & objectDescriptionArg, const ajn::MsgArg & aboutDataArg){
    announced_async.data = (void*) &announced;

    announced.busName = strdup(busName);
    announced.version = version;
    announced.port = 17;
    announced.objectDescriptionArg = new ajn::MsgArg(objectDescriptionArg);
    announced.aboutDataArg = new ajn::MsgArg(aboutDataArg);
    uv_async_send(&announced_async);
}
