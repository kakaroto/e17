// Spot cases in which the wrong size is allocated by E_NEW, calloc or malloc.
// For example, it's wrong to do:
//    MyType *a = calloc(1, sizeof(a))
// The right statement would be:
//    MyType *a = calloc(1, sizeof(*a))

@r@
type T;
T *x;
position p;
@@
x@p = <+...sizeof(T)...+>

@rconst1@
type T;
const T *x;
position p;
@@
x@p = <+...sizeof(T)...+>

@rconst2@
type T;
T *x;
position p;
@@
x@p = <+...sizeof(const T)...+>

@rconst3@
type T;
T *x;
const T *y;
position p;
@@
x@p = <+...sizeof(*y)...+>

@runsigned1@
type T;
unsigned T *x;
position p;
@@
x@p = <+...sizeof(T)...+>

@runsigned2@
type T;
T *x;
position p;
@@
x@p = <+...sizeof(unsigned T)...+>

@runsigned3@
type T;
T *x;
unsigned T *y;
position p;
@@
x@p = <+...sizeof(*y)...+>

@s@
expression x;
position p;
@@
x@p = <+...sizeof(*x)...+>


@bad@
position p!={r.p,rconst1.p, rconst2.p, rconst3.p, runsigned1.p, runsigned2.p,runsigned3.p,s.p};
type T, T1, T2;
T1 *x;
T2 **y;
typedef u8;
{void *, char *, unsigned char *, u8*} a;
@@

(
y = E_NEW(<+...sizeof(T)...+>,...)
|
y = calloc(...,<+...sizeof(T)...+>)
|
y = malloc(<+...sizeof(T)...+>)
|
a = E_NEW(<+...sizeof(T)...+>,...)
|
a = calloc(...,<+...sizeof(T)...+>)
|
a = malloc(<+...sizeof(T)...+>)
|
x@p = E_NEW(<+...sizeof(T)...+>,...)
|
x@p = calloc(...,<+...sizeof(T)...+>)
|
x@p = malloc(<+...sizeof(T)...+>)
)

@script:python@
p << bad.p;
@@

print "%s:%s" % (p[0].file, p[0].line)
