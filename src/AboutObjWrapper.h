#ifndef LD_ABOUTOBJWRAPPER_H
#define LD_ABOUTOBJWRAPPER_H

#include <nan.h>
#include <uv.h>
#include <alljoyn/BusObject.h>
#include <AboutObj.h>
#include <TransportMask.h>
#include <alljoyn/AllJoynStd.h>

NAN_METHOD(AboutObjConstructor);

class AboutObjWrapper : public node::ObjectWrap {
  private:

    static NAN_METHOD(New);
    static NAN_METHOD(Announce);

  public:
    AboutObjWrapper(ajn::BusAttachment* bus);
    ~AboutObjWrapper();
    static void Init ();
    static v8::Handle<v8::Value> NewInstance();

    ajn::AboutObj *aboutobj;
};

#endif