#include "nan.h"

#include "util.h"
#include "AboutListenerImpl.h"
#include <alljoyn/AboutData.h>
#include <alljoyn/AboutObjectDescription.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

AboutListenerImpl::AboutListenerImpl(Nan::Callback* announcedCallback){
  loop = uv_default_loop();
  announced.callback = announcedCallback;
  uv_async_init(loop, &announced_async, announced_callback);
}

AboutListenerImpl::~AboutListenerImpl(){
}

void AboutListenerImpl::announced_callback(uv_async_t *handle, int status) {
  CallbackHolder* holder = (CallbackHolder*) handle->data;

  // TODO: pull this into a library that can
  // be shared with AboutProxyWrapper

  ajn::MsgArg objectDescriptionArg = *holder->objectDescriptionArg;

  // this objectDescription will head back to Node.js after we
  // populate it using the AllJoyn helper methods
  v8::Local<v8::Object> objectDescription = Nan::New<v8::Object>();

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
      Nan::Set(interfaceNames, j, Nan::New<v8::String>(intfs[j]).ToLocalChecked());
    }
    Nan::Set(objectDescription, Nan::New<v8::String>(paths[i]).ToLocalChecked(), interfaceNames);
    delete [] intfs;
  }

  // this aboutData object will head back to Node.js after we
  // populate it using the AllJoyn helper methods
  // const ajn::MsgArg* aboutDataArgIn = holder->aboutDataArg;
  // msgArgToObject(aboutDataArgIn, 0, aboutDataArgOut);
  v8::Local<v8::Object> aboutData = Nan::New<v8::Object>();
  ajn::MsgArg aboutDataArg = *holder->aboutDataArg;
  ajn::AboutData ajnAboutData(aboutDataArg);
  size_t count = ajnAboutData.GetFields();
  const char** fields = new const char*[count];
  ajnAboutData.GetFields(fields, count);

  for (size_t i = 0; i < count; ++i) {
    ajn::MsgArg* tmp;
    ajnAboutData.GetField(fields[i], tmp, NULL);
    if (tmp->Signature() == "s") {
      const char* tmp_s;
      tmp->Get("s", &tmp_s);
      Nan::Set(aboutData, Nan::New<v8::String>(fields[i]).ToLocalChecked(), Nan::New<v8::String>(tmp_s).ToLocalChecked());
    } else if (tmp->Signature() == "ay") {
      size_t lay;
      uint8_t* pay;
      tmp->Get("ay", &lay, &pay);
      v8::Local<v8::Array> v8_pay = v8::Array::New(lay);
      for (size_t j = 0; j < lay; ++j) {
        Nan::Set(v8_pay, j, Nan::New<v8::Integer>(pay[j]));
      }
      Nan::Set(aboutData, Nan::New<v8::String>(fields[i]).ToLocalChecked(), v8_pay);
    } else if (tmp->Signature() == "as") {
      // TODO: need to write a test to hit this block
      // it is only called after a session is created
      size_t las;
      ajn::MsgArg* as_arg;
      tmp->Get("as", &las, &as_arg);
      v8::Local<v8::Array> v8_las = v8::Array::New(las);
      for (size_t j = 0; j < las; ++j) {
        const char* tmp_s;
        as_arg[j].Get("s", &tmp_s);
        Nan::Set(v8_las, j, Nan::New<v8::String>(tmp_s).ToLocalChecked());
      }
      Nan::Set(aboutData, Nan::New<v8::String>(fields[i]).ToLocalChecked(), v8_las);
    }
  }
  delete [] fields;

  // Pass the v8 objects back to Node
  v8::Local<v8::Value> argv[] = {
    Nan::New<v8::String>(holder->busName).ToLocalChecked(),
    Nan::New<v8::Integer>(holder->version),
    Nan::New<v8::Integer>(holder->port),
    objectDescription,
    aboutData
  };
  holder->callback->Call(5, argv);
}

void AboutListenerImpl::Announced(const char * busName, uint16_t version, ajn::SessionPort port, const ajn::MsgArg & objectDescriptionArg, const ajn::MsgArg & aboutDataArg){
  announced_async.data = (void*) &announced;

  announced.busName = strdup(busName);
  announced.version = version;
  announced.port = port;
  announced.objectDescriptionArg = new ajn::MsgArg(objectDescriptionArg);
  announced.aboutDataArg = new ajn::MsgArg(aboutDataArg);
  uv_async_send(&announced_async);
}
