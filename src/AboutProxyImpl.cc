#include "nan.h"

#include "util.h"
#include "AboutProxyImpl.h"
#include <alljoyn/AboutData.h>
#include <alljoyn/AboutObjectDescription.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

AboutProxyImpl::AboutProxyImpl(ajn::BusAttachment* busAttachment, const char* busName, ajn::SessionId sessionId) : ajn::AboutProxy(*busAttachment, busName, sessionId) {
  
}

AboutProxyImpl::~AboutProxyImpl(){
}

