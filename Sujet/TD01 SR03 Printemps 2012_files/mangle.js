// Copyright (c) 2004, 2005 Cisco Systems, Inc.

var webvpn_print_output=null
var webvpn_print_object=null
var webvpn_width=24*5
var webvpn_loaded=null
var sessionIndex=""
var loginTime=""
var webvpn_cookie
var securedesktop_cookie
var webvpn_onload_handler=null
var webvpn_onbeforeunload_handler=null
var webvpn_onload_set=false
var webvpn_onbeforeunload_set=false
var webvpn_frameset_set=false

var WEBVPN_LOC_HOST=1
var WEBVPN_LOC_HOSTNAME=2
var WEBVPN_LOC_HREF=3
var WEBVPN_LOC_PATHNAME=4
var WEBVPN_LOC_PORT=5
var WEBVPN_LOC_PROTOCOL=6

function webvpn_document_write(s,t){
 if(t) t.write(webvpn_mangle_html(s,true))
 else document.write(webvpn_mangle_html(s,true))
 webvpn_print_object=t
}

function webvpn_document_writeln(s,t){
 if(t) t.writeln(webvpn_mangle_html(s,true))
 else document.writeln(webvpn_mangle_html(s,true))
 if(webvpn_print_output!=null) webvpn_print_output+="\n"
 webvpn_print_object=t
}

function webvpn_dump_write(){
 if(webvpn_print_output){
  var s=webvpn_print_output
  webvpn_print_output=null
  if(webvpn_print_object){
   webvpn_print_object.write(webvpn_mangle_html(s))
   webvpn_print_object=null
  }
  else document.write(webvpn_mangle_html(s))
 }
}

function webvpn_mangle_eval(s){
 if(s&&s.length&&typeof(s)=="string"){
  s=s.replace(/([a-zA-z$0-9\._]+)\.writeln\(([^;]*)\)/g,"webvpn_document_writeln($2,$1)")
  s=s.replace(/([a-zA-z$0-9\._]+)\.write\(([^;]*)\)/g,"webvpn_document_write($2,$1)")
  s=s.replace(/([a-zA-z$0-9\._]+)\.cookie\s*=\s*([^;]*)/g,"webvpn_mangle_cookie($2,$1)")
  s=s.replace(/location.assign\(([^;]*)\)/g,"location.assign(webvpn_mangle_url($1))")
  s=s.replace(/location.replace\(([^;]*)\)/g,"location.replace(webvpn_mangle_url($1))")
  if(s.match(/location\s*=\s*([^;]*)(;?)/)!=null&&s.match(/\.open\(.+,.+,.*location\s*=.+\)/)==null)
   s=s.replace(/location\s*=\s*([^;]*)(;?)/g,"location=webvpn_mangle_url($1)$2")
  s=s.replace(/location\.href\s*=\s*([^;]*)(;?)/g,"location.href=webvpn_mangle_url($1)$2")
  s=s.replace(/window\.open\(([^,]*)(,.*)?\)/g,"window.open(webvpn_mangle_url($1)$2)")
  s=s.replace(/\.src\s*=\s*([^;]*)(;?)/g,".src=webvpn_mangle_url($1)$2")
  s=s.replace(/\.action\s*=\s*([^;]*)(;?)/g,".action=webvpn_mangle_url($1)$2")
  s=s.replace(/\.innerHTML\s*=\s*([^;]*)(;?)/g,".innerHTML=webvpn_mangle_html($1)$2")
  s=s.replace(/\.outerHTML\s*=\s*([^;]*)(;?)/g,".outerHTML=webvpn_mangle_html($1)$2")
 }
 return s
}

function webvpn_mangle_style(s){
 var r=""
 var re=/^url\((.*)\)$/i
 var a=s.split(";")
 var i,i2
 for(i=0;i<a.length;i++){
  if(r.length!=0){
   r+=";"
  }
  var idx=a[i].indexOf(":")
  if(idx!=(-1)){
   r+=a[i].substring(0, idx+1)
   var s2=a[i].substring(idx+1)
   var a2=re.exec(s2)
   if(a2!=null){
    r+="url("+webvpn_mangle_url(a2[1])+")"
   }else{
    r+=s2
   }
  }else{
   r+=a[i]
  }
 }
 return r
}

