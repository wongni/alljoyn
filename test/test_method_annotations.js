// for this suite to work, the AllJoyn AboutPlusService sample must be running
// how to run it? after running npm install:
// > <project_root>/build/Release/sample-mock-device-discovery

// TODO: start, inspect and stop OS process from within the test
// http://stackoverflow.com/questions/20643470/execute-a-command-line-binary-with-node-js
// Or just start the service side from the test too

require('./test_helper');

global.BUS_OBJECT_PATH = '/example/path'
global.SERVICE_INTERFACE_NAME = 'com.se.bus.discovery';

var isStatusMethodAnnotationName = 'IsStatusMethod';

var methodForName = function(methodName) {
  for (m = 0; m < methods.length; m++) {
    if (methods[m].name == methodName) {
      return methods[m];
    }
  }
}

var isStatusMethodAnnotationValueForMethodName = function(methodName) {
  var method = methodForName(methodName);

  if (typeof(method.annotation) == 'undefined') {
    return undefined;
  }
  
  annotations = ('length' in method.annotation) ? method.annotation : [method.annotation]
  for (a = 0; a < annotations.length; a++) {
    if (annotations[a].name == isStatusMethodAnnotationName) {
      return annotations[a].value;
    }
  }
}

var annotationsForMethodName = function(methodName) {
  var method = methodForName(methodName);

  if (typeof(method.annotation) == 'undefined') {
    return undefined;
  }
  return ('length' in method.annotation) ? method.annotation : [method.annotation]
}

var methods = null;

describe('An AllJoyn Method', function() {

  it('should support custom annotations', function() {
    methods = sessionfulData.interfaceNamesForPath[BUS_OBJECT_PATH].descriptionForInterfaceName[SERVICE_INTERFACE_NAME].descriptionFromXML.interface.method;
    assert.equal(isStatusMethodAnnotationValueForMethodName('ReadDeviceStatus'), true.toString());
    assert.equal(isStatusMethodAnnotationValueForMethodName('TurnCircuitOn'), false.toString());
  });
  
  it('should handle undefined annotations', function() {
    methods = sessionfulData.interfaceNamesForPath[BUS_OBJECT_PATH].descriptionForInterfaceName[SERVICE_INTERFACE_NAME].descriptionFromXML.interface.method;
    assert.equal(isStatusMethodAnnotationValueForMethodName('TurnCircuitOff'), undefined);
  });

  it('should support multiple custom annotations', function() {
    methods = sessionfulData.interfaceNamesForPath[BUS_OBJECT_PATH].descriptionForInterfaceName[SERVICE_INTERFACE_NAME].descriptionFromXML.interface.method;
    assert.equal(annotationsForMethodName('TurnCircuitOn').length, 2);
  });
});
