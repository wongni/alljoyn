// for this suite to work, the AllJoyn AboutPlusService sample must be running
// how to run it? after running npm install:
// > <project_root>/build/Release/sample-mock-device-discovery

// TODO: start, inspect and stop OS process from within the test
// http://stackoverflow.com/questions/20643470/execute-a-command-line-binary-with-node-js
// Or just start the service side from the test too

var util = require('util');
var assert = require('assert');
var alljoyn = require('../');

var ALL_GOOD = 0;
var SESSION_PORT = 900;
var SERVICE_INTERFACE_NAME = 'com.se.bus.discovery';
var BUS_OBJECT_PATH = '/example/path'

var sessionlessData = {};
var sessionfulData = {};


var setupClientBusAttachment = function(clientApplicationName) {
  // sanity check test suite
  assert.equal(true, true);
  // sanity check AllJoyn bus
  assert.equal(typeof alljoyn.BusAttachment, 'function');

  // for this suite to work, the AllJoyn AboutPlusService sample must be running
  // how to run it? after running npm install, run from the command line:
  // ./build/Release/sample-about-plus-service
  clientBusAttachment = alljoyn.BusAttachment(clientApplicationName, true);
  assert.equal(typeof clientBusAttachment, 'object');

  // start the bus attachment
  assert.equal(clientBusAttachment.start(), ALL_GOOD);

  // connect to bus
  assert.equal(clientBusAttachment.connect(), ALL_GOOD);
  return clientBusAttachment;
}

var sessionPortListenerCallback = alljoyn.SessionPortListener(
  function(port, joiner){
    console.log("AcceptSessionJoiner", port, joiner);
    if (port != SESSION_PORT) {
      return false;
    } else {
      return true;
    }
  },
  function(port, sessionId, joiner){
    console.log("SessionJoined", port, sessionId, joiner);
  }
);

describe('An AllJoyn about announcement', function() {
  var aboutListenerWasCalled = false;
  var clientBusAttachment = null;
  
  var clientApplicationName = 'AboutPlusServiceTest';
  
  before(function(done){

    // setup the service Bus Attachment with the Application Name
    // setup the client Bus Attachment with the Application Name
    clientBusAttachment = setupClientBusAttachment(clientApplicationName);
    var clientBusUniqueName = clientBusAttachment.getUniqueName();
    console.log('clientBusAttachment: ' + util.inspect(clientBusAttachment));

    //TODO: do some assertion testing on the two bus attachments.
    assert.equal(typeof(clientBusUniqueName), 'string');
    assert.equal(clientBusUniqueName.length, 11);

    // create an Announced callback that will get called
    // after the AboutListener is registered and the
    // WhoImplements function is called
    var announcedCallback = function(busName, version, port, objectDescription, aboutData){
      console.log('announcedCallback');
      aboutListenerWasCalled = true;
      sessionlessData.busName = busName;
      sessionlessData.version = version;
      sessionlessData.port = port;
      sessionlessData.objectDescription = objectDescription;
      sessionlessData.aboutData = aboutData;
  
      // once the Announced callback has fired let's go ahead and 
      // join the session and get more About info
      var sessionId = 0;

      sessionId = clientBusAttachment.joinSession(busName, port, sessionId);
      // if the returned sessionId a string then it's an error message
      // number is good
      assert.equal(typeof(sessionId),'number');
  
      // let's get the About proxy
      var aboutProxy = alljoyn.AboutProxy(clientBusAttachment, busName, sessionId);
      assert.equal(typeof(aboutProxy), 'object');

      sessionfulData.sessionId = aboutProxy.getSessionId();
      assert.equal(sessionfulData.sessionId, sessionId);
      sessionfulData.uniqueName = aboutProxy.getUniqueName();
      assert.equal(sessionfulData.uniqueName, busName);

      // now that we have an About Proxy we can grab the Object Description,
      // About Data and Version.
      sessionfulData.version = aboutProxy.getVersion();
      sessionfulData.objectDescription = aboutProxy.getObjectDescription();
      sessionfulData.aboutData = aboutProxy.getAboutData('en');

      var paths = Object.keys(sessionlessData.objectDescription);
      for (i = 0; i < paths.length; i++) {
        var proxyBusObject = alljoyn.ProxyBusObject(clientBusAttachment, busName, paths[i], sessionId);
        var interfaceNames = proxyBusObject.getInterfaceNames();
        assert.equal(interfaceNames.length, 4);
        assert.equal(interfaceNames[0], 'com.se.bus.discovery');
        assert.equal(interfaceNames[1], 'org.allseen.Introspectable');
        assert.equal(interfaceNames[2], 'org.freedesktop.DBus.Introspectable');
        assert.equal(interfaceNames[3], 'org.freedesktop.DBus.Peer');
        var numberOfMembersPerInterface = [36, 2, 1, 2];
        for (j = 0; j < interfaceNames.length; j++) {
          var serviceInterfaceDescription = alljoyn.InterfaceDescription();
          proxyBusObject.getInterface(interfaceNames[j], serviceInterfaceDescription);
          assert.notEqual(serviceInterfaceDescription, null);
          
          var members = serviceInterfaceDescription.getMembers();
          assert.equal(members.length,numberOfMembersPerInterface[j]);
          for (k = 0; k < members.length; k++) {
            var member = members[k];
            assert.notEqual(member, null);
            assert.equal(typeof(member.memberType), 'number');
            assert.equal(typeof(member.name), 'string');
            assert.equal(typeof(member.signature), 'string');
            assert.equal(typeof(member.returnSignature), 'string');
            assert.equal(typeof(member.argNames), 'string');
            assert.equal(typeof(member.accessPerms), 'string');
            assert.equal(typeof(member.description), 'string');
            assert.equal(typeof(member.isSessionlessSignal), 'boolean');
            assert.equal(Object.keys(member).length,8);
          }
        }
      }

      // the done function in this callback tells the test framework
      // that the 'before' work is done and now we can proceed to the tests
      done();
    }

    // create a new About Listener
    var aboutListener = alljoyn.AboutListener(announcedCallback);

    //register the About Listener with the Announced Callback
    assert.equal(clientBusAttachment.registerAboutListener(aboutListener), undefined);
    
    // call WhoImplements on the service interface name 
    // to trigger the Announced Callback
    assert.equal(clientBusAttachment.whoImplements([SERVICE_INTERFACE_NAME]), ALL_GOOD);
    console.log('whoImplements');
    
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
    assert(objectDescriptionKeys.indexOf(BUS_OBJECT_PATH) > -1);
    assert.equal(Object.prototype.toString.call( sessionlessData.objectDescription[BUS_OBJECT_PATH] ), '[object Array]');
    assert.equal(sessionlessData.objectDescription[BUS_OBJECT_PATH][0], SERVICE_INTERFACE_NAME);
  });
  it('should have About data', function() {
    assert.equal(typeof(sessionlessData.aboutData), 'object');
  });
  it('should have a valid DeviceID', function() {
    assert.equal(sessionlessData.aboutData['DeviceId'], '93c06771-c725-48c2-b1ff-6a2a59d445b8');
  });
  it('should have a valid model number', function() {
    assert.equal(sessionlessData.aboutData['ModelNumber'],'0001');
  });
  it('should have a valid hex AppId', function() {
    assert.equal(sessionlessData.aboutData['AppId'][1].toString(16),'b3');
  });
  it('should have the 7 metadata fields published the Announce signal', function() {
    assert.equal(Object.keys(sessionlessData.aboutData).length, 7);
  });
  it('should have matching version numbers before and after joining a session', function() {
    assert.equal(sessionfulData.version, sessionlessData.version);
  });
  it('should give us a matching Object Description after joining a session', function() {
    assert.equal(Object.prototype.toString.call( sessionfulData.objectDescription[BUS_OBJECT_PATH] ), '[object Array]');
    assert.equal(sessionfulData.objectDescription[BUS_OBJECT_PATH][0], SERVICE_INTERFACE_NAME);
  });
  it('should have About data', function() {
    assert.equal(typeof(sessionfulData.aboutData), 'object');
  });
  it('should have a valid DeviceID', function() {
    assert.equal(sessionfulData.aboutData['DeviceId'], '93c06771-c725-48c2-b1ff-6a2a59d445b8');
  });
  it('should have a valid model number', function() {
    assert.equal(sessionfulData.aboutData['ModelNumber'],'0001');
  });
  it('should have a valid hex AppId', function() {
    assert.equal(sessionfulData.aboutData['AppId'][1].toString(16),'b3');
  });
  it('should have the 14 metadata fields published the Announce signal', function() {
    assert.equal(Object.keys(sessionfulData.aboutData).length, 14);
  });
});
