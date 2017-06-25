/* modify from DownThemAll tree.js*/

const FilePicker = Components.Constructor('@mozilla.org/filepicker;1', 'nsIFilePicker', 'init');
 
var Tree = {
	_downloads: [],
	init: function T_init(elem) {
		this.elem = elem;

		let as = Serv('@mozilla.org/atom-service;1', 'nsIAtomService');
		for each (let e in ['iconic', 'completed', 'inprogress', 'paused', 'canceled', 'pausedUndetermined', 'pausedAutoretrying', 'verified', 'progress']) {
			this['_' + e] = as.getAtom(e);
		}

		this.elem.view = this;	
		
		this.refreshTools();
	},

	/*
	 * actual nsITreeView follows
	 */
	get rowCount() {
		return this._downloads.length;
	},
	setTree: function T_setTree(box) {
		this._box = box;
	},
	getParentIndex: function T_getParentIndex(idx) {
		// no parents, as we are actually a list
		return -1;
	},
	getLevel: function T_getLevel(idx) {
		// ... and being a list all nodes are on the same level
		return 0;
	},
	getCellText: function T_getCellText(idx, col) {
		let d = this._downloads[idx];

		switch (col.index) {
			case 0: return d.name
			case 2: return d.percent;
			case 3: return d.dimensionString;
			case 4: return d.status
			case 5: return d.is(RUNNING) ? d.speedString : '';
			case 6: return d.numOfSrc;
			case 7: return d.numOfClient;
			case 8: return d.priority;
			case 9: return d.link;
		}
		return '';
	},
	isSorted: function T_isSorted() {
		// not sorted
		return false;
	},
	isContainer: function T_isContainer(idx) {
		// being a container means we got children... but we don't have any children because we're a list actually
		return false;
	},
	isContainerOpen: function T_isContainerOpen(idx) {
		return false;
	},
	isContainerEmpty: function T_isContainerEmpty(idx) {
		return false;
	},

	isSeparator: function T_isSeparator(idx) {
		// no separators
		return false;
	},
	isEditable: function T_isEditable(idx) {
		// and nothing is editable
		return true;
	},
	// will grab the "icon" for a cell.
	getImageSrc: function T_getImageSrc(idx, col) {
		//TODO
		return null;
		switch (col.index) {
			case 0: return this._downloads[idx].icon;
		}
		return null;
	},
	getProgressMode : function T_getProgressMode(idx, col) {
		if (col.index == 1) {
			let d = this._downloads[idx]; 
			if (d.isOf(RUNNING, PAUSED) && !d.size) {
				return 2; // PROGRESS_UNDETERMINED;
			}
			if (d.is(PAUSED) && d.downloaded / d.size < .05) {
				return 2; // PROGRESS_UNDETERMINED;			
			}
			return 1; // PROGRESS_NORMAL;
		}
		return 3; // PROGRESS_NONE;
	},
	// will be called for cells other than textcells
	getCellValue: function T_getCellValue(idx, col) {
		if (col.index == 1) {
			let d = this._downloads[idx];
			if (d.isOf(CANCELED, COMPLETE)) {
				return 100; 
			}
			return d.size ? d.downloaded * 100 / d.size : 0;
		}
		return null;
	},
	getCellProperties: function T_getCellProperties(idx, col, prop) {
		let cidx = col.index;
		if (cidx == 1) {
			prop.AppendElement(this._iconic);
			prop.AppendElement(this._progress);
			let d = this._downloads[idx];
			if (d.state == RUNNING || d.state == FINISHING) {
				prop.AppendElement(this._inprogress);
			}
			else if (d.state == PAUSED) {
				prop.AppendElement(this._paused);
				if (!d.size || d.downloaded / d.size < .05) {
					prop.AppendElement(this._pausedUndetermined);
				}				
			}
			else if (d.state == CANCELED) {
				prop.AppendElement(this._canceled);
			}
			else if (d.state == COMPLETE) {
				prop.AppendElement(this._completed);
			}
		}
		else if (cidx == 0) {
			//TODO
			//prop.AppendElement(this._iconic);
		}
	},
	// just some stubs we need to provide anyway to implement a full nsITreeView
	cycleHeader: function T_cycleHeader(col, elem) {},
	cycleCell: function(idx, column) {},
	performAction: function(action) {},
	performActionOnRow: function(action, index, column) {},
	performActionOnCell: function(action, index, column) {},
	getColumnProperties: function(column, element, prop) {},
	getRowProperties: function(idx, prop) {},
	setCellValue: function(idx, col, value) {},
	selectionChanged: function T_selectionChanged() {
		this.refreshTools();
	},
	
	_updating: 0,
	beginUpdate: function T_beginUpdate() {
		if (++this._updating == 1) {
			this._box.beginUpdateBatch();
		}
	},
	endUpdate: function T_endUpdate() {
		if (--this._updating == 0) {
			this._box.endUpdateBatch();
			this.refreshTools();
		}
	},
	add: function T_add(download) {
		this._downloads.push(download);
		download.position = this._downloads.length - 1;
		if (!this._updating) {
			this._box.rowCountChanged(download.position, 1);
		}
	},
	removeWithConfirmation: function T_removeWithConfirmation() {
		if (true /*TODO*/) {
			let res = Prompts.confirm(window, _('removetitle'), _('removequestion'), Prompts.YES, Prompts.NO, null, 0, false, _('removecheck'));
			if (res.checked) {
				Preferences.setExt('confirmremove', false);
			}
			if (res.button) {
				return;
			}
		}
		this.remove(null, true);
	},
	remove: function T_remove(downloads, performJump) {
		if (downloads && !(downloads instanceof Array)) {
			downloads = [downloads];
		}
		else if (!downloads) {
			downloads = this._getSelectedIds(true).map(function(idx) this._downloads[idx], this);
		}
		if (!downloads.length) {
			return;
		}
	
		downloads = downloads.sort(function(a, b) b.position - a.position);	 
		this.beginUpdate();
		let last = 0;
		for each (let d in downloads) {
			if (d.is(FINISHING)) {
				// un-removable :p
				return;
			}
			// wipe out any info/tmpFiles
			if (!d.isOf(COMPLETE, CANCELED)) {
				d.cancel();
			}
			//remove from the downloads array!
			this._downloads.splice(d.position, 1);
			this._box.rowCountChanged(d.position, -1);
			last = Math.max(d.position, last);
			d.remove();
			//Dialog.wasRemoved(d);
		}
		this.endUpdate();
		this.invalidate();
		if (performJump) {
			this._removeJump(downloads.length, last);
		}
	},
	_removeCompleted: function T__removeCompleted(onlyGone) {
		this.beginUpdate();
		let delta = this._downloads.length, last = 0;
		for (let i = delta - 1; i > -1; --i) {
			let d = this._downloads[i];
			if (!d.is(COMPLETE)) {
				continue;
			}
			if (onlyGone && (new FileFactory(d.destinationFile).exists())) {
				continue;
			}
			this._downloads.splice(d.position, 1);
			this._box.rowCountChanged(d.position, -1);
			last = Math.max(d.position, last);
			d.remove();						
		}
		this.endUpdate();	
		if (delta == this._downloads.length) {
			return;
		}
		this.invalidate();		
		this._removeJump(delta - this._downloads.length, last);
	},
	removeCompleted: function T_removeCompleted() {
		this._removeCompleted(false);
	},
	removeGone: function T_removeGone() {
		this._removeCompleted(true);
	},
	_removeJump: function(delta, last) {
		if (!this.rowCount) {
			this._box.ensureRowIsVisible(0);
		}
		else {
			let np = Math.max(0, Math.min(last - delta + 1, this.rowCount - 1));
			if (np < this._box.getFirstVisibleRow() || np > this._box.getLastVisibleRow()) {
				this._box.ensureRowIsVisible(np);
			}
			this.selection.currentIndex = np;
		}
	},
	pause: function T_pause() {
		this.updateSelected(
			function(d) {
				if (d.is(QUEUED) || (d.is(RUNNING) && d.resumable)) {
					d.pause();
					d.status = _("paused");
					d.state = PAUSED;
				}
				return true;
			}
		);
	},
	resume: function T_resume(d) {
		this.updateSelected(
			function(d) {
				if (d.isOf(PAUSED, CANCELED)) {
					d.resume();
				}
				return true;
			}
		);
	},
	cancel: function T_cancel() {
		this.updateSelected(function(d) { d.cancel(); return true; });
	},
	selectAll: function T_selectAll() {
		this.selection.selectAll();
		this.selectionChanged();
	},
	selectInv: function T_selectInv() {
		for (let d in this.all) {
			this.selection.toggleSelect(d.position);
		}
		this.selectionChanged();
	},
	force: function T_force() {
		for (let d in Tree.selected) {
			if (d.isOf(QUEUED, PAUSED, CANCELED)) {
				d.resume();
//				Dialog.run(d);
			}
		}
	},
	export: function T_export() {
		try {
			let fp = new FilePicker(window, _('exporttitle'), Ci.nsIFilePicker.modeSave);
			fp.appendFilters(Ci.nsIFilePicker.filterHTML | Ci.nsIFilePicker.filterText);
			fp.appendFilter(_('filtermetalink'), '*.metalink');
			fp.defaultExtension = "metalink";
			fp.filterIndex = 2;
			
			let rv = fp.show();
			if (rv == Ci.nsIFilePicker.returnOK || rv == Ci.nsIFilePicker.returnReplace) {
				switch (fp.filterIndex) {
					case 0: ImEx.exportToHtml(this.selected, fp.file); return;
					case 1: ImEx.exportToTxt(this.selected, fp.file); return;
					case 2: ImEx.exportToMetalink(this.selected, fp.file); return;
				} 
			}
		}
		catch (ex) {
			dumpErr("Cannot export downloads:" + ex);		
			Prompts.alert(window, _('exporttitle'), _('exportfailed'));
		}
	},
	import: function T_import() {
		try {
			let fp = new FilePicker(window, _('importtitle'), Ci.nsIFilePicker.modeOpen);
			fp.appendFilters(Ci.nsIFilePicker.filterText);
			fp.appendFilter(_('filtermetalink'), '*.metalink');
			fp.defaultExtension = "metalink";
			fp.filterIndex = 1;
			
			let rv = fp.show();
			if (rv == Ci.nsIFilePicker.returnOK) {
				switch (fp.filterIndex) {
					case 0: ImEx.importFromTxt(fp.file); return;
					case 1: ImEx.importFromMetalink(fp.file); return;
				} 
			}
		}
		catch (ex) {
			dumpErr("Cannot import downloads:" + ex);		
			Prompts.alert(window, _('importtitle'), _('importfailed'));
		}
	},
	showInfo: function T_showInfo() {
		this.beginUpdate();
		let downloads = [];
		for (let d in Tree.selected) {
			downloads.push(d);
		}
		if (downloads.length) {
			window.openDialog("manager/info.xul","_blank","chrome, centerscreen, dialog=no", downloads, this);		 
		}
		this.endUpdate();
	},
	_hoverItem: null,
	_ww: Serv('@mozilla.org/embedcomp/window-watcher;1', 'nsIWindowWatcher'),
	hovering: function(event) {
		if (this._ww.activeWindow != window) {
			return;
		}
		this._hoverItem = {x: event.clientX, y: event.clientY};
	},
	showTip: function(event) {
		if (typeof(DOWNLOADER_INDIVIDUAL_MODE) == "undefined") return true;

		if (!this._hoverItem || this._ww.activeWindow != window) {
			return false;
		}
		let row = {};
		this._box.getCellAt(this._hoverItem.x, this._hoverItem.y, row, {}, {});
		if (row.value == -1) {
			return false;
		}
		let d = this.at(row.value);
		if (!d) {
			return false;
		}
		//TODO: $("infoIcon").src = d.largeIcon;
		$("infoURL").value = d.link; //FIXME
		//TODO: $("infoDest").value = d.destinationFile;
	
		Tooltip.start(d);			
		return true;
	},	
	stopTip: function T_stopTip() {
		if (typeof(DOWNLOADER_INDIVIDUAL_MODE) != "undefined") {
			Tooltip.stop();
		}
	},
	refreshTools: function T_refreshTools(d) {
		if (this._updating || (d && ('position' in d) && !this.selection.isSelected(d.position))) {
			return;
		}
		try {
			let empty = this.current == null;
				
			let states = {
				_state: 0,
				resumable: false,
				is: function(s) this._state & s,  
				isOf: QueueItem.prototype.isOf,
				count: this.selection.count,
				rows: this.rowCount,
				min: this.rowCount,
				max: 0
			};
			for (let d in this.selected) {
				states._state |= d.state;
				states.resumable |= d.resumable;
				states.min = Math.min(d.position, states.min);
				states.max = Math.max(d.position, states.max);
			}
			let cur = this.current;
//			states.curFile = (cur && cur.is(COMPLETE) && (new FileFactory(cur.destinationFile)).exists());
//			states.curFolder = (cur && (new FileFactory(cur.destinationPath)).exists());
							
			function modifySome(items, f) {
				let disabled;
				if (empty) {
					disabled = true;
				}
				else {
					disabled = !f(states);
				}
				if (!(items instanceof Array)) {
					items = [items];
				}
				for each (let o in items) {
					if (typeof(o) != "object") continue;
					o.setAttribute('disabled', disabled);
				}
			}
			modifySome($('play', 'toolplay'), function(d) !d.isOf(COMPLETE, RUNNING, QUEUED, FINISHING));
			modifySome($('pause', 'toolpause'), function(d) d.isOf(RUNNING, QUEUED));
			modifySome($('cancel', 'toolcancel'), function(d) !d.isOf(FINISHING, CANCELED));
//			modifySome($('launch'), function(d) !!d.curFile);
//			modifySome($('folder'), function(d) !!d.curFolder);
//			modifySome($('delete'), function(d) d.is(COMPLETE));
//			modifySome($('export'), function(d) !!d.count);
//			modifySome($('addchunk', 'removechunk', 'force'), function(d) d.isOf(QUEUED, RUNNING, PAUSED, CANCELED));
//			modifySome($('movetop', 'moveup', 'toolmovetop', 'toolmoveup'), function(d) d.min > 0); 
//			modifySome($('movedown', 'movebottom', 'toolmovedown', 'toolmovebottom'), function(d) d.max != d.rows - 1);  
		}
		catch (ex) {
			dumpErr("rt:" + ex);
		}
	},
	invalidate: function T_invalidate(d) {
		if (!d) {
			let complete = 0;
			this._downloads.forEach(
				function(e, i) {
					e.position = i;
					if (e.is(COMPLETE)) {
						complete++;
					}
				}
			);
			this._box.invalidate();
			this.refreshTools(this);
//TODO			Dialog.completed = complete;
		}
		else if (d instanceof Array) {
			this.beginUpdate();
			for each (let e in d) {
				this.invalidate(e);
			}
			this.endUpdate();
		}
		else if (d.position >= 0) {
			this._box.invalidateRow(d.position);
		}
	},
	get box() {
		return this._box;
	},
	// generator for all download elements.
	get all() {
		for (let i = 0, e = this._downloads.length; i < e; ++i) {
			yield this._downloads[i];
		}
	},
	// generator for selected download elements.
	// do not make any assumptions about the order.
	get selected() {
		// loop through the selection as usual
		for (let i = 0, e = this.selection.getRangeCount(); i < e; ++i) {
			let start = {}, end = {value: -1};
			this.selection.getRangeAt(i, start, end);
			for (let j = start.value, k = end.value; j <= k; ++j) {
					yield this._downloads[j];
			}
		}
	},
	// returns an ASC sorted array of IDs that are currently selected.
	_getSelectedIds: function T_getSelectedIds(getReversed) {
		var rv = [];
		let select = this.selection;
		// loop through the selection as usual
		for (let i = 0, e = select.getRangeCount(); i < e; ++i) {
				let start = {}, end = {};
				this.selection.getRangeAt(i, start, end);
				for (let j = start.value, k = end.value; j <= k; ++j) {
					rv.push(j);
				}
		}
		this.selection.clearSelection();
		if (getReversed) {
			rv.sort(function(a, b) { return b - a; });
		}
		else {
			rv.sort(function(a, b) { return a - b; });
		}
		return rv;
	},
	// get the first selected item, NOT the item which has the input focus.
	get current() {
		let select = this.selection;
		try {
			let ci = {value: -1};
			this.selection.getRangeAt(0, ci, {});			
			if (ci.value > -1 && ci.value < this.rowCount) {
				return this._downloads[ci.value];
			}
		}
		catch (ex) {
			// fall-through
		}
		return null;		
	},
	// get the currently focused item.
	get focused() {
		let ci = this.selection.currentIndex;
		if (ci > -1 && ci < this.rowCount) {
			return this._downloads[ci];
		}
		return null;		
	},
	at: function T_at(idx) {
		return this._downloads[idx];
	},
	some: function T_some(f, t) {
		return this._downloads.some(f, t);
	},
	every: function T_every(f, t) {
		return this._downloads.every(f, t);
	},
	update: function T_update(f, t) {
		this.beginUpdate();
		f.call(t);
		this.endUpdate();
	},
	updateSelected: function T_updateSelected(f, t) {
		this.beginUpdate();
		for (d in this.selected) {
			if (!f.call(t, d)) {
				break;
			}
		}
		this.endUpdate();
		this.invalidate();
	},
	updateAll: function T_updateAll(f, t) {
		this.beginUpdate();
		for (d in this.all) {
			if (!f.call(t, d)) {
				break;
			}
		}
		this.endUpdate();
	},
};