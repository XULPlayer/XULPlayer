/* Helper module for create html element using html dom 
   Created by Zoominla, 01-01-2009         
   
   @Function: Simplify html element creation
   @Usage:    var table = make("table", {border:1}, make("tr", [make("th", "Name"),
                                                  make("th", "Type"),
                                                  make("th", "Value")]));

			But you can do even better. Example 15-10 follows the make() function with another function called maker().
			Pass a tag name to maker(), and it returns a nested function that calls make() with the tag name you specified hardcoded.
			If you want to create a lot of tables, you can define creation functions for common table tags like this:
			var table = maker("table"), tr = maker("tr"), th = maker("th");

			Then, with these maker functions defined, the table-creation and header-creation code shrinks down to a single line:
			var mytable = table({border:1}, tr([th("Name"), th("Type"), th("Value")]));
*/

if(typeof(HTML_HELPER_INCLUDE) != "boolean") {
	HTML_HELPER_INCLUDE = true;
	
	function HtmHelper()
	{
		
	}
	
	HtmHelper.make = function(tagname, attribList, children) {
		// If we were invoked with two arguments, the attributes argument is
		// an array or string; it should really be the children arguments.
		if (arguments.length == 2 &&
			(attribList instanceof Array || typeof attribList == "string")) {
			children = attribList;
			attribList = null;
		}
		// Create the element
		var e = document.createElement(tagname);
	
		// Set attributes
		if (attribList) {
			for(var name in attribList) {
				var idx = name.indexOf("style.");
				if(idx >= 0) {
					var styleName = name.substr(idx+6);
					e.style[styleName] = attribList[name];
				} else {
					e.setAttribute(name, attribList[name]);
				}
			}
		}
	
		// Add children, if any were specified.
		if (children) {
			if (children instanceof Array) {  // If it really is an array
				for(var i = 0; i < children.length; i++) { // Loop through kids
					var child = children[i];
					if (typeof child == "string")          // Handle text nodes
						child = document.createTextNode(child);
					e.appendChild(child);  // Assume anything else is a Node
				}
			} else if (typeof children == "string") {// Handle single text child
				e.appendChild(document.createTextNode(children));
			} else {
				e.appendChild(children);         // Handle any other single child
			}
		}
		// Finally, return the element.
		return e;
	};
	
	/**
	* maker(tagname): return a function that calls make() for the specified tag.
	* Example: var table = maker("table"), tr = maker("tr"), td = maker("td");
	*/
    HtmHelper.maker = function(tagname) {
	   return function(attrs, kids) {
		   if (arguments.length == 1) return HtmHelper.make(tagname, attrs);
		   else return HtmHelper.make(tagname, attrs, kids);
	   }
	};
}