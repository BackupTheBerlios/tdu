/* tdu - TreeDiskUsage -
   copyright michael.heide@student.uni-siegen.de

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

var xmlHttp
var prefixes = new Array("","k","M","G","T","P","E")
var ieworkaround = false

//decodes &xxx; in a string
String.prototype.htmlDecode = function ()
{
   mydiv = document.createElement("div");
   mydiv.innerHTML = this;
   if (!mydiv.lastChild.nodeValue) return this;
   return mydiv.lastChild.nodeValue;
}

//for internationalization
function msg(s)
{
  if (typeof messages !== "undefined")
  {
   if (typeof messages[s] !== "undefined")
    return messages[s].htmlDecode()
  }
  return s
}

//var counter=2
function sitereset()
{
  location.reload()
}

//TODO: elid? line? dachte elid loeschen, ist aber line!?
function closeMinus(elid,dir,line)
{
  var el_parent, el_to_del, id
  elid=line
  //alert("elid:"+elid+" dir:"+dir+"line:"+line)
  //return
  //var weiter = true
  el_parent = document.getElementById(elid)
  el_to_del = el_parent.nextSibling
  //while (el_to_del=el_parent.firstChild)
  //if (att_id = el_to_del.getAttribute("id"))
  att_id = el_to_del.getAttribute("id")
  if (true)
  {
    //while (el_to_del.getAttribute("id").indexOf(elid)==0)
    while (att_id.indexOf(line+".")==0)
    {
      //alert("remove:"+el_to_del)
      el_to_del.parentNode.removeChild(el_to_del)
      el_to_del = el_parent.nextSibling
      //alert( el_to_del.nodeName)
      //if (! att_id = getAttribute("id")) break
      //alert("att_id:"+att_id+" line:"+line)
      //if (el_to_del.nodeName!="TR") break
      if (el_to_del==null || el_to_del.nodeName!="TR") break
      att_id = el_to_del.getAttribute("id")
    }
  }
  div = el_parent.firstChild.firstChild
  div.setAttribute("onclick",div.getAttribute("openPlusFct"))
  el_img=document.createElement("img")
  el_img.setAttribute("alt","+")
  if (div.firstChild.getAttribute("src")=="minusoru.png")
    el_img.setAttribute("src","plusoru.png")
  else if (div.firstChild.getAttribute("src")=="minusor.png")
    el_img.setAttribute("src","plusor.png")
  else
    el_img.setAttribute("src","plus.png")
  div.replaceChild(el_img,div.firstChild)
}

//TODO: erstmal - deaktivieren, erst nach wirklichem oeffnen aktivieren
function editPlus(elid)
{
  //TODO: praezise ansteuern, nicht raten
  div = document.getElementById(elid).firstChild.firstChild
  //div.setAttribute("class","notlinked")
  //div.removeAttribute("onclick");
  //div.setAttribute("onclick","closeMinus('"+elid+"','.','1')")
  //alert(closeMinus)
  div.setAttribute("onclick",div.getAttribute("closeMinusFct"))
  el_img=document.createElement("img")
  el_img.setAttribute("alt","-")
  if (div.firstChild.getAttribute("src")=="plusoru.png")
    el_img.setAttribute("src","minusoru.png")
  else if (div.firstChild.getAttribute("src")=="plusor.png")
    el_img.setAttribute("src","minusor.png")
  else
    el_img.setAttribute("src","minus.png")
  div.replaceChild(el_img,div.firstChild)
}

function openPlus(dir,line)
{
  if (dir.length==0 || line.length==0)
  {
    alert ("dir or line is 0") //debug
    return
  }
  xmlHttp=GetXmlHttpObject()
  if (xmlHttp==null)
  {
    alert (msg("Browser does not support HTTP Request"))
    return
  }
  var url="tdu-getdir.php"
  url=url+"?dir="+dir
  url=url+"&line="+line
  url=url+"&fileno="+fileno
  //alert("dir: "+dir+" line:"+line+" fileno:"+fileno)
  //url=url+"&deep="+deep
  //url=url+"&counter="+counter

  editPlus(line)
  xmlHttp.onreadystatechange=openPlusInsert
  xmlHttp.open("GET",url,true)
  xmlHttp.send(null)
}

// number of needle in str; author hede
function numberOf(needle,str)
{
  var last = str.lastIndexOf(needle)
  var act = str.indexOf(needle)
  var counter = 0
  if (act==-1)
  {
    return 0
  }
  counter++
  while (act<last)
  {
    counter++
    act = str.indexOf(needle,act+1)
  }
  return counter
}


function openPlusInsert()
{
  if (xmlHttp.readyState==4 || xmlHttp.readyState=="complete")
  {
    //document.getElementById("1b").innerHTML=xmlHttp.responseText
    var response = xmlHttp.responseText.split("\n")
    //var out = ""
    //var tmp = response[0].split("/")
    var tr = ""

    if (response[0]=="error")
    {
      //alert("Serverscript meldet Fehler:\n"+response[2])
      alert(msg("Serverscipt returned error:\n")+response[2])
      return
    }
    if (response[0]!=startzeit)
    {
      //alert("Neue Suche wurde gestartet oder es ist ein Scriptfehler aufgetreten, Seite wird zurueckgesetzt.")
      alert(msg("New Search or scripting error => resetting Page."))
      sitereset()
      return
    }
    var dir = response[1]
    var elid = response[2]
    var el_tr = document.getElementById(elid)
    try {
      el_tr.nodeType
    } catch (e) {
      //alert(e+"\nKann kein Element zum einf&uuml;gen finden")
      alert(e+"\n"+msg("There is no Element to insert"))
      return
    }
    //alert (response.length)
    //return

    //es kann eingefügt werden, vorher noch altes plus entfernen

    /* mach ich anders
    //berechnen der Prozentangaben
    for (var i = 3; i < response.length-1; i++)
    {
      var line = response[i].split("/");
    }
    */
    
    //gesamtgroesse
    var parentsize
    try
    {
      //parentsize = parseInt(el_tr.firstChild.nextSibling.firstChild.firstChild.data)

      switch (sizedisplay)
      {
        //Typ1: Prozentwerte für jedes Verzeichnis alleine
        //      Elternverzeichnis als Referenz
        case 1:
          parentsize = el_tr.getAttribute("dirsize")
          break;
        //Typ2: Prozentwerte pro Gesamtplatz
	case 2:
          parentsize = fullsize
          break;
        //Typ3: Prozentwerte fuer jedes Verzeichnis alleine
        //      Groesstes als Referenz
	case 3:
          parentsize = response[3].split("/")[1];
          break;
	default:
	  throw(msg("illegal graphic method (parameter sd)"))
	  //throw("Methode zur Groessenanzeige nicht gueltig")
      }
    } 
    catch (e)
    {
      //alert("Bei Ermittlung der Gesamtgroesse: "+e)
      alert(msg("While calculating size summary: ")+e)
      parentsize = 0
    }
    //alert("Parentsize:"+parentsize)
    parentsize = parseInt(parentsize)

    //alles fertig zum einfuegen
    for (var i = 3; i < response.length-1; i++)
    {            //TODO: += ???
      //TODO: hier wird nochmal gesplittet. ist es vielleicht guenstiger
      //      oben den split hier weiterzunutzen?
      var line = response[i].split("/");
//alert("line: "+line)
      //var abstand = ""
      if (line.length!=3)
      {
        alert(msg("Scripterror: line.length!=3 in Element ")+i)
        return
      }
      var newdir = dir+"/"+line[0]
      var newelid = elid+"."+i
      var el_newtr = document.createElement("tr")
      var el_td1 = document.createElement("td")
      var el_td2 = document.createElement("td")
      el_newtr.setAttribute("id",newelid)
      el_newtr.setAttribute("dirsize",line[1]) // zur Prozentberechnung

      //erste spalte
      var el_div = document.createElement("div")
      var abstand = (numberOf(".",elid) * 12) + 12
      //for (var j = 0; j <= numberOf(".",elid); j++)
      //{
      //  abstand += "."
      //}
      //var tn_td1text = document.createTextNode(abstand)
      //el_div.appendChild(tn_td1text)
      var dirimgsrc="weiss.png"
      var dirimgalt=" "
      var el_dirimg = document.createElement("img")
      el_div.style.marginLeft = abstand + "px" // + "10px"

      //Verzeichnisname
      var tn_td1text = document.createTextNode(line[0])

      // plus oder minus einfuegen, je nachdem, welcher Typ vorliegt
      switch (line[2])
      {
        case "1": 
          //abstand += "+" //has subdirs
          el_div.setAttribute
                         ("onclick","openPlus('"+newdir+"','"+newelid+"')")
//el_div.onclick = alert("1")
//el_div.onclick = openPlus('"+newdir+"','"+newelid+"')
//el_div.onclick = new Function ("evt","openPlus('"+newdir+"','"+newelid+"');" );
//el_div.onclick = new Function("evt","document.bgColor = blue;" );
//el_div.onclick = function() { alert(newdir) };
          el_div.setAttribute
                         ("openPlusFct","openPlus('"+newdir+"','"+newelid+"')")
          el_div.setAttribute
                         ("closeMinusFct","closeMinus('"+elid+"','"+newdir+"','"+newelid+"')")
          el_div.setAttribute("class","linked")
          dirimgalt="+"
          if  (i < response.length-2) dirimgsrc="plusoru.png"
          else dirimgsrc="plusor.png"
          break;
        case "0": 
          el_div.setAttribute("class","floated")
          //abstand += "#" //has no subdirs
        case "2": 
          el_div.setAttribute("class","floated")
          //tn_td1text = document.createTextNode(".")
          //abstand += "~" //thisdir-entry so has no subdir 
          dirimgalt=" "
          if  (i < response.length-2) dirimgsrc="vertikaloru.png"
          else dirimgsrc="vertikalor.png"
          break;
      }
      el_dirimg.setAttribute("src",dirimgsrc)
      el_dirimg.setAttribute("alt",dirimgalt)
      el_div.appendChild(el_dirimg)

      var el_div2 = document.createElement("div")
      el_div2.setAttribute("class","linked")
      //el_div2.className = "linked";
      el_div2.setAttribute("onclick","alert('"+newdir+"')")
      //TODO: irgendwann wie folgt:
      //      el_div2.setAttribute("onclick","alert('"+scanroot+newdir+"')")
      el_div2.appendChild(tn_td1text)
      el_td1.setAttribute("class","floated")
      el_td1.appendChild(el_div)
      el_td1.appendChild(el_div2)

      //Groesse berechnen
      var sizeH = line[1]
      
      var j=0
      while (sizeH > 1024)
      {
        sizeH = parseInt (sizeH / 1024)
        j++
      }
      //TODO: unnoetig, wenn nicht eingerueckt wird
      /*
      if (sizeH<100)
      {
        if (sizeH<10)
	{
          sizeH = "_"+sizeH
	}
        sizeH = "_"+sizeH
      }*/
      sizeH = sizeH + prefixes[j]
      
      //Groesse einfuegen
      var el_td2div = document.createElement("div")
      //TODO: wenn nicht eingerueckt wird, kann darauf verzichtet werden
      //el_td2div.style.marginLeft = abstand + "px" // + "10px"
      var tn_td2text = document.createTextNode(sizeH)
      el_td2div.appendChild(tn_td2text)
      el_td2.appendChild(el_td2div)
      el_td2.setAttribute("class","size");

      //Balkengrafik berechnen
      var percent
      if (parentsize == 0)
      {
        percent = line[1]
      }
      else
      {
        percent = parseInt((parseInt(line[1]) / parentsize) * 100)
	if (percent<10)
	{
          percent = "0"+percent
	}
      }

      //Balkengrafik einfuegen
      var el_img = document.createElement("img")
      el_img.setAttribute("alt",percent+"%")
      el_img.setAttribute("title",percent+"%")
      el_img.setAttribute("src","img/"+percent+".png")
      var el_td3div = document.createElement("div")
      el_td3div.style.marginLeft = abstand + "px" // + "10px"
      el_td3div.appendChild(el_img)
      var el_td3 = document.createElement("td")
      el_td3.setAttribute("class","size");
      el_td3.appendChild(el_td3div)

      //alles zusammenfuegen
      el_newtr.appendChild(el_td1)
      el_newtr.appendChild(el_td2)
      el_newtr.appendChild(el_td3)

      //Neue Zeile einfuegen
      el_tr.parentNode.insertBefore(el_newtr,el_tr.nextSibling);
      el_tr=el_newtr
      //tr.parentNode.insertBefore(newtr,tr);
      //tr.parentNode.appendChild(newtr);
    /*
      out += '</span>\n' // <tr><td id="'+response[0]
      var line = response[i].split("/");
      if (line.length!=2)
      {
        alert("Scriptfehler: line.length!=2 in Element "+i)
	return
      }
      out += '<tr><td id="'+deep+'.'+i+'"><span onclick="openPlus(\''+dir+'/'+line[0]+'\',\''+deep+'.'+i+'\')">'
      for (var j = 0; j < numberOf(".",deep); j++)
      {
        out += "&nbsp;"
      }
      out += "+" + line[0] + "</span></td><td>"
      out += line[1] + '</td></tr><span id="' + deep + "." + i + 'c">'
    */
    }
    //alert (out+" eingefügt in: "+deep+"c")
    //document.getElementById(deep+"c").innerHTML=out

  }
}

