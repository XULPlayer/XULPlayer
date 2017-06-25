//this is for convert options

function time_catch()
{
    var item = plMan.getCurrentItemNode();
    if(!item)
    {
        return;
    }
    
    var time = mp.getCurPos();
    var radio = document.getElementById("ratio_mark_in");
    var node = DomHelper.getChildNode(item);
    if(radio.selected)
    {
        var innode = DomHelper.getNodeByAttribute(node,"node","key","time.markIn");
        if(!innode)
        {
           innode =  DomHelper.addElementNode(plMan.getCurDoc(),item,'node');
           innode.setAttribute("key",'time.markIn'); 
        }
        innode.setAttribute('value', time); 
        document.getElementById("time_mark_in").value = mp.getTimeString(time /1000);
    }
    else
    {
        var outnode = DomHelper.getNodeByAttribute(node,"node","key","time.markOut");
        if(!outnode)
        {
            outnode = DomHelper.addElementNode(plMan.getCurDoc(),item,'node');
            outnode.setAttribute('key','time.markOut');
        }
        outnode.setAttribute('value', time);
        document.getElementById("time_mark_out").value =  mp.getTimeString(time /1000);
    }
}

function time_clear()
{
    var item = plMan.getCurrentItemNode();
    if(!item)
    {
        return;
    }
    var node = DomHelper.getChildNode(item);
    var innode = DomHelper.getNodeByAttribute(node,"node","key","time.markIn");
    if(innode)
    {
        innode.setAttribute('value', 0);
        document.getElementById("time_mark_in").value = "";
    }
    var outnode = DomHelper.getNodeByAttribute(node,"node","key","time.markOut");
    if(outnode)
    {
        outnode.setAttribute('value', 0);
        document.getElementById("time_mark_out").value = "";
    }
}

function Option_Time_Stirng()
{
    var item = plMan.getCurrentItemNode();
    if(!item)
    {
        return null;
    }
    var node = DomHelper.getChildNode(item);
    var innode = DomHelper.getNodeByAttribute(node,"node","key","time.markIn");
    var outnode = DomHelper.getNodeByAttribute(node,"node","key","time.markOut");
    if(innode && outnode)
    {        
        var timein = parseInt(innode.getAttribute("value"))/1000;
        var timeout = parseInt(outnode.getAttribute("value"))/1000;
        if(timeout > 0 && timeout > timein)
        {
            return '-ss ' + timein.toFixed(0) + ' -endpos ' + (timeout - timein).toFixed(0) + ' ';
        }
        else
        {
            return null;
        }
    }
    else
    {
        return null;
    }
}

function onPrefsChange()
{
    showPrefsTab(true);
}

var prefsXsl;

function showPrefsTab(expandSidebar)
{
    var extDoc = new XMLDom();
    var prefsXml = mc.m_plugin.getPrefsXml();
    if (!prefsXsl) prefsXsl = loadXML("prefs/prefs.xsl");
    extDoc.loadXML(prefsXml);
    transformXML(extDoc, prefsXsl, "prefsBox");
}