function webvpn_mangle_html(s,f){
 var retval=""
 if(s&&typeof(s)!="string") return s;
 if(f&&s){ 
  if(webvpn_print_output==null) webvpn_print_output=s
  else webvpn_print_output+=s
  if(webvpn_print_output.lastIndexOf(">")<webvpn_print_output.lastIndexOf("<")) return ""
  s=webvpn_print_output
  webvpn_print_output=null
 }
 if(s&&s.length){
  s=s.replace(/</g,"<<<")
  s=s.replace(/>/g,"><<")
  var r=s.split("<<")
  var x=new Array(r.length)
  for(var i=0;i<r.length;i++) x[i]=r[i]
  for(i=0;i<x.length;i++){
   var str=x[i]
   if(str.match(/<[^>]+>/)){
    var frameset=(str.toLowerCase().substr(1,8)=="frameset")
    var body_end=(str.toLowerCase().substr(1,5)=="/body")
    var inotes=(str.search(/\/iNotes\d*\.cab/i))
    var is_script=(str.toLowerCase().substr(1,6)=="script")
    var is_iframe=(str.toLowerCase().substr(1,6)=="iframe")
    var is_param=(str.toLowerCase().substr(1,5)=="param")
    var new_str=""
    var found_iframe_src = false
    if(frameset && !webvpn_frameset_set) {
     new_str+="<script language=\"JavaScript\">\n", 
     new_str+="if (window.onload) webvpn_onload_handler=window.onload\n"
     if(navigator.userAgent.indexOf("MSIE") != -1) new_str+="if(window.onbeforeunload) webvpn_onbeforeunload_handler=window.onbeforeunload\n"
     new_str+="</script>\n"
     webvpn_frameset_set=true;
    }
    if(str.toLowerCase().substr(1,4)=="html") new_str+="<script language=\"JavaScript\" src=\"/webvpn/mangle.js\"></script>";
    while(str.length){
     var re=/(\w*\s*=\s*'[^']*'|\w*\s*=\s*"[^"]*"|\w*\s*=\s*[^\s>]+)/
     var a=str.match(re)
     if(a&&a.length){
      if(a.index>0) new_str+=str.substring(0,a.index)
      var m=a[0]
      str=str.substring(a.index+m.length)
      re=/^(\w+)(\s*=\s*)/
      var attr=""
      var value=""
      var equals=""
      var quote=""
      var b=m.match(re)
      if(b&&b.length){
       attr=b[1]
       equals=b[2]
       value=m.substr((attr+equals).length)
       var c=value.match(/^'([^']*)'/)
       if(c&&c.length){
        quote="'"
        value=c[1]
       }
       else{
        c=value.match(/^"([^"]*)"/)
        if(c&&c.length){
         quote='"'
         value=c[1]
        }
       }
      }
      if(b&&b.length){
       if((is_param && (attr.toLowerCase().search(/^(value)$/) != -1) ||
           (attr.toLowerCase().search(/^(href|src|action|archive|codebase|cite|background|bgsound|profile|lowsrc|usemap|dynsrc|borderimage)$/) != -1))) value=webvpn_mangle_url(value)
       if(attr.toLowerCase()=="codebase"&&inotes!=-1) value=value.replace(/iNotes\d*\.cab#Version=\d+,\d+,\d+,\d+/ig, "/iNotes.car#Version=99,99,99,99")
       if(attr.toLowerCase()=="style") value=webvpn_mangle_style(value) 
       if(is_script&&attr.toLowerCase()=="src") value+=",SCRIPT"
       if(is_iframe&&attr.toLowerCase()=="src") found_iframe_src=true
       if(attr.toLowerCase().substr(0,2)=="on") value=webvpn_mangle_eval(value)
       if(attr.toLowerCase()=="onload" && frameset && !webvpn_onload_set){
        value+=";webvpn_show_frame_toolbar()"
        webvpn_onload_set=true
       }
       else if(attr.toLowerCase()=="onbeforeunload" && frameset && !webvpn_onbeforeunload_set){
        value="webvpn_onbeforeunload();"+value
        webvpn_onbeforeunload_set=true
       }
       new_str+=attr+equals+quote+value+quote
      }
     }
     else{
      if (frameset) {
       if(!webvpn_onload_set) {
        new_str+=" onload=\"webvpn_show_frame_toolbar()\""
        webvpn_onload_set=true
       }
       if(!webvpn_onbeforeunload_set) {
        new_str+=" onbeforeunload=\"webvpn_onbeforeunload()\""
        webvpn_onbeforeunload_set=true
       }
      }
      else if (body_end) {
       new_str+="<script language=\"JavaScript\">\n"
       new_str+="if(window.onload) webvpn_onload_handler=window.onload\n"
       new_str+="window.onload=webvpn_show_body_toolbar\n"

       if (navigator.userAgent.indexOf("MSIE") != -1) {
        new_str+="if(window.onbeforeunload) webvpn_onbeforeunload_handler=window.onbeforeunload\n"
        new_str+="window.onbeforeunload=webvpn_onbeforeunload\n"
       }
       new_str += "</script>\n"
      }
      else if (is_iframe && !found_iframe_src) {
         if (new_str)
           new_str+=' src="javascript:false"'
         else
           str='<iframe src="javascript:false">'
      }
      new_str+=str
      str=''
     }
    }
    retval+=new_str
   }
   else{
    retval+=str
   }
  }
 }
 return retval
}

