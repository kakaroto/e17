/* Console input/output functions
 *
 * T. Riemersma, ITB CompuPhase, 1998
 */
#if defined _console_included
  #endinput
#endif
#define _console_included

enum
    {
    black,      /* colours as per the ANSI Escape sequences, ISO 6429 */
    red,
    green,
    yellow,
    blue,
    magenta,
    cyan,
    white,
    }

native getchar(echo=true);
native getstring(string[], maxlength, pack=false);
native getvalue(base=10, end='^r', ...);
native print(string[],foreground=-1,background=-1);
native printf(format[], ...);
