es = setInterval(function() {
   print("Every Second");
}, 1000);

setInterval(function() {
   print("Every 2 Seconds");
}, 2000);

setTimeout(function() {
   print("Stop 'Every Second' timer");
   clearInterval(es);
}, 5000);

setTimeout(function() {
   print("Say Goodbye!");
}, 9000);

setTimeout(function() {
   print("Goodbye!");
   elm.exit();
}, 11000);

