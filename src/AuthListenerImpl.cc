#include "nan.h"

#include "util.h"
#include "AuthListenerImpl.h"
#include <alljoyn/AllJoynStd.h>

static void SetCredentials(ajn::AuthListener::Credentials* creds, char * credentials);

AuthListenerImpl::AuthListenerImpl(Nan::Callback* requestCredentialsCallback, Nan::Callback* authenticationCompleteCallback){
  loop = uv_default_loop();
  requestCredentials.callback = requestCredentialsCallback;
  authenticationComplete.callback = authenticationCompleteCallback;
  uv_async_init(loop, &requestCredentials_async, requestCredentials_callback);
  uv_async_init(loop, &authenticationComplete_async, authenticationComplete_callback);
}

AuthListenerImpl::~AuthListenerImpl() {
}

void AuthListenerImpl::requestCredentials_callback(uv_async_t *handle, int status) {
  CallbackHolder_requestCredentials* holder = (CallbackHolder_requestCredentials*) handle->data;

  uv_mutex_lock(&holder->datalock);

  // Pass the v8 objects back to Node
  v8::Local<v8::Value> argv[] = {
    Nan::New<v8::String>(holder->authMechanism).ToLocalChecked(),
    Nan::New<v8::String>(holder->authPeer).ToLocalChecked(),
    Nan::New<v8::Integer>(holder->authCount),
    Nan::New<v8::String>(holder->userId).ToLocalChecked(),
    Nan::New<v8::Integer>(holder->credMask)
  };

  v8::Handle<v8::Value> retVal = holder->callback->Call(5, argv);
  char * credentials = strdup(*v8::String::Utf8Value(retVal));
  if (!strcmp(credentials, "undefined") || !strcmp(credentials, "NULL")) {
    holder->rval = false;
  }
  else {
    SetCredentials(holder->creds, credentials);
    holder->rval = true;
  }

  holder->complete = true;
  uv_cond_signal(&holder->datacond);
  uv_mutex_unlock(&holder->datalock);
}

static void SetCredentials(ajn::AuthListener::Credentials* creds, char * credentials) {
  char * username = NULL;
  char * password = NULL;
  char * token = NULL;

  token = strchr(credentials, ':');
  if (token == NULL) {
    password = credentials;
  }
  else {
    token[0] = '\0';
    username = credentials;
    password = &token[1];
  }

  creds->SetUserName(username);
  creds->SetPassword(password);
}

bool AuthListenerImpl::RequestCredentials(const char* authMechanism, const char* authPeer, uint16_t authCount, const char* userId, uint16_t credMask, ajn::AuthListener::Credentials& creds){
  requestCredentials_async.data = (void*) &requestCredentials;

  uv_mutex_lock(&requestCredentials.datalock);
  requestCredentials.authMechanism = strdup(authMechanism);
  requestCredentials.authPeer = strdup(authPeer);
  requestCredentials.authCount = authCount;
  requestCredentials.userId = strdup(userId);
  requestCredentials.credMask = credMask;
  requestCredentials.creds = &creds;
  requestCredentials.complete = false;
  requestCredentials.rval = false;
  uv_async_send(&requestCredentials_async);

  while(!requestCredentials.complete)
    uv_cond_wait(&requestCredentials.datacond, &requestCredentials.datalock);

  uv_mutex_unlock(&requestCredentials.datalock);

  return requestCredentials.rval;
}

void AuthListenerImpl::authenticationComplete_callback(uv_async_t *handle, int status) {
  CallbackHolder_authenticationComplete* holder = (CallbackHolder_authenticationComplete*) handle->data;

  // Pass the v8 objects back to Node
  v8::Local<v8::Value> argv[] = {
    Nan::New<v8::String>(holder->authMechanism).ToLocalChecked(),
    Nan::New<v8::String>(holder->authPeer).ToLocalChecked(),
    Nan::New<v8::Boolean>(holder->success)
  };
  holder->callback->Call(3, argv);
}

void AuthListenerImpl::AuthenticationComplete(const char* authMechanism, const char* authPeer, bool success){
  authenticationComplete_async.data = (void*) &authenticationComplete;

  uv_mutex_lock(&authenticationComplete.datalock);
  authenticationComplete.authMechanism = strdup(authMechanism);
  authenticationComplete.authPeer = strdup(authPeer);
  authenticationComplete.success = success;
  uv_async_send(&authenticationComplete_async);
  uv_mutex_unlock(&authenticationComplete.datalock);
}
