#include "nan.h"

#include "util.h"
#include "AboutListenerImpl.h"
#include <alljoyn/AboutObjectDescription.h>
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

    // v8::Local<v8::Object> objectDescriptionArgOut = v8::Object::New();
    // const ajn::MsgArg* objectDescriptionArgIn = holder->objectDescriptionArg;
    // msgArgToObject(objectDescriptionArgIn, 0, objectDescriptionArgOut);






    // AboutObjectDescription aod(holder->objectDescriptionArg);
    // size_t path_num = aod.GetPaths(NULL, 0);
    // const char** paths = new const char*[path_num];
    // aod.GetPaths(paths, path_num);
    // for (size_t i = 0; i < path_num; ++i) {
    //     size_t intf_num = aod.GetInterfaces(paths[i], NULL, 0);
    //     const char** intfs = new const char*[intf_num];
    //     aod.GetInterfaces(paths[i], intfs, intf_num);
    //     for (size_t j = 0; j < intf_num; ++j) {
    //         printf("\t\t\t%s\n", intfs[j]);
    //     }
    //     delete [] intfs;
    // }
    //
    //














    v8::Local<v8::Array> interfaceNames = v8::Array::New(1);
    interfaceNames->Set(0, NanNew<v8::String>("com.example.about.feature.interface.sample"));
      
    v8::Local<v8::Object> objectDescription = v8::Object::New();
    objectDescription->Set(NanNew<v8::String>("/example/path"), interfaceNames);
    

    v8::Local<v8::Object> aboutDataArgOut = v8::Object::New();
    const ajn::MsgArg* aboutDataArgIn = holder->aboutDataArg;
    msgArgToObject(aboutDataArgIn, 0, aboutDataArgOut);

    v8::Handle<v8::Value> argv[] = {
      NanNew<v8::String>(holder->busName),
      NanNew<v8::Integer>(holder->version),
      NanNew<v8::Integer>(holder->port),
      objectDescription,
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
