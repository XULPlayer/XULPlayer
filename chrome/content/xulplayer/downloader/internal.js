/**
 * Get DOM Element(s) by Id. Missing ids are silently ignored!
 * 
 * @param ids
 *          One of more Ids
 * @return Either the element when there was just one parameter, or an array of
 *         elements.
 */
function $() {
	if (arguments.length == 1) {
		return document.getElementById(arguments[0]);
	}
	let elements = [];
	for (let i = 0, e = arguments.length; i < e; ++i) {
		let element = document.getElementById(arguments[i]);
		if (element) {
			elements.push(element);
		}
		else {
			dumpErr("requested a non-existing element: " + arguments[i]);
		}
	}
	return elements;
}

if (typeof(Cc) == "undefined")
	const Cc = Components.classes;
if (typeof(Ci) == "undefined")
	const Ci = Components.interfaces;
if (typeof(Cr) == "undefined")
	const Cr = Components.results;
if (typeof(dumpErr) == "undefined")
	const dumpErr = Components.utils.reportError;
if (typeof(dumpMsg) == "undefined")
	const dumpMsg =	Cc['@mozilla.org/consoleservice;1'].getService(Ci.nsIConsoleService).logStringMessage;


var DOWNLOADER_INDIVIDUAL_MODE = 1