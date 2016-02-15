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
  uv_rwlock_t calllock;

  struct CallbackHolder_requestCredentials {
    NanCallback* callback;
    const char* authMechanism;
    const char* authPeer;
    uint16_t authCount;
    const char* userId;
    uint16_t credMask;
    ajn::AuthListener::Credentials* creds;
    uv_rwlock_t datalock;
    bool rval;
    bool complete;
  } requestCredentials;

  struct CallbackHolder_authenticationComplete {
    NanCallback* callback;
    const char* authMechanism;
    const char* authPeer;
    bool success;
    uv_rwlock_t datalock;
  } authenticationComplete;

public:
  AuthListenerImpl(NanCallback* requestCredentials, NanCallback* authenticationComplete);
  ~AuthListenerImpl();
  static void requestCredentials_callback(uv_async_t *handle, int status);
  static void authenticationComplete_callback(uv_async_t *handle, int status);

  virtual bool RequestCredentials(const char* authMechanism, const char* authPeer, uint16_t authCount, const char* userId, uint16_t credMask, ajn::AuthListener::Credentials& creds);
  virtual void AuthenticationComplete(const char* authMechanism, const char* authPeer, bool success);

};

#endif
