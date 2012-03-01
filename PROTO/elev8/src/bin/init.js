/*
 * Elev8 initialization file. This is read every time elev8 boots up. Things
 * here will be available in the global context, unlike modules, which will
 * be available in their own context.
 */

/*
 * Elementary support should be enabled by default.
 */
elm = require('elm');
