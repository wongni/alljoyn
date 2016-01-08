#include "nan.h"

#include "AboutListenerImpl.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

AboutListenerImpl::AboutListenerImpl(NanCallback* announcedCallback){
  loop = uv_default_loop();
  announced.callback = announcedCallback;
  uv_async_init(loop, &announced_async, announced_callback);
  
  announced.data = 0;
  uv_rwlock_init(&announced.datalock);
}

AboutListenerImpl::~AboutListenerImpl(){
  uv_rwlock_destroy(&announced.datalock);
  free(announced.data);
}

void AboutListenerImpl::announced_callback(uv_async_t *handle, int status) {
    CallbackHolder* holder = (CallbackHolder*) handle->data;

    uv_rwlock_rdlock(&holder->datalock);
    v8::Handle<v8::Value> argv[] = {
      NanNew<v8::String>(holder->data)
    };
    uv_rwlock_rdunlock(&holder->datalock);
    holder->callback->Call(1, argv);
}

void AboutListenerImpl::Announced(const char * busName, uint16_t version, ajn::SessionPort port, const ajn::MsgArg & objectDescriptionArg, const ajn::MsgArg & aboutDataArg){
    announced_async.data = (void*) &announced;

    uv_rwlock_wrlock(&announced.datalock);
    free(announced.data);
    announced.data = strdup(busName);
    uv_rwlock_wrunlock(&announced.datalock);

    uv_async_send(&announced_async);
}
