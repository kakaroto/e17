#!/usr/local/bin/elev8

var exports=exports||this;exports.OAuth=function(a){function t(a,b,c,d){var e=n(b),f=n(c),g=e.length,h,i,j,k;g>a.blocksize&&(e=a.hash(e),g=e.length),e=e.concat(m(a.blocksize-g)),i=e.slice(0),j=e.slice(0);for(k=0;k<a.blocksize;k++)i[k]^=92,j[k]^=54;h=a.hash(i.concat(a.hash(j.concat(f))));if(d)return p(h);return q(h)}function s(a){if(a!==undefined){var b=a,c,d;b.constructor===String&&(b=n(b)),this instanceof s?c=this:c=new s(a),d=c.hash(b);return p(d)}if(!(this instanceof s))return new s;return this}function r(a,b){return a<<b|a>>>32-b}function q(a){var b="",c=a.length,d;for(d=0;d<c;d++)b+=String.fromCharCode(a[d]);return b}function p(a){var b=[],c=a.length,d;for(d=0;d<c;d++)b.push((a[d]>>>4).toString(16)),b.push((a[d]&15).toString(16));return b.join("")}function o(a){var b=[],c;for(c=0;c<a.length*32;c+=8)b.push(a[c>>>5]>>>24-c%32&255);return b}function n(a){var b=[],c,d;for(d=0;d<a.length;d++)c=a.charCodeAt(d),c<128?b.push(c):c<2048?b.push(192+(c>>6),128+(c&63)):c<65536?b.push(224+(c>>12),128+(c>>6&63),128+(c&63)):c<2097152&&b.push(240+(c>>18),128+(c>>12&63),128+(c>>6&63),128+(c&63));return b}function m(a){var b=Array(++a);return b.join(0).split("")}function l(){var b;typeof a.Titanium!="undefined"&&typeof a.Titanium.Network.createHTTPClient!="undefined"?b=a.Titanium.Network.createHTTPClient():typeof require!="undefined"?b=(new require("xhr")).XMLHttpRequest():b=new a.XMLHttpRequest;return b}function k(a){function b(){return Math.floor(Math.random()*h.length)}a=a||64;var c=a/8,d="",e=c/4,f=c%4,g,h=["20","21","22","23","24","25","26","27","28","29","2A","2B","2C","2D","2E","2F","30","31","32","33","34","35","36","37","38","39","3A","3B","3C","3D","3E","3F","40","41","42","43","44","45","46","47","48","49","4A","4B","4C","4D","4E","4F","50","51","52","53","54","55","56","57","58","59","5A","5B","5C","5D","5E","5F","60","61","62","63","64","65","66","67","68","69","6A","6B","6C","6D","6E","6F","70","71","72","73","74","75","76","77","78","79","7A","7B","7C","7D","7E"];for(g=0;g<e;g++)d+=h[b()]+h[b()]+h[b()]+h[b()];for(g=0;g<f;g++)d+=h[b()];return d}function j(){return parseInt(+(new Date)/1e3,10)}function i(a,b,c,d){var e=[],f,h=g.urlEncode;for(f in c)c[f]!==undefined&&c[f]!==""&&e.push(g.urlEncode(f)+"="+g.urlEncode(c[f]+""));for(f in d)d[f]!==undefined&&d[f]!==""&&(c[f]||e.push(h(f)+"="+h(d[f]+"")));return[a,h(b),h(e.sort().join("&"))].join("&")}function h(a){var b=[],c,d;for(c in a)a[c]&&a[c]!==undefined&&a[c]!==""&&(c==="realm"?d=c+'="'+a[c]+'"':b.push(c+'="'+g.urlEncode(a[c]+"")+'"'));b.sort(),d&&b.unshift(d);return b.join(", ")}function g(a){if(!(this instanceof g))return new g(a);return this.init(a)}function e(a){var b=arguments,c=b.callee,d=b.length,e,f=this;if(!(this instanceof c))return new c(a);if(a!=undefined)for(e in a)a.hasOwnProperty(e)&&(f[e]=a[e]);return f}function d(a){var b=arguments,c=b.callee,d,f,g,h,i,j,k,l=/^([^:\/?#]+?:\/\/)*([^\/:?#]*)?(:[^\/?#]*)*([^?#]*)(\?[^#]*)?(#(.*))*/;uri=this;if(!(this instanceof c))return new c(a);uri.scheme="",uri.host="",uri.port="",uri.path="",uri.query=new e,uri.anchor="";if(a!==null){d=a.match(l),f=d[1],g=d[2],h=d[3],i=d[4],j=d[5],k=d[6],f=f!==undefined?f.replace("://","").toLowerCase():"http",h=h?h.replace(":",""):f==="https"?"443":"80",f=f=="http"&&h==="443"?"https":f,j=j?j.replace("?",""):"",k=k?k.replace("#",""):"";if(f==="https"&&h!=="443"||f==="http"&&h!=="80")g=g+":"+h;uri.scheme=f,uri.host=g,uri.port=h,uri.path=i||"/",uri.query.setQueryParams(j),uri.anchor=k||""}}function c(){}function b(a){var b=arguments,c=b.callee,d=b.length,e,f=this;if(!(this instanceof c))return new c(a);for(e in a)a.hasOwnProperty(e)&&(f[e]=a[e]);return f}c.prototype={join:function(a){a=a||"";return this.values().join(a)},keys:function(){var a,b=[],c=this;for(a in c)c.hasOwnProperty(a)&&b.push(a);return b},values:function(){var a,b=[],c=this;for(a in c)c.hasOwnProperty(a)&&b.push(c[a]);return b},shift:function(){throw"not implimented"},unshift:function(){throw"not implimented"},push:function(){throw"not implimented"},pop:function(){throw"not implimented"},sort:function(){throw"not implimented"},ksort:function(a){var b=this,c=b.keys(),d,e,f;a==undefined?c.sort():c.sort(a);for(d=0;d<c.length;d++)f=c[d],e=b[f],delete b[f],b[f]=e;return b},toObject:function(){var a={},b,c=this;for(b in c)c.hasOwnProperty(b)&&(a[b]=c[b]);return a}},b.prototype=new c,d.prototype={scheme:"",host:"",port:"",path:"",query:"",anchor:"",toString:function(){var a=this,b=a.query+"";return a.scheme+"://"+a.host+a.path+(b!=""?"?"+b:"")+(a.anchor!==""?"#"+a.anchor:"")}},e.prototype=new b,e.prototype.toString=function(){var a,b=this,c=[],d="",e="",f=g.urlEncode;b.ksort();for(a in b)b.hasOwnProperty(a)&&a!=undefined&&b[a]!=undefined&&(e=f(a)+"="+f(b[a]),c.push(e));c.length>0&&(d=c.join("&"));return d},e.prototype.setQueryParams=function(a){var b=arguments,c=b.length,d,e,f,g=this,h;if(c==1){if(typeof a=="object")for(d in a)a.hasOwnProperty(d)&&(g[d]=a[d]);else if(typeof a=="string"){e=a.split("&");for(d=0,f=e.length;d<f;d++)h=e[d].split("="),g[h[0]]=h[1]}}else for(d=0;d<arg_length;d+=2)g[b[d]]=b[d+1]};var f="1.0";g.prototype={realm:"",requestTokenUrl:"",authorizationUrl:"",accessTokenUrl:"",init:function(a){var b="",c={enablePrivilege:a.enablePrivilege||!1,callbackUrl:a.callbackUrl||"oob",consumerKey:a.consumerKey,consumerSecret:a.consumerSecret,accessTokenKey:a.accessTokenKey||b,accessTokenSecret:a.accessTokenSecret||b,verifier:b,signatureMethod:a.signatureMethod||"HMAC-SHA1"};this.realm=a.realm||b,this.requestTokenUrl=a.requestTokenUrl||b,this.authorizationUrl=a.authorizationUrl||b,this.accessTokenUrl=a.accessTokenUrl||b,this.getAccessToken=function(){return[c.accessTokenKey,c.accessTokenSecret]},this.getAccessTokenKey=function(){return c.accessTokenKey},this.getAccessTokenSecret=function(){return c.accessTokenSecret},this.setAccessToken=function(a,b){b&&(a=[a,b]),c.accessTokenKey=a[0],c.accessTokenSecret=a[1]},this.getVerifier=function(){return c.verifier},this.setVerifier=function(a){c.verifier=a},this.setCallbackUrl=function(a){c.callbackUrl=a},this.request=function(a){var b,e,m,n,o,p,q,r,s,t,u,v,w=[],x,y={},z,A;b=a.method||"GET",e=d(a.url),m=a.data||{},n=a.headers||{},o=a.success||function(){},p=a.failure||function(){},A=function(){var a=!1;for(var b in m)typeof m[b].fileName!="undefined"&&(a=!0);return a}(),x=a.appendQueryString?a.appendQueryString:!1,c.enablePrivilege&&netscape.security.PrivilegeManager.enablePrivilege("UniversalBrowserRead UniversalBrowserWrite"),q=l(),q.onreadystatechange=function(){if(q.readyState===4){var a=/^(.*?):\s*(.*?)\r?$/mg,b=n,c={},d="",e;if(!q.getAllResponseHeaders){if(!!q.getResponseHeaders){d=q.getResponseHeaders();for(var f=0,g=d.length;f<g;++f)c[d[f][0]]=d[f][1]}}else{d=q.getAllResponseHeaders();while(e=a.exec(d))c[e[1]]=e[2]}var h={text:q.responseText,requestHeaders:b,responseHeaders:c};q.status>=200&&q.status<=226||q.status==304||q.status===0?o(h):q.status>=400&&q.status!==0&&p(h)}},s={oauth_callback:c.callbackUrl,oauth_consumer_key:c.consumerKey,oauth_token:c.accessTokenKey,oauth_signature_method:c.signatureMethod,oauth_timestamp:j(),oauth_nonce:k(),oauth_verifier:c.verifier,oauth_version:f},t=c.signatureMethod;if((!("Content-Type"in n)||n["Content-Type"]=="application/x-www-form-urlencoded")&&!A){z=e.query.toObject();for(r in z)y[r]=z[r];for(r in m)y[r]=m[r]}urlString=e.scheme+"://"+e.host+e.path,u=i(b,urlString,s,y),v=g.signatureMethod[t](c.consumerSecret,c.accessTokenSecret,u),s.oauth_signature=v,this.realm&&(s.realm=this.realm);if(x||b=="GET")e.query.setQueryParams(m),w=null;else if(!A)if(typeof m=="string")w=m,"Content-Type"in n||(n["Content-Type"]="text/plain");else{for(r in m)w.push(g.urlEncode(r)+"="+g.urlEncode(m[r]+""));w=w.sort().join("&"),"Content-Type"in n||(n["Content-Type"]="application/x-www-form-urlencoded")}else if(A){w=new FormData;for(r in m)w.append(r,m[r])}q.open(b,e+"",!0),q.setRequestHeader("Authorization","OAuth "+h(s)),q.setRequestHeader("X-Requested-With","XMLHttpRequest");for(r in n)q.setRequestHeader(r,n[r]);q.send(w)};return this},get:function(a,b,c){this.request({url:a,success:b,failure:c})},post:function(a,b,c,d){this.request({method:"POST",url:a,data:b,success:c,failure:d})},getJSON:function(a,b,c){this.get(a,function(a){b(JSON.parse(a.text))},c)},postJSON:function(a,b,c,d){this.post(a,JSON.stringify(b),function(a){c(JSON.parse(a.text))},d)},parseTokenRequest:function(a){var b=0,c=a.split("&"),d=c.length,e={};for(;b<d;++b){var f=c[b].split("=");e[g.urlDecode(f[0])]=g.urlDecode(f[1])}return e},fetchRequestToken:function(a,b){var c=this;c.setAccessToken("","");var d=c.authorizationUrl;this.get(this.requestTokenUrl,function(b){var e=c.parseTokenRequest(b.text);c.setAccessToken([e.oauth_token,e.oauth_token_secret]),a(d+"?"+b.text)},b)},fetchAccessToken:function(a,b){var c=this;this.get(this.accessTokenUrl,function(b){var d=c.parseTokenRequest(b.text);c.setAccessToken([d.oauth_token,d.oauth_token_secret]),c.setVerifier(""),a(b)},b)}},g.signatureMethod={"HMAC-SHA1":function(b,c,d){var e,f,h=g.urlEncode;b=h(b),c=h(c||""),e=b+"&"+c,f=t(s.prototype,e,d);return a.btoa(f)}},g.urlEncode=function(a){function b(a){var b=a.toString(16).toUpperCase();b.length<2&&(b=0+b);return"%"+b}if(!a)return"";a=a+"";var c=/[ \r\n!*"'();:@&=+$,\/?%#\[\]<>{}|`^\\\u0080-\uffff]/,d=a.length,e,f=a.split(""),g;for(e=0;e<d;e++)if(g=f[e].match(c))g=g[0].charCodeAt(0),g<128?f[e]=b(g):g<2048?f[e]=b(192+(g>>6))+b(128+(g&63)):g<65536?f[e]=b(224+(g>>12))+b(128+(g>>6&63))+b(128+(g&63)):g<2097152&&(f[e]=b(240+(g>>18))+b(128+(g>>12&63))+b(128+(g>>6&63))+b(128+(g&63)));return f.join("")},g.urlDecode=function(a){if(!a)return"";return a.replace(/%[a-fA-F0-9]{2}/ig,function(a){return String.fromCharCode(parseInt(a.replace("%",""),16))})},s.prototype=new s,s.prototype.blocksize=64,s.prototype.hash=function(a){function A(a,b,c,d){switch(a){case 0:return b&c|~b&d;case 1:case 3:return b^c^d;case 2:return b&c|b&d|c&d}return-1}var b=[1732584193,4023233417,2562383102,271733878,3285377520],c=[1518500249,1859775393,2400959708,3395469782],d,e,f,g,h,i,j,k,l,p,q,s,t,u,v,w,x,y,z;a.constructor===String&&(a=n(a.encodeUTF8())),f=a.length,g=Math.ceil((f+9)/this.blocksize)*this.blocksize-(f+9),e=Math.floor(f/4294967296),d=Math.floor(f%4294967296),h=[e*8>>24&255,e*8>>16&255,e*8>>8&255,e*8&255,d*8>>24&255,d*8>>16&255,d*8>>8&255,d*8&255],a=a.concat([128],m(g),h),i=Math.ceil(a.length/this.blocksize);for(j=0;j<i;j++){k=a.slice(j*this.blocksize,(j+1)*this.blocksize),l=k.length,p=[];for(q=0;q<l;q++)p[q>>>2]|=k[q]<<24-(q-(q>>2)*4)*8;s=b[0],t=b[1],u=b[2],v=b[3],w=b[4];for(x=0;x<80;x++)x>=16&&(p[x]=r(p[x-3]^p[x-8]^p[x-14]^p[x-16],1)),y=Math.floor(x/20),z=r(s,5)+A(y,t,u,v)+w+c[y]+p[x],w=v,v=u,u=r(t,30),t=s,s=z;b[0]+=s,b[1]+=t,b[2]+=u,b[3]+=v,b[4]+=w}return o(b)};return g}(this),function(a){var b="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";a.btoa=a.btoa||function(a){var c=0,d=a.length,e,f,g="";for(;c<d;c+=3)e=[a.charCodeAt(c),a.charCodeAt(c+1),a.charCodeAt(c+2)],f=[e[0]>>2,(e[0]&3)<<4|e[1]>>4,(e[1]&15)<<2|e[2]>>6,e[2]&63],isNaN(e[1])&&(f[2]=64),isNaN(e[2])&&(f[3]=64),g+=b.charAt(f[0])+b.charAt(f[1])+b.charAt(f[2])+b.charAt(f[3]);return g}}(this)


/*
    http://www.JSON.org/json2.js
    2011-02-23

    Public Domain.

    NO WARRANTY EXPRESSED OR IMPLIED. USE AT YOUR OWN RISK.

    See http://www.JSON.org/js.html


    This code should be minified before deployment.
    See http://javascript.crockford.com/jsmin.html

    USE YOUR OWN COPY. IT IS EXTREMELY UNWISE TO LOAD CODE FROM SERVERS YOU DO
    NOT CONTROL.


    This file creates a global JSON object containing two methods: stringify
    and parse.

        JSON.stringify(value, replacer, space)
            value       any JavaScript value, usually an object or array.

            replacer    an optional parameter that determines how object
                        values are stringified for objects. It can be a
                        function or an array of strings.

            space       an optional parameter that specifies the indentation
                        of nested structures. If it is omitted, the text will
                        be packed without extra whitespace. If it is a number,
                        it will specify the number of spaces to indent at each
                        level. If it is a string (such as '\t' or '&nbsp;'),
                        it contains the characters used to indent at each level.

            This method produces a JSON text from a JavaScript value.

            When an object value is found, if the object contains a toJSON
            method, its toJSON method will be called and the result will be
            stringified. A toJSON method does not serialize: it returns the
            value represented by the name/value pair that should be serialized,
            or undefined if nothing should be serialized. The toJSON method
            will be passed the key associated with the value, and this will be
            bound to the value

            For example, this would serialize Dates as ISO strings.

                Date.prototype.toJSON = function (key) {
                    function f(n) {
                        // Format integers to have at least two digits.
                        return n < 10 ? '0' + n : n;
                    }

                    return this.getUTCFullYear()   + '-' +
                         f(this.getUTCMonth() + 1) + '-' +
                         f(this.getUTCDate())      + 'T' +
                         f(this.getUTCHours())     + ':' +
                         f(this.getUTCMinutes())   + ':' +
                         f(this.getUTCSeconds())   + 'Z';
                };

            You can provide an optional replacer method. It will be passed the
            key and value of each member, with this bound to the containing
            object. The value that is returned from your method will be
            serialized. If your method returns undefined, then the member will
            be excluded from the serialization.

            If the replacer parameter is an array of strings, then it will be
            used to select the members to be serialized. It filters the results
            such that only members with keys listed in the replacer array are
            stringified.

            Values that do not have JSON representations, such as undefined or
            functions, will not be serialized. Such values in objects will be
            dropped; in arrays they will be replaced with null. You can use
            a replacer function to replace those with JSON values.
            JSON.stringify(undefined) returns undefined.

            The optional space parameter produces a stringification of the
            value that is filled with line breaks and indentation to make it
            easier to read.

            If the space parameter is a non-empty string, then that string will
            be used for indentation. If the space parameter is a number, then
            the indentation will be that many spaces.

            Example:

            text = JSON.stringify(['e', {pluribus: 'unum'}]);
            // text is '["e",{"pluribus":"unum"}]'


            text = JSON.stringify(['e', {pluribus: 'unum'}], null, '\t');
            // text is '[\n\t"e",\n\t{\n\t\t"pluribus": "unum"\n\t}\n]'

            text = JSON.stringify([new Date()], function (key, value) {
                return this[key] instanceof Date ?
                    'Date(' + this[key] + ')' : value;
            });
            // text is '["Date(---current time---)"]'


        JSON.parse(text, reviver)
            This method parses a JSON text to produce an object or array.
            It can throw a SyntaxError exception.

            The optional reviver parameter is a function that can filter and
            transform the results. It receives each of the keys and values,
            and its return value is used instead of the original value.
            If it returns what it received, then the structure is not modified.
            If it returns undefined then the member is deleted.

            Example:

            // Parse the text. Values that look like ISO date strings will
            // be converted to Date objects.

            myData = JSON.parse(text, function (key, value) {
                var a;
                if (typeof value === 'string') {
                    a =
/^(\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2}):(\d{2}(?:\.\d*)?)Z$/.exec(value);
                    if (a) {
                        return new Date(Date.UTC(+a[1], +a[2] - 1, +a[3], +a[4],
                            +a[5], +a[6]));
                    }
                }
                return value;
            });

            myData = JSON.parse('["Date(09/09/2001)"]', function (key, value) {
                var d;
                if (typeof value === 'string' &&
                        value.slice(0, 5) === 'Date(' &&
                        value.slice(-1) === ')') {
                    d = new Date(value.slice(5, -1));
                    if (d) {
                        return d;
                    }
                }
                return value;
            });


    This is a reference implementation. You are free to copy, modify, or
    redistribute.
*/

/*jslint evil: true, strict: false, regexp: false */

/*members "", "\b", "\t", "\n", "\f", "\r", "\"", JSON, "\\", apply,
    call, charCodeAt, getUTCDate, getUTCFullYear, getUTCHours,
    getUTCMinutes, getUTCMonth, getUTCSeconds, hasOwnProperty, join,
    lastIndex, length, parse, prototype, push, replace, slice, stringify,
    test, toJSON, toString, valueOf
*/


// Create a JSON object only if one does not already exist. We create the
// methods in a closure to avoid creating global variables.

var JSON;
if (!JSON) {
    JSON = {};
}

(function () {
    "use strict";

    function f(n) {
        // Format integers to have at least two digits.
        return n < 10 ? '0' + n : n;
    }

    if (typeof Date.prototype.toJSON !== 'function') {

        Date.prototype.toJSON = function (key) {

            return isFinite(this.valueOf()) ?
                this.getUTCFullYear()     + '-' +
                f(this.getUTCMonth() + 1) + '-' +
                f(this.getUTCDate())      + 'T' +
                f(this.getUTCHours())     + ':' +
                f(this.getUTCMinutes())   + ':' +
                f(this.getUTCSeconds())   + 'Z' : null;
        };

        String.prototype.toJSON      =
            Number.prototype.toJSON  =
            Boolean.prototype.toJSON = function (key) {
                return this.valueOf();
            };
    }

    var cx = /[\u0000\u00ad\u0600-\u0604\u070f\u17b4\u17b5\u200c-\u200f\u2028-\u202f\u2060-\u206f\ufeff\ufff0-\uffff]/g,
        escapable = /[\\\"\x00-\x1f\x7f-\x9f\u00ad\u0600-\u0604\u070f\u17b4\u17b5\u200c-\u200f\u2028-\u202f\u2060-\u206f\ufeff\ufff0-\uffff]/g,
        gap,
        indent,
        meta = {    // table of character substitutions
            '\b': '\\b',
            '\t': '\\t',
            '\n': '\\n',
            '\f': '\\f',
            '\r': '\\r',
            '"' : '\\"',
            '\\': '\\\\'
        },
        rep;


    function quote(string) {

// If the string contains no control characters, no quote characters, and no
// backslash characters, then we can safely slap some quotes around it.
// Otherwise we must also replace the offending characters with safe escape
// sequences.

        escapable.lastIndex = 0;
        return escapable.test(string) ? '"' + string.replace(escapable, function (a) {
            var c = meta[a];
            return typeof c === 'string' ? c :
                '\\u' + ('0000' + a.charCodeAt(0).toString(16)).slice(-4);
        }) + '"' : '"' + string + '"';
    }


    function str(key, holder) {

// Produce a string from holder[key].

        var i,          // The loop counter.
            k,          // The member key.
            v,          // The member value.
            length,
            mind = gap,
            partial,
            value = holder[key];

// If the value has a toJSON method, call it to obtain a replacement value.

        if (value && typeof value === 'object' &&
                typeof value.toJSON === 'function') {
            value = value.toJSON(key);
        }

// If we were called with a replacer function, then call the replacer to
// obtain a replacement value.

        if (typeof rep === 'function') {
            value = rep.call(holder, key, value);
        }

// What happens next depends on the value's type.

        switch (typeof value) {
        case 'string':
            return quote(value);

        case 'number':

// JSON numbers must be finite. Encode non-finite numbers as null.

            return isFinite(value) ? String(value) : 'null';

        case 'boolean':
        case 'null':

// If the value is a boolean or null, convert it to a string. Note:
// typeof null does not produce 'null'. The case is included here in
// the remote chance that this gets fixed someday.

            return String(value);

// If the type is 'object', we might be dealing with an object or an array or
// null.

        case 'object':

// Due to a specification blunder in ECMAScript, typeof null is 'object',
// so watch out for that case.

            if (!value) {
                return 'null';
            }

// Make an array to hold the partial results of stringifying this object value.

            gap += indent;
            partial = [];

// Is the value an array?

            if (Object.prototype.toString.apply(value) === '[object Array]') {

// The value is an array. Stringify every element. Use null as a placeholder
// for non-JSON values.

                length = value.length;
                for (i = 0; i < length; i += 1) {
                    partial[i] = str(i, value) || 'null';
                }

// Join all of the elements together, separated with commas, and wrap them in
// brackets.

                v = partial.length === 0 ? '[]' : gap ?
                    '[\n' + gap + partial.join(',\n' + gap) + '\n' + mind + ']' :
                    '[' + partial.join(',') + ']';
                gap = mind;
                return v;
            }

// If the replacer is an array, use it to select the members to be stringified.

            if (rep && typeof rep === 'object') {
                length = rep.length;
                for (i = 0; i < length; i += 1) {
                    if (typeof rep[i] === 'string') {
                        k = rep[i];
                        v = str(k, value);
                        if (v) {
                            partial.push(quote(k) + (gap ? ': ' : ':') + v);
                        }
                    }
                }
            } else {

// Otherwise, iterate through all of the keys in the object.

                for (k in value) {
                    if (Object.prototype.hasOwnProperty.call(value, k)) {
                        v = str(k, value);
                        if (v) {
                            partial.push(quote(k) + (gap ? ': ' : ':') + v);
                        }
                    }
                }
            }

// Join all of the member texts together, separated with commas,
// and wrap them in braces.

            v = partial.length === 0 ? '{}' : gap ?
                '{\n' + gap + partial.join(',\n' + gap) + '\n' + mind + '}' :
                '{' + partial.join(',') + '}';
            gap = mind;
            return v;
        }
    }

// If the JSON object does not yet have a stringify method, give it one.

    if (typeof JSON.stringify !== 'function') {
        JSON.stringify = function (value, replacer, space) {

// The stringify method takes a value and an optional replacer, and an optional
// space parameter, and returns a JSON text. The replacer can be a function
// that can replace values, or an array of strings that will select the keys.
// A default replacer method can be provided. Use of the space parameter can
// produce text that is more easily readable.

            var i;
            gap = '';
            indent = '';

// If the space parameter is a number, make an indent string containing that
// many spaces.

            if (typeof space === 'number') {
                for (i = 0; i < space; i += 1) {
                    indent += ' ';
                }

// If the space parameter is a string, it will be used as the indent string.

            } else if (typeof space === 'string') {
                indent = space;
            }

// If there is a replacer, it must be a function or an array.
// Otherwise, throw an error.

            rep = replacer;
            if (replacer && typeof replacer !== 'function' &&
                    (typeof replacer !== 'object' ||
                    typeof replacer.length !== 'number')) {
                throw new Error('JSON.stringify');
            }

// Make a fake root object containing our value under the key of ''.
// Return the result of stringifying the value.

            return str('', {'': value});
        };
    }


// If the JSON object does not yet have a parse method, give it one.

    if (typeof JSON.parse !== 'function') {
        JSON.parse = function (text, reviver) {

// The parse method takes a text and an optional reviver function, and returns
// a JavaScript value if the text is a valid JSON text.

            var j;

            function walk(holder, key) {

// The walk method is used to recursively walk the resulting structure so
// that modifications can be made.

                var k, v, value = holder[key];
                if (value && typeof value === 'object') {
                    for (k in value) {
                        if (Object.prototype.hasOwnProperty.call(value, k)) {
                            v = walk(value, k);
                            if (v !== undefined) {
                                value[k] = v;
                            } else {
                                delete value[k];
                            }
                        }
                    }
                }
                return reviver.call(holder, key, value);
            }


// Parsing happens in four stages. In the first stage, we replace certain
// Unicode characters with escape sequences. JavaScript handles many characters
// incorrectly, either silently deleting them, or treating them as line endings.

            text = String(text);
            cx.lastIndex = 0;
            if (cx.test(text)) {
                text = text.replace(cx, function (a) {
                    return '\\u' +
                        ('0000' + a.charCodeAt(0).toString(16)).slice(-4);
                });
            }

// In the second stage, we run the text against regular expressions that look
// for non-JSON patterns. We are especially concerned with '()' and 'new'
// because they can cause invocation, and '=' because it can cause mutation.
// But just to be safe, we want to reject all unexpected forms.

// We split the second stage into 4 regexp operations in order to work around
// crippling inefficiencies in IE's and Safari's regexp engines. First we
// replace the JSON backslash pairs with '@' (a non-JSON character). Second, we
// replace all simple value tokens with ']' characters. Third, we delete all
// open brackets that follow a colon or comma or that begin the text. Finally,
// we look to see that the remaining characters are only whitespace or ']' or
// ',' or ':' or '{' or '}'. If that is so, then the text is safe for eval.

            if (/^[\],:{}\s]*$/
                    .test(text.replace(/\\(?:["\\\/bfnrt]|u[0-9a-fA-F]{4})/g, '@')
                        .replace(/"[^"\\\n\r]*"|true|false|null|-?\d+(?:\.\d*)?(?:[eE][+\-]?\d+)?/g, ']')
                        .replace(/(?:^|:|,)(?:\s*\[)+/g, ''))) {

// In the third stage we use the eval function to compile the text into a
// JavaScript structure. The '{' operator is subject to a syntactic ambiguity
// in JavaScript: it can begin a block or an object literal. We wrap the text
// in parens to eliminate the ambiguity.

                j = eval('(' + text + ')');

// In the optional fourth stage, we recursively walk the new structure, passing
// each name/value pair to a reviver function for possible transformation.

                return typeof reviver === 'function' ?
                    walk({'': j}, '') : j;
            }

// If the text is not JSON parseable, then a SyntaxError is thrown.

            throw new SyntaxError('JSON.parse');
        };
    }
}());



var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var EXPAND_X = { x : 1.0, y : 0.0 };
var EXPAND_Y = { x : 0.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };
var FILL_X = { x : -1.0, y : 0.0 };
var FILL_Y = { x : 0.0, y : -1.0 };

var oauth;
var new_item = new Array();

function image_ready_cb() {
	print("Image ready= " + arguments[0]);
	print("Image path= " + arguments[1]);

	for (i=0; i<15; i++)
	{
		if ( arguments[0] == array[i].user.profile_image_url )
		{
			new_item[i].icon.image = arguments[1];
			print("index = " + i + " image=" + arguments[1]);

            var newicon = {
                            type : "icon",
                            image : arguments[1],
            };
			my_window.elements.tweets.elements.the_list.set_icon(i, newicon);
			print("match found= " + array[i].user.profile_image_url);
		}
	}
	
}

function success(data){
	print("Login Success");
        my_window.elements.login.visible = false;
	my_window.elements.tweets.visible = true;
	array = JSON.parse(data.text);

	for (i=0; i<15; i++)
	{
		print("-------------------------");
		print(array[i].user.name);
		print(array[i].text);
		print(array[i].user.profile_image_url);
		print("-------------------------");


		new_item[i] = {
				label : array[i].text,
				icon : {
					type : "icon",
    					image :  "NA",
    					tooltip :  "Tooltip",
				},
				tooltip : array[i].user.name,
				on_clicked : function(arg) {
					print("Twitter element clicked\n");
				},
			};
		my_window.elements.tweets.elements.the_list.append(new_item[i]);

        //if(i%2)
         //   my_window.elements.tweets.elements.the_list.disabled(i, true);

		var req = new XMLHttpRequest();
		req.onreadystatechange = image_ready_cb;
		req.open("GET", array[i].user.profile_image_url);
		req.send("");
	}
}

function failure(data){
	print("Login failure");
}

var my_window = new elm.window({
		type : "main",
		label : "Twitter Demo",
		width : 400,
		height : 580,
		weight : EXPAND_BOTH,
		align : FILL_BOTH,
		elements : {
			the_background : {
				type : "background",
				weight : EXPAND_BOTH,
				align : FILL_BOTH,
                file : elm.datadir + "data/images/twitter.png",
    			resize : true,
			},
			login : {
				type : "box",
				weight : EXPAND_BOTH,
				align : FILL_BOTH,
    				resize : true,
				elements : {
					the_photo : {
						type : "photo",
						size : 200,
						image : elm.datadir + "data/images/twitter.png",
						weight : EXPAND_BOTH,
						align : FILL_BOTH,
					},
					the_user : {
						type : "entry",
						text : "Enter Username Here",
						weight : EXPAND_BOTH,
						align : FILL_BOTH,
						scrollable : true,
				        },
					the_pass : {
						type : "entry",
						text : "Enter Password Here",
						weight : EXPAND_X,
						align : FILL_BOTH,
						scrollable : true,
						password : true,
				        },
					the_button : {
						type : "button",
						label : "Sign In Here",
						weight : EXPAND_X,
						align : FILL_BOTH,
						on_clicked : function(arg) {
							var oauth = OAuth({
							consumerKey : "0KnH68gpgyHoszB2IXIew",
							consumerSecret : "6qVXP8LFuHnDR4bpN6NwnMBI8sMkvPkVtDAs3RVTCPk",
							accessTokenKey :  "101332537-foetvzzAsjm6RidEyRNWkt1A5SlvqQD9k5X4aM0U",
							accessTokenSecret : "3KnUleEkIaPHyYTm7fLlINKvOvfrIdyuQlc6UCAok"
							});
							oauth.get("http://api.twitter.com/1/statuses/home_timeline.json", success, failure);
						},
					},
				},
			},
			tweets : {
				type : "box",
				weight : EXPAND_BOTH,
				align : FILL_BOTH,
				visible : false,
				homogeneous : false,
				resize : true,
				elements : {
					the_list : {
						type : "list",
						weight : EXPAND_BOTH,
						align : FILL_BOTH,
                        mode : 0,
						items : {
						},
					},
					controls : {
						type : "box",
						weight : EXPAND_X,
						align : FILL_BOTH,
						horizontal : true,
						elements : {
							prev : {
								type : "button",
								weight : EXPAND_X,
								align : FILL_X,
								label : "Prev 20",
							},
							logout : {
								type : "button",
								weight : EXPAND_X,
								align : FILL_X,
								label : "Logout",
							},
							newtweet : {
								type : "button",
								weight : EXPAND_X,
								align : FILL_X,
								label : "New Tweet",
							},
							next : {
								type : "button",
								weight : EXPAND_X,
								align : FILL_X,
								label : "Next 20",
							},
						},
					},
				},
			},
		},
});
