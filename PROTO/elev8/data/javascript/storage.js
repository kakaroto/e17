print("Setting... firt = 1")
localStorage.setItem("fist", 1);
print("Getting... fist = ", localStorage.getItem("fist"));
print("Lenght: ", localStorage.length);

print("Setting... second = 2")
localStorage.setItem("second", 2);
print("Getting... second = ", localStorage.getItem("second"));
print("Lenght: ", localStorage.length);

print("Setting... third = 3")
localStorage.setItem("third", 3);
print("Getting... third = ", localStorage.getItem("third"));
print("Lenght: ", localStorage.length);

print("Remove... second")
localStorage.removeItem("second");

print("Printing...");
for (var i = 0; i < localStorage.length; i++) {
   var key = localStorage.key(i);
   print(key, ' = ', localStorage.getItem(key));
}

print("Lenght: ", localStorage.length);

print("== Cleanig ==");
localStorage.clear();
print("Lenght: ", localStorage.length);

elm.exit();
