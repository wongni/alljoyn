#ifndef LD_ABOUTDATAWRAPPER_H
#define LD_ABOUTDATAWRAPPER_H

#include <nan.h>
#include <uv.h>
#include <alljoyn/BusObject.h>
#include <AboutData.h>
#include <TransportMask.h>
#include <alljoyn/AllJoynStd.h>

NAN_METHOD(AboutDataConstructor);

class AboutDataWrapper : public Nan::ObjectWrap {
  private:
    static NAN_METHOD(New);
    static NAN_METHOD(IsValid);
    static NAN_METHOD(SetAppId);
    static NAN_METHOD(SetDeviceName);
    static NAN_METHOD(SetDeviceId);
    static NAN_METHOD(SetAppName);
    static NAN_METHOD(SetManufacturer);
    static NAN_METHOD(SetModelNumber);
    static NAN_METHOD(SetDescription);
    static NAN_METHOD(SetDateOfManufacture);
    static NAN_METHOD(SetSoftwareVersion);
    static NAN_METHOD(SetHardwareVersion);
    static NAN_METHOD(SetSupportUrl);

  public:
    AboutDataWrapper(const char* languageTag);
    ~AboutDataWrapper();
    static void Init ();
    static v8::Handle<v8::Value> NewInstance();

    ajn::AboutData *aboutdata;
};

#endif