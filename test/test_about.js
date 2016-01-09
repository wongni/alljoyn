// for this suite to work, the AllJoyn AboutPlusService sample must be running
// how to run it? after running npm install:
// > <project_root>/build/Release/sample-about-plus-service

// TODO: start, inspect and stop OS process from within the test
// http://stackoverflow.com/questions/20643470/execute-a-command-line-binary-with-node-js
// Or just start the service side from the test too

var util = require('util');
var assert = require('assert');
var alljoyn = require('../');

var ALL_GOOD = 0;

var setupBusAttachment = function(applicationName) {
  // sanity check test suite
  assert.equal(true, true);
  // sanity check AllJoyn bus
  assert.equal(typeof alljoyn.BusObject, 'function');

  // for this suite to work, the AllJoyn AboutPlusService sample must be running
  // how to run it? after running npm install, run from the command line:
  // ./build/Release/sample-about-plus-service
  busAttachment = alljoyn.BusAttachment(applicationName, true);
  assert.equal(typeof busAttachment, 'object');

  // start the bus attachment
  assert.equal(busAttachment.start(), ALL_GOOD);

  // connect to bus
  assert.equal(busAttachment.connect(), ALL_GOOD);
  return busAttachment;
}

describe('An AllJoyn about announcement', function() {
  var aboutListenerWasCalled = false;
  var sessionlessData = {};
  var sessionfulData = {};

  before(function(done){
    var applicationName = 'AboutPlusServiceTest';
    var serviceInterfaceName = 'com.example.about.feature.interface.sample';

    // setup the Bus Attachment with the Application Name
    var busAttachment = setupBusAttachment(applicationName);

    // create an Announced callback that will get called
    // after the AboutListener is registered and the
    // WhoImplements function is called
    var announcedCallback = function(busName, version, port, objectDescription, aboutData){
      aboutListenerWasCalled = true;
      sessionlessData.busName = busName;
      sessionlessData.version = version;
      sessionlessData.port = port;
      sessionlessData.objectDescription = objectDescription;
      sessionlessData.aboutData = aboutData;
      
      // once the Announced callback has fired let's go ahead and 
      // join the session and get more About info
      var sessionID = 0;
      sessionID = busAttachment.joinSession(busName, port, sessionID);
      assert(sessionID > 0);
      sessionfulData.sessionID = sessionID;
      
      // let's get the About proxy
      var aboutProxy = alljoyn.AboutProxy(busAttachment, busName, sessionID);
      assert.equal(typeof(aboutProxy), 'object');

      // now that we have the About proxy we can grab the Object Description
      // sessionfulData.objectDescription = aboutProxy.getObjectDescription();

      // and the About Data
      // sessionfulData.aboutData = aboutProxy.getAboutData("en");
      
      // the done function in this callback tells the test framework
      // that the 'before' work is done and now we can proceed to the tests
      done();
    }

    // create a new About Listener
    var aboutListener = alljoyn.AboutListener(announcedCallback);

    //register the About Listener with the Announced Callback
    assert.equal(busAttachment.registerAboutListener(aboutListener), undefined);
    
    // call WhoImplements on the service interface name 
    // to trigger the Announced Callback
    assert.equal(busAttachment.whoImplements([serviceInterfaceName]), ALL_GOOD);
    
  });

  it('should call AboutListener after whoImplements is called', function() {
    assert.equal(aboutListenerWasCalled, true);
  });
  it('should have a bus name', function() {
    assert.equal(typeof(sessionlessData.busName), 'string');
    assert(sessionlessData.busName.length > 0);
  });
  it('should have a version number', function() {
    assert.equal(typeof(sessionlessData.version), 'number');
    assert(sessionlessData.version > 0);
  });
  it('should have a port number', function() {
    assert.equal(typeof(sessionlessData.port), 'number');
    assert(sessionlessData.port > 0);
  });
  it('should have an Object description', function() {
    assert.equal(typeof(sessionlessData.objectDescription), 'object');
    var objectDescriptionKeys = Object.keys(sessionlessData.objectDescription);
    assert(objectDescriptionKeys.length > 0);
    assert(objectDescriptionKeys.indexOf('/example/path') > -1);
    assert.equal(Object.prototype.toString.call( sessionlessData.objectDescription['/example/path'] ), '[object Array]');
    assert.equal(sessionlessData.objectDescription['/example/path'][0], 'com.example.about.feature.interface.sample');
  });
  it('should have About data', function() {
    assert.equal(typeof(sessionlessData.aboutData), 'object');
  });
  it('should have a valid DeviceID', function() {
    assert.equal(sessionlessData.aboutData['DeviceId'], '93c06771-c725-48c2-b1ff-6a2a59d445b8');
  });
  it('should have a valid model number', function() {
    assert.equal(sessionlessData.aboutData['ModelNumber'],'123456');
  });
  it('should have a valid hex AppId', function() {
    assert.equal(sessionlessData.aboutData['AppId'][1].toString(16),'b3');
  });
  it('should have the 7 metadata fields published the Announce signal', function() {
    assert.equal(Object.keys(sessionlessData.aboutData).length, 7);
  });
  it('should give us more About information after joining a session', function() {
    assert.equal(Object.keys(sessionfulData.aboutData).length, 500);
  });
});
