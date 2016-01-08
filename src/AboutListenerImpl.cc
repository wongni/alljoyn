#include "nan.h"

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

    v8::Handle<v8::Value> argv[] = {
      NanNew<v8::String>(holder->busName),
      NanNew<v8::Integer>(holder->version),
    };
    holder->callback->Call(2, argv);
}

void AboutListenerImpl::Announced(const char * busName, uint16_t version, ajn::SessionPort port, const ajn::MsgArg & objectDescriptionArg, const ajn::MsgArg & aboutDataArg){
    announced_async.data = (void*) &announced;

    announced.busName = strdup(busName);
    announced.version = version;

    uv_async_send(&announced_async);
}
