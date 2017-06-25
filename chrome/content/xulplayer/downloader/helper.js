/**
 * Get a (formatted) locale property string.
 * 
 * @param stringId
 *          Id of desired string corresponding to the .properties file(s)
 * @param ...
 *          Optional. Format parameters
 * @return String for given Name
 * @throws Exception
 *           if stringID is not found or before the dialog was initialized
 * @author Nils
 */
function _() {
	var bundles = new StringBundles();
	_ = function() {
		if (arguments.length == 1) {
			return bundles.getString(arguments[0]);
		}
		return bundles.getFormattedString.apply(bundles, arguments);
	}
	return _.apply(this, arguments);
}

/**
 * Encapulates all stringbundles of the current document and provides unified
 * access
 * 
 * @author Nils
 * @see _
 */
function StringBundles() {
	this.init();
}
StringBundles.prototype = {
	_bundles: [],
	init: function() {
		this._bundles = document.getElementsByTagName('stringbundle');
	},
	getString: function(id) {
		for each (var bundle in this._bundles) {
			try {
				return bundle.getString(id);
			}
			catch (ex) {
				// no-op
			}
		}
		throw new Components.Exception('BUNDLE STRING NOT FOUND (' + id + ')');
	},
	getFormattedString: function(id, params) {
		for each (var bundle in this._bundles) {
			try {
				return bundle.getFormattedString(id, params);
			}
			catch (ex) {
				// no-op
			}
		}
		throw new Components.Exception('BUNDLE STRING NOT FOUND (' + id + ')');	 
	}
};

/**
 * returns a formated representation of a (file) size
 * 
 * @param aNumber
 *          The number to format
 * @author Nils
 */
function formatBytes(aNumber) {
	const formatBytes_units = [['sizeB', 0], ['sizeKB', 1], ['sizeMB', 2], ['sizeGB', 2], ['sizeTB', 3]];
	const formatBytes_nunits = formatBytes_units.length;

	aNumber = Number(aNumber);
	
	if (!isFinite(aNumber)) {
		return 'NaN';
	}
	
	let unit = formatBytes_units[0];
	
	for (let i = 1; aNumber > 875 && i < formatBytes_nunits; ++i) {
		aNumber /= 1024;
		unit = formatBytes_units[i];
	}
	
	return _(unit[0], [aNumber.toFixed(unit[1])]);
}

/**
 * Cast non-strings to strings (using toSource if required instead of toString()
 * @param {Object} data
 */
function atos(data) {
	if (typeof(data) == 'string') {
		return data;
	}
	if (data instanceof String || typeof(data) == 'object') {
		try {
			return data.toSource();
		}
		catch (ex) {
			// fall-trough
		}
	}
	return data.toString();
}

/**
 * Head-Pads a number so that at it contains least "digits" digits.
 * @param {Object} num The number in question
 * @param {Object} digits Number of digits the results must contain at least
 */
function formatNumber(num, digits) {
	let rv = atos(num);
	digits = Number(digits);
	if (!isFinite(digits)) {
		digits = 3;
	}
	for (let i = rv.length; i < digits; ++i) {
		rv = '0' + rv;
	}
	return rv;
}

/**
 * Formats a time delta (seconds)
 * @param {Number} delta in seconds
 * @return {String} formatted result
 */
function formatTimeDelta(delta) {
	let rv = (delta < 0) ? '-' : '';

	delta = Math.abs(delta);
	let h = Math.floor(delta / 3600);
	let m = Math.floor((delta % 3600) / 60);
	let s = Math.floor(delta % 60);
	
	if (h) {
		rv += formatNumber(h, 2) + ':';
	}
	return rv + formatNumber(m, 2) + ':' + formatNumber(s, 2);
}

/**
 * Converts a Datestring into an integer timestamp.
 * @param {Object} str Datestring or null for current time.
 */
function getTimestamp(str) {
	if (!str) {
		return Date.now();
	}
	let rv = Date.parse(atos(str));
	if (!isFinite(rv)) {
		throw new Error('invalid date');
	}
	return rv;
}

function Serv(c, i) {
	return Cc[c].getService(i ? Ci[i] : null);
}

/**
 * returns a new UUID in string representation
 * @return String UUID
 * @author Nils
 */
function newUUIDString() {
	let uuidgen = Cc["@mozilla.org/uuid-generator;1"].getService(Ci.nsIUUIDGenerator);
	
	newUUIDString = function() {
		return uuidgen.generateUUID().toString();
	}
	return newUUIDString();
}

const FileFactory = new Components.Constructor(
	'@mozilla.org/file/local;1',
	'nsILocalFile',
	'initWithPath'
);

// XXX switch to nsITimer?
function Timer(func, interval, persist, now) {
  this._id = newUUIDString();
	if (typeof(func) != 'function') {
		func = new Function(func);
	}
	this._func = func;
  this._interval = interval;
  this._persist = persist;
  
  TimerManager._push(this);
  if (now) {
  	this.exec();
  }
}
Timer.prototype = {
	_install: function TI__install() {
	  var tp = this; 
		this._tid = window.setTimeout(
	    function() {
	      if (tp._persist) {
	        tp._install();
	      }
	      else {
	        TimerManager.kill(tp);
	      }
	      tp.exec();
	    },
	    this._interval
	  );
	},
	exec: function TI_exec() {
		this._func.call(window);
	},
	kill: function TI_kill() {
		TimerManager.kill(this);
	},
	toString: function TI_toString() {
		return this._id;
	}
}

var TimerManager = {
	_timers: {},
	_push: function TM_push(timer) {
		this.kill(timer);
		this._timers[timer] = timer;
		timer._install();
	},
	kill: function TM_kill(timer) {
		if (timer in this._timers) {
			window.clearTimeout(timer._tid);
		}
		delete this._timers[timer];
	},
	killAll: function TM_killAll() {
		for (id in this._timers) {
			Debug.logString("killing: " + id);
			window.clearTimeout(this._timers[id]._tid);
		}
		this._timers = {};
	}
};

function has(B, A) {
    return Object.prototype.hasOwnProperty.apply(B, [A])
}
