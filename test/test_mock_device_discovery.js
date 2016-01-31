// for this suite to work, the AllJoyn AboutPlusService sample must be running
// how to run it? after running npm install:
// > <project_root>/build/Release/sample-mock-device-discovery

// TODO: start, inspect and stop OS process from within the test
// http://stackoverflow.com/questions/20643470/execute-a-command-line-binary-with-node-js
// Or just start the service side from the test too

require('./test_helper');

describe('An AllJoyn about announcement', function() {
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