function GetXmlHttpObject()
{ 
var objXMLHttp=null
if (window.XMLHttpRequest)
  {
    objXMLHttp=new XMLHttpRequest()
  }
else if (window.ActiveXObject)
  { //TODO: Msxml2.XMLHTTP für neuere IE
    objXMLHttp=new ActiveXObject("Microsoft.XMLHTTP")
    if (!ieworkaround)
    {
      ieworkaround = true
      IEworkaround()
    }
  }
return objXMLHttp
}

function ChangeFile (Formular)
{
  var Element = Formular.filename.selectedIndex;
  //Check = confirm ("Wenn sie die Datei wechseln, wird die Seite neu geladen.")
  Check = confirm (msg("Changing the file will reset this page."))
  if (Check == true)
  {
    if (typeof refresh !== 'undefined')
      window.clearTimeout(refresh);
    location = "tdu.php?sd="+sizedisplay+"&fileno="+Formular.filename.options[Element].value
    return
  }
  //Formular.filename.options[fileno].focus()
  //alert ("Formular:"+Formular+"\nFormular.filename:"+Formular.filename+"\nFormular.filename.selectedIndex:"+Formular.filename.selectedIndex+"\nfileno:"+fileno)
  //Formular.filename.selectedIndex=fileno
  for (var j = 0; j < Formular.filename.length; j++)
  {
    if (Formular.filename[j].value==fileno)
      Formular.filename[j].selected=true;
  }
  //TODO: wieder anschalten des autorefresh
}

function ChangeDisplay (Formular)
{
  var Element = Formular.sizedisplay.selectedIndex;
  if (Formular.sizedisplay.options[Element].value != 0)
  {
    //Check = confirm ("Wenn Sie diese Einstellung aendern, muss die Seite neu geladen werden.")
    Check = confirm (msg("Changing this setting will reset this page."))
    if (Check == true)
    {
      if (typeof refresh !== 'undefined')
        window.clearTimeout(refresh);
      location = "tdu.php?fileno="+fileno+"&sd=" + Formular.sizedisplay.options[Element].value
      return
    }
    //Formular.sizedisplay.options[sizedisplay].focus()
    Formular.sizedisplay.selectedIndex=sizedisplay-1
  }
}

