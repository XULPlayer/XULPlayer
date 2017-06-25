/* Helper function to check whether a class implements certain interface,
   Created by Zoominla, 02-16-2009         
*/

if (typeof(INTERFACE_HELPER_INCLUDE) != "boolean") {
	INTERFACE_HELPER_INCLUDE = true;

	var Interface = function(name, methods) {
		if (arguments.length != 2) {
			throw new Error("Interface constructor called with " + arguments.length + "arguments, but expected exactly 2.");
		}
		this.name = name;
		this.methods = [];
		for (var i = 0,
		len = methods.length; i < len; i++) {
			if (typeof methods[i] !== 'string') {
				throw new Error("Interface constructor expects method names to be " + "passed in as a string.");
			}
			this.methods.push(methods[i]);
		}
	};
	
	// Static class method.
	Interface.ensureImplements = function(object) {
		if (arguments.length < 2) {
			throw new Error("Function Interface.ensureImplements called with " + arguments.length + "arguments, but expected at least 2.");
		}
		for (var i = 1,len = arguments.length; i < len; i++) {
			var interfaceObj = arguments[i];
			if (interfaceObj.constructor !== Interface) {
				throw new Error("Function Interface.ensureImplements expects arguments" + "two and above to be instances of Interface.");
			}
			for (var j = 0, methodsLen = interfaceObj.methods.length; j < methodsLen; j++) {
				var method = interfaceObj.methods[j];
				if (!object[method] || typeof object[method] !== 'function') {
					throw new Error("Function Interface.ensureImplements: object " + "does not implement the " + interfaceObj.name + " interface. Method " + method + " was not found.");
				}
			}
		}
	};

}