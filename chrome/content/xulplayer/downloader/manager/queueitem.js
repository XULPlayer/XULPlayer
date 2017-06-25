function QueueItem(id, name, size, format, link, p2p) {
    this._id = id;
    this._name = name;
    this._size = size;
    this._format = format;
    this._link = link;
    this._p2p = p2p;

    this.speeds = new Array();
    this.chunks = new Array();
}

QueueItem.prototype = {
    resumable: true,
    _state: QUEUED,
    //state
    get state() {
        return this._state;
    },
    set state(nv) {
        if (this._state == nv) {
            return nv;
        }
        if (this._state == RUNNING) {
            // remove ourself from inprogresslist
            //Dialog.wasStopped(this);
        }
        this._state = nv;
        this.invalidate();
        //Dialog.signal(this);
        //Tree.refreshTools();
        return nv;
    },

    _status : '',
    get status() {
	if (false /*TODO*/ && this.isOf(QUEUED, PAUSED)) {
	    return _('offline');
	}
	return this._status;
    },
    set status(nv) {
	if (nv != this._status) {
	    this._status = nv;
	    this.invalidate();
	}
	return this._status;
    },

    //id
    get id() {
        return this._id;
    },

    //name
    get name() {
        return this._name;
    },
    set name(nv) {
	//dumpErr("name = " + this._name + ":" + nv);
        if (this._name == nv) {
            return nv;
        }
        this._name = nv;
        this.invalidate();
        return nv;
    },

    //size
    get size() {
        return this._size;
    },

    //format
    get format() {
        return this._format;
    },

    //link
    get link() {
        return this._link;
    },

    //downloaded
    _downloaded: 0,
    get downloaded() {
	return this._downloaded;
    },
    set downloaded(d) {
	if (d == this._downloaded) {
	    return d;
	}
	this._downloaded = d;
	this.invalidate();
	return d;
    },
    get dimensionString() {
	if (this.size <= 0) {
	    return _('transfered', [formatBytes(this.downloaded), _('nas')]);
	}
	else if (this.is(COMPLETE)) {
	    return formatBytes(this.totalSize);
	}
	    return _('transfered', [formatBytes(this.downloaded), formatBytes(this.size)]);
    },
    get percent() {
	if (!this.size && this.is(RUNNING)) {
	    return _('nas');
	}
	else if (!this.size) {
	    return "0%";
	}
	else if (this.is(COMPLETE)) {
	    return "100%";
	}
	return Math.floor(this.downloaded / this.size * 100) + "%";
    },
    //speed
    _speed: 0,
    get speed() {
	return this._speed;
    },
    get speedString() {
	return formatBytes(this._speed) + "/" + _('sec');
    },
    set speed(s) {
	this.speeds.push(s > 0 ? s : 0);
	//dumpMsg(this.speeds.length);
	if (this.speeds.length > SPEED_COUNT) {
		this.speeds.shift();
	}

	if (s == this._speed) {
	    return s;
	}
	this._speed = s;
	this.invalidate();
	return s;
    },

    //numOfSrc
    _numOfSrc: 0,
    get numOfSrc() {
	return this._numOfSrc;
    },
    set numOfSrc(n) {
	if (n == this._numOfSrc) {
	    return n;
	}
	this._numOfSrc = n;
	this.invalidate();
	return n;
    },

    //numOfClient
    _numOfClient: 0,
    get numOfClient() {
	return this._numOfClient;
    },
    set numOfClient(n) {
	if (n == this._numOfClient) {
	    return n;
	}
	this._numOfClient = n;
	this.invalidate();
	return n;
    },
    
    //TODO: timeStart
    _timeStart: 0,
    get timeStart() {
	return this._timeStart;
    },
    set timeStart(t) {
	return this._timeStart;
    },

    //priority
    _priority: 0,
    get priority() {
	return this._priority;
    },
    set priority(p) {
	if (p == this._priority) {
	    return p;
	}
	this._priority = p;
	this.invalidate();
	return p;
    },

    //priority
    _fileAge: 0,
    get fileAge() {
	return this._fileAge;
    },
    set fileAge(a) {
	if (a == this._fileAge) {
	    return a;
	}
	this._fileAge = a;
	this.invalidate();
	return a;
    },

    invalidate: function QI_invalidate() {
	//dumpErr("to call Tree.invalidate()");
	Tree.invalidate(this);
    },
    /**
     * Takes one or more state indicators and returns if this download is in state
     * of any of them
     */
    is: function QI_is(state) {
        return this._state == state; 
    },
    isOf: function QI_isOf() {
        let state = this._state;
        for (let i = 0, e = arguments.length; i < e; ++i) {
            if (state == arguments[i]) {
                return true;
            }
        }
        return false;		
    },

    //for treeview
    _position: -1,
    get position() {
	return this._position;
    },
    set position(nv) {
	if (nv == this._position) {
	    return;
	}
	this._position = nv;
    },

    get destinationFile() {
	//TODO
	dumpErr("get destinationFile: TODO");
	return "";
    },

    get destinationPath() {
	//TODO
	dumpErr("get destinationPath: TODO")
	return "";
    },

    pause: function QI_pause(){
	this._p2p.pauseItem(this._id);
        this.state = PAUSED;
        this.speed = 0;
    },

    resume: function QI_resume() {
	this._p2p.resumeItem(this._id);
	this.state = RUNNING;
    },
    
    cancel: function QI_cancel() {
	try {
	    this._p2p.cancelItem(this._id);
	}
	catch (ex) {
	    dumpErr("cancel():" +  ex);
	}
    },
    
    remove: function QI_remove() {
	this._p2p.removeItem(this._id);
    },

    dumpItem: function QI_dumpItem() {
	dumpErr( "id:" + this.id + "\n" +
		"name:" + this.name + "\n" +
		"size:" + this.size + "\n" +
		"downloaded:" + this.downloaded + "\n" +
		"numOfSrc:" + this.numOfSrc + "\n" +
		"numOfClient" + this.numOfClient + "\n" +
		"speed:" + this.speed + "\n" +
		"state:" + this.state + "\n" +
		"priority:" + this.priority + "\n" +
		"link:" + this.link + "\n" +
		"format:" + this.format);
    }
};