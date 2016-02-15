#include "nan.h"

#include "util.h"
#include "AuthListenerImpl.h"
#include <alljoyn/AllJoynStd.h>

AuthListenerImpl::AuthListenerImpl(NanCallback* requestCredentialsCallback, NanCallback* authenticationCompleteCallback){
  loop = uv_default_loop();
  requestCredentials.callback = requestCredentialsCallback;
  authenticationComplete.callback = authenticationCompleteCallback;
  uv_async_init(loop, &requestCredentials_async, requestCredentials_callback);
  uv_async_init(loop, &authenticationComplete_async, authenticationComplete_callback);
  uv_rwlock_init(&requestCredentials.datalock);
  uv_rwlock_init(&authenticationComplete.datalock);
}

AuthListenerImpl::~AuthListenerImpl(){
  uv_rwlock_destroy(&requestCredentials.datalock);
  uv_rwlock_destroy(&authenticationComplete.datalock);
}

void AuthListenerImpl::requestCredentials_callback(uv_async_t *handle, int status) {
  CallbackHolder_requestCredentials* holder = (CallbackHolder_requestCredentials*) handle->data;

  ajn::AuthListener::Credentials creds = *holder->creds;

  // this credentials will head back to Node.js after we
  // populate it using the AllJoyn helper methods
  v8::Local<v8::Object> credentials = v8::Object::New();

  credentials->Set(NanNew<v8::String>("password"), NanNew<v8::String>(creds.GetPassword().c_str()));
  credentials->Set(NanNew<v8::String>("userName"), NanNew<v8::String>(creds.GetUserName().c_str()));
  credentials->Set(NanNew<v8::String>("certChain"), NanNew<v8::String>(creds.GetCertChain().c_str()));
  credentials->Set(NanNew<v8::String>("privateKey"), NanNew<v8::String>(creds.GetPrivateKey().c_str()));
  credentials->Set(NanNew<v8::String>("logonEntry"), NanNew<v8::String>(creds.GetLogonEntry().c_str()));
  credentials->Set(NanNew<v8::String>("expiration"), NanNew<v8::Uint32>(creds.GetExpiration()));

  // Pass the v8 objects back to Node
  v8::Handle<v8::Value> argv[] = {
    NanNew<v8::String>(holder->authMechanism),
    NanNew<v8::String>(holder->authPeer),
    NanNew<v8::Integer>(holder->authCount),
    NanNew<v8::String>(holder->userId),
    NanNew<v8::Integer>(holder->credMask),
    credentials
  };
  holder->callback->Call(6, argv);
}

bool AuthListenerImpl::RequestCredentials(const char* authMechanism, const char* authPeer, uint16_t authCount, const char* userId, uint16_t credMask, ajn::AuthListener::Credentials& creds){
  requestCredentials_async.data = (void*) &requestCredentials;

  requestCredentials.authMechanism = strdup(authMechanism);
  requestCredentials.authPeer = strdup(authPeer);
  requestCredentials.authCount = authCount;
  requestCredentials.userId = strdup(userId);
  requestCredentials.credMask = credMask;
  requestCredentials.creds = new ajn::AuthListener::Credentials(creds);
  uv_async_send(&requestCredentials_async);

  while(!requestCredentials.complete){sleep(1);}
  return requestCredentials.rval;
}

void AuthListenerImpl::authenticationComplete_callback(uv_async_t *handle, int status) {
  CallbackHolder_authenticationComplete* holder = (CallbackHolder_authenticationComplete*) handle->data;

  // Pass the v8 objects back to Node
  v8::Handle<v8::Value> argv[] = {
    NanNew<v8::String>(holder->authMechanism),
    NanNew<v8::String>(holder->authPeer),
    NanNew<v8::Boolean>(holder->success)
  };
  holder->callback->Call(3, argv);
}

void AuthListenerImpl::AuthenticationComplete(const char* authMechanism, const char* authPeer, bool success){
  authenticationComplete_async.data = (void*) &authenticationComplete;

  authenticationComplete.authMechanism = strdup(authMechanism);
  authenticationComplete.authPeer = strdup(authPeer);
  authenticationComplete.success = success;
  uv_async_send(&authenticationComplete_async);
}
