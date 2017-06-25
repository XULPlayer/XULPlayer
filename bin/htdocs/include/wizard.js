
if (mcport != "19819") {
	var e = document.getElementById("port");
	if (e) e.value = mcport;
}
if (window.setDefaults) setDefaults();

function GetUrlArg(name)
{
	var idx=document.location.href.indexOf(name+'=');
	if (idx<=0) return null;
	var argstr=document.location.href.substring(idx+name.length+1);
	idx = argstr.indexOf('&');
	return idx>=0?argstr.substring(0, idx):argstr;
}

function goNext(page)
{
	var url = page;
	if (mcport != "19819") url = url + "?p=" + mcport;
	document.location.href = url;
}
