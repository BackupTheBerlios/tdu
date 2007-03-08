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

//Thanks to http://delete.me.uk/2004/09/ieproto.html
// Emulating Prototyping of DOM Objects in Internet Explorer by Paul Sowden

function IEworkaround()
{
    alert("Using IE mode with IE quirks. Try another Browser (Firefox,Opera,Konqueror,Safari,etc...(?maybe IE7?)) for full Support")
    // some Workarounds for IE:

    __IEcreateElement = document.createElement;

    document.createElement = function (tagName) {
      var element = __IEcreateElement(tagName);

      var interface = new Element;
      for (method in interface)
        element[method] = interface[method];

      return element;
    }

    Element = function () {};

    Element.prototype.getAttribute = function (attribute) {
      if (attribute == "class") attribute = "className";
      if (attribute == "for") attribute = "htmlFor";
      return this[attribute];
    }

    Element.prototype.setAttribute = function (attribute, value) {
      if (attribute == "class") attribute = "className";
      if (attribute == "for") attribute = "htmlFor";
      if (attribute == "onclick") 
      {
        //alert(value);
        this[attribute] = function () { value }
        this[attribute] = new Function ("evt",value );
        //alert ( "function () { "+value+" }")
        //alert( function () { this.value })
        return
      }
      
      this[attribute] = value;
    }
}
