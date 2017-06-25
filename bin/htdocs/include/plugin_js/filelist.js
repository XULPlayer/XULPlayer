/* File list class. Provide add/remove file operation, meamwhile update xml doc
  associated with the filelist.
  It should be created with two params: id of file list element, full path of queue.xml
   Created by Zoominla, 01-08-2009         
*/

if(typeof(MEIDAFILE_LIST_INCLUDE) != "boolean") {
	MEIDAFILE_LIST_INCLUDE = true;

function CFileList(listId, queuePath)
{
	this.list = document.getElementById(listId);
	this.path = queuePath;
	// doc of queue.xml
	this.doc = null;
	this.dirty = false;
	// An arry indicate whether each file info is generated
	this.infoDumped = [];
}

CFileList.prototype.initialize = function() {
	this.doc = DomHelper.LoadLocalXMLFile(this.path);
	if(!this.doc) {
		this.doc = new XMLDom();
		this.doc.async = false;
		this.doc.preserveWhiteSpace=false;
		var head = this.doc.createProcessingInstruction('xml','version="1.0" standalone="yes"');     	
		var root = this.doc.createElement("queue");
		this.doc.appendChild(head);
		this.doc.appendChild(root);
	}
	var rootNode = DomHelper.getChildNode(this.doc);
	var child = DomHelper.getChildNode(rootNode);
	for(;child;child=DomHelper.getNextNode(child)) {
		//clean empty node
		/*
		if(child.childNodes.length < 2)
		{
			var temp = DomHelper.getNextNode(child);
			rootNode.removeChild(child);
			if(temp == null)
			{
				break;
			}
			else
			{
				child = temp;
			}
		}*/
		// Judge info of this item is generated
		if(DomHelper.getChildrenCount(child) > 2) {	// original child number is 2
			this.infoDumped.push(true);
		} else {
			this.infoDumped.push(false);
		}
		var node = DomHelper.getChildNode(child);
		var valnode = DomHelper.getNodeByAttribute(node,"node","key","overall.file");
		var tagnode = DomHelper.getNodeByAttribute(node,"node","key","tags.title");
		if(valnode != null && tagnode != null){
			var filePath = valnode.getAttribute("value");
			var fileTitle = tagnode.getAttribute("value");
			this.list.options.add(new Option(fileTitle, filePath));
		}
	}
	if(this.list.options.length > 0) this.list.selectedIndex = 0;	
};

CFileList.prototype.isEmpty = function() {
	return !(this.list.options.length > 0);
};

CFileList.prototype.getLength = function() {
	return this.list.options.length;
};

CFileList.prototype.getSelectedFile = function() {
	var fileName = null;
	var selIdx = this.list.selectedIndex;
	if(selIdx >= 0) {
		fileName = this.list.options[selIdx].value;
	}
	return fileName;
};

CFileList.prototype.getSelectedFileName = function() {
	var fileName = null;
	var selIdx = this.list.selectedIndex;
	if(selIdx >= 0) {
		fileName = this.list.options[selIdx].text;
	}
	return fileName;
};

CFileList.prototype.moveNext = function() {
	var itemCount = this.getLength();
	if(itemCount < 1)
       return;
	var newSelIdx = (this.list.selectedIndex + 1)%itemCount;
	this.list.selectedIndex = newSelIdx;
};

CFileList.prototype.movePrevious = function() {
	var itemCount = this.getLength();
	if(itemCount < 1)
       return;
	var newSelIdx = this.list.selectedIndex - 1;
	if(newSelIdx < 0) newSelIdx += itemCount;
	this.list.selectedIndex = newSelIdx;
};

CFileList.prototype.addFile = function(fName) {
	if(!fName) return;
	var fileTitle = getFileTitle(fName);
	this.list.options.add(new Option(fileTitle, fName));
	if(this.doc) {
		var rootNode = DomHelper.getChildNode(this.doc);
		var fileItem = DomHelper.addElementNode(this.doc, rootNode, 'item');
		DomHelper.insertNode(this.doc, fileItem, 'overall.file', fName);
		DomHelper.insertNode(this.doc, fileItem, 'tags.title', fileTitle);
	}
	this.list.selectedIndex = this.list.options.length-1;
	this.infoDumped.push(false);
	this.dirty = true;
};

CFileList.prototype.removeSelectedItem = function() {
	if(this.list.options.length <1)
		return;
	var selIdx = this.list.selectedIndex;
	if(selIdx < 0) return;
	// Remove item from queue.xml and update it
	if(this.doc) {
		var rootNode = DomHelper.getChildNode(this.doc); 
		DomHelper.removeChildByIndex(rootNode, selIdx);
	}
	
	// Remove related item in fileInfoDumped
	this.infoDumped.splice(selIdx, 1);
	// Remove file list item and update it
	var newSelIdx = selIdx;
	if(this.list.options.length-1 == selIdx) {
		newSelIdx = selIdx - 1;
	}
	this.list.remove(selIdx);
	this.list.selectedIndex = newSelIdx;
	this.dirty = true;
};


CFileList.prototype.clear = function() {
	if(this.list.options.length <1)
       return;
	// Clear queue.xml
	if(this.doc) {
		var rootNode = DomHelper.getChildNode(this.doc);
		DomHelper.removeChildren(rootNode);
	}
	// Clear file list on web page
	if(this.list.options.length > 0){
       DomHelper.removeChildren(this.list);
	   this.list.selectedIndex = -1;
    }
	this.infoDumped = [];
	this.dirty = true;
};

// When click the item of fileList, generate file info
CFileList.prototype.generateFileInfo = function() {
	var selIdx = this.list.selectedIndex;
	if(selIdx < 0) return;
	if(!this.infoDumped[selIdx]) {
		var selFileName = this.list.options[selIdx].value;
		var fileInfo = mc.dumpMediaInfo(selFileName);
		// Update file queue doc
		if(this.doc) {
			var rootNode = DomHelper.getChildNode(this.doc);
			var node = DomHelper.getChildNode(rootNode);
			for(i=0;i<selIdx;i++) {		
				node=DomHelper.getNextNode(node);
			}
			if(!node) return;
			var tempDoc = new XMLDom();
			tempDoc.loadXML(fileInfo);			
			var childNode = tempDoc.selectSingleNode("//item/node");
			while(childNode) {
				var tmp = DomHelper.getNextNode(childNode);
				node.appendChild(childNode);
				childNode = tmp;
			}
			$("TestFileInfos").value = fileInfo;
			this.infoDumped[selIdx] = true;
		}
		this.dirty = true;
	}
};

CFileList.prototype.getDocContent = function() {
	if(this.doc) {
		var docContent = DomHelper.GetXmlContent(this.doc);
		return docContent;
	}
	return null;
};

CFileList.prototype.getCurItemInfo = function() {
	var selIdx = this.list.selectedIndex;
	if(this.doc && selIdx >= 0) {
		if(!this.infoDumped[selIdx]) this.generateFileInfo();
		var rootNode = DomHelper.getChildNode(this.doc);
			var node = DomHelper.getChildNode(rootNode);
			for(i=0;i<selIdx;i++) {		
				node=DomHelper.getNextNode(node);
			}
			return DomHelper.GetXmlContent(node);
	}
	return null;
};

CFileList.prototype.getCurItemNode = function() {
	var selIdx = this.list.selectedIndex;
	if(this.doc && selIdx >= 0) {
		if(!this.infoDumped[selIdx]) this.generateFileInfo();
		var rootNode = DomHelper.getChildNode(this.doc);
			var node = DomHelper.getChildNode(rootNode);
			for(i=0;i<selIdx;i++) {		
				node=DomHelper.getNextNode(node);
			}
			return node;
	}
	return null;
};

CFileList.prototype.save = function() {
	if(this.doc && this.dirty) {
		var docContent = DomHelper.GetXmlContent(this.doc);
		if(docContent.indexOf('version="1.0"') < 0) {
			docContent = '<?xml version="1.0" standalone="yes" ?>' + docContent;
		}
		mc.createXmlFile(docContent, this.path);
	}
};

}