function webvpn_mangle_url(s){
 if (s=="")
   return "javascript:false"
 if(s&&s.length){
  var host=window.location.host
  var prot=window.location.protocol
  var re
  var rprot
  var rhost
  var rpath
  var ruser
  var rport
  var r

  var colon=host.indexOf(":")
  if (colon!=-1) {
   var hport=host.substr(colon+1)
   if ((prot.length==5 && parseInt(hport)==80) ||
      (prot.length==6 && parseInt(hport)==443)) {
    host=host.substring(0,colon)
   }
  }
  re=/^httpss+:\/\/(.*)/
  if((r=re.exec(s))!=null){
   s="https://"+r[1]
   re=/^(https:\/\/[^\/]+)\/http\/([0-9]+\/.*)/
   if((r=re.exec(s))!=null) s=r[1]+"/https/"+r[2]
  }
  re=/^(https?):\/\/([^\/]+)\/(.*)/
  if((r=re.exec(s))!=null){
   rprot=r[1]
   rhost=r[2]
   rpath=r[3]
   ruser=""
   rport="0"
   re=/^([^@]+@)(.*)/
   if((r=re.exec(rhost))!=null){
    ruser=r[1]
    rhost=r[2]
   }
   re=/^([^:]+):(\d+)/
   if((r=re.exec(rhost))!=null){
    rhost=r[1]
    rport=r[2]
   }
   re=/^https?\/\d+\/.+/
   if(!re.test(rpath)){
    if(rhost==host){
     re=/^(https?):\/\/([^\/]+)\/(https?)\/([0-9]+)\/([^\/]+)\/(.*)/
     if((r=re.exec(window.location.href))!=null){
      rhost = r[5]
      rport = r[4]
      rprot = r[3]
     }
    }

    s=prot+"//"+ruser+host+"/"+rprot+"/"+rport+"/"+rhost+"/"+rpath
   }
   return s
  }
  re=/^(https?):\/\/([^\/]+)/
  if((r=re.exec(s))!=null){
   rprot=r[1]
   rhost=r[2]
   ruser=""
   rport="0"
   re=/^([^@]+@)(.*)/
   if((r=re.exec(rhost))!=null){
    ruser=r[1]
    rhost=r[2]
   }
   re=/^([^:]+):(\d+)/
   if((r=re.exec(rhost))!=null){
    rhost=r[1]
    rport=r[2]
   }
   if(rhost==host){
    re=/^(https?):\/\/([^\/]+)\/(https?)\/([0-9]+)\/([^\/]+)\/(.*)/
    if((r=re.exec(window.location.href))!=null){
     rhost=r[5]
     rport=r[4]
     rprot=r[3]
    }
   }
   return prot+"//"+ruser+host+"/"+rprot+"/"+rport+"/"+rhost
  }
  re=/^\/(.*)/
  if(re.test(s)){
   re=/^\/https?\/\d+\/.+/
   if(!re.test(s)){
    path=window.location.pathname
    re=/^(\/[^\/]+\/[^\/]+\/[^\/\\]+)\/?/
    if((r=re.exec(path))!=null) return r[1]+s
    re=/^(https?\/[^\/]+\/[^\/\\]+)\/?/
    if((r=re.exec(path))!=null) return "/"+r[1]+s
   }
   return s
  }
  re=/^\.\.\//
  if(re.test(s)){
   r=document.location.pathname.match(/\//g)
   var depth=0
   if(r&&r.length>4) depth=r.length-4
   re=/^((\.\.\/)+)(.*)/
   if((r=re.exec(s))!=null){
    var dotdot=r[1]
    s=r[r.length-1] 
    r=dotdot.match(/\.\.\//g)
    if(r){
     if(r.length<depth) depth=r.length
     while(depth--) s="../"+s
    } 
   }
  }
 }
 return s
}

function webvpn_get_session_info(){
 var webvpnName=document.cookie.indexOf("webvpn=")
 if (webvpnName!=-1){
  var semicolon=document.cookie.indexOf(';',webvpnName)
  var webvpnValue
  if (semicolon != -1) webvpnValue=document.cookie.substring(webvpnName+7,semicolon)
  else webvpnValue=document.cookie.substring(webvpnName+7)
  var atSign=webvpnValue.indexOf('@')
  var lastAtSign
  for(var i=0;i<3&&atSign!=-1;i++){ 
   if(i==1) sessionIndex=webvpnValue.substring(lastAtSign+1,atSign)
   else if(i==2) loginTime=webvpnValue.substring(lastAtSign+1,atSign)
   lastAtSign=atSign
   atSign=webvpnValue.indexOf('@',atSign+1)
  }
 }
}


function webvpn_kill_cookie(theCookie){
 if(theCookie){
  var lastPlus=theCookie.lastIndexOf('+')
  if (lastPlus!=-1){
   if(theCookie.charAt(lastPlus+1)=='.'){
    document.cookie=theCookie+"=; path=/http; expires=Thu, 01 Jan 1970 22:00:00 GMT;"
    if(navigator.appName=="Netscape"){
     var is_mozilla=navigator.userAgent.indexOf("Mozilla/");
     if(is_mozilla!=-1&&parseInt(navigator.userAgent.charAt(is_mozilla+8))<5) document.cookie = theCookie + "=; path=/https; expires=Thu, 01 Jan 1970 22:00:00 GMT;"
    }
   }
   else if(theCookie.charAt(lastPlus+1)=='/'){
    document.cookie=theCookie+"=; path=/http"+theCookie.substring(lastPlus+1)+"; expires=Thu, 01 Jan 1970 22:00:00 GMT;"
    document.cookie=theCookie+"=; path=/https"+theCookie.substring(lastPlus+1)+"; expires=Thu, 01 Jan 1970 22:00:00 GMT;"
   }
  }
 }
}

function webvpn_unmangle_cookie(web_cookies){  
 var new_cookies=""
 if(web_cookies){
  webvpn_get_session_info()
  if(sessionIndex&&loginTime){
   var cookie_arr=web_cookies.split(';')
   var equals
   var plus
   var afterPlus
   var atSign
   var lastAtSign
   var thisSessionIndex
   var thisLoginTime
   var cookieName
   var newCookieName
   for(var i=0;i<cookie_arr.length;i++){
    newCookieName=""
    equals=cookie_arr[i].indexOf('=')
    if(equals!=-1){
     cookieName=cookie_arr[i].substring(0,equals)
     plus=cookieName.indexOf('+')
     if(plus!=-1){
      atSign=cookieName.indexOf('@',plus+1)
      if(atSign!=-1){
       if(cookieName.charAt(plus+1)=='+'){
        afterPlus=0
        numPluses=0
        while(plus!=-1&&plus<atSign){
         if(cookieName.charAt(plus+1)=='+'){
          newCookieName+=cookieName.substring(afterPlus,plus+1)
          numPluses++
         }
         else break
         afterPlus=plus+2;
         plus=cookieName.indexOf('+',afterPlus);
        }
        newCookieName+ cookieName.substring(afterPlus)
       }
       thisSessionIndex=cookieName.substring(plus+1,atSign)
       lastAtSign=atSign
       atSign=cookieName.indexOf('@',atSign+1)
       if(atSign!=1){
        thisLoginTime=cookieName.substring(lastAtSign+1,atSign);
        if(sessionIndex==thisSessionIndex&&loginTime==thisLoginTime){
         if(newCookieName) new_cookies+=newCookieName.substring(0,plus-numPluses)+"="+cookie_arr[i].substring(equals+1)+";"
         else new_cookies+=cookieName.substring(0,plus)+"="+cookie_arr[i].substring(equals+1)+";"
        }
        else webvpn_kill_cookie(cookieName)
       }
      }
     }
     else if(cookieName.indexOf("webvpn")<0) new_cookies+=cookie_arr[i]+";"
    }
   }
  }
 }
 return new_cookies;
}


function Trim(str){
 return str.replace(/^\s*/,"").replace(/\s*$/,"")
}

function webvpn_check_cookie_index(cookieName){
 var returnName=cookieName
 if(cookieName){
  var cookieIndex=cookieName.indexOf("@999+")
  if(cookieIndex!=-1){
   var subCookieName=cookieName.substring(0, cookieIndex)
   var postName=cookieName.substring(cookieIndex+5)
   var anIndex=document.cookie.indexOf(subCookieName)
   while(anIndex!=-1){
    var postIndex=document.cookie.indexOf(postName,anIndex)
    if(postIndex==anIndex+ subCookieName.length+5){
     returnName=subCookieName+document.cookie.substring(postIndex-5,postIndex)+postName
     break
    }
    anIndex=document.cookie.indexOf(subCookieName,anIndex+1)	
   }
  }
 }
 return returnName
}

function webvpn_mangle_cookie(theCookie,doc){
 if(!doc) doc=document
 if(theCookie){
  webvpn_get_session_info()
  if(sessionIndex&&loginTime){
   var cookie_arr=theCookie.split(';')
   var equals
   var attrName
   var attrVal
   var cookieName
   var cookieVal
   var domainVal=""
   var expiresVal=""
   var pathVal=""
   var secure=false
   for(var i=0;i<cookie_arr.length;i++){
    equals=cookie_arr[i].indexOf('=')
    if (equals!=-1){
     attrName=cookie_arr[i].substring(0, equals)
     attrVal=cookie_arr[i].substring(equals+1)
     attrName=Trim(attrName)
     attrVal=Trim(attrVal)
     if(i==0){
      cookieName=attrName
      cookieVal=attrVal
     }
     else{
      if(attrName.toLowerCase()=="path") pathVal=attrVal
      else if(attrName.toLowerCase()=="domain") domainVal=attrVal
      else if(attrName.toLowerCase()=="expires"){
       var theDate=new Date()
       var oldDate=new Date(attrVal)
       if(oldDate&&oldDate<theDate) expiresVal = attrVal
      }
     }
    }
    else{
     attrName=Trim(cookie_arr[i])
     if(attrName.toLowerCase()=="secure") secure=true
    }
   }
   var newCookie=""
   var plus=cookieName.indexOf('+')
   if(plus==-1) newCookie = cookieName
   else {
    var last_plus=0
    while(plus!=-1){
     newCookie+=cookieName.substring(last_plus,plus+1)+"+"
     last_plus=plus+1
     plus=cookieName.indexOf('+',last_plus)
    }
    newCookie+=cookieName.substring(last_plus)
   }
   newCookie+="+"+sessionIndex+"@"+loginTime+"@999"+"+"
   var sslCookie=""
   if(domainVal!=""){
    if(domainVal.charAt(0)!='.') newCookie+='.'
    newCookie+=domainVal
    if(pathVal!=""){
     if(pathVal != '/') newCookie += pathVal
     else pathVal = "";
    }
    newCookie=webvpn_check_cookie_index(newCookie)
    sslCookie=newCookie
    newCookie+="="+cookieVal+"; path=/http;"
    var is_mozilla=-1
    if (navigator.appName=="Netscape"){
     is_mozilla = navigator.userAgent.indexOf("Mozilla/")
     if (is_mozilla != -1&&parseInt(navigator.userAgent.charAt(is_mozilla+8)) < 5) is_mozilla = -1
    }
    if (is_mozilla>-1) sslCookie+="="+cookieVal+"; path=/https;"
    else sslCookie = ""
   }
   else{
    if(pathVal==""){
     pathVal=document.location.pathname
     pathVal=pathVal.substring(0,pathVal.lastIndexOf('/')+1)
    }
    pathVal=webvpn_mangle_url(pathVal)
    pathVal=pathVal.substring(pathVal.indexOf('/', 1))
    newCookie=webvpn_check_cookie_index(newCookie)
    sslCookie=newCookie
    newCookie+=pathVal+"="+cookieVal+"; path=/http"+pathVal+";"
    sslCookie+=pathVal+"="+cookieVal+"; path=/https"+pathVal+";"
   }
   if(expiresVal){
    newCookie+=" expires="+expiresVal+";"
    if (sslCookie) sslCookie+=" expires="+expiresVal+";"
   }
   if(secure){
    newCookie+=" secure;"
    if(sslCookie!="") sslCookie+=" secure;"
   }
   doc.cookie=newCookie
   if(sslCookie!="") doc.cookie=sslCookie
  }
 }
}

function webvpn_onload(){
 var webvpnName=document.cookie.indexOf("webvpn=")
 if (webvpnName!=-1){
  var semicolon=document.cookie.indexOf(';',webvpnName)
  if (semicolon!=-1) webvpn_cookie=document.cookie.substring(webvpnName+7,semicolon)
  else webvpn_cookie=document.cookie.substring(webvpnName+7)
 }
 var securedesktopName=document.cookie.indexOf("sdesktop=")
 if (securedesktopName!=-1){
  var semicolon=document.cookie.indexOf(';',securedesktopName)
  if (semicolon!=-1) securedesktop_cookie=document.cookie.substring(securedesktopName+9,semicolon)
  else securedesktop_cookie=document.cookie.substring(securedesktopName+9)
 }
 if (webvpn_onload_handler&&webvpn_onload_handler!=webvpn_show_body_toolbar&&webvpn_onload_handler!=webvpn_show_frame_toolbar){
     var local_hndlr=webvpn_onload_handler
     webvpn_onload_handler=null
     local_hndlr()
 }
}

function webvpn_onbeforeunload(){
 var retval=null
 if(webvpn_cookie&&webvpn_cookie.length) document.cookie="webvpn="+webvpn_cookie+";path=/"
 if(securedesktop_cookie&&securedesktop_cookie.length) document.cookie="sdesktop="+securedesktop_cookie+";path=/"
 if(webvpn_onbeforeunload_handler&&webvpn_onbeforeunload_handler!=webvpn_onbeforeunload) {
     var local_hndlr=webvpn_onbeforeunload_handler
     webvpn_onbeforeunload_handler=null
     retval=local_hndlr()
 }
 if(retval) return retval
}

function webvpn_moveit(){
 if(document.all&&document.all.webvpnicons&&document.all.webvpnicons.style) document.all.webvpnicons.style.pixelLeft=(document.all.webvpnicons.style.pixelLeft!=0)?0:(document.body.clientWidth-(webvpn_width))
 else if(document.webvpnicons) document.webvpnicons.moveTo(((!document.webvpnicons.left)?(window.innerWidth-(webvpn_width)-16):0),0)
 else if(document.getElementById&&document.body.clientWidth) document.getElementById("webvpnicons").style.left=""+((document.getElementById("webvpnicons").style.left!="0px")?0:(document.body.clientWidth-(webvpn_width)))+"px"
}

function webvpn_logout(){
 if (confirm("Are you sure you want to close your session?")){
  if (navigator.userAgent.indexOf("Windows CE; PPC; 240x320") != -1){
   location.href="/webvpn_logout.html"
  }else{
    window.open("/webvpn_logout.html","webvpntoolbar","height=10,width=10,screenX=0,screenY=0,left=0,top=0")
    top.location.href="/webvpn_logout.html"
  }
 }
}

function webvpn_open_toolbar(){
 var x=0
 if (navigator.userAgent.indexOf("Windows CE; PPC; 240x320") == -1){
  if(screen.width>=215&&navigator.userAgent.indexOf("Opera")==-1) x=screen.width-215
   var newwin=window.open("/webvpn/toolbartest.html","webvpntoolbar","resizable,height=360,width=207,top=0,left="+x+"screenY=0,screenX="+x)
   if(newwin) newwin.top.focus()
 }
}

function webvpn_go(){
 var s=window.prompt("Enter URL/Web Address","")
 if(s&&s.length){
  webvpn_onbeforeunload()
  top.location.href="/webvpn/enter_url.html?url="+escape(s)
 }
}

function webvpn_show_toolbar_in_frame(thewin){
 var foundwin=null
 var i
 if(top.webvpn_loaded) return
 for(i=0;i<thewin.frames.length;i++){
  var win=thewin.frames[i]
  var winw=0
  var winh=0
  if (win.frames&&win.frames.length&&win.document&&(!win.document.getElementsByTagName||!win.document.getElementsByTagName("iframe").length)){
   webvpn_show_toolbar_in_frame(win)
   if(top.webvpn_loaded) return
   continue
  } 
  if(!win.document) continue
  if(win.document.all&&win.document.body){
   winw=win.document.body.clientWidth
   winh=win.document.body.clientHeight
  }
  else if(win.document.webvpnicons){
   winw=win.innerWidth-16
   winh=win.innerHeight
  }
  else if(win.document.getElementById){
   if(win.document.body){
    winw=win.document.body.clientWidth
    winh=win.document.body.clientHeight
   }
   if(!winw) winw=(win.innerWidth-16)
   if(!winh) winh=(win.innerHeight-16)
  }
  if(winw>webvpn_width){
   foundwin=win
   if(winh>24) break
  } 
 }
 if (foundwin){
  foundwin.webvpn_show_toolbar_on_window(foundwin)
  top.webvpn_loaded=foundwin
 }
}

function webvpn_show_frame_toolbar(){
 webvpn_onload()
 top.webvpn_loaded=null
 top.webvpn_show_toolbar_in_frame(top)
 if(!top.webvpn_loaded) top.setTimeout("webvpn_show_frame_toolbar()",500)
}

function webvpn_show_body_toolbar(){
 webvpn_onload()
 if(self==top||top.webvpn_loaded==self){
  webvpn_show_toolbar_on_window(self)
  top.webvpn_loaded=self
 }
}

function webvpn_show_toolbar_on_window(win){
 if(win.document.all&&win.document.all.webvpnicons&&win.document.all.webvpnicons.style) win.document.all.webvpnicons.style.visibility="visible"
 else if(win.document.webvpnicons) win.document.webvpnicons.visibility="show"
 else if(win.document.getElementById&&win.document.getElementById("webvpnicons")&&win.document.getElementById("webvpnicons").style) win.document.getElementById("webvpnicons").style.visibility="visible"
}

function webvpn_frame(width){
 webvpn_width=width
 webvpn_moveit()
}

function webvpn_unmangle_location(locObj, which)
{
 if (locObj==null) locObj=location;
 var path=locObj.pathname;
 if (path){
  if (path.charAt(0) == '/')
   baseIndex=1
  else
   baseIndex=0
  var parts=path.split("/")
  if ((parts.length)&&(parts[baseIndex]=="http"||parts[baseIndex]=="https")) {
   switch(which){
    case WEBVPN_LOC_HOST:
     if (parseInt(parts[baseIndex+1])>0)
      return(parts[baseIndex+2]+":"+parts[baseIndex+1])
     else
      return parts[baseIndex+2]
    break;

    case WEBVPN_LOC_HOSTNAME:
     return parts[baseIndex+2]
    break;

    case WEBVPN_LOC_HREF:
     var port = parseInt(parts[baseIndex+1])?":"+parts[baseIndex+1]:"";
     parts.splice(0,baseIndex+3,parts[baseIndex]+":/",parts[baseIndex+2]+port)
     return (parts.join("/")+locObj.search)
    break;

    case WEBVPN_LOC_PATHNAME:
     if (baseIndex==1)
      parts.splice(baseIndex,3)
     else
      parts.splice(baseIndex,3,"")
     return (parts.join("/"))
    break;

    case WEBVPN_LOC_PORT:
     if (parseInt(parts[baseIndex+1]))
      return parts[baseIndex+1]
     else
      return ""
    break;

    case WEBVPN_LOC_PROTOCOL:
     return (parts[baseIndex]+":")
    break;

    default:
    break;
   }
  }
 }

 switch(which){
  case WEBVPN_LOC_HOST:
   return locObj.host
  break;

  case WEBVPN_LOC_HOSTNAME:
   return locObj.hostname
  break;

  case WEBVPN_LOC_HREF:
   return locObj.href
  break;

  case WEBVPN_LOC_PATHNAME:
   return locObj.pathname
  break;

  case WEBVPN_LOC_PORT:
   return locObj.port
  break;

  case WEBVPN_LOC_PROTOCOL:
   return locObj.protocol
  break;

  default:
  break;
 }
}

function webvpn_mangle_attr(s){
 var retval=""
 if(s&&typeof(s)!="string") return s;
 if(s&&s.length){
  var str=s
  var new_str=""
  while(str.length){
   var re=/(\w*\s*=\s*'[^']*'|\w*\s*=\s*"[^"]*"|\w*\s*=\s*[^\s>]+)/
   var a=str.match(re)
   if(a&&a.length){
    if(a.index>0) new_str+=str.substring(0,a.index)
     var m=a[0]
     str=str.substring(a.index+m.length)
     re=/^(\w+)(\s*=\s*)/
     var attr=""
     var value=""
     var equals=""
     var quote=""
     var b=m.match(re)
     if(b&&b.length){
      attr=b[1]
      equals=b[2]
      value=m.substr((attr+equals).length)
      var c=value.match(/^'([^']*)'/)
      if(c&&c.length){
       quote="'"
       value=c[1]
      }
      else{
       c=value.match(/^"([^"]*)"/)
       if(c&&c.length){
        quote='"'
        value=c[1]
       }
      } 
     }
     if(b&&b.length){
      if(attr.toLowerCase().search(/^(url)$/) != -1) value=webvpn_mangle_url(value)
       new_str+=attr+equals+quote+value+quote
     }
   }
   else{
    new_str+=str
    str=''
   }
  }
  retval+=new_str
 }
 return retval
}


function webvpn_unmangle_url(s){
 // r[1]=webvpnPrefix r[2]=proto r[3]=port r[4]=uname/host r[5]=path
 if(s&&s.length&&typeof(s)=="string"){
   var re=/^(https?:\/\/[^\/]+)?\/(https?)\/(\d+)\/([^\/]+)\/(.*)/
   var r=re.exec(s);
   if (r!=null){
    var ns="";
    if (r[1]){
     ns += r[2] + "://" + r[4];
     if (r[3]!="0")
      ns += ":" + r[3];
    }
    ns += "/" + r[5];
    return ns
   }
 }
 return s
}

function webvpn_frames(x)
{
   var arr = new Array();
   var index = -1;
  
   for(i = 0; i < x.frames.length; i++)
   {
      if(x.frames[i].name == "webvpn_timeout")
   {
     index = i;
   }
   arr[i] = x.frames[i];
   }
   if(index >= 0)
     arr.splice(index, 1);
    
   return arr;
}