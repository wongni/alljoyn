#ifndef LD_ABOUTPROXYIMPL_H
#define LD_ABOUTPROXYIMPL_H

#include <nan.h>
#include <uv.h>
#include <alljoyn/BusObject.h>
#include <AboutProxy.h>
#include <TransportMask.h>
#include <alljoyn/AllJoynStd.h>

class AboutProxyImpl : public ajn::AboutProxy {
private:

public:
  AboutProxyImpl(ajn::BusAttachment* busAttachment, const char* busName, ajn::SessionId sessionId);
  ~AboutProxyImpl();
};

#endif
