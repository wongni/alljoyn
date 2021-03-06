#ifndef LD_ALLJOYN_BINDING_UTIL_H
#define LD_ALLJOYN_BINDING_UTIL_H

#include "nan.h"
#include <alljoyn/AllJoynStd.h>

void msgArgToObject(const ajn::MsgArg* arg, size_t index, v8::Local<v8::Object> out);
ajn::MsgArg* objToMsgArg(v8::Local<v8::Value> obj);


#endif