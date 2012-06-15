print("Setting... first = 1")
localStorage.setItem("first", 1);
print("Getting... first = ", localStorage.getItem("first"));
print("Length: ", localStorage.length);

print("Setting... second = 2")
localStorage.setItem("second", 2);
print("Getting... second = ", localStorage.getItem("second"));
print("Length: ", localStorage.length);

print("Setting... third = 3")
localStorage.setItem("third", 3);
print("Getting... third = ", localStorage.getItem("third"));
print("Length: ", localStorage.length);

print("Remove... second")
localStorage.removeItem("second");

print("Printing...");
for (var i = 0; i < localStorage.length; i++) {
   var key = localStorage.key(i);
   print(key, ' = ', localStorage.getItem(key));
}

print("Length: ", localStorage.length);

print("== Cleaning ==");
localStorage.clear();
print("Length: ", localStorage.length);

elm.exit();
