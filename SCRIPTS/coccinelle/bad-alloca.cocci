@@
identifier x;
identifier fn;
@@

x = alloca(...)
...
(
  return fn(<+...x...+>);
|
* return <+...x...+>;
)
