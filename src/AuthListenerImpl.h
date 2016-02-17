#ifndef LD_AUTHLISTENERIMPL_H
#define LD_AUTHLISTENERIMPL_H

#include <nan.h>
#include <uv.h>
#include <alljoyn/BusObject.h>
#include <AuthListener.h>
#include <TransportMask.h>
#include <alljoyn/AllJoynStd.h>

class AuthListenerImpl : public ajn::AuthListener {
private:
  uv_loop_t *loop;
  uv_async_t requestCredentials_async;
  uv_async_t authenticationComplete_async;

  struct CallbackHolder_requestCredentials {
    Nan::Callback* callback;
    const char* authMechanism;
    const char* authPeer;
    uint16_t authCount;
    const char* userId;
    uint16_t credMask;
    ajn::AuthListener::Credentials* creds;
    uv_mutex_t datalock;
    uv_cond_t datacond;
    bool rval;
    bool complete;
    CallbackHolder_requestCredentials() {
      uv_mutex_init(&datalock);
      uv_cond_init(&datacond);
    }
    ~CallbackHolder_requestCredentials() {
      uv_mutex_destroy(&datalock);
      uv_cond_destroy(&datacond);
    }
  } requestCredentials;

  struct CallbackHolder_authenticationComplete {
    Nan::Callback* callback;
    const char* authMechanism;
    const char* authPeer;
    bool success;
    uv_mutex_t datalock;
    CallbackHolder_authenticationComplete() {
      uv_mutex_init(&datalock);
    }
    ~CallbackHolder_authenticationComplete() {
      uv_mutex_destroy(&datalock);
    }
  } authenticationComplete;

public:
  AuthListenerImpl(Nan::Callback* requestCredentials, Nan::Callback* authenticationComplete);
  ~AuthListenerImpl();
  static void requestCredentials_callback(uv_async_t *handle, int status);
  static void authenticationComplete_callback(uv_async_t *handle, int status);

  virtual bool RequestCredentials(const char* authMechanism, const char* authPeer, uint16_t authCount, const char* userId, uint16_t credMask, ajn::AuthListener::Credentials& creds);
  virtual void AuthenticationComplete(const char* authMechanism, const char* authPeer, bool success);
};

#endif
