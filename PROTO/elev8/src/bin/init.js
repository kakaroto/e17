/*
 * Elev8 initialization file. This is read every time elev8 boots up. Things
 * here will be available in the global context, unlike modules, which will
 * be available in their own context.
 */

/*
 * Elementary support should be enabled by default.
 */
elm = require('elm');

/*
 * Allows to test if an object is a function:
 *    x = function() { return 42; }
 *    print(x.callable() ? "x is a function!" : "wtf?");
 */
Object.prototype.callable = function() {
  return typeof(this) === 'function';
}

/*
 * Allows checking if an object is inside an array:
 *   arr = [101,202,303,404,505];
 *   print(arr.contains(404) ? "arr contains 404!" : "404 not found");
 */
Array.prototype.contains = function(obj) {
  for (var i = this.length; i--;)
    if (this[i] == obj)
      return true;
  return false;
}
