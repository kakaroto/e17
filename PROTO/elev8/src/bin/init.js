/*
 * Elev8 initialization file. This is read every time elev8 boots up. Things
 * here will be available in the global context, unlike modules, which will
 * be evaluated in their own context.
 */

/*
 * Elementary support should be enabled by default.
 */
elm = require('elm');
localStorage = new LocalStorage();
