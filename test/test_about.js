// for this suite to work, the AllJoyn sample chat app must be running
// how to run it? after running npm install:
// > <project_root>/build/Release/sample-about-plus-service

// TODO: start, inspect and stop OS process from within the test
 // http://stackoverflow.com/questions/20643470/execute-a-command-line-binary-with-node-js

var assert = require('assert');
var alljoyn = require('../');
var good = false;

describe('How an AllJoyn client listens for an About announcement', function() {
  var ALL_GOOD = 0;
  var serviceInterfaceName = 'com.example.about.feature.interface.sample';
  var serviceObjectPath = '/chatService';
  var namePrefix = serviceInterfaceName + '.';
  var applicationName = 'AboutPlusServiceTest';
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
    busAttachment = alljoyn.BusAttachment(applicationName, true);
    assert.equal(typeof busAttachment, 'object');

    // start the bus attachment
    assert.equal(busAttachment.start(), ALL_GOOD);

    // connect to bus
    assert.equal(busAttachment.connect(), ALL_GOOD);

    // register a buslistener
    aboutListener = alljoyn.AboutListener(
      function(busName, version, port, objectDescriptionArg, aboutDataArg){
        console.log('Announce signal discovered from bus', busName);
        console.log('busName: ' + busName);
        console.log('version: ' + version);
        console.log('port: ' + port);
        console.log('objectDescriptionArg: ' + objectDescriptionArg);
        console.log('aboutDataArg: ' + aboutDataArg);
        good = true;
        done();
      }
    );
    assert.equal(busAttachment.registerAboutListener(aboutListener), undefined);
    assert.equal(busAttachment.whoImplements([serviceInterfaceName]), ALL_GOOD);
  });

  it('should respond to whoImplements', function() {
    assert.equal(good, true);
  });
  
});