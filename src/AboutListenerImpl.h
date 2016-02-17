#ifndef LD_ABOUTLISTENERIMPL_H
#define LD_ABOUTLISTENERIMPL_H

#include <nan.h>
#include <uv.h>
#include <alljoyn/BusObject.h>
#include <AboutListener.h>
#include <TransportMask.h>
#include <alljoyn/AllJoynStd.h>

class AboutListenerImpl : public ajn::AboutListener {
private:
  uv_loop_t *loop;
  uv_async_t announced_async;
  uv_rwlock_t calllock;

  struct CallbackHolder{
    Nan::Callback* callback;
    const char* busName;
    int version;
    int port;
    ajn::MsgArg* objectDescriptionArg;
    ajn::MsgArg* aboutDataArg;
  } announced;

public:
  AboutListenerImpl(Nan::Callback* announced);
  ~AboutListenerImpl();
  static void announced_callback(uv_async_t *handle, int status);

  virtual void Announced(const char* busName, uint16_t version, ajn::SessionPort port, const ajn::MsgArg& objectDescriptionArg, const ajn::MsgArg& aboutDataArg);

};

#endif
