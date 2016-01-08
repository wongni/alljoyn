// for this suite to work, the AllJoyn sample chat app must be running
// how to run it? after running npm install:
// > <project_root>/build/Release/sample-about-plus-service

// TODO: start, inspect and stop OS process from within the test
// http://stackoverflow.com/questions/20643470/execute-a-command-line-binary-with-node-js

var util = require('util');
var assert = require('assert');
var alljoyn = require('../');
var aboutListenerWasCalled = false;

var actualVersion = null;
var actualBusName = null;
var actualPort = null;
var actualAboutDataArg = null;
var actualObjectDescription = null;

describe('An AllJoyn about announcement', function() {
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
      function(busName, version, port, objectDescription, aboutDataArg){
        console.log('Announce signal discovered from bus', busName);
        console.log('busName: ' + busName);
        actualBusName = busName;
        console.log('version: ' + version);
        actualVersion = version;
        console.log('port: ' + port);
        actualPort = port;
        console.log('objectDescription: ' + util.inspect(objectDescription));
        actualObjectDescription = objectDescription;
        console.log('actualObjectDescription: ' + util.inspect(actualObjectDescription));
        console.log('aboutDataArg: ' + util.inspect(aboutDataArg));
        actualAboutDataArg = aboutDataArg;
        aboutListenerWasCalled = true;
        done();
      }
    );
    assert.equal(busAttachment.registerAboutListener(aboutListener), undefined);
    assert.equal(busAttachment.whoImplements([serviceInterfaceName]), ALL_GOOD);
  });

  it('should call AboutListener after whoImplements is called', function() {
    assert.equal(aboutListenerWasCalled, true);
  });
  it('should have a bus name', function() {
    assert.equal(typeof(actualBusName), 'string');
    assert(actualBusName.length > 0);
  });
  it('should have a version number', function() {
    assert.equal(typeof(actualVersion), 'number');
    assert(actualVersion > 0);
  });
  it('should have a port number', function() {
    assert.equal(typeof(actualPort), 'number');
    assert(actualPort > 0);
  });
  it('should have an Object description', function() {
    assert.equal(typeof(actualObjectDescription), 'object');
    var objectDescriptionKeys = Object.keys(actualObjectDescription);
    assert(objectDescriptionKeys.length > 0);
    assert(objectDescriptionKeys.indexOf('/example/path') > -1);
    assert.equal(Object.prototype.toString.call( actualObjectDescription['/example/path'] ), '[object Array]');
    assert.equal(actualObjectDescription['/example/path'][0], 'com.example.about.feature.interface.sample');
  });
  it('should have About data', function() {
    assert.equal(typeof(actualAboutDataArg), 'object');
    assert(Object.keys(actualAboutDataArg).length > 0);
  });
  
});