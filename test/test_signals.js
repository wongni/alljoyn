var assert = require('assert');
var alljoyn = require('../');

describe('Signals on a Connected Session', function() {
  // this.timeout(15000);
  
  var ALL_GOOD = 0;

  var serviceInterfaceName = 'org.alljoyn.bus.samples.chat';
  var serviceObjectPath = '/chatService';
  var namePrefix = serviceInterfaceName + '.';
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
  var chatRoomName = 'Detroit';
  var advertisedChatRoomName = namePrefix + chatRoomName;
  var busListener = null;
  
  it('should be true that true is true', function(done) {
    assert.equal(true, true);
  });
  
  it('should load the alljoyn bus', function(done) {
    assert.equal(typeof alljoyn.BusObject, 'function');
    setTimeout(done, 500);
  });
  
  //TODO: setup should be pulled out and put somewhere else.
  it('should attach to the AllJoyn bus for chat sample app', function(done) {
    busAttachment = alljoyn.BusAttachment(applicationName);
    assert.equal(typeof busAttachment, 'object');
    setTimeout(done, 500);
  });

  it('should create an interface to the chat sample app', function(done) {
    busInterface = alljoyn.InterfaceDescription();
    assert.equal(typeof busInterface, 'object');
    assert.equal(busAttachment.createInterface(serviceInterfaceName, busInterface),ALL_GOOD);
    setTimeout(done, 500);
  });

  it('should add a signal to the interface', function(done) {
    assert.equal(busInterface.addSignal(signalName, signalSignature, signalArgumentNames, signalAnnotation), ALL_GOOD);
    setTimeout(done, 500);
  });

  it('should activate the interface', function(done) {
    assert.equal(busInterface.activate(),undefined);
    setTimeout(done, 500);
  });

  it('should register the bus listener', function(done) {
    busListener = alljoyn.BusListener(
      function(name){
        sessionID = busAttachment.joinSession(name, port, sessionID);
        setTimeout(function(){
          busObject.signal(null, sessionID, busInterface, "Chat", "Hello, I am a node.js client!");
        }, 1000);
      }, function(name){},function(name){}
    );
    assert.equal(busAttachment.registerBusListener(busListener), undefined);
    setTimeout(done, 500);
  });

  it('should start the bus', function(done) {
    assert.equal(busAttachment.start(), ALL_GOOD);
    setTimeout(done, 500);
  });

  it('should create the bus object that will send and receive signals', function(done) {
    busObject = alljoyn.BusObject(serviceObjectPath);
    assert.equal(typeof busObject, 'object');
    setTimeout(done, 500);
  });

  it('should register the bus object', function(done) {
    assert.equal(busAttachment.registerBusObject(busObject), ALL_GOOD);
    setTimeout(done, 500);
  });

  it('should connect to the bus with a null connect spec', function(done) {
    assert.equal(busAttachment.connect(), ALL_GOOD);
    assert.equal(busAttachment.getConnectSpec(),'null:')
    setTimeout(done, 500);
  });

  it('should discover a well-known adverstised name on the bus', function(done) {
    assert.equal(busAttachment.findAdvertisedName(advertisedChatRoomName), ALL_GOOD);
    setTimeout(done, 500);
  });

  it('should send a chat signal', function(done) {
    var chatMessage = '"Put your hands up 4 Detroit!" -Fedde le Grand, Matthew Dear & Disco D.';
    assert.equal(busObject.signal(null, sessionID, busInterface, signalName, chatMessage), ALL_GOOD);
    setTimeout(done, 500);
  });
});
