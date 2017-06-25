 /*============================================================*/ 
 var loopCallback = null;
 var openerCloseCallback = null;
 var progressTimer = null;
 var xuldoc = null;
 
 function winInit(arguments)
 {
  xuldoc = document;
  var win = document.getElementById('info_box');

  var infoObject = arguments[0];
  loopCallback = arguments[1];
  openerCloseCallback = arguments[2];
  var funIndex = 4;
  
  var main = document.getElementById('mainPanel');
  var bottom = document.getElementById('bottom_spacer');
  var buttonGroup = document.getElementById('button_groups');
  var bottomBt = document.getElementById('bottom_button');
  var top = document.getElementById('top_spacer');
  for (obj in infoObject)
  {
    if(obj == 'title')
    {
     var title = document.getElementById("title_desc");
     title.value = infoObject[obj];
     continue;
    }
    else if( obj.indexOf('progress') >= 0)
    {
      /*var stack = HtmHelper.make('stack',{flex:'1'});
      var progressFront = HtmHelper.make('box',{id:"procbar_front", class:"progressbar"});
      var progressBack = HtmHelper.make('box', {id:"procbar_background", class:"progressbar"});
      stack.appendChild(progressBack);
      stack.appendChild(progressFront);
      main.insertBefore(stack, top.nextSibling);
      progressBack.style.width = win.width * 0.9 + 'px';
      progressBack.style.left = win.width * 0.05 + 'px';
      progressFront.style.left = win.width * 0.05 + 'px';
      progressFront.style.width = infoObject[obj] + 'px';*/
      var hbox = HtmHelper.make('hbox',{flex:'1'});
      var spacer =HtmHelper.make('spacer', {flex:'1'});
      hbox.appendChild(spacer);
      var proc = HtmHelper.make('progressmeter',{id:obj + '_value', flex:'10', height:'6px'});
      hbox.appendChild(proc);
      spacer =HtmHelper.make('spacer', {flex:'1'});
      hbox.appendChild(spacer);
      main.insertBefore(hbox, top.nextSibling);
      continue;
    }
    else if( obj.indexOf('button') >= 0)
    {
      var button = HtmHelper.make('button', {id:obj+"_label",label:infoObject[obj].toString(),oncommand:arguments[funIndex]});
      funIndex++;
      var vbox = HtmHelper.make('vbox',{align:"center",flex:'1'});
      var spacer =HtmHelper.make('spacer', {flex:'4'});
      vbox.appendChild(spacer);
      vbox.appendChild(button);
      spacer =HtmHelper.make('spacer', {flex:'4'});
      vbox.appendChild(spacer);
      buttonGroup.insertBefore(vbox,bottomBt);
      continue;  
    }
    else if( obj.indexOf('text') >= 0)
    {
      //var textbox = HtmHelper.make();
    }
    else if(obj == 'btn_close' )
    {
      var btnlast =  document.getElementById(obj);
      btnlast.label = infoObject[obj];
      continue;  
    }
    else if(obj == 'stopLoop')
    {
      continue;
    }
    var hbox = HtmHelper.make('hbox',{flex:'1'});
    var label = HtmHelper.make('label',{class:'label_text_right label_head', value:infoObject[obj], flex:"3"});
    hbox.appendChild(label);
    label = HtmHelper.make('label',{class:'label_text_left label_mid', value:':', flex:"2"});
    hbox.appendChild(label);
    label = HtmHelper.make('label',{id:obj+'_value', class:'lable_text_left label_back', value:'', flex:"4"});
    hbox.appendChild(label);
    main.insertBefore(hbox, bottom);    
   }  
   
   // Start progerss
   progressTimer = setTimeout('progressLoop()', 500);
   
 }
 
 function progressLoop() {
    var curValue = null;
    if(typeof loopCallback == 'function') {
        curValue = loopCallback();   
        for (obj in curValue)
        {
          /*if(obj == 'progress')
          {
            var proc = curValue[obj];
            if(proc < 0) proc = 0;
            if(proc > 100)proc = 100;
            var progressFront = xuldoc.getElementById('procbar_front');
            var progressBack = xuldoc.getElementById('procbar_background');
            progressFront.style.width = proc *  parseInt(progressBack.style.width) / 100 + 'px';
            continue;
          }*/
          if( obj.indexOf('button') >= 0)
          {
            var val = xuldoc.getElementById(obj + '_label');
            val.label = curValue[obj];
            continue;
          }
          else if(obj == 'btn_close' )
          {
            var val = xuldoc.getElementById(obj);
            val.label = curValue[obj];
            continue;
          }
          else if(obj == 'stopLoop')
          {
            if(curValue[obj])
              return;
          }
          var val = xuldoc.getElementById(obj + '_value');
          if(val != null)
          {
            val.value = curValue[obj];
          }
        }
        progressTimer = setTimeout('progressLoop()', 500);
    }
 }
 
 function onUserClose() {
    window.close();
    clearTimeout(progressTimer);
    if(typeof openerCloseCallback == 'function')
    {
      openerCloseCallback();
    }
 }
