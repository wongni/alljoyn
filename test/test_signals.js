var assert = require('assert');
var alljoyn = require('../');

var ALL_GOOD = 0;

describe('Signals on a Connected Session', function() {
  var serviceInterfaceName = 'org.alljoyn.bus.samples.chat';
  var namePrefix = 'org.alljoyn.bus.samples.chat.';
  var serviceObjectPath = '/chatService';
  var applicationName = 'chat';
  var signalName = 'Chat';
  var signalSignature = 's';
  var signalArgumentNames = 'str';
  var signalAnnotation = 0;
  var port = 27;
  var sessionID = 0;
  var busAttachment = null;
  var busInterface = null;
  var busObject = null;

  var busListener = alljoyn.BusListener(
    function(name){
      sessionId = busAttachment.joinSession(name, port, sessionId);
      setTimeout(function(){
        busObject.signal(null, sessionId, busInterface, "Chat", "Hello, I am a node.js client!");
      }, 1000);
    },
    function(name){
    },
    function(name){
    }
  );
  
  it('should be true that true is true', function() {
    assert.equal(true, true);
  });
  
  //TODO: setup should be pulled out and put somewhere else.
  it('should attach to the AllJoyn bus for chat sample app', function() {
    busAttachment = alljoyn.BusAttachment(applicationName);
    assert.equal(typeof busAttachment, 'object');
  });

  it('should create an interface to the chat sample app', function() {
    busInterface = alljoyn.InterfaceDescription();
    assert.equal(typeof busInterface, 'object');
    assert.equal(busAttachment.createInterface(serviceInterfaceName, busInterface),ALL_GOOD);
  });

  it('should add a signal to the interface', function() {
    assert.equal(busInterface.addSignal(signalName, signalSignature, signalArgumentNames, signalAnnotation), ALL_GOOD);
  });

  it('should activate the interface', function() {
    assert.equal(busInterface.activate(),undefined);
  });

  it('should register the bus listener', function() {
    assert.equal(busAttachment.registerBusListener(busListener), undefined);
  });

  it('should start the bus', function() {
    assert.equal(busAttachment.start(), ALL_GOOD);
  });

  it('should create the bus object that will send and receive signals', function() {
    busObject = alljoyn.BusObject(serviceObjectPath);
    assert.equal(typeof busObject, 'object');
  });

  it('should register the bus object', function() {
    assert.equal(busAttachment.registerBusObject(busObject), ALL_GOOD);
  });

  it('should connect to the bus with a null connect spec', function() {
    assert.equal(busAttachment.connect(), ALL_GOOD);
    assert.equal(busAttachment.getConnectSpec(),'null:')
  });

  it('should discover a well-known adverstised name on the bus', function() {
    assert.equal(busAttachment.findAdvertisedName(serviceInterfaceName), ALL_GOOD);
  });

});
