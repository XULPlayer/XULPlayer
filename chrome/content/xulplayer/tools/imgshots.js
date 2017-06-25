var xuldoc = null;
var outputPath = null;
var oriwidth = null;
var oriheight = null;
var ratio = 1;
var fileduration = 0;
var makeImgCallback = null;
var srcfullPath;
var index = 0;
var progressTimer = null;

function winInit(arguments)
{
   xuldoc = document;
   var obj = arguments[0];
   var fileName = xuldoc.getElementById("src_text");
   srcfullPath = obj.filePath;
   fileName.value = getFileTitle(srcfullPath);
   var wobj = xuldoc.getElementById("width_text");
   wobj.value = obj.width;
   oriwidth = parseInt(obj.width);
   var hobj = xuldoc.getElementById("height_text");
   hobj.value = obj.height;
   oriheight = parseInt(obj.height);
   fileduration = parseInt(obj.duration);
   ratio = parseFloat(wobj.value) / parseFloat(hobj.value);
   makeImgCallback = arguments[1];
   //file name
   //width height
   //file duration
   //make file fun
}

function setOutput(title)
{
   var dir = Io.openDir(title);
   if(!dir)
   {
       return;
   }
   var text = xuldoc.getElementById("output_text");
   text.value = dir.path;
   outputPath = dir.path;
}

function keepHeight()
{
   var iskeep = xuldoc.getElementById("keep_aspect");
   if(!iskeep.checked)
       return;
   var wobj = xuldoc.getElementById("width_text");
   var h = parseInt(wobj.value) / ratio;
   var hobj = xuldoc.getElementById("height_text");
   hobj.value = h;
}

function keepWidth()
{
   var iskeep = xuldoc.getElementById("keep_aspect");
   if(!iskeep.checked)
       return;
   var hobj = xuldoc.getElementById("height_text");
   var w = parseInt(hobj.value) * ratio;
   var wobj = xuldoc.getElementById("width_text");
   wobj.value = w;
}

function shots_loop()
{
    var pics = xuldoc.getElementById("pics_text");
    pics = parseInt(pics.value);
    var hobj = xuldoc.getElementById("height_text");
    var h = parseInt(hobj.value);
    var wobj = xuldoc.getElementById("width_text");
    var w = parseInt(wobj.value);
    var name = xuldoc.getElementById("src_text");
    name = name.value;
    var proc = xuldoc.getElementById("progress_pics");
    var lab = xuldoc.getElementById("file_percent"); 
    if(index < pics)
    {
        name= name + index.toString() + ".jpg";
        var ss = (fileduration - 5) * 0.9 /pics * index + 5;
        ss = parseInt(ss);
        makeImgCallback(srcfullPath, oriwidth, oriheight, ss, w, h, 0, 0, outputPath, name);
        index++;
        proc.value = index * 100 / pics;
        lab.value = index.toString() + "/" + pics.toString();
        progressTimer = setTimeout(shots_loop, 500);
    }
    else
    {
      var btn = xuldoc.getElementById("btn_start");
      btn.disabled = false;
      lab.value = "";
      return;
    }
}

function start_shots()
{
   if(typeof makeImgCallback != 'function')
   {
        alert("Failed, please restart the programe and try again.");
        return;
   }
   index = 0;
   var btn = xuldoc.getElementById("btn_start");
   btn.disabled = true;
   var lab = xuldoc.getElementById("file_percent");
   var pics = xuldoc.getElementById("pics_text");
   lab.value = "0/" + pics.value;
   progressTimer = setTimeout(shots_loop, 500);
}

function stop_shots()
{
    clearTimeout(progressTimer);
}