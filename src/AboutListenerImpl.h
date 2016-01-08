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

  public:
  	AboutListenerImpl();
  	~AboutListenerImpl();

    virtual void Announced(const char* busName, uint16_t version, ajn::SessionPort port, const ajn::MsgArg& objectDescriptionArg, const ajn::MsgArg& aboutDataArg);
};

#endif
