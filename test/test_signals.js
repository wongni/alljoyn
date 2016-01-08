// for this suite to work, the AllJoyn sample chat app must be running
// how to run it? after running npm install:
// > <project_root>/build/Release/sample-chat -s detroit

// TODO: start, inspect and stop OS process from within the test
 // http://stackoverflow.com/questions/20643470/execute-a-command-line-binary-with-node-js

var assert = require('assert');
var alljoyn = require('../');

describe('Signals on a Connected AllJoyn Session', function() {
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
  var busName = null;
  var chatRoomName = 'detroit';
  var advertisedChatRoomName = namePrefix + chatRoomName;
  var busListener = null;
  var foundAdvertisedNameAlreadyCalled = false;
    
  // i wish the before all function weren't so sloppy.
  // is this a function of AllJoyn being sloppy?
  before(function(done){
    // sanity check test suite
    assert.equal(true, true);
    // sanity check AllJoyn bus
    assert.equal(typeof alljoyn.BusObject, 'function');
    // attach to the AllJoyn bus for chat sample app in
    // <project_root>/alljoyn/alljoyn_core/samples/chat/linux/chat.cc
    // by first running: <project_root>/build/Release/sample-chat -s detroit
    busAttachment = alljoyn.BusAttachment(applicationName);
    assert.equal(typeof busAttachment, 'object');
    // create an interface to the chat sample app
    busInterface = alljoyn.InterfaceDescription();
    assert.equal(typeof busInterface, 'object');
    // create the interface on the bus attachment
    assert.equal(busAttachment.createInterface(serviceInterfaceName, busInterface),ALL_GOOD);
    // add a signal
    assert.equal(busInterface.addSignal(signalName, signalSignature, signalArgumentNames, signalAnnotation), ALL_GOOD);
    // register a buslistener
    busListener = alljoyn.BusListener(
      function(name){
        console.log('FoundAdvertisedName', name);
        if (foundAdvertisedNameAlreadyCalled) {
        } else {
          sessionID = busAttachment.joinSession(name, port, sessionID);
          busObject.signal(null, sessionID, busInterface, signalName, 'Hello, I am the client!');
          foundAdvertisedNameAlreadyCalled = true;
          done();
        }
      },
      function(name){
        console.log('LostAdvertisedName', name);
      },
      function(name){
        console.log('NameOwnerChanged', name);
      }
    );
    assert.equal(busAttachment.registerBusListener(busListener), undefined);
    // start the bus attachment
    assert.equal(busAttachment.start(), ALL_GOOD);
    // create the bus object that will send and receive signals
    busObject = alljoyn.BusObject(serviceObjectPath);
    assert.equal(typeof busObject, 'object');
    // add interface
    assert.equal(busObject.addInterface(busInterface), ALL_GOOD);
    // register signal handler
    assert.equal(busAttachment.registerSignalHandler(busObject, function(msg, info){
      console.log(msg[0]);
    }, busInterface, signalName), ALL_GOOD);
    // register bus object
    assert.equal(busAttachment.registerBusObject(busObject), ALL_GOOD);
    // connect to bus
    assert.equal(busAttachment.connect(), ALL_GOOD);
    busName = busAttachment.getUniqueName();

    // try to ping the bus attachment
    assert.equal(busAttachment.ping(busName,5000), ALL_GOOD);

    // find advertised name
    assert.equal(busAttachment.findAdvertisedName(advertisedChatRoomName), ALL_GOOD);

  });

  it('should send a message', function() {
    var chatMessage = '"Put your hands up 4 Detroit!" -Fedde le Grand, Matthew Dear & Disco D.';
    busObject.signal(null, sessionID, busInterface, signalName, chatMessage);
  });

  it('should return a null connect spec', function(){
    assert.equal(busAttachment.getConnectSpec(),'null:');
  });
  
  it('should unregister the bus listener ', function(){
    assert.equal(busAttachment.unregisterBusListener(busListener),undefined)
  });
    
});