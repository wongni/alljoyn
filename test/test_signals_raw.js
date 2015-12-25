var assert = require('assert');
var alljoyn = require('../');

describe('Signals on a Connected AllJoyn Session', function() {
  this.timeout(15000);
  var ALL_GOOD = 0;
  var sessionId = 0;
  var portNumber = 27;
  var advertisedName = "org.alljoyn.bus.samples.chat.detroit";
  var bus = null;
  var inter = null;
  var listener = null;
  var chatObject = null;
    
  before(function(done){
    bus = alljoyn.BusAttachment("chat");
    inter = alljoyn.InterfaceDescription();
    listener = alljoyn.BusListener(
      function(name){
        sessionId = bus.joinSession(name, portNumber, 0);
        setTimeout(function(){
          chatObject.signal(null, sessionId, inter, "Chat", "Hello, I am the client!");
          done();
        }, 500);
      },
      function(name){
        console.log("LostAdvertisedName", name);
      },
      function(name){
        console.log("NameOwnerChanged", name);
      }
    );
    bus.createInterface("org.alljoyn.bus.samples.chat", inter);
    inter.addSignal("Chat", "s",  "msg");
    bus.registerBusListener(listener);
    bus.start();
    chatObject = alljoyn.BusObject("/chatService");
    chatObject.addInterface(inter);
    bus.registerSignalHandler(chatObject, function(msg, info){
      console.log(msg["0"]);
    }, inter, "Chat");
    bus.registerBusObject(chatObject);
    bus.connect();
    bus.findAdvertisedName('org.alljoyn.bus.samples.chat');
  });

  it('should send a message', function(done) {
    setTimeout(function(){
      chatObject.signal(null, sessionId, inter, "Chat", "Hello, I am a test!");
      done();
    }, 750);
  });

  it('should return a null connect spec', function(done){
    setTimeout(function(){
      assert.equal(bus.getConnectSpec(),'null:')
      done();
    }, 750);
  });
});