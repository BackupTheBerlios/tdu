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
