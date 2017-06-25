/* SQLite wrapper for mozilla storage API, the orginal version is from
  http://www.bytemycode.com/snippets/snippet/845/.
  Modified by zoominla, 02-12-2009.
  Main functions and examples as fowllowing:
  //----------------------------------------------------------------
        //Opens or creates a database file for SQLite access.
        (SQLiteObject) SQLite(databaseFile);
         
        //Creates a named SQL query. 
        (function) SQLite#queryFunction(functionName, SQL)
        (mixed) SQLite#queries[functionName](...)
        
        //Example:
        var S = SQLite('myDB.sqlite');
        S.queryFunction('getByID','SELECT data FROM myTable WHERE id=%n1 LIMIT 1');
        var X = S.queries.getByID(5)[0].data; //returns array of rows in object form
        
        //Create an object with a set of queryFunctions.
        (object) SQLite#queryFunction((object)functionDefs)
         
        //Example:
        var S = new SQLite('database.SQLite');
        var test = S.queryFunction({
                create: 'CREATE TABLE IF NOT EXISTS test (a INTEGER, b STRING)',
                addRow: 'INSERT INTO test (a,b) VALUES(%n1, %s2)',
                getAll: 'SELECT * FROM test',
                getRowByA: 'SELECT b FROM test WHERE a = %n1 LIMIT 1',
                getRowByB: 'SELECT a FROM test WHERE b like %s1 LIMIT 1',
                clear: 'DELETE FROM test'
        });
        test.addRow(5,'testing');
        test.addRow(6,'also testing');
        var X = test.getRowByA(6)[0].a;
 */

if (typeof(SQL_LITE_INCLUDE) != "boolean") {
    SQL_LITE_INCLUDE = true;
    if (typeof(Cc) == "undefined")
        var Cc = Components.classes;
    if (typeof(Ci) == "undefined")
        var Ci = Components.interfaces;

    var undefined; //for fast tests for undefined values
    var RE_SET_ORDER = 1,
    RE_MATCH_ORDER = 0; //constants for matchAll
    String.prototype.matchAll = function(re, set) {
        //Required for SQLite parameter matching
        if (set == undefined) set = RE_MATCH_ORDER;
        //copy source string, initialize output array
        var ret = [],
        ts = this;
        while (re.test(ts)) {
            //match
            ret.push(ts.match(re));
            //remove match
            ts = ts.replace(re, '');
        }
        //If we have to swap orders, do so.
        if (set == RE_SET_ORDER) {
            var i, j, nRet = [];
            for (i = 0; i < ret.length; i++) {
                for (j = 0; j < ret[i].length; j++) {
                    if (i == 0) nRet[j] = [];
                    nRet[j].push(ret[i][j]);
                }
            }
            ret = nRet;
        }
        return ret;
    }
    function SQLite(dbName) {
        //simple call should return object, not just run function
        //if (! (this instanceof SQLite)) return new SQLite(db);
        //Create file.  AChrom is install directory for XUL app
        var file = Io.getFileFrom(userDataPath);
        //expand the file to the requested database
        file.append(dbName);
        //Create storage service instance
        var storageService = Cc["@mozilla.org/storage/service;1"].getService(Ci.mozIStorageService);
        //open the database connection
        this.conn = storageService.openDatabase(file);
        
        //Return SQLite object
        //return this;
    }
    //Cache for SQLite queries
    SQLite.prototype.queries = {};
    //Create new Query function(s)
    SQLite.prototype.queryFunction = function(name, query) {
        //If name is an object
        if (typeof name == 'object') {
            //initialize our return value
            var queryGroup = {};
            //for each function NVP
            for (var i in name) {
                //create a matching queryFunction
                queryGroup[i] = arguments.callee.apply(this, [i, name[i]]);
            }
            //return the query namespace
            return queryGroup;
        }

        //allow the internal function to access this SQLite instance
        var _this = this;
        //use _conn as a shortcut if necessary
        var _conn = this.conn;
        //Match all % parameters (for type matching)
        var params = query.matchAll(/%(i|n|s|f)(\d+)/i);
        //And replace with their real counterparts
        var realQuery = query.replace(/%(i|n|s|f)(\d+)/ig, '?$2');
        dumpMsg(realQuery);
        //create the statement
        var statement = _conn.createStatement(realQuery);       
        //Create the query function, loaded with the variables available above.
        var exec = function() {
            var i, idx;
            //walk through the parameters
            for (i = 0; i < params.length; i++) {
                //get the param index for the parameter
                idx = statement.getParameterIndex('?' + params[i][2]);
                switch (params[i][1].toLowerCase()) {
                    //it's a int32 number.
                case 'i':
                    statement.bindInt32Parameter(idx, arguments[idx]);
                    break;
                case 'n':
                    //it's a int64 number.
                    statement.bindInt64Parameter(idx, arguments[idx]);
                    break;
                case 'f':
                    //it's a float.  Use doubles.
                    statement.bindDoubleParameter(idx, arguments[idx]);
                    break;
                default:
                    //Assume it's a UTF8 string; ASCII and blobs will handle this fine on the input side
                    statement.bindUTF8StringParameter(idx, arguments[idx]);
                }
            }
            //Some variables for the return
            var cols = statement.columnCount,
            col = 0,
            colTypes = [],
            colNames = [],
            ret = [],
            colName,
            rowData;
            //If we've requested columns
            if (cols > 0) {
                while (statement.executeStep()) {
                    //execute like we're expecting a return value.
                    rowData = {};
                    for (col = 0; col < cols; col++) {
                        if (colNames[col] == undefined) {
                            //populate the colNames and colTypes arrays, as needed
                            colNames[col] = statement.getColumnName(col);
                            colTypes[col] = statement.getTypeOfIndex(col);
                        }
                        colName = colNames[col];
                        switch (colTypes[col]) {
                            //Depending on the column type (this really matters on output)
                            //fill the rowData object
                        case 0:
                            rowData[colName] = null;
                            break;
                        case 1:
                            rowData[colName] = statement.getInt64(col);
                            break;
                        case 2:
                            rowData[colName] = statement.getDouble(col);
                            break;
                        case 3:
                            rowData[colName] = statement.getUTF8String(col);
                            break;
                        case 4:
                            rowData[colName] = statement.getBlob(col);
                            break;
                        }
                    }
                    //And, once this row is done, append it to the array.
                    ret.push(rowData);
                }
            } else {
                //If there's no columns to grab, just run the query
                statement.execute();
            }
            //Either way, reset to stave off hobgoblins
            statement.reset();
            if (ret.length > 0) //We've got return values?  Good!  Send 'em up.
            return ret;
            //We don't?  Well, an insert will return the last insert ID
            //and a no-value select will return 0.
            return _conn.lastInsertRowID;
        };
        //Add exec to our set of precached queries, and return it.
        return this.queries[name] = exec;
    };
    
}