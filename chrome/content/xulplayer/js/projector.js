/* Copyright (c) 2006 Stanley Huang
   See the file LICENSE.txt for licensing information. */

function GetProjectorOpts()
{
    var opts = "";
    if (isChecked("optEnableProjector")) {
            var fuseWidth = getValueFloat("fuseWidth");
            var rollWidth = getValueFloat("rollWidth");
            var wndstyle = 0x80000000 | 0x00040000 | 0x10000000;
            var pos = new Array (parseFloat(getValue("fusePos1")), parseFloat(getValue("fusePos2")), parseFloat(getValue("fusePos3")));
            var ew = 100 + fuseWidth;
            if (pos[1]) ew += fuseWidth;

            opts += " -vf expand=::0::::16:" + ew + ":100:" + pos[0] + ":" + pos[1] + ":" + pos[2];

            var tabs = document.getElementById("screenColor").getElementsByTagName("tabpanel");
            for (var i = 0; i < tabs.length && pos[i]; i++) {
                if (tabs[i].firstChild.checked) {
                    opts += ",eq2=1";
                    var r = tabs[i].getElementsByTagName("textbox");
                    for (var j = 0; j < r.length; j++) {
                        opts += ":" + r[j].value;
                    }
                    opts += ":1:" + (pos[i]) + ":" + ((i == pos.length - 1) ? 100 : pos[i + 1]);
                }
            }
            
           opts += ",scale"

           for (var i = 0; i < pos.length && pos[i]; i++) {
                if (pos[i]) {
                    opts += ",rolloff2=" + getValue("fuseArg1") + ":" + getValue("fuseArg2") + ":" +
                            pos[i] + ":" + rollWidth;
                    opts += ",rolloff2=" + getValue("fuseArg1") + ":" + getValue("fuseArg2") + ":" +
                            pos[i] + ":" + rollWidth + ":1";
                }
            }

            if (isChecked("enableCurve")) {
                var i;
                for (i = 0; i < pos.length && pos[i]; i++) {
                    opts += ",geo=" + getValue("curveTop") + ":" + getValue("curveBottom") + ":" + (i == 0 ? 0 : (pos[i - 1] + rollWidth)) + ":" + (i == 0 ? (pos[0] - rollWidth): (pos[i] - pos[i - 1] - rollWidth * 2));
                }
                opts += ",geo=" + getValue("curveTop") + ":" + getValue("curveBottom") + ":" + (pos[i - 1] + rollWidth) + ":0";
            }
    
                
            if (isChecked("windowPos"))
                opts += " -geometry " + getValue("optW") + "x" + getValue("optH") + "+" + getValue("optX") + "+" + getValue("optY");
                
            opts += " -framedrop -nokeepaspect -loop 0 -wstyle " + wndstyle;
            //opts += " -framedrop -nokeepaspect -loop 0 -vo gl";
    } else {
        return null;
    }
    return opts;
}

var valueElements = new Array("fuseWidth", "rollWidth", "fuseArg1", "fuseArg2", "fusePos1", "fusePos2", "fusePos3", "optX", "optY", "optW", "optH", "curveTop", "curveBottom");

function initProjectorOpts()
{
    for (var i = 0; i < valueElements.length; i++) {
       	var e = document.getElementById(valueElements[i]);
        if (!e) continue;
        var v = e.getAttribute("data");
        if (v) e.value = v;
        var s = e;
        while ((s = s.nextSibling)) {
            if (s.nodeName == "scale") {
                s.value = parseFloat(v) * 100;
                break;
            }
        }
    }
}

function uninitProjectorOpts()
{
    for (var i = 0; i < valueElements.length; i++)
        saveValue(valueElements[i]);
}
