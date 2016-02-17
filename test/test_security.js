// globals
global.assert = require('assert');

global.aboutListenerWasCalled = false;
global.clientBusAttachment = null;

global.sessionlessData = {};
global.sessionfulData = {};

// setup
var alljoyn = require('../');
var xml = require('xml2json');

var util = require('util');


var ALL_GOOD = 0;
var SESSION_PORT = 900;


var MESSAGE_INVALID     = 0; ///< an invalid message type
var MESSAGE_METHOD_CALL = 1; ///< a method call message type
var MESSAGE_METHOD_RET  = 2; ///< a method return message type
var MESSAGE_ERROR       = 3; ///< an error message type
var MESSAGE_SIGNAL      = 4; ///< a signal message type


var heartbeatSignalHandler = null;
var heartbeatSignalHandlerAlreadyCalled = false;
var heartbeatSignalName = 'Heartbeat';

var clientApplicationName = 'AboutPlusServiceTest';

var authMechanism = 'ALLJOYN_SRP_LOGON ALLJOYN_SRP_KEYX';
var keyStoreFileName = './test_helper.ks';
var isKeyStoreShared = false;

var authenticationPin = '123456';
var authenticationLogon = 'admin:password';

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
    if (port != SESSION_PORT) {
      return false;
    } else {
      return true;
    }
  },
  function(port, sessionId, joiner){
  }
);

var argumentsForSignature = function(signature) {
  [1];
}

before(function(done){

  // setup the service Bus Attachment with the Application Name
  // setup the client Bus Attachment with the Application Name
  clientBusAttachment = setupClientBusAttachment(clientApplicationName);
  var clientBusUniqueName = clientBusAttachment.getUniqueName();

  //TODO: do some assertion testing on the two bus attachments.
  assert.equal(typeof(clientBusUniqueName), 'string');
  assert.equal(clientBusUniqueName.length, 11);

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
    sessionfulData.interfaceNamesForPath = {};

    var paths = Object.keys(sessionlessData.objectDescription);
    for (i = 0; i < paths.length; i++) {
      var proxyBusObject = alljoyn.ProxyBusObject(clientBusAttachment, busName, paths[i], sessionId);
      var status = proxyBusObject.secureConnectionAsync(false);
    }
  }

  // create a new About Listener with the Announced Callback
  var aboutListener = alljoyn.AboutListener(announcedCallback);

  // register the About Listener
  assert.equal(clientBusAttachment.registerAboutListener(aboutListener), undefined);

  // create an RequestCredentials callback that will get called
  // after the AuthListener is registered and a method is invoked.
  var requestCredentialsCallback = function(authMechanism, peerName, authCount, userName, credMask){
    authListenerRequestCredentialsWasCalled = true;
    sessionfulData.authMechanism = authMechanism;
    sessionfulData.peerName = peerName;
    sessionfulData.authCount = authCount;
    sessionfulData.userName = userName;
    sessionfulData.credMask = credMask;

    if (authMechanism === 'ALLJOYN_SRP_LOGON') {
      return authenticationLogon;
    }
    else if (authMechanism === 'ALLJOYN_SRP_KEYX') {
      return authenticationPin;
    }
  };

  var authenticationCompleteCallback = function(authMechanism, peerName, success){
    authListenerAuthenticationCompleteWasCalled = true;
    sessionfulData.authMechanism = authMechanism;
    sessionfulData.peerName = peerName;
    sessionfulData.authSuccess = success;

    done();
  };

  // create a new Auth Listener
  var authListener = alljoyn.AuthListener(requestCredentialsCallback, authenticationCompleteCallback);

  // enable peer security with the Auth Listener
  assert.equal(clientBusAttachment.enablePeerSecurity(authMechanism, authListener, keyStoreFileName, isKeyStoreShared), ALL_GOOD);

  // call WhoImplements on the service interface name
  // to trigger the Announced Callback
  assert.equal(clientBusAttachment.whoImplements([SERVICE_INTERFACE_NAME]), ALL_GOOD);
});
