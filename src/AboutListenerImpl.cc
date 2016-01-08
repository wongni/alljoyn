#include "nan.h"

#include "AboutListenerImpl.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

AboutListenerImpl::AboutListenerImpl(){
}

AboutListenerImpl::~AboutListenerImpl(){
}

void AboutListenerImpl::Announced(const char* busName, uint16_t version, ajn::SessionPort port, const ajn::MsgArg& objectDescriptionArg, const ajn::MsgArg& aboutDataArg){
}
