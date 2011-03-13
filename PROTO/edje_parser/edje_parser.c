/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is include which follows the "include" declaration
** in the input file. */
#include <stdio.h>
#line 27 "edje_parser.y"

#include "edje_parser_lib.h"
#include "edje.h"
#include "edje_parser.h"
#include "edje_parser_math.h"
#include "edje_parser_macros.h"
#include <ctype.h>
#include <errno.h>

#define YYCTYPE  char
#define YYCURSOR c
#define YYMARKER q
#define YYCTXMARKER t

#define EDJE_EOF     0
#define EDJE_UNKNOWN (-1)

#define RET(tok) return edje_parser_token_new(s, tok, c - b)

#define NEXT \
       edje_parser_stream_next(s, c - b); \
       b = c

extern void *edje_parser_Alloc(void *(*)(size_t));
extern void  edje_parser_Free(void *, void (*)(void *));
extern void  edje_parser_(void *,
                         int,
                         Edje_Parser_Token *,
                         Edje_Parser *);
/* for reference
extern void        edje_parser_Trace(FILE *,
                                    char *);
*/
   static Edje_Parser_Token *
   edje_parser_stream_to_token(Edje_Stream *s)
   {
      const char *c, *b, *q;
      int brace_open = 0, brace_close = 0;

      if (s->length == s->index)
        return edje_parser_token_new(s, EDJE_EOF, 0);

      c = b = s->buffer + s->index;

main:

  {
   YYCTYPE yych;
   unsigned int yyaccept = 0;
   static const unsigned char yybm[] = {
    /* table 1 .. 8: 0 */
    128, 192, 192, 192, 192, 192, 192, 192, 
    192,  88,  16, 192, 192,  80, 192, 192, 
    192, 192, 192, 192, 192, 192, 192, 192, 
    192, 192, 192, 192, 192, 192, 192, 192, 
     89, 193, 192, 193, 193, 193, 193, 193, 
    193, 193, 225, 193, 193, 193, 193, 193, 
    197, 197, 197, 197, 197, 197, 197, 197, 
    197, 197, 193, 195, 193, 193, 193, 193, 
    193, 193, 193, 193, 193, 193, 193, 193, 
    193, 193, 193, 193, 193, 193, 193, 193, 
    193, 193, 193, 193, 193, 193, 193, 193, 
    193, 193, 193, 193, 193, 193, 193, 197, 
    193, 197, 197, 197, 197, 197, 197, 197, 
    197, 197, 197, 197, 197, 197, 197, 197, 
    197, 197, 197, 197, 197, 197, 197, 197, 
    197, 197, 197, 193, 193, 193, 193, 192, 
    192, 192, 192, 192, 192, 192, 192, 192, 
    192, 192, 192, 192, 192, 192, 192, 192, 
    192, 192, 192, 192, 192, 192, 192, 192, 
    192, 192, 192, 192, 192, 192, 192, 192, 
    192, 192, 192, 192, 192, 192, 192, 192, 
    192, 192, 192, 192, 192, 192, 192, 192, 
    192, 192, 192, 192, 192, 192, 192, 192, 
    192, 192, 192, 192, 192, 192, 192, 192, 
    192, 192, 192, 192, 192, 192, 192, 192, 
    192, 192, 192, 192, 192, 192, 192, 192, 
    192, 192, 192, 192, 192, 192, 192, 192, 
    192, 192, 192, 192, 192, 192, 192, 192, 
    192, 192, 192, 192, 192, 192, 192, 192, 
    192, 192, 192, 192, 192, 192, 192, 192, 
    192, 192, 192, 192, 192, 192, 192, 192, 
    192, 192, 192, 192, 192, 192, 192, 192, 
    /* table 9 .. 10: 256 */
      0,   0,   0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0,   0,   0, 
      0,  64,   0,   0,  64,  64,  64,  64, 
     64,  64,  64,  64,  64,  64,   0,  64, 
    192, 192, 192, 192, 192, 192, 192, 192, 
    192, 192,   0,   0,  64,  64,  64,  64, 
     64,  64,  64,  64,  64,  64,  64,  64, 
     64,  64,  64,  64,  64,  64,  64,  64, 
     64,  64,  64,  64,  64,  64,  64,  64, 
     64,  64,  64,  64,  64,  64,  64,  64, 
     64,  64,  64,  64,  64,  64,  64,  64, 
     64,  64,  64,  64,  64,  64,  64,  64, 
     64,  64,  64,  64,  64,  64,  64,  64, 
     64,  64,  64,   0,  64,   0,  64,   0, 
      0,   0,   0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0,   0,   0, 
   };

   yych = *YYCURSOR;
   if (yych <= 'b') {
    if (yych <= '#') {
     if (yych <= '\r') {
      if (yych <= 0x08) {
       if (yych <= 0x00) goto yy32;
       goto yy34;
      } else {
       if (yych <= '\n') goto yy4;
       if (yych <= '\f') goto yy34;
       goto yy4;
      }
     } else {
      if (yych <= ' ') {
       if (yych <= 0x1F) goto yy34;
       goto yy4;
      } else {
       if (yych <= '!') goto yy31;
       if (yych <= '"') goto yy29;
       goto yy34;
      }
     }
    } else {
     if (yych <= ':') {
      if (yych <= '.') {
       if (yych <= '-') goto yy31;
       goto yy27;
      } else {
       if (yych <= '/') goto yy2;
       if (yych <= '9') goto yy31;
       goto yy34;
      }
     } else {
      if (yych <= 'L') {
       if (yych <= ';') goto yy25;
       if (yych <= 'K') goto yy31;
       goto yy20;
      } else {
       if (yych <= '`') goto yy31;
       if (yych <= 'a') goto yy6;
       goto yy7;
      }
     }
    }
   } else {
    if (yych <= 'o') {
     if (yych <= 'g') {
      if (yych <= 'd') {
       if (yych <= 'c') goto yy8;
       goto yy9;
      } else {
       if (yych <= 'e') goto yy10;
       if (yych <= 'f') goto yy11;
       goto yy12;
      }
     } else {
      if (yych <= 'l') {
       if (yych == 'i') goto yy13;
       goto yy6;
      } else {
       if (yych <= 'm') goto yy14;
       if (yych <= 'n') goto yy6;
       goto yy15;
      }
     }
    } else {
     if (yych <= 't') {
      if (yych <= 'q') {
       if (yych <= 'p') goto yy16;
       goto yy6;
      } else {
       if (yych <= 'r') goto yy17;
       if (yych <= 's') goto yy18;
       goto yy19;
      }
     } else {
      if (yych <= '|') {
       if (yych <= 'z') goto yy6;
       if (yych <= '{') goto yy21;
       goto yy31;
      } else {
       if (yych <= '}') goto yy23;
       if (yych <= '~') goto yy31;
       goto yy34;
      }
     }
    }
   }
yy2:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 32) {
    goto yy223;
   }
   if (yych == '/') goto yy226;
   goto yy36;
yy3:
   { RET(EDJE_ID); }
yy4:
   ++YYCURSOR;
   yych = *YYCURSOR;
   goto yy222;
yy5:
   { NEXT; goto main; }
yy6:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych <= '#') goto yy3;
     goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
      goto yy3;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy58;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy7:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych == 'o') goto yy218;
      goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy8:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych == 'o') goto yy195;
      goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy9:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '_') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= ':') {
      if (yych <= '/') goto yy35;
      if (yych <= '9') goto yy58;
      goto yy63;
     } else {
      if (yych <= ';') goto yy3;
      if (yych <= '^') goto yy35;
      goto yy58;
     }
    }
   } else {
    if (yych <= 'r') {
     if (yych <= 'd') {
      if (yych <= '`') goto yy35;
      if (yych <= 'a') goto yy172;
      goto yy58;
     } else {
      if (yych <= 'e') goto yy173;
      if (yych <= 'q') goto yy58;
      goto yy174;
     }
    } else {
     if (yych <= '|') {
      if (yych <= 'z') goto yy58;
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy10:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych == 'x') goto yy163;
      goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy11:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '_') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= ':') {
      if (yych <= '/') goto yy35;
      if (yych <= '9') goto yy58;
      goto yy63;
     } else {
      if (yych <= ';') goto yy3;
      if (yych <= '^') goto yy35;
      goto yy58;
     }
    }
   } else {
    if (yych <= 'o') {
     if (yych <= 'h') {
      if (yych <= '`') goto yy35;
      goto yy58;
     } else {
      if (yych <= 'i') goto yy153;
      if (yych <= 'n') goto yy58;
      goto yy154;
     }
    } else {
     if (yych <= '|') {
      if (yych <= 'z') goto yy58;
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy12:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych == 'r') goto yy148;
      goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy13:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych == 'm') goto yy141;
      goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy14:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych <= '#') goto yy3;
     goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
      goto yy3;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '_') {
      if (yych <= '^') goto yy35;
      goto yy58;
     } else {
      if (yych <= '`') goto yy35;
      if (yych <= 'a') goto yy138;
      goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy15:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych == 'r') goto yy132;
      goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy16:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '_') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= ':') {
      if (yych <= '/') goto yy35;
      if (yych <= '9') goto yy58;
      goto yy63;
     } else {
      if (yych <= ';') goto yy3;
      if (yych <= '^') goto yy35;
      goto yy58;
     }
    }
   } else {
    if (yych <= 'r') {
     if (yych <= 'd') {
      if (yych <= '`') goto yy35;
      if (yych <= 'a') goto yy102;
      goto yy58;
     } else {
      if (yych <= 'e') goto yy103;
      if (yych <= 'q') goto yy58;
      goto yy104;
     }
    } else {
     if (yych <= '|') {
      if (yych <= 'z') goto yy58;
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy17:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '_') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= ':') {
      if (yych <= '/') goto yy35;
      if (yych <= '9') goto yy58;
      goto yy63;
     } else {
      if (yych <= ';') goto yy3;
      if (yych <= '^') goto yy35;
      goto yy58;
     }
    }
   } else {
    if (yych <= 'o') {
     if (yych <= 'd') {
      if (yych <= '`') goto yy35;
      goto yy58;
     } else {
      if (yych <= 'e') goto yy88;
      if (yych <= 'n') goto yy58;
      goto yy89;
     }
    } else {
     if (yych <= '|') {
      if (yych <= 'z') goto yy58;
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy18:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= 'b') {
    if (yych <= '/') {
     if (yych <= '#') {
      if (yych == '!') goto yy35;
      goto yy3;
     } else {
      if (yych == '.') goto yy37;
      goto yy35;
     }
    } else {
     if (yych <= ';') {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
      goto yy3;
     } else {
      if (yych == '_') goto yy58;
      if (yych <= '`') goto yy35;
      goto yy58;
     }
    }
   } else {
    if (yych <= 's') {
     if (yych <= 'e') {
      if (yych <= 'c') goto yy68;
      if (yych <= 'd') goto yy58;
      goto yy69;
     } else {
      if (yych == 'i') goto yy70;
      goto yy58;
     }
    } else {
     if (yych <= '{') {
      if (yych <= 't') goto yy71;
      if (yych <= 'z') goto yy58;
      goto yy3;
     } else {
      if (yych == '}') goto yy3;
      if (yych <= '~') goto yy35;
      goto yy3;
     }
    }
   }
yy19:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych == 'e') goto yy60;
      goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy20:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yych == 'O') goto yy53;
   goto yy36;
yy21:
   ++YYCURSOR;
   { RET(EDJE_LB); }
yy23:
   ++YYCURSOR;
   if ((yych = *YYCURSOR) == ';') goto yy52;
yy24:
   { RET(EDJE_RB); }
yy25:
   ++YYCURSOR;
   yych = *YYCURSOR;
   goto yy51;
yy26:
   { RET(EDJE_SEMICOLON); }
yy27:
   ++YYCURSOR;
   if (yybm[256+(yych = *YYCURSOR)] & 128) {
    goto yy39;
   }
   { RET(EDJE_DOT); }
yy29:
   yyaccept = 1;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yych <= 0x1F) goto yy30;
   if (yych <= '~') goto yy44;
yy30:
   { RET(EDJE_UNKNOWN); }
yy31:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   goto yy36;
yy32:
   ++YYCURSOR;
   { RET(EDJE_EOF); }
yy34:
   yych = *++YYCURSOR;
   goto yy30;
yy35:
   yyaccept = 0;
   YYMARKER = ++YYCURSOR;
   yych = *YYCURSOR;
yy36:
   if (yybm[256+yych] & 64) {
    goto yy35;
   }
   if (yych != '.') goto yy3;
yy37:
   ++YYCURSOR;
   yych = *YYCURSOR;
   if (yybm[256+yych] & 128) {
    goto yy39;
   }
yy38:
   YYCURSOR = YYMARKER;
   if (yyaccept <= 16) {
    if (yyaccept <= 8) {
     if (yyaccept <= 4) {
      if (yyaccept <= 2) {
       if (yyaccept <= 1) {
        if (yyaccept <= 0) {
         goto yy3;
        } else {
         goto yy30;
        }
       } else {
        goto yy49;
       }
      } else {
       if (yyaccept <= 3) {
        goto yy57;
       } else {
        goto yy67;
       }
      }
     } else {
      if (yyaccept <= 6) {
       if (yyaccept <= 5) {
        goto yy75;
       } else {
        goto yy77;
       }
      } else {
       if (yyaccept <= 7) {
        goto yy80;
       } else {
        goto yy82;
       }
      }
     }
    } else {
     if (yyaccept <= 12) {
      if (yyaccept <= 10) {
       if (yyaccept <= 9) {
        goto yy87;
       } else {
        goto yy96;
       }
      } else {
       if (yyaccept <= 11) {
        goto yy99;
       } else {
        goto yy101;
       }
      }
     } else {
      if (yyaccept <= 14) {
       if (yyaccept <= 13) {
        goto yy110;
       } else {
        goto yy112;
       }
      } else {
       if (yyaccept <= 15) {
        goto yy122;
       } else {
        goto yy126;
       }
      }
     }
    }
   } else {
    if (yyaccept <= 25) {
     if (yyaccept <= 21) {
      if (yyaccept <= 19) {
       if (yyaccept <= 18) {
        if (yyaccept <= 17) {
         goto yy128;
        } else {
         goto yy131;
        }
       } else {
        goto yy137;
       }
      } else {
       if (yyaccept <= 20) {
        goto yy140;
       } else {
        goto yy145;
       }
      }
     } else {
      if (yyaccept <= 23) {
       if (yyaccept <= 22) {
        goto yy147;
       } else {
        goto yy152;
       }
      } else {
       if (yyaccept <= 24) {
        goto yy159;
       } else {
        goto yy162;
       }
      }
     }
    } else {
     if (yyaccept <= 29) {
      if (yyaccept <= 27) {
       if (yyaccept <= 26) {
        goto yy171;
       } else {
        goto yy181;
       }
      } else {
       if (yyaccept <= 28) {
        goto yy191;
       } else {
        goto yy194;
       }
      }
     } else {
      if (yyaccept <= 31) {
       if (yyaccept <= 30) {
        goto yy206;
       } else {
        goto yy209;
       }
      } else {
       if (yyaccept <= 32) {
        goto yy217;
       } else {
        goto yy220;
       }
      }
     }
    }
   }
yy39:
   yyaccept = 0;
   YYMARKER = ++YYCURSOR;
   yych = *YYCURSOR;
   if (yybm[256+yych] & 128) {
    goto yy39;
   }
   if (yych <= '/') {
    if (yych <= '#') {
     if (yych != '!') goto yy3;
    } else {
     if (yych == '.') goto yy37;
    }
   } else {
    if (yych <= '{') {
     if (yych <= ';') goto yy3;
     if (yych >= '{') goto yy3;
    } else {
     if (yych == '}') goto yy3;
     if (yych >= 0x7F) goto yy3;
    }
   }
yy41:
   yyaccept = 0;
   YYMARKER = ++YYCURSOR;
   yych = *YYCURSOR;
   if (yych <= '9') {
    if (yych <= '#') {
     if (yych == '!') goto yy41;
     goto yy3;
    } else {
     if (yych == '.') goto yy37;
     goto yy41;
    }
   } else {
    if (yych <= '{') {
     if (yych <= ';') goto yy3;
     if (yych <= 'z') goto yy41;
     goto yy3;
    } else {
     if (yych == '}') goto yy3;
     if (yych <= '~') goto yy41;
     goto yy3;
    }
   }
yy43:
   ++YYCURSOR;
   yych = *YYCURSOR;
yy44:
   if (yybm[0+yych] & 1) {
    goto yy43;
   }
   if (yych != '"') goto yy38;
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yych != '"') goto yy3;
yy46:
   ++YYCURSOR;
   yych = *YYCURSOR;
   if (yych <= 0x1F) goto yy38;
   if (yych == '"') goto yy48;
   if (yych <= '~') goto yy46;
   goto yy38;
yy48:
   yyaccept = 2;
   YYMARKER = ++YYCURSOR;
   yych = *YYCURSOR;
   if (yych == '"') goto yy46;
yy49:
   { RET(EDJE_DOUBLEQUOTES); }
yy50:
   ++YYCURSOR;
   yych = *YYCURSOR;
yy51:
   if (yybm[0+yych] & 2) {
    goto yy50;
   }
   goto yy26;
yy52:
   yych = *++YYCURSOR;
   goto yy24;
yy53:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yych != 'S') goto yy36;
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yych != 'S') goto yy36;
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yych != 'Y') goto yy36;
   yyaccept = 3;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yych <= ';') {
    if (yych <= '!') {
     if (yych >= '!') goto yy36;
    } else {
     if (yych <= '#') goto yy57;
     if (yych <= '9') goto yy36;
    }
   } else {
    if (yych <= '|') {
     if (yych != '{') goto yy36;
    } else {
     if (yych == '~') goto yy36;
    }
   }
yy57:
   { RET(EDJE_LOSSY); }
yy58:
   yyaccept = 0;
   YYMARKER = ++YYCURSOR;
   yych = *YYCURSOR;
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych <= '#') goto yy3;
     goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
      goto yy3;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy58;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy60:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych == 'x') goto yy65;
      goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy61:
   ++YYCURSOR;
   yych = *YYCURSOR;
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych != ':') goto yy38;
yy63:
   ++YYCURSOR;
yy64:
   { RET(EDJE_PROPERTY); }
yy65:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 't') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 4;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych >= '$') goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy58;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy67:
   { RET(EDJE_TEXT); }
yy68:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych == 'r') goto yy83;
      goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy69:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych == 't') goto yy81;
      goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy70:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych <= '#') goto yy3;
     goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
      goto yy3;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '_') {
      if (yych <= '^') goto yy35;
      goto yy58;
     } else {
      if (yych <= '`') goto yy35;
      if (yych <= 'y') goto yy58;
      goto yy78;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy71:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'y') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'l') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'e') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 5;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych >= '!') goto yy35;
     } else {
      if (yych <= '#') goto yy75;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych >= '<') goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych == 's') goto yy76;
      goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy75:
   { RET(EDJE_STYLE); }
yy76:
   yyaccept = 6;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych >= '$') goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy58;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy77:
   { RET(EDJE_STYLES); }
yy78:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'e') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 7;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych >= '$') goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy58;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy80:
   { RET(EDJE_SIZE); }
yy81:
   yyaccept = 8;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych >= '$') goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy58;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy82:
   { RET(EDJE_SET); }
yy83:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'i') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'p') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 't') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 9;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych >= '$') goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy58;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy87:
   {
   while (c++)
     {
        if (*c == '{') brace_open++;
        else if (*c == '}') brace_close++;
        if (brace_open && (*c == '}') && (brace_open == brace_close))
          {
             c++;
             RET(EDJE_EMBRYO);
          }
     }
   RET(EDJE_UNKNOWN);
}
yy88:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych == 'l') goto yy97;
      goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy89:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 't') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych <= '#') goto yy3;
     goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
      goto yy3;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '_') {
      if (yych <= '^') goto yy35;
      goto yy58;
     } else {
      if (yych <= '`') goto yy35;
      if (yych >= 'b') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 't') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'i') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'o') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'n') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 10;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych >= '$') goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy58;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy96:
   { RET(EDJE_ROTATION); }
yy97:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '9') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '0') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= '1') goto yy98;
      if (yych <= '2') goto yy100;
      goto yy58;
     }
    }
   } else {
    if (yych <= '`') {
     if (yych <= ';') {
      if (yych <= ':') goto yy63;
      goto yy3;
     } else {
      if (yych == '_') goto yy58;
      goto yy35;
     }
    } else {
     if (yych <= '|') {
      if (yych <= 'z') goto yy58;
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy98:
   yyaccept = 11;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych >= '$') goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy58;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy99:
   { RET(EDJE_REL1); }
yy100:
   yyaccept = 12;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych >= '$') goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy58;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy101:
   { RET(EDJE_REL2); }
yy102:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych == 'r') goto yy123;
      goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy103:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych == 'r') goto yy113;
      goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy104:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'o') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'g') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'r') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych <= '#') goto yy3;
     goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
      goto yy3;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '_') {
      if (yych <= '^') goto yy35;
      goto yy58;
     } else {
      if (yych <= '`') goto yy35;
      if (yych >= 'b') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'm') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 13;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych >= '!') goto yy35;
     } else {
      if (yych <= '#') goto yy110;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych >= '<') goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych == 's') goto yy111;
      goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy110:
   { RET(EDJE_PROGRAM); }
yy111:
   yyaccept = 14;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych >= '$') goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy58;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy112:
   { RET(EDJE_PROGRAMS); }
yy113:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 's') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'p') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'e') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'c') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 't') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'i') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'v') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'e') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 15;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych >= '$') goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy58;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy122:
   { RET(EDJE_PERSPECTIVE); }
yy123:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 't') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych <= 'a') goto yy124;
      if (yych <= 's') goto yy58;
      goto yy125;
     }
    } else {
     if (yych <= '|') {
      if (yych <= 'z') goto yy58;
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy124:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych == 'm') goto yy129;
      goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy125:
   yyaccept = 16;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych >= '!') goto yy35;
     } else {
      if (yych <= '#') goto yy126;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych >= '<') goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych == 's') goto yy127;
      goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy126:
   { RET(EDJE_PART); }
yy127:
   yyaccept = 17;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych >= '$') goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy58;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy128:
   { RET(EDJE_PARTS); }
yy129:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 's') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 18;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych >= '$') goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy58;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy131:
   { RET(EDJE_PARAMS); }
yy132:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'i') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'g') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'i') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'n') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 19;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych >= '$') goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy58;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy137:
   { RET(EDJE_ORIGIN); }
yy138:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'p') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 20;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych >= '$') goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy58;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy140:
   { RET(EDJE_MAP); }
yy141:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych <= '#') goto yy3;
     goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
      goto yy3;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '_') {
      if (yych <= '^') goto yy35;
      goto yy58;
     } else {
      if (yych <= '`') goto yy35;
      if (yych >= 'b') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'g') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'e') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 21;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych >= '!') goto yy35;
     } else {
      if (yych <= '#') goto yy145;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych >= '<') goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych == 's') goto yy146;
      goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy145:
   { RET(EDJE_IMAGE); }
yy146:
   yyaccept = 22;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych >= '$') goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy58;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy147:
   { RET(EDJE_IMAGES); }
yy148:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'o') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'u') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'p') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 23;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych >= '$') goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy58;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy152:
   { RET(EDJE_GROUP); }
yy153:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych == 'l') goto yy160;
      goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy154:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'n') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 't') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy157;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych == 's') goto yy158;
      goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy157:
   yych = *++YYCURSOR;
   goto yy64;
yy158:
   yyaccept = 24;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych >= '$') goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy58;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy159:
   { RET(EDJE_FONTS); }
yy160:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'l') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 25;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych >= '$') goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy58;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy162:
   { RET(EDJE_FILL); }
yy163:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 't') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'e') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'r') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'n') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych <= '#') goto yy3;
     goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
      goto yy3;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '_') {
      if (yych <= '^') goto yy35;
      goto yy58;
     } else {
      if (yych <= '`') goto yy35;
      if (yych >= 'b') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'l') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 's') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 26;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych >= '$') goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy58;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy171:
   { RET(EDJE_EXTERNALS); }
yy172:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych == 't') goto yy192;
      goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy173:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych == 's') goto yy182;
      goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy174:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych <= '#') goto yy3;
     goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
      goto yy3;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '_') {
      if (yych <= '^') goto yy35;
      goto yy58;
     } else {
      if (yych <= '`') goto yy35;
      if (yych >= 'b') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'g') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych <= '#') goto yy3;
     goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
      goto yy3;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '_') {
      if (yych <= '^') goto yy35;
      goto yy58;
     } else {
      if (yych <= '`') goto yy35;
      if (yych >= 'b') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'b') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'l') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'e') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 27;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych >= '$') goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy58;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy181:
   { RET(EDJE_DRAGABLE); }
yy182:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'c') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'r') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'i') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'p') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 't') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'i') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'o') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'n') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 28;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych >= '$') goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy58;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy191:
   { RET(EDJE_DESCRIPTION); }
yy192:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych <= '#') goto yy3;
     goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
      goto yy3;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '_') {
      if (yych <= '^') goto yy35;
      goto yy58;
     } else {
      if (yych <= '`') goto yy35;
      if (yych >= 'b') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 29;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych >= '$') goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy58;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy194:
   { RET(EDJE_DATA); }
yy195:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'l') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '_') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= ':') {
      if (yych <= '/') goto yy35;
      if (yych <= '9') goto yy58;
      goto yy63;
     } else {
      if (yych <= ';') goto yy3;
      if (yych <= '^') goto yy35;
      goto yy58;
     }
    }
   } else {
    if (yych <= 'o') {
     if (yych <= 'k') {
      if (yych <= '`') goto yy35;
      goto yy58;
     } else {
      if (yych <= 'l') goto yy197;
      if (yych <= 'n') goto yy58;
      goto yy198;
     }
    } else {
     if (yych <= '|') {
      if (yych <= 'z') goto yy58;
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy197:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych == 'e') goto yy210;
      goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy198:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'r') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych <= '#') goto yy3;
     goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
      goto yy3;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy200;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
yy200:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'c') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'l') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych <= '#') goto yy3;
     goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
      goto yy3;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '_') {
      if (yych <= '^') goto yy35;
      goto yy58;
     } else {
      if (yych <= '`') goto yy35;
      if (yych >= 'b') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 's') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 's') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 30;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych >= '!') goto yy35;
     } else {
      if (yych <= '#') goto yy206;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych >= '<') goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych == 'e') goto yy207;
      goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy206:
   { RET(EDJE_COLOR_CLASS); }
yy207:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 's') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 31;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych >= '$') goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy58;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy209:
   { RET(EDJE_COLOR_CLASSES); }
yy210:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'c') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 't') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'i') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'o') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'n') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 's') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 32;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych >= '$') goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy58;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy217:
   { RET(EDJE_COLLECTIONS); }
yy218:
   yyaccept = 0;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= '^') {
    if (yych <= '.') {
     if (yych <= '!') {
      if (yych <= ' ') goto yy3;
      goto yy35;
     } else {
      if (yych <= '#') goto yy3;
      if (yych <= '-') goto yy35;
      goto yy37;
     }
    } else {
     if (yych <= '9') {
      if (yych <= '/') goto yy35;
      goto yy58;
     } else {
      if (yych <= ':') goto yy63;
      if (yych <= ';') goto yy3;
      goto yy35;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '`') {
      if (yych <= '_') goto yy58;
      goto yy35;
     } else {
      if (yych != 'x') goto yy58;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy3;
      goto yy35;
     } else {
      if (yych == '~') goto yy35;
      goto yy3;
     }
    }
   }
   yyaccept = 33;
   yych = *(YYMARKER = ++YYCURSOR);
   if (yybm[0+yych] & 8) {
    goto yy61;
   }
   if (yych <= ';') {
    if (yych <= '-') {
     if (yych == '!') goto yy35;
     if (yych >= '$') goto yy35;
    } else {
     if (yych <= '/') {
      if (yych <= '.') goto yy37;
      goto yy35;
     } else {
      if (yych <= '9') goto yy58;
      if (yych <= ':') goto yy63;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych == '_') goto yy58;
     if (yych <= '`') goto yy35;
     goto yy58;
    } else {
     if (yych <= '|') {
      if (yych >= '|') goto yy35;
     } else {
      if (yych == '~') goto yy35;
     }
    }
   }
yy220:
   { RET(EDJE_BOX); }
yy221:
   ++YYCURSOR;
   yych = *YYCURSOR;
yy222:
   if (yybm[0+yych] & 16) {
    goto yy221;
   }
   goto yy5;
yy223:
   ++YYCURSOR;
   yych = *YYCURSOR;
   if (yybm[0+yych] & 32) {
    goto yy223;
   }
   if (yych <= '-') {
    if (yych <= '\r') {
     if (yych <= 0x08) goto yy237;
     if (yych <= '\n') goto yy225;
     if (yych <= '\f') goto yy237;
    } else {
     if (yych <= ' ') {
      if (yych <= 0x1F) goto yy237;
     } else {
      if (yych <= '!') goto yy239;
      if (yych <= '#') goto yy237;
      goto yy239;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '.') goto yy241;
     if (yych <= '9') goto yy239;
     if (yych <= ';') goto yy237;
     goto yy239;
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy237;
      goto yy239;
     } else {
      if (yych == '~') goto yy239;
      goto yy237;
     }
    }
   }
yy225:
   {
   while (c++)
     {
        if ((*c == '*') && (c + 1) && (c[1] == '/'))
          {
             c += 2;
             NEXT;
             goto main;
          }
     }
   RET(EDJE_UNKNOWN);
}
yy226:
   yyaccept = 0;
   YYMARKER = ++YYCURSOR;
   yych = *YYCURSOR;
   if (yych <= '.') {
    if (yych <= ' ') {
     if (yych <= 0x00) goto yy231;
     if (yych == '\n') goto yy231;
    } else {
     if (yych <= '!') goto yy226;
     if (yych <= '#') goto yy228;
     if (yych <= '-') goto yy226;
     goto yy230;
    }
   } else {
    if (yych <= '{') {
     if (yych <= '9') goto yy226;
     if (yych <= ';') goto yy228;
     if (yych <= 'z') goto yy226;
    } else {
     if (yych == '}') goto yy228;
     if (yych <= '~') goto yy226;
    }
   }
yy228:
   ++YYCURSOR;
   yych = *YYCURSOR;
   if (yybm[0+yych] & 64) {
    goto yy228;
   }
   goto yy231;
yy230:
   ++YYCURSOR;
   yych = *YYCURSOR;
   if (yych <= '\n') {
    if (yych <= 0x00) goto yy231;
    if (yych <= '\t') goto yy228;
   } else {
    if (yych <= '/') goto yy228;
    if (yych <= '9') goto yy233;
    goto yy228;
   }
yy231:
   ++YYCURSOR;
   { NEXT; goto main; }
yy233:
   yyaccept = 0;
   YYMARKER = ++YYCURSOR;
   yych = *YYCURSOR;
   if (yych <= '.') {
    if (yych <= ' ') {
     if (yych <= 0x00) goto yy231;
     if (yych == '\n') goto yy231;
     goto yy228;
    } else {
     if (yych <= '!') goto yy235;
     if (yych <= '#') goto yy228;
     if (yych >= '.') goto yy230;
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '/') goto yy235;
     if (yych <= '9') goto yy233;
     if (yych <= ';') goto yy228;
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy228;
     } else {
      if (yych != '~') goto yy228;
     }
    }
   }
yy235:
   yyaccept = 0;
   YYMARKER = ++YYCURSOR;
   yych = *YYCURSOR;
   if (yych <= '.') {
    if (yych <= ' ') {
     if (yych <= 0x00) goto yy231;
     if (yych == '\n') goto yy231;
     goto yy228;
    } else {
     if (yych <= '!') goto yy235;
     if (yych <= '#') goto yy228;
     if (yych <= '-') goto yy235;
     goto yy230;
    }
   } else {
    if (yych <= '{') {
     if (yych <= '9') goto yy235;
     if (yych <= ';') goto yy228;
     if (yych <= 'z') goto yy235;
     goto yy228;
    } else {
     if (yych == '}') goto yy228;
     if (yych <= '~') goto yy235;
     goto yy228;
    }
   }
yy237:
   ++YYCURSOR;
   yych = *YYCURSOR;
   if (yybm[0+yych] & 128) {
    goto yy237;
   }
   goto yy225;
yy239:
   ++YYCURSOR;
   yych = *YYCURSOR;
   if (yych <= '-') {
    if (yych <= '\r') {
     if (yych <= 0x08) goto yy237;
     if (yych <= '\n') goto yy225;
     if (yych <= '\f') goto yy237;
     goto yy225;
    } else {
     if (yych <= ' ') {
      if (yych <= 0x1F) goto yy237;
      goto yy225;
     } else {
      if (yych <= '!') goto yy239;
      if (yych <= '#') goto yy237;
      goto yy239;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '.') goto yy241;
     if (yych <= '9') goto yy239;
     if (yych <= ';') goto yy237;
     goto yy239;
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy237;
      goto yy239;
     } else {
      if (yych == '~') goto yy239;
      goto yy237;
     }
    }
   }
yy241:
   ++YYCURSOR;
   yych = *YYCURSOR;
   if (yych <= '\r') {
    if (yych <= 0x08) goto yy237;
    if (yych <= '\n') goto yy225;
    if (yych <= '\f') goto yy237;
    goto yy225;
   } else {
    if (yych <= ' ') {
     if (yych <= 0x1F) goto yy237;
     goto yy225;
    } else {
     if (yych <= '/') goto yy237;
     if (yych >= ':') goto yy237;
    }
   }
yy242:
   ++YYCURSOR;
   yych = *YYCURSOR;
   if (yych <= '-') {
    if (yych <= '\r') {
     if (yych <= 0x08) goto yy237;
     if (yych <= '\n') goto yy225;
     if (yych <= '\f') goto yy237;
     goto yy225;
    } else {
     if (yych <= ' ') {
      if (yych <= 0x1F) goto yy237;
      goto yy225;
     } else {
      if (yych <= '!') goto yy244;
      if (yych <= '#') goto yy237;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '/') {
      if (yych <= '.') goto yy241;
     } else {
      if (yych <= '9') goto yy242;
      if (yych <= ';') goto yy237;
     }
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy237;
     } else {
      if (yych != '~') goto yy237;
     }
    }
   }
yy244:
   ++YYCURSOR;
   yych = *YYCURSOR;
   if (yych <= '-') {
    if (yych <= '\r') {
     if (yych <= 0x08) goto yy237;
     if (yych <= '\n') goto yy225;
     if (yych <= '\f') goto yy237;
     goto yy225;
    } else {
     if (yych <= ' ') {
      if (yych <= 0x1F) goto yy237;
      goto yy225;
     } else {
      if (yych <= '!') goto yy244;
      if (yych <= '#') goto yy237;
      goto yy244;
     }
    }
   } else {
    if (yych <= 'z') {
     if (yych <= '.') goto yy241;
     if (yych <= '9') goto yy244;
     if (yych <= ';') goto yy237;
     goto yy244;
    } else {
     if (yych <= '|') {
      if (yych <= '{') goto yy237;
      goto yy244;
     } else {
      if (yych == '~') goto yy244;
      goto yy237;
     }
    }
   }
  }



     return NULL;
   }

   Edje *edje_parse_string(const char *str, Eina_Bool *err)
   {
      return edje_parser_string_parse(str, err, edje_parser_, edje_parser_Alloc, edje_parser_Free, edje_parser_stream_to_token);
   }
   Edje *edje_parse_file(const char *path, Eina_Bool *err)
   {
      return edje_parser_file_parse(path, err, edje_parser_, edje_parser_Alloc, edje_parser_Free, edje_parser_stream_to_token);
   }
#line 6717 "edje_parser.c"
/* Next is all token values, in a form suitable for use by makeheaders.
** This section will be null unless lemon is run with the -m switch.
*/
/* 
** These constants (all generated automatically by the parser generator)
** specify the various kinds of tokens (terminals) that the parser
** understands. 
**
** Each symbol here is a terminal symbol in the grammar.
*/
/* Make sure the INTERFACE macro is defined.
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/* The next thing included is series of defines which control
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 terminals
**                       and nonterminals.  "int" is used otherwise.
**    YYNOCODE           is a number of type YYCODETYPE which corresponds
**                       to no legal terminal or nonterminal number.  This
**                       number is used to fill in empty slots of the hash 
**                       table.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       have fall-back values which should be used if the
**                       original value of the token will not parse.
**    YYACTIONTYPE       is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 rules and
**                       states combined.  "int" is used otherwise.
**    edje_parser_TOKENTYPE     is the data type used for minor tokens given 
**                       directly to the parser from the tokenizer.
**    YYMINORTYPE        is the data type used for all minor tokens.
**                       This is typically a union of many types, one of
**                       which is edje_parser_TOKENTYPE.  The entry in the union
**                       for base tokens is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    edje_parser_ARG_SDECL     A static variable declaration for the %extra_argument
**    edje_parser_ARG_PDECL     A parameter declaration for the %extra_argument
**    edje_parser_ARG_STORE     Code to store %extra_argument into yypParser
**    edje_parser_ARG_FETCH     Code to extract %extra_argument from yypParser
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
*/
#define YYCODETYPE unsigned char
#define YYNOCODE 191
#define YYACTIONTYPE unsigned short int
#define edje_parser_TOKENTYPE Edje_Parser_Token *
typedef union {
  edje_parser_TOKENTYPE yy0;
  Edje_Group * yy63;
  const char * yy144;
  int yy381;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 2000
#endif
#define edje_parser_ARG_SDECL Edje_Parser *ep;
#define edje_parser_ARG_PDECL ,Edje_Parser *ep
#define edje_parser_ARG_FETCH Edje_Parser *ep = yypParser->ep
#define edje_parser_ARG_STORE yypParser->ep = ep
#define YYNSTATE 540
#define YYNRULE 329
#define YYERRORSYMBOL 40
#define YYERRSYMDT yy381
#define YY_NO_ACTION      (YYNSTATE+YYNRULE+2)
#define YY_ACCEPT_ACTION  (YYNSTATE+YYNRULE+1)
#define YY_ERROR_ACTION   (YYNSTATE+YYNRULE)

/* Next are that tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N < YYNSTATE                  Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   YYNSTATE <= N < YYNSTATE+YYNRULE   Reduce by rule N-YYNSTATE.
**
**   N == YYNSTATE+YYNRULE              A syntax error has occurred.
**
**   N == YYNSTATE+YYNRULE+1            The parser accepts its input.
**
**   N == YYNSTATE+YYNRULE+2            No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as
**
**      yy_action[ yy_shift_ofst[S] + X ]
**
** If the index value yy_shift_ofst[S]+X is out of range or if the value
** yy_lookahead[yy_shift_ofst[S]+X] is not equal to X or if yy_shift_ofst[S]
** is equal to YY_SHIFT_USE_DFLT, it means that the action is not in the table
** and that yy_default[S] should be used instead.  
**
** The formula above is for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array and YY_REDUCE_USE_DFLT is used in place of
** YY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
*/
static const YYACTIONTYPE yy_action[] = {
 /*     0 */   412,  413,  496,  498,   90,  411,  117,  418,  151,   56,
 /*    10 */   260,  353,  355,  359,  360,  361,  352,  114,  362,  321,
 /*    20 */   415,   61,  225,  338,  354,   71,  270,  148,  271,  272,
 /*    30 */   161,  273,  125,  322,  323,  274,   63,  152,  324,   67,
 /*    40 */   237,  238,   61,  414,   98,  197,   65,  540,  225,  225,
 /*    50 */    67,  416,   73,  227,  356,   98,  417,   74,  125,  125,
 /*    60 */   447,   69,  357,   73,  225,  313,  138,  358,   74,  140,
 /*    70 */   141,  178,   69,  446,  125,   40,  345,  169,    5,  534,
 /*    80 */   363,  364,  102,  258,  307,   63,  152,  148,  270,    2,
 /*    90 */   525,  419,  423,   78,  445,  189,   63,  274,  432,   79,
 /*   100 */   250,  238,   61,  439,   81,  306,  225,  270,  451,   82,
 /*   110 */   272,  161,  273,  175,  322,  323,  274,  472,   85,  209,
 /*   120 */   368,   69,  483,   87,  169,  225,  468,  493,   89,  197,
 /*   130 */   428,  307,  194,  189,   22,  310,  225,  511,  512,   92,
 /*   140 */   536,   94,  225,   63,  412,  413,  125,  454,  506,  411,
 /*   150 */   117,  418,  306,  140,  141,  353,  355,  359,  360,  361,
 /*   160 */   352,  114,  362,  470,  415,   61,   18,  449,  354,   71,
 /*   170 */   270,   62,  245,  272,  161,  273,  239,  322,  323,  274,
 /*   180 */    63,  241,  305,   67,  251,  384,  453,  414,   98,  197,
 /*   190 */    65,   48,  429,  426,   67,  416,   73,  276,  356,   98,
 /*   200 */   417,   74,   59,  469,  447,   69,  357,   73,  256,  157,
 /*   210 */   138,  358,   74,  140,  141,  447,   69,  446,  509,  498,
 /*   220 */    90,  138,  157,  351,  363,  364,  102,  160,  446,   63,
 /*   230 */    25,  268,  255,   64,  410,  419,  423,   78,  445,  278,
 /*   240 */    63,  295,  432,   79,  253,  255,   64,  439,   81,  445,
 /*   250 */   225,  313,  451,   82,  533,  507,  168,  178,  209,  283,
 /*   260 */   125,  472,   85,  285,  286,   69,  483,   87,  157,  225,
 /*   270 */   225,  493,   89,  248,  236,  270,   69,  271,  272,  160,
 /*   280 */   273,  511,  512,   92,  274,   94,  275,  506,  412,  413,
 /*   290 */   269,  255,   64,  411,  117,  418,   43,  100,  303,  353,
 /*   300 */   355,  359,  360,  361,  352,  114,  362,  225,  415,   61,
 /*   310 */   312,  444,  354,   71,  170,  290,  286,  270,  338,  314,
 /*   320 */   272,  161,  450,  225,   63,  323,  274,   67,  325,  225,
 /*   330 */   149,  414,   98,  347,   65,  313,  175,  316,   67,  416,
 /*   340 */    73,  176,  356,   98,  417,   74,   66,  281,  225,   69,
 /*   350 */   357,   73,  292,  160,  298,  358,   74,  164,  225,  335,
 /*   360 */    69,  148,  288,  127,  261,  169,  388,  535,  363,  364,
 /*   370 */   102,  337,  307,   27,  249,  238,   61,  155,  526,  419,
 /*   380 */   423,   78,  495,  390,   63,  209,  432,   79,   34,  289,
 /*   390 */   286,  439,   81,  306,  225,  313,  451,   82,  529,  171,
 /*   400 */   308,  215,  112,  225,  125,  472,   85,  284,  435,  538,
 /*   410 */   483,   87,  321,  155,  506,  493,   89,   68,  299,  407,
 /*   420 */   396,  271,  272,  311,  273,  511,  512,   92,  274,   94,
 /*   430 */   225,  763,  763,  313,  326,  763,  344,  175,  763,  176,
 /*   440 */   338,  763,  763,  763,  763,  225,  763,  763,  763,  301,
 /*   450 */   763,  763,  763,   32,  508,  498,   90,  763,  763,  763,
 /*   460 */   763,  763,   72,  319,  763,  763,  763,  440,  763,  763,
 /*   470 */   777,  777,  225,  452,  777,  244,  448,  777,  146,  194,
 /*   480 */   777,  777,  777,  777,  433,  777,  777,  777,  225,  777,
 /*   490 */   777,  777,  443,  346,  457,  194,  777,  777,  777,  777,
 /*   500 */   777,  463,  225,  777,  777,  777,  309,  777,  777,  170,
 /*   510 */   404,  397,   76,  394,  402,  403,  116,  405,  225,  313,
 /*   520 */   171,  329,  350,  401,  473,  178,  404,  397,  125,  475,
 /*   530 */   402,  403,  116,  405,  155,   63,  247,  484,  408,  401,
 /*   540 */   272,  161,  273,  225,  322,  323,  274,  267,  225,   67,
 /*   550 */   149,   63,  304,  398,   98,  170,  225,  486,  139,  442,
 /*   560 */   252,  399,   73,  149,  499,   67,  400,   74,  494,  398,
 /*   570 */    98,   69,  155,  280,  168,  515,  158,  399,   73,  425,
 /*   580 */   426,   41,  400,   74,   96,  437,  501,   69,  192,    8,
 /*   590 */   530,  387,   75,   80,  137,  513,  406,   77,  225,  313,
 /*   600 */   459,  521,  524,  199,   11,  186,  386,  387,   75,   83,
 /*   610 */   460,  318,  406,   77,  172,   99,  376,  371,   84,  466,
 /*   620 */   378,  375,  115,  380,   86,  480,  431,  438,  471,  377,
 /*   630 */   226,  482,  492,  510,  313,  522,  523,  336,  225,  313,
 /*   640 */   176,   63,  385,  225,  313,  215,  393,  531,  125,  183,
 /*   650 */   180,   93,  519,  125,  527,   67,   88,  490,  240,  372,
 /*   660 */    98,  146,  503,  146,  396,  211,  424,  373,   73,  189,
 /*   670 */   527,  518,  374,   74,  213,  376,  371,   69,  246,  378,
 /*   680 */   375,  115,  380,  465,   91,  504,  202,  158,  377,   10,
 /*   690 */   532,  379,  381,  103,  225,  313,  479,  282,  369,  205,
 /*   700 */    63,  180,  489,   35,  125,  207,  259,  260,  430,  426,
 /*   710 */    21,  265,  260,   28,   67,  291,   44,   33,  372,   98,
 /*   720 */    57,  320,   42,  147,  242,   97,  373,   73,  153,  263,
 /*   730 */    29,  374,   74,   38,    9,   20,   69,  277,   24,  294,
 /*   740 */   158,    4,  164,   45,    6,  315,  101,  293,  172,  370,
 /*   750 */   379,  381,  103,  165,  296,  302,  870,   12,  539,  218,
 /*   760 */   219,  220,  221,  222,  223,  224,  119,   14,   95,   31,
 /*   770 */    16,  383,   46,   47,   71,  172,  104,  367,  232,  235,
 /*   780 */   233,  183,  234,  231,  118,  113,  130,  339,   13,  537,
 /*   790 */   229,  230,  131,  340,  389,   65,    7,  183,  395,   67,
 /*   800 */   132,  341,    1,   63,   98,    3,  461,   36,   49,   17,
 /*   810 */    58,  105,   19,   65,  192,  456,  434,   67,  199,  192,
 /*   820 */    52,   69,   98,  108,  217,  218,  219,  220,  221,  222,
 /*   830 */   223,  224,  119,   39,   95,  462,   60,  467,  202,   69,
 /*   840 */    71,  474,  199,   70,  205,   53,  205,  485,  109,   26,
 /*   850 */   207,  500,   37,   54,  211,   55,  110,  514,  111,   23,
 /*   860 */   213,   65,   30,  177,  342,   67,  202,  232,  235,  233,
 /*   870 */    98,  234,  231,  118,  179,  365,  481,  207,  228,  229,
 /*   880 */   230,  184,  391,  136,  420,  187,  421,   69,  779,  195,
 /*   890 */   263,  779,   63,   50,   51,  491,  106,  107,  505,  520,
 /*   900 */   143,  478,   65,  206,  477,  211,   67,  208,  488,  213,
 /*   910 */   243,   98,  126,  331,  333,  254,  225,  313,  247,  120,
 /*   920 */   409,  214,  517,  186,  257,  121,  334,  150,   69,  154,
 /*   930 */   161,  262,   70,  156,  871,  264,  266,  159,   63,  279,
 /*   940 */   162,  163,  300,  287,  431,  438,  471,  330,  128,  482,
 /*   950 */   492,  510,   67,  522,  523,  166,  332,  167,  270,  331,
 /*   960 */   333,  297,  122,  871,  173,  120,  263,   15,  348,  328,
 /*   970 */    74,  174,  334,  317,   69,  123,  129,  871,  343,  133,
 /*   980 */   871,  366,  181,  182,   63,  382,  134,  871,  185,  871,
 /*   990 */   392,  135,  188,  330,  422,  190,  191,  871,   67,  331,
 /*  1000 */   333,  427,  332,  193,  124,  120,  436,  441,  196,  198,
 /*  1010 */   871,  455,  334,  200,  327,  328,   74,  201,  871,  458,
 /*  1020 */    69,  203,  204,  142,   63,  464,  476,  144,  871,  871,
 /*  1030 */   487,  210,  212,  330,  497,  502,  145,  216,   67,  871,
 /*  1040 */   225,  313,  332,  516,  528,  871,  871,  186,  871,  871,
 /*  1050 */   871,  871,  871,  871,  349,  328,   74,  871,  871,  871,
 /*  1060 */    69,  871,  871,  871,  871,  871,  871,  871,  431,  438,
 /*  1070 */   471,  871,  871,  482,  492,  510,  871,  522,  523,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */    45,   46,  176,  177,  178,   50,   51,   52,   77,   78,
 /*    10 */    79,   45,   46,   47,   48,   49,   50,   51,   52,    6,
 /*    20 */    65,   66,    1,   52,   58,   59,   13,   51,   15,   16,
 /*    30 */    17,   18,   11,   20,   21,   22,   70,    8,   25,   84,
 /*    40 */    64,   65,   66,   88,   89,    8,   80,    0,    1,    1,
 /*    50 */    84,   96,   97,    5,   88,   89,  101,  102,   11,   11,
 /*    60 */    45,  106,   96,   97,    1,    2,   51,  101,  102,   32,
 /*    70 */    33,    8,  106,   58,   11,  104,  105,   51,  112,  113,
 /*    80 */   114,  115,  116,    5,   58,   70,    8,   51,   13,  134,
 /*    90 */   135,  136,  137,  138,   79,    8,   70,   22,  143,  144,
 /*   100 */    64,   65,   66,  148,  149,   79,    1,   13,  153,  154,
 /*   110 */    16,   17,   18,    8,   20,   21,   22,  162,  163,    8,
 /*   120 */    26,  106,  167,  168,   51,    1,    5,  172,  173,    8,
 /*   130 */     5,   58,    8,    8,  108,  109,    1,  182,  183,  184,
 /*   140 */     5,  186,    1,   70,   45,   46,   11,  157,   37,   50,
 /*   150 */    51,   52,   79,   32,   33,   45,   46,   47,   48,   49,
 /*   160 */    50,   51,   52,  157,   65,   66,  151,  152,   58,   59,
 /*   170 */    13,   68,   69,   16,   17,   18,   67,   20,   21,   22,
 /*   180 */    70,   69,  109,   84,   71,   28,    5,   88,   89,    8,
 /*   190 */    80,  140,  141,  142,   84,   96,   97,   81,   88,   89,
 /*   200 */   101,  102,  156,  157,   45,  106,   96,   97,   76,   51,
 /*   210 */    51,  101,  102,   32,   33,   45,  106,   58,  176,  177,
 /*   220 */   178,   51,   51,  113,  114,  115,  116,   51,   58,   70,
 /*   230 */    72,   73,   74,   75,  135,  136,  137,  138,   79,   83,
 /*   240 */    70,   95,  143,  144,   73,   74,   75,  148,  149,   79,
 /*   250 */     1,    2,  153,  154,    5,    5,   51,    8,    8,   85,
 /*   260 */    11,  162,  163,   87,   88,  106,  167,  168,   51,    1,
 /*   270 */     1,  172,  173,    5,    5,   13,  106,   15,   16,   51,
 /*   280 */    18,  182,  183,  184,   22,  186,   24,   37,   45,   46,
 /*   290 */    73,   74,   75,   50,   51,   52,   91,   92,  107,   45,
 /*   300 */    46,   47,   48,   49,   50,   51,   52,    1,   65,   66,
 /*   310 */   111,  152,   58,   59,    8,   87,   88,   13,   52,   60,
 /*   320 */    16,   17,  152,    1,   70,   21,   22,   84,   98,    1,
 /*   330 */     8,   88,   89,    5,   80,    2,    8,   62,   84,   96,
 /*   340 */    97,    8,   88,   89,  101,  102,   82,   83,    1,  106,
 /*   350 */    96,   97,    5,   51,    5,  101,  102,    8,    1,  103,
 /*   360 */   106,   51,    5,    9,   10,   51,  128,  113,  114,  115,
 /*   370 */   116,  105,   58,   63,   64,   65,   66,   23,  135,  136,
 /*   380 */   137,  138,    5,  130,   70,    8,  143,  144,   86,   87,
 /*   390 */    88,  148,  149,   79,    1,    2,  153,  154,    5,    9,
 /*   400 */    10,    8,  139,    1,   11,  162,  163,    5,  147,    3,
 /*   410 */   167,  168,    6,   23,   37,  172,  173,   94,   95,  133,
 /*   420 */    27,   15,   16,  109,   18,  182,  183,  184,   22,  186,
 /*   430 */     1,    1,    2,    2,    5,    5,    5,    8,    8,    8,
 /*   440 */    52,   11,   12,   13,   14,    1,   16,   17,   18,    5,
 /*   450 */    20,   21,   22,  175,  176,  177,  178,   27,   28,   29,
 /*   460 */    30,   31,   61,   62,   34,   35,   36,  150,   38,   39,
 /*   470 */     1,    2,    1,  155,    5,    5,    5,    8,    8,    8,
 /*   480 */    11,   12,   13,   14,  145,   16,   17,   18,    1,   20,
 /*   490 */    21,   22,    5,  105,  159,    8,   27,   28,   29,   30,
 /*   500 */    31,  161,    1,   34,   35,   36,    5,   38,   39,    8,
 /*   510 */    45,   46,  129,  130,   49,   50,   51,   52,    1,    2,
 /*   520 */     9,   10,    5,   58,  164,    8,   45,   46,   11,  166,
 /*   530 */    49,   50,   51,   52,   23,   70,   12,  169,   14,   58,
 /*   540 */    16,   17,   18,    1,   20,   21,   22,    5,    1,   84,
 /*   550 */     8,   70,    5,   88,   89,    8,    1,  171,    9,   10,
 /*   560 */     5,   96,   97,    8,  179,   84,  101,  102,  174,   88,
 /*   570 */    89,  106,   23,    5,   51,  188,    8,   96,   97,  141,
 /*   580 */   142,    4,  101,  102,    7,    5,  181,  106,    8,  124,
 /*   590 */   125,  126,  127,  146,  147,  185,  131,  132,    1,    2,
 /*   600 */     5,  185,    5,    8,    4,    8,  125,  126,  127,  158,
 /*   610 */   159,    5,  131,  132,    8,   92,   45,   46,  160,  161,
 /*   620 */    49,   50,   51,   52,  165,  166,   29,   30,   31,   58,
 /*   630 */    54,   34,   35,   36,    2,   38,   39,    5,    1,    2,
 /*   640 */     8,   70,    5,    1,    2,    8,    5,    5,   11,    8,
 /*   650 */     8,  187,  188,   11,  189,   84,  170,  171,    5,   88,
 /*   660 */    89,    8,    5,    8,   27,    8,    5,   96,   97,    8,
 /*   670 */   189,    5,  101,  102,    8,   45,   46,  106,   69,   49,
 /*   680 */    50,   51,   52,    5,  180,  181,    8,    8,   58,  118,
 /*   690 */   119,  120,  121,  122,    1,    2,    5,   83,    5,    8,
 /*   700 */    70,    8,    5,    4,   11,    8,   78,   79,  141,  142,
 /*   710 */     4,   78,   79,    7,   84,   90,    4,    4,   88,   89,
 /*   720 */     7,   62,    4,    9,   10,    7,   96,   97,    9,   10,
 /*   730 */     4,  101,  102,    7,    4,    4,  106,    5,    7,    5,
 /*   740 */     8,    4,    8,    4,    7,    5,    7,   93,    8,  119,
 /*   750 */   120,  121,  122,    9,   10,   93,   41,   42,   43,   44,
 /*   760 */    45,   46,   47,   48,   49,   50,   51,    4,   53,    4,
 /*   770 */     7,  123,    7,    4,   59,    8,    7,  117,   45,   46,
 /*   780 */    47,    8,   49,   50,   51,  147,    9,   10,   55,   56,
 /*   790 */    57,   58,    9,   10,    5,   80,    4,    8,  130,   84,
 /*   800 */     9,   10,    4,   70,   89,    7,  159,    4,    4,    4,
 /*   810 */     7,    7,    7,   80,    8,    5,    5,   84,    8,    8,
 /*   820 */     4,  106,   89,    7,   43,   44,   45,   46,   47,   48,
 /*   830 */    49,   50,   51,    4,   53,    5,    7,  161,    8,  106,
 /*   840 */    59,    5,    8,  110,    8,    4,    8,    5,    7,    4,
 /*   850 */     8,    5,    7,    4,    8,    4,    7,    5,    7,    4,
 /*   860 */     8,   80,    7,    9,   10,   84,    8,   45,   46,   47,
 /*   870 */    89,   49,   50,   51,    9,   10,  166,    8,   56,   57,
 /*   880 */    58,    9,   10,    9,   10,    9,   10,  106,    5,    9,
 /*   890 */    10,    8,   70,    4,    4,  171,    7,    7,  181,  188,
 /*   900 */     9,   10,   80,    9,   10,    8,   84,    9,   10,    8,
 /*   910 */    10,   89,    9,   45,   46,   10,    1,    2,   12,   51,
 /*   920 */     5,    9,   10,    8,    5,    9,   58,    9,  106,    9,
 /*   930 */    17,   10,  110,    9,  190,   10,   14,    9,   70,   10,
 /*   940 */     9,    9,   19,   10,   29,   30,   31,   79,    9,   34,
 /*   950 */    35,   36,   84,   38,   39,    9,   88,    9,   13,   45,
 /*   960 */    46,   10,    9,  190,    9,   51,   10,   99,  100,  101,
 /*   970 */   102,    9,   58,   10,  106,    9,    9,  190,   10,    9,
 /*   980 */   190,   10,    9,    9,   70,   10,    9,  190,    9,  190,
 /*   990 */    10,    9,    9,   79,   10,    9,    9,  190,   84,   45,
 /*  1000 */    46,   10,   88,    9,    9,   51,   10,   10,    9,    9,
 /*  1010 */   190,   10,   58,    9,  100,  101,  102,    9,  190,   10,
 /*  1020 */   106,    9,    9,    9,   70,   10,   10,    9,  190,  190,
 /*  1030 */    10,    9,    9,   79,   10,   10,    9,    9,   84,  190,
 /*  1040 */     1,    2,   88,   10,   10,  190,  190,    8,  190,  190,
 /*  1050 */   190,  190,  190,  190,  100,  101,  102,  190,  190,  190,
 /*  1060 */   106,  190,  190,  190,  190,  190,  190,  190,   29,   30,
 /*  1070 */    31,  190,  190,   34,   35,   36,  190,   38,   39,
};
#define YY_SHIFT_USE_DFLT (-1)
#define YY_SHIFT_MAX 216
static const short yy_shift_ofst[] = {
 /*     0 */    21,  597,  915, 1039,  249,  517,   63,  393,  637,  642,
 /*    10 */   693,  135,   47,   48,  328,  429,  105,  471,  487,  124,
 /*    20 */   501,  542,  547,  268,  306,  555,  250,  269,  322,  357,
 /*    30 */   141,  431,  377,   78,  402,  444,  125,  111,  141,  121,
 /*    40 */   632,  470,  568,  347,  349,  606,  333,  641,  661,  580,
 /*    50 */   595,  678,  691,  697,  657,  666,   29,   29,   87,  181,
 /*    60 */    37,  577,  653,  706,  713,  718,  732,  726,  734,  731,
 /*    70 */   737,  739,  740,  763,  765,  769,  789,  798,  803,  804,
 /*    80 */   811,  805,  829,  810,  830,  816,  836,  841,  842,  845,
 /*    90 */   849,  846,  851,  852,  851,  600,  655,  679,  699,  712,
 /*   100 */   712,  767,  730,  792,  773,  806,  834,  858,  838,  869,
 /*   110 */   897,  901,  430,  469,   13,   94,  157,  524,  262,  406,
 /*   120 */   304,  354,  390,  511,  549,  855,  714,  719,  744,  777,
 /*   130 */   783,  791,  854,  865,  872,  874,  876,  883,   75,  880,
 /*   140 */   889,  890,  891,  894,  898,  912,  903,  900,  906,  918,
 /*   150 */   905,  919,  916,  920,  921,  924,  925,  922,  928,  929,
 /*   160 */   913,  931,  932,  933,  939,  946,  948,  951,  923,  945,
 /*   170 */   953,  956,  955,  962,  963,  966,  967,  968,  970,  971,
 /*   180 */   973,  974,  975,  977,  979,  980,  982,  983,  984,  986,
 /*   190 */   987,  991,  994,  996,  995,  999,  997, 1000, 1001, 1004,
 /*   200 */  1008, 1009, 1012, 1013, 1015, 1014, 1016, 1018, 1020, 1022,
 /*   210 */  1024, 1023, 1025, 1027, 1033, 1028, 1034,
};
#define YY_REDUCE_USE_DFLT (-175)
#define YY_REDUCE_MAX 111
static const short yy_reduce_ofst[] = {
 /*     0 */   715,  -45,   99,  243,  -34,  110,  254,  465,  481,  571,
 /*    10 */   630,  733,  781,  822,  868,  914,  954,   15,  159,  170,
 /*    20 */    26,  158,   73,  310,  314,  171,  278,  -24,  217,  302,
 /*    30 */    36,  -29, -174,  -69,  176,  205,   51,   42,  228,   46,
 /*    40 */   266,  103,  264,  523,  323,  401,  388,  383,  438,  447,
 /*    50 */   451,  458,  459,  486,  504,  464,  628,  633,  567,  -10,
 /*    60 */     6,  109,  112,  113,  132,  116,  156,  174,  146,  191,
 /*    70 */   199,  259,  275,  230,  256,  238,  253,  286,  263,  339,
 /*    80 */   261,  317,  318,  335,  340,  360,  363,  368,  386,  394,
 /*    90 */   385,  405,  410,  387,  416,  576,  609,  614,  625,  654,
 /*   100 */   662,  659,  660,  648,  668,  638,  647,  676,  710,  724,
 /*   110 */   717,  711,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   550,  550,  550,  550,  550,  550,  550,  550,  550,  550,
 /*    10 */   550,  550,  550,  550,  550,  550,  550,  550,  550,  550,
 /*    20 */   550,  550,  550,  550,  550,  550,  869,  550,  550,  550,
 /*    30 */   550,  869,  869,  869,  550,  550,  869,  869,  550,  869,
 /*    40 */   869,  869,  869,  550,  869,  869,  869,  869,  869,  869,
 /*    50 */   869,  869,  869,  869,  869,  869,  604,  869,  869,  869,
 /*    60 */   869,  869,  869,  869,  869,  869,  869,  869,  869,  869,
 /*    70 */   869,  869,  869,  869,  869,  869,  869,  869,  869,  869,
 /*    80 */   869,  869,  869,  869,  869,  869,  869,  869,  869,  869,
 /*    90 */   869,  869,  869,  869,  869,  869,  869,  869,  869,  869,
 /*   100 */   869,  869,  869,  869,  869,  869,  869,  869,  869,  869,
 /*   110 */   869,  869,  869,  869,  869,  869,  869,  869,  869,  869,
 /*   120 */   869,  869,  869,  869,  869,  869,  869,  869,  869,  869,
 /*   130 */   869,  869,  869,  869,  869,  869,  869,  869,  869,  869,
 /*   140 */   869,  869,  869,  869,  869,  869,  869,  869,  869,  869,
 /*   150 */   869,  869,  869,  869,  869,  869,  869,  869,  869,  869,
 /*   160 */   869,  869,  869,  869,  869,  869,  869,  869,  869,  869,
 /*   170 */   869,  869,  869,  869,  869,  869,  869,  869,  869,  869,
 /*   180 */   869,  869,  869,  869,  869,  869,  869,  869,  869,  869,
 /*   190 */   869,  869,  869,  869,  869,  869,  869,  869,  869,  869,
 /*   200 */   869,  869,  869,  869,  869,  869,  869,  869,  869,  869,
 /*   210 */   869,  869,  869,  869,  869,  869,  869,  542,  543,  544,
 /*   220 */   545,  546,  547,  548,  549,  551,  553,  555,  557,  559,
 /*   230 */   560,  561,  562,  563,  564,  565,  574,  577,  579,  580,
 /*   240 */   582,  585,  587,  588,  583,  586,  584,  581,  575,  578,
 /*   250 */   576,  589,  591,  594,  596,  597,  598,  600,  601,  603,
 /*   260 */   605,  606,  607,  675,  676,  602,  599,  592,  595,  593,
 /*   270 */   590,  609,  617,  626,  666,  678,  608,  610,  613,  615,
 /*   280 */   611,  614,  612,  616,  618,  621,  623,  624,  619,  622,
 /*   290 */   620,  625,  627,  629,  632,  634,  636,  637,  633,  635,
 /*   300 */   631,  628,  630,  665,  667,  670,  672,  673,  674,  668,
 /*   310 */   671,  669,  677,  552,  566,  568,  571,  573,  569,  572,
 /*   320 */   570,  567,  639,  653,  700,  638,  640,  643,  645,  646,
 /*   330 */   647,  648,  649,  650,  651,  652,  654,  657,  659,  660,
 /*   340 */   661,  662,  663,  664,  655,  658,  656,  641,  644,  642,
 /*   350 */   679,  682,  684,  685,  686,  687,  688,  689,  690,  691,
 /*   360 */   692,  693,  694,  695,  696,  697,  698,  699,  718,  701,
 /*   370 */   703,  705,  706,  707,  708,  709,  710,  711,  712,  713,
 /*   380 */   714,  715,  716,  717,  744,  719,  721,  723,  724,  726,
 /*   390 */   729,  731,  732,  727,  730,  728,  725,  733,  734,  735,
 /*   400 */   736,  737,  738,  739,  740,  741,  742,  743,  783,  745,
 /*   410 */   748,  750,  751,  752,  753,  754,  755,  756,  757,  758,
 /*   420 */   759,  760,  761,  762,  765,  768,  770,  771,  766,  769,
 /*   430 */   767,  764,  772,  773,  775,  778,  780,  776,  774,  781,
 /*   440 */   782,  790,  789,  784,  787,  791,  792,  793,  785,  788,
 /*   450 */   786,  794,  795,  797,  800,  802,  803,  806,  808,  804,
 /*   460 */   807,  805,  809,  812,  814,  810,  813,  811,  798,  801,
 /*   470 */   799,  796,  815,  816,  818,  821,  823,  824,  825,  819,
 /*   480 */   822,  820,  817,  826,  827,  829,  832,  834,  835,  830,
 /*   490 */   833,  831,  828,  836,  837,  839,  842,  844,  845,  846,
 /*   500 */   848,  851,  853,  849,  852,  850,  847,  840,  843,  841,
 /*   510 */   838,  854,  855,  856,  860,  863,  865,  866,  861,  864,
 /*   520 */   862,  857,  858,  859,  746,  749,  747,  867,  868,  720,
 /*   530 */   722,  702,  704,  680,  683,  681,  556,  558,  554,  541,
};
#define YY_SZ_ACTTAB (int)(sizeof(yy_action)/sizeof(yy_action[0]))

/* The next table maps tokens into fallback tokens.  If a construct
** like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammer, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
*/
struct yyStackEntry {
  int stateno;       /* The state-number */
  int major;         /* The major token value.  This is the code
                     ** number for the token at this stack level */
  YYMINORTYPE minor; /* The user-supplied minor token value.  This
                     ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  int yyidx;                    /* Index of top element in stack */
  int yyerrcnt;                 /* Shifts left before out of the error */
  edje_parser_ARG_SDECL                /* A place to hold %extra_argument */
#if YYSTACKDEPTH<=0
  int yystksz;                  /* Current side of the stack */
  yyStackEntry *yystack;        /* The parser's stack */
#else
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
#endif
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/* 
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL 
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void edje_parser_Trace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const yyTokenName[] = { 
  "$",             "DOC",           "EMBRYO",        "COLLECTIONS", 
  "LB",            "RB",            "DATA",          "DOT",         
  "PROPERTY",      "ID",            "SEMICOLON",     "STYLES",      
  "STYLE",         "SET",           "IMAGE",         "EXTERNALS",   
  "FONTS",         "FONT",          "COLOR_CLASSES",  "COLOR_CLASS", 
  "PROGRAMS",      "PROGRAM",       "IMAGES",        "LOSSY",       
  "GROUP",         "PARTS",         "PART",          "DRAGABLE",    
  "DESCRIPTION",   "PARAMS",        "PERSPECTIVE",   "FILL",        
  "ORIGIN",        "SIZE",          "TEXT",          "BOX",         
  "MAP",           "ROTATION",      "REL1",          "REL2",        
  "error",         "edje",          "toplevel_blocks",  "toplevel_block",
  "collection",    "images",        "fonts",         "externals",   
  "data",          "styles",        "color_classes",  "doc_comment", 
  "script",        "collection_start",  "collection_block",  "collections_subblocks",
  "collections_subblock",  "group",         "set",           "data_start",  
  "data_block",    "data_subblocks",  "data_subblock",  "styles_blocks",
  "styles_block",  "style",         "style_start",   "style_block", 
  "style_subblocks",  "style_subblock",  "set_start",     "set_block",   
  "set_subblocks",  "set_subblock",  "set_image",     "set_image_start",
  "set_image_block",  "set_image_subblocks",  "set_image_subblock",  "images_image",
  "externals_start",  "externals_block",  "externals_subblocks",  "externals_subblock",
  "fonts_start",   "fonts_block",   "fonts_subblocks",  "fonts_subblock",
  "font",          "color_classes_start",  "color_classes_block",  "color_class", 
  "color_class_start",  "color_class_block",  "color_class_properties",  "color_class_property",
  "programs",      "programs_start",  "programs_blocks",  "programs_subblocks",
  "programs_subblock",  "program",       "program_start",  "program_blocks",
  "program_subblocks",  "program_subblock",  "images_start",  "images_blocks",
  "images_subblocks",  "images_subblock",  "group_start",   "group_block", 
  "group_subblocks",  "group_subblock",  "parts",         "group_property",
  "parts_start",   "parts_block",   "parts_subblocks",  "parts_subblock",
  "part",          "parts_property",  "part_start",    "part_block",  
  "part_subblocks",  "part_subblock",  "part_dragable",  "part_dragable_start",
  "part_dragable_block",  "part_dragable_props",  "part_dragable_prop",  "part_description",
  "part_description_start",  "part_description_block",  "part_description_subblocks",  "part_description_subblock",
  "part_description_property",  "part_description_params",  "part_description_params_start",  "part_description_params_block",
  "part_description_params_subblocks",  "part_description_params_subblock",  "part_description_param",  "part_description_perspective",
  "part_description_perspective_start",  "part_description_perspective_block",  "part_description_perspective_subblocks",  "part_description_perspective_subblock",
  "part_description_image",  "part_description_image_start",  "part_description_image_block",  "part_description_image_subblocks",
  "part_description_image_subblock",  "part_description_fill",  "part_description_fill_start",  "part_description_fill_block",
  "part_description_fill_subblocks",  "part_description_fill_subblock",  "part_description_fill_subblock_origin_subblocks",  "part_description_fill_subblock_origin_subblock",
  "part_description_fill_subblock_size_subblocks",  "part_description_fill_subblock_size_subblock",  "part_description_text",  "part_description_text_start",
  "part_description_text_block",  "part_description_text_props",  "part_description_text_prop",  "part_description_box",
  "part_description_box_start",  "part_description_box_block",  "part_description_box_props",  "part_description_box_prop",
  "part_description_map",  "part_description_map_start",  "part_description_map_block",  "part_description_map_props",
  "part_description_map_prop",  "part_description_map_rotation",  "part_description_map_rotation_start",  "part_description_map_rotation_block",
  "part_description_map_rotation_subblocks",  "part_description_map_rotation_subblock",  "part_description_relone",  "part_description_reltwo",
  "part_description_relone_start",  "part_description_rel_props",  "part_description_reltwo_start",  "relative_properties",
  "relative_property",  "part_property",
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "edje ::= toplevel_blocks",
 /*   1 */ "toplevel_blocks ::= toplevel_block",
 /*   2 */ "toplevel_blocks ::= toplevel_blocks toplevel_block",
 /*   3 */ "toplevel_block ::= collection",
 /*   4 */ "toplevel_block ::= images",
 /*   5 */ "toplevel_block ::= fonts",
 /*   6 */ "toplevel_block ::= externals",
 /*   7 */ "toplevel_block ::= data",
 /*   8 */ "toplevel_block ::= styles",
 /*   9 */ "toplevel_block ::= color_classes",
 /*  10 */ "doc_comment ::=",
 /*  11 */ "doc_comment ::= DOC",
 /*  12 */ "script ::= EMBRYO",
 /*  13 */ "collection ::= collection_start collection_block",
 /*  14 */ "collection_start ::= doc_comment COLLECTIONS",
 /*  15 */ "collection_block ::= LB collections_subblocks RB",
 /*  16 */ "collection_block ::= LB RB",
 /*  17 */ "collections_subblocks ::= collections_subblocks collections_subblock",
 /*  18 */ "collections_subblocks ::= collections_subblock",
 /*  19 */ "collections_subblock ::= group",
 /*  20 */ "collections_subblock ::= set",
 /*  21 */ "collections_subblock ::= color_classes",
 /*  22 */ "collections_subblock ::= images",
 /*  23 */ "collections_subblock ::= externals",
 /*  24 */ "collections_subblock ::= styles",
 /*  25 */ "collections_subblock ::= fonts",
 /*  26 */ "data ::= data_start data_block",
 /*  27 */ "data_start ::= doc_comment DATA",
 /*  28 */ "data_block ::= LB data_subblocks RB",
 /*  29 */ "data_block ::= LB RB",
 /*  30 */ "data_block ::= DOT data_subblock",
 /*  31 */ "data_subblocks ::= data_subblocks data_subblock",
 /*  32 */ "data_subblocks ::= data_subblock",
 /*  33 */ "data_subblock ::= PROPERTY ID ID SEMICOLON",
 /*  34 */ "styles ::= STYLES LB styles_blocks RB",
 /*  35 */ "styles ::= STYLES LB RB",
 /*  36 */ "styles ::= STYLES DOT styles_block",
 /*  37 */ "styles_blocks ::= styles_blocks styles_block",
 /*  38 */ "styles_blocks ::= styles_block",
 /*  39 */ "styles_block ::= style",
 /*  40 */ "style ::= style_start style_block",
 /*  41 */ "style_start ::= doc_comment STYLE",
 /*  42 */ "style_block ::= LB style_subblocks RB",
 /*  43 */ "style_block ::= LB RB",
 /*  44 */ "style_block ::= DOT style_subblock",
 /*  45 */ "style_subblocks ::= style_subblocks style_subblock",
 /*  46 */ "style_subblocks ::= style_subblock",
 /*  47 */ "style_subblock ::= PROPERTY ID SEMICOLON",
 /*  48 */ "style_subblock ::= PROPERTY ID ID SEMICOLON",
 /*  49 */ "set ::= set_start set_block",
 /*  50 */ "set_start ::= doc_comment SET",
 /*  51 */ "set_block ::= LB set_subblocks RB",
 /*  52 */ "set_block ::= LB RB",
 /*  53 */ "set_block ::= DOT set_subblock",
 /*  54 */ "set_subblocks ::= set_subblocks set_subblock",
 /*  55 */ "set_subblocks ::= set_subblock",
 /*  56 */ "set_subblock ::= PROPERTY ID SEMICOLON",
 /*  57 */ "set_subblock ::= set_image",
 /*  58 */ "set_image ::= set_image_start set_image_block",
 /*  59 */ "set_image_start ::= doc_comment IMAGE",
 /*  60 */ "set_image_block ::= LB set_image_subblocks RB",
 /*  61 */ "set_image_block ::= LB RB",
 /*  62 */ "set_image_block ::= DOT set_image_subblock",
 /*  63 */ "set_image_subblocks ::= set_image_subblock set_image_subblock",
 /*  64 */ "set_image_subblocks ::= set_image_subblock",
 /*  65 */ "set_image_subblock ::= images_image",
 /*  66 */ "set_image_subblock ::= PROPERTY ID SEMICOLON",
 /*  67 */ "set_image_subblock ::= PROPERTY ID ID ID ID SEMICOLON",
 /*  68 */ "externals ::= externals_start externals_block",
 /*  69 */ "externals_start ::= doc_comment EXTERNALS",
 /*  70 */ "externals_block ::= LB externals_subblocks RB",
 /*  71 */ "externals_block ::= LB RB",
 /*  72 */ "externals_block ::= DOT externals_subblock",
 /*  73 */ "externals_subblocks ::= externals_subblocks externals_subblock",
 /*  74 */ "externals_subblocks ::= externals_subblock",
 /*  75 */ "externals_subblock ::= PROPERTY ID SEMICOLON",
 /*  76 */ "fonts ::= fonts_start fonts_block",
 /*  77 */ "fonts_start ::= doc_comment FONTS",
 /*  78 */ "fonts_block ::= LB fonts_subblocks RB",
 /*  79 */ "fonts_block ::= LB RB",
 /*  80 */ "fonts_block ::= DOT fonts_subblock",
 /*  81 */ "fonts_subblocks ::= fonts_subblocks fonts_subblock",
 /*  82 */ "fonts_subblocks ::= fonts_subblock",
 /*  83 */ "fonts_subblock ::= font",
 /*  84 */ "font ::= doc_comment FONT ID ID SEMICOLON",
 /*  85 */ "color_classes ::= color_classes_start color_classes_block",
 /*  86 */ "color_classes_start ::= doc_comment COLOR_CLASSES",
 /*  87 */ "color_classes_block ::= LB color_class RB",
 /*  88 */ "color_classes_block ::= LB RB",
 /*  89 */ "color_class ::= color_class color_class_start color_class_block",
 /*  90 */ "color_class ::= color_class_start color_class_block",
 /*  91 */ "color_class_start ::= doc_comment COLOR_CLASS",
 /*  92 */ "color_class_block ::= LB color_class_properties RB",
 /*  93 */ "color_class_block ::= LB RB",
 /*  94 */ "color_class_properties ::= color_class_properties color_class_property",
 /*  95 */ "color_class_properties ::= color_class_property",
 /*  96 */ "color_class_property ::= PROPERTY ID SEMICOLON",
 /*  97 */ "color_class_property ::= PROPERTY ID ID ID ID SEMICOLON",
 /*  98 */ "programs ::= programs_start programs_blocks",
 /*  99 */ "programs_start ::= doc_comment PROGRAMS",
 /* 100 */ "programs_blocks ::= LB programs_subblocks RB",
 /* 101 */ "programs_blocks ::= LB RB",
 /* 102 */ "programs_blocks ::= DOT programs_subblock",
 /* 103 */ "programs_subblocks ::= programs_subblocks programs_subblock",
 /* 104 */ "programs_subblocks ::= programs_subblock",
 /* 105 */ "programs_subblock ::= program",
 /* 106 */ "programs_subblock ::= PROPERTY ID SEMICOLON",
 /* 107 */ "programs_subblock ::= images_image",
 /* 108 */ "programs_subblock ::= images",
 /* 109 */ "programs_subblock ::= font",
 /* 110 */ "programs_subblock ::= fonts",
 /* 111 */ "programs_subblock ::= set",
 /* 112 */ "program ::= program_start program_blocks",
 /* 113 */ "program_start ::= doc_comment PROGRAM",
 /* 114 */ "program_blocks ::= LB program_subblocks RB",
 /* 115 */ "program_blocks ::= LB RB",
 /* 116 */ "program_blocks ::= DOT program_subblock",
 /* 117 */ "program_subblocks ::= program_subblocks program_subblock",
 /* 118 */ "program_subblocks ::= program_subblock",
 /* 119 */ "program_subblock ::= script",
 /* 120 */ "program_subblock ::= PROPERTY ID SEMICOLON",
 /* 121 */ "program_subblock ::= PROPERTY ID ID SEMICOLON",
 /* 122 */ "program_subblock ::= PROPERTY ID ID ID SEMICOLON",
 /* 123 */ "program_subblock ::= PROPERTY ID ID ID ID SEMICOLON",
 /* 124 */ "program_subblock ::= PROPERTY ID ID ID ID ID SEMICOLON",
 /* 125 */ "images ::= images_start images_blocks",
 /* 126 */ "images_start ::= doc_comment IMAGES",
 /* 127 */ "images_blocks ::= LB images_subblocks RB",
 /* 128 */ "images_blocks ::= LB RB",
 /* 129 */ "images_blocks ::= DOT images_subblock",
 /* 130 */ "images_subblocks ::= images_subblocks images_subblock",
 /* 131 */ "images_subblocks ::= images_subblock",
 /* 132 */ "images_subblock ::= images_image",
 /* 133 */ "images_subblock ::= set",
 /* 134 */ "images_subblock ::= PROPERTY ID SEMICOLON",
 /* 135 */ "images_image ::= PROPERTY ID ID SEMICOLON",
 /* 136 */ "images_image ::= PROPERTY ID LOSSY ID SEMICOLON",
 /* 137 */ "group ::= group_start group_block",
 /* 138 */ "group_start ::= doc_comment GROUP",
 /* 139 */ "group_block ::= LB group_subblocks RB",
 /* 140 */ "group_block ::= LB RB",
 /* 141 */ "group_block ::= DOT group_subblock",
 /* 142 */ "group_subblocks ::= group_subblocks group_subblock",
 /* 143 */ "group_subblocks ::= group_subblock",
 /* 144 */ "group_subblock ::= color_classes",
 /* 145 */ "group_subblock ::= images",
 /* 146 */ "group_subblock ::= set",
 /* 147 */ "group_subblock ::= fonts",
 /* 148 */ "group_subblock ::= font",
 /* 149 */ "group_subblock ::= programs",
 /* 150 */ "group_subblock ::= program",
 /* 151 */ "group_subblock ::= externals",
 /* 152 */ "group_subblock ::= data",
 /* 153 */ "group_subblock ::= styles",
 /* 154 */ "group_subblock ::= script",
 /* 155 */ "group_subblock ::= parts",
 /* 156 */ "group_subblock ::= group_property",
 /* 157 */ "group_property ::= PROPERTY ID SEMICOLON",
 /* 158 */ "group_property ::= PROPERTY ID ID SEMICOLON",
 /* 159 */ "parts ::= parts_start parts_block",
 /* 160 */ "parts_start ::= doc_comment PARTS",
 /* 161 */ "parts_block ::= LB parts_subblocks RB",
 /* 162 */ "parts_block ::= LB RB",
 /* 163 */ "parts_subblocks ::= parts_subblocks parts_subblock",
 /* 164 */ "parts_subblocks ::= parts_subblock",
 /* 165 */ "parts_subblock ::= fonts",
 /* 166 */ "parts_subblock ::= font",
 /* 167 */ "parts_subblock ::= programs",
 /* 168 */ "parts_subblock ::= program",
 /* 169 */ "parts_subblock ::= color_classes",
 /* 170 */ "parts_subblock ::= images",
 /* 171 */ "parts_subblock ::= set",
 /* 172 */ "parts_subblock ::= styles",
 /* 173 */ "parts_subblock ::= part",
 /* 174 */ "parts_subblock ::= script",
 /* 175 */ "parts_subblock ::= parts_property",
 /* 176 */ "parts_property ::= PROPERTY ID ID SEMICOLON",
 /* 177 */ "part ::= part_start part_block",
 /* 178 */ "part_start ::= doc_comment PART",
 /* 179 */ "part_block ::= LB part_subblocks RB",
 /* 180 */ "part_block ::= LB RB",
 /* 181 */ "part_subblocks ::= part_subblocks part_subblock",
 /* 182 */ "part_subblocks ::= part_subblock",
 /* 183 */ "part_subblock ::= part_dragable",
 /* 184 */ "part_dragable ::= part_dragable_start part_dragable_block",
 /* 185 */ "part_dragable_start ::= DRAGABLE",
 /* 186 */ "part_dragable_block ::= LB part_dragable_props RB",
 /* 187 */ "part_dragable_block ::= LB RB",
 /* 188 */ "part_dragable_block ::= DOT part_dragable_prop",
 /* 189 */ "part_dragable_props ::= part_dragable_props part_dragable_prop",
 /* 190 */ "part_dragable_props ::= part_dragable_prop",
 /* 191 */ "part_dragable_prop ::= PROPERTY ID SEMICOLON",
 /* 192 */ "part_dragable_prop ::= PROPERTY ID ID ID SEMICOLON",
 /* 193 */ "part_subblock ::= fonts",
 /* 194 */ "part_subblock ::= font",
 /* 195 */ "part_subblock ::= programs",
 /* 196 */ "part_subblock ::= program",
 /* 197 */ "part_subblock ::= set",
 /* 198 */ "part_subblock ::= styles",
 /* 199 */ "part_subblock ::= color_classes",
 /* 200 */ "part_subblock ::= images",
 /* 201 */ "part_subblock ::= script",
 /* 202 */ "part_subblock ::= part_description",
 /* 203 */ "part_description ::= part_description_start part_description_block",
 /* 204 */ "part_description_start ::= doc_comment DESCRIPTION",
 /* 205 */ "part_description_block ::= LB part_description_subblocks RB",
 /* 206 */ "part_description_block ::= LB RB",
 /* 207 */ "part_description_block ::= DOT part_description_subblock",
 /* 208 */ "part_description_subblocks ::= part_description_subblocks part_description_subblock",
 /* 209 */ "part_description_subblocks ::= part_description_subblock",
 /* 210 */ "part_description_subblock ::= color_classes",
 /* 211 */ "part_description_subblock ::= images",
 /* 212 */ "part_description_subblock ::= fonts",
 /* 213 */ "part_description_subblock ::= font",
 /* 214 */ "part_description_subblock ::= style",
 /* 215 */ "part_description_subblock ::= programs",
 /* 216 */ "part_description_subblock ::= program",
 /* 217 */ "part_description_subblock ::= script",
 /* 218 */ "part_description_subblock ::= part_description_property",
 /* 219 */ "part_description_property ::= PROPERTY ID SEMICOLON",
 /* 220 */ "part_description_property ::= PROPERTY ID ID SEMICOLON",
 /* 221 */ "part_description_property ::= PROPERTY ID ID ID ID SEMICOLON",
 /* 222 */ "part_description_subblock ::= part_description_params",
 /* 223 */ "part_description_params ::= part_description_params_start part_description_params_block",
 /* 224 */ "part_description_params_start ::= PARAMS",
 /* 225 */ "part_description_params_block ::= LB part_description_params_subblocks RB",
 /* 226 */ "part_description_params_block ::= LB RB",
 /* 227 */ "part_description_params_block ::= DOT part_description_params_subblock",
 /* 228 */ "part_description_params_subblocks ::= part_description_params_subblocks part_description_params_subblock",
 /* 229 */ "part_description_params_subblocks ::= part_description_params_subblock",
 /* 230 */ "part_description_params_subblock ::= part_description_param",
 /* 231 */ "part_description_param ::= PROPERTY ID ID SEMICOLON",
 /* 232 */ "part_description_subblock ::= part_description_perspective",
 /* 233 */ "part_description_perspective ::= part_description_perspective_start part_description_perspective_block",
 /* 234 */ "part_description_perspective_start ::= PERSPECTIVE",
 /* 235 */ "part_description_perspective_block ::= LB part_description_perspective_subblocks RB",
 /* 236 */ "part_description_perspective_block ::= LB RB",
 /* 237 */ "part_description_perspective_block ::= DOT part_description_perspective_subblock",
 /* 238 */ "part_description_perspective_subblocks ::= part_description_perspective_subblocks part_description_perspective_subblock",
 /* 239 */ "part_description_perspective_subblocks ::= part_description_perspective_subblock",
 /* 240 */ "part_description_perspective_subblock ::= PROPERTY ID SEMICOLON",
 /* 241 */ "part_description_subblock ::= part_description_image",
 /* 242 */ "part_description_image ::= part_description_image_start part_description_image_block",
 /* 243 */ "part_description_image_start ::= doc_comment IMAGE",
 /* 244 */ "part_description_image_block ::= LB part_description_image_subblocks RB",
 /* 245 */ "part_description_image_block ::= LB RB",
 /* 246 */ "part_description_image_block ::= DOT part_description_image_subblock",
 /* 247 */ "part_description_image_subblocks ::= part_description_image_subblocks part_description_image_subblock",
 /* 248 */ "part_description_image_subblocks ::= part_description_image_subblock",
 /* 249 */ "part_description_image_subblock ::= PROPERTY ID SEMICOLON",
 /* 250 */ "part_description_image_subblock ::= PROPERTY ID ID ID ID SEMICOLON",
 /* 251 */ "part_description_image_subblock ::= images_image",
 /* 252 */ "part_description_image_subblock ::= set",
 /* 253 */ "part_description_image_subblock ::= images",
 /* 254 */ "part_description_subblock ::= part_description_fill",
 /* 255 */ "part_description_fill ::= part_description_fill_start part_description_fill_block",
 /* 256 */ "part_description_fill_start ::= FILL",
 /* 257 */ "part_description_fill_block ::= LB part_description_fill_subblocks RB",
 /* 258 */ "part_description_fill_block ::= LB RB",
 /* 259 */ "part_description_fill_block ::= DOT part_description_fill_subblock",
 /* 260 */ "part_description_fill_subblocks ::= part_description_fill_subblocks part_description_fill_subblock",
 /* 261 */ "part_description_fill_subblocks ::= part_description_fill_subblock",
 /* 262 */ "part_description_fill_subblock ::= PROPERTY ID SEMICOLON",
 /* 263 */ "part_description_fill_subblock ::= ORIGIN LB part_description_fill_subblock_origin_subblocks RB",
 /* 264 */ "part_description_fill_subblock ::= ORIGIN LB RB",
 /* 265 */ "part_description_fill_subblock ::= ORIGIN DOT part_description_fill_subblock_origin_subblock",
 /* 266 */ "part_description_fill_subblock_origin_subblocks ::= part_description_fill_subblock_origin_subblocks part_description_fill_subblock_origin_subblock",
 /* 267 */ "part_description_fill_subblock_origin_subblocks ::= part_description_fill_subblock_origin_subblock",
 /* 268 */ "part_description_fill_subblock_origin_subblock ::= PROPERTY ID ID SEMICOLON",
 /* 269 */ "part_description_fill_subblock ::= SIZE LB part_description_fill_subblock_size_subblocks RB",
 /* 270 */ "part_description_fill_subblock ::= SIZE LB RB",
 /* 271 */ "part_description_fill_subblock ::= SIZE DOT part_description_fill_subblock_size_subblock",
 /* 272 */ "part_description_fill_subblock_size_subblocks ::= part_description_fill_subblock_size_subblocks part_description_fill_subblock_size_subblock",
 /* 273 */ "part_description_fill_subblock_size_subblocks ::= part_description_fill_subblock_size_subblock",
 /* 274 */ "part_description_fill_subblock_size_subblock ::= PROPERTY ID ID SEMICOLON",
 /* 275 */ "part_description_subblock ::= part_description_text",
 /* 276 */ "part_description_text ::= part_description_text_start part_description_text_block",
 /* 277 */ "part_description_text_start ::= TEXT",
 /* 278 */ "part_description_text_block ::= LB part_description_text_props RB",
 /* 279 */ "part_description_text_block ::= LB RB",
 /* 280 */ "part_description_text_block ::= DOT part_description_text_prop",
 /* 281 */ "part_description_text_props ::= part_description_text_props part_description_text_prop",
 /* 282 */ "part_description_text_props ::= part_description_text_prop",
 /* 283 */ "part_description_text_prop ::= PROPERTY ID ID ID SEMICOLON",
 /* 284 */ "part_description_text_prop ::= PROPERTY ID ID SEMICOLON",
 /* 285 */ "part_description_text_prop ::= PROPERTY ID SEMICOLON",
 /* 286 */ "part_description_subblock ::= part_description_box",
 /* 287 */ "part_description_box ::= part_description_box_start part_description_box_block",
 /* 288 */ "part_description_box_start ::= BOX",
 /* 289 */ "part_description_box_block ::= LB part_description_box_props RB",
 /* 290 */ "part_description_box_block ::= LB RB",
 /* 291 */ "part_description_box_block ::= DOT part_description_box_prop",
 /* 292 */ "part_description_box_props ::= part_description_box_props part_description_box_prop",
 /* 293 */ "part_description_box_props ::= part_description_box_prop",
 /* 294 */ "part_description_box_prop ::= PROPERTY ID ID SEMICOLON",
 /* 295 */ "part_description_box_prop ::= PROPERTY ID SEMICOLON",
 /* 296 */ "part_description_subblock ::= part_description_map",
 /* 297 */ "part_description_map ::= part_description_map_start part_description_map_block",
 /* 298 */ "part_description_map_start ::= MAP",
 /* 299 */ "part_description_map_block ::= LB part_description_map_props RB",
 /* 300 */ "part_description_map_block ::= LB RB",
 /* 301 */ "part_description_map_block ::= DOT part_description_map_prop",
 /* 302 */ "part_description_map_props ::= part_description_map_props part_description_map_prop",
 /* 303 */ "part_description_map_props ::= part_description_map_prop",
 /* 304 */ "part_description_map_prop ::= PROPERTY ID SEMICOLON",
 /* 305 */ "part_description_map_prop ::= part_description_map_rotation",
 /* 306 */ "part_description_map_rotation ::= part_description_map_rotation_start part_description_map_rotation_block",
 /* 307 */ "part_description_map_rotation_start ::= ROTATION",
 /* 308 */ "part_description_map_rotation_block ::= LB part_description_map_rotation_subblocks RB",
 /* 309 */ "part_description_map_rotation_block ::= LB RB",
 /* 310 */ "part_description_map_rotation_block ::= DOT part_description_map_rotation_subblock",
 /* 311 */ "part_description_map_rotation_subblocks ::= part_description_map_rotation_subblocks part_description_map_rotation_subblock",
 /* 312 */ "part_description_map_rotation_subblocks ::= part_description_map_rotation_subblock",
 /* 313 */ "part_description_map_rotation_subblock ::= PROPERTY ID SEMICOLON",
 /* 314 */ "part_description_subblock ::= part_description_relone",
 /* 315 */ "part_description_subblock ::= part_description_reltwo",
 /* 316 */ "part_description_relone ::= part_description_relone_start part_description_rel_props",
 /* 317 */ "part_description_reltwo ::= part_description_reltwo_start part_description_rel_props",
 /* 318 */ "part_description_relone_start ::= REL1",
 /* 319 */ "part_description_reltwo_start ::= REL2",
 /* 320 */ "part_description_rel_props ::= LB relative_properties RB",
 /* 321 */ "part_description_rel_props ::= LB RB",
 /* 322 */ "part_description_rel_props ::= DOT relative_property",
 /* 323 */ "relative_properties ::= relative_properties relative_property",
 /* 324 */ "relative_properties ::= relative_property",
 /* 325 */ "relative_property ::= PROPERTY ID ID SEMICOLON",
 /* 326 */ "relative_property ::= PROPERTY ID SEMICOLON",
 /* 327 */ "part_subblock ::= part_property",
 /* 328 */ "part_property ::= PROPERTY ID SEMICOLON",
};
#endif /* NDEBUG */


#if YYSTACKDEPTH<=0
/*
** Try to increase the size of the parser stack.
*/
static void yyGrowStack(yyParser *p){
  int newSize;
  yyStackEntry *pNew;

  newSize = p->yystksz*2 + 100;
  pNew = realloc(p->yystack, newSize*sizeof(pNew[0]));
  if( pNew ){
    p->yystack = pNew;
    p->yystksz = newSize;
#ifndef NDEBUG
    if( yyTraceFILE ){
      fprintf(yyTraceFILE,"%sStack grows to %d entries!\n",
              yyTracePrompt, p->yystksz);
    }
#endif
  }
}
#endif

/* 
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to edje_parser_ and edje_parser_Free.
*/
void *edje_parser_Alloc(void *(*mallocProc)(size_t)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (size_t)sizeof(yyParser) );
  if( pParser ){
    pParser->yyidx = -1;
#if YYSTACKDEPTH<=0
    yyGrowStack(pParser);
#endif
  }
  return pParser;
}

/* The following function deletes the value associated with a
** symbol.  The symbol can be either a terminal or nonterminal.
** "yymajor" is the symbol code, and "yypminor" is a pointer to
** the value.
*/
static void yy_destructor(YYCODETYPE yymajor, YYMINORTYPE *yypminor){
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is 
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are not used
    ** inside the C code.
    */
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
#line 22 "edje_parser.y"
{edje_parser_token_free((yypminor->yy0));}
#line 7739 "edje_parser.c"
      break;
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
**
** Return the major token number for the symbol popped.
*/
static int yy_pop_parser_stack(yyParser *pParser){
  YYCODETYPE yymajor;
  yyStackEntry *yytos = &pParser->yystack[pParser->yyidx];

  if( pParser->yyidx<0 ) return 0;
#ifndef NDEBUG
  if( yyTraceFILE && pParser->yyidx>=0 ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yymajor = yytos->major;
  yy_destructor( yymajor, &yytos->minor);
  pParser->yyidx--;
  return yymajor;
}

/* 
** Deallocate and destroy a parser.  Destructors are all called for
** all stack elements before shutting the parser down.
**
** Inputs:
** <ul>
** <li>  A pointer to the parser.  This should be a pointer
**       obtained from edje_parser_Alloc.
** <li>  A pointer to a function used to reclaim memory obtained
**       from malloc.
** </ul>
*/
void edje_parser_Free(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
  yyParser *pParser = (yyParser*)p;
  if( pParser==0 ) return;
  while( pParser->yyidx>=0 ) yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH<=0
  free(pParser->yystack);
#endif
  (*freeProc)((void*)pParser);
}

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;
 
  if( stateno>YY_SHIFT_MAX || (i = yy_shift_ofst[stateno])==YY_SHIFT_USE_DFLT ){
    return yy_default[stateno];
  }
  if( iLookAhead==YYNOCODE ){
    return YY_NO_ACTION;
  }
  i += iLookAhead;
  if( i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
    if( iLookAhead>0 ){
#ifdef YYFALLBACK
      int iFallback;            /* Fallback token */
      if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
             && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
             yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
        }
#endif
        return yy_find_shift_action(pParser, iFallback);
      }
#endif
#ifdef YYWILDCARD
      {
        int j = i - iLookAhead + YYWILDCARD;
        if( j>=0 && j<YY_SZ_ACTTAB && yy_lookahead[j]==YYWILDCARD ){
#ifndef NDEBUG
          if( yyTraceFILE ){
            fprintf(yyTraceFILE, "%sWILDCARD %s => %s\n",
               yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[YYWILDCARD]);
          }
#endif /* NDEBUG */
          return yy_action[j];
        }
      }
#endif /* YYWILDCARD */
    }
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_reduce_action(
  int stateno,              /* Current state number */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
  /* int stateno = pParser->yystack[pParser->yyidx].stateno; */
 
  if( stateno>YY_REDUCE_MAX ||
      (i = yy_reduce_ofst[stateno])==YY_REDUCE_USE_DFLT ){
    return yy_default[stateno];
  }
  if( iLookAhead==YYNOCODE ){
    return YY_NO_ACTION;
  }
  i += iLookAhead;
  if( i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** The following routine is called if the stack overflows.
*/
static void yyStackOverflow(yyParser *yypParser, YYMINORTYPE *yypMinor){
   edje_parser_ARG_FETCH;
   yypParser->yyidx--;
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
#line 26 "edje_parser.y"
eina_stringshare_replace(&ep->error, "Parser stack overflow!\n");
#line 7901 "edje_parser.c"
   edje_parser_ARG_STORE; /* Suppress warning about unused %extra_argument var */
}

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  YYMINORTYPE *yypMinor         /* Pointer ot the minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yyidx++;
#if YYSTACKDEPTH>0 
  if( yypParser->yyidx>=YYSTACKDEPTH ){
    yyStackOverflow(yypParser, yypMinor);
    return;
  }
#else
  if( yypParser->yyidx>=yypParser->yystksz ){
    yyGrowStack(yypParser);
    if( yypParser->yyidx>=yypParser->yystksz ){
      yyStackOverflow(yypParser, yypMinor);
      return;
    }
  }
#endif
  yytos = &yypParser->yystack[yypParser->yyidx];
  yytos->stateno = yyNewState;
  yytos->major = yyMajor;
  yytos->minor = *yypMinor;
#ifndef NDEBUG
  if( yyTraceFILE && yypParser->yyidx>0 ){
    int i;
    fprintf(yyTraceFILE,"%sShift %d\n",yyTracePrompt,yyNewState);
    fprintf(yyTraceFILE,"%sStack:",yyTracePrompt);
    for(i=1; i<=yypParser->yyidx; i++)
      fprintf(yyTraceFILE," %s",yyTokenName[yypParser->yystack[i].major]);
    fprintf(yyTraceFILE,"\n");
  }
#endif
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct {
  YYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} yyRuleInfo[] = {
  { 41, 1 },
  { 42, 1 },
  { 42, 2 },
  { 43, 1 },
  { 43, 1 },
  { 43, 1 },
  { 43, 1 },
  { 43, 1 },
  { 43, 1 },
  { 43, 1 },
  { 51, 0 },
  { 51, 1 },
  { 52, 1 },
  { 44, 2 },
  { 53, 2 },
  { 54, 3 },
  { 54, 2 },
  { 55, 2 },
  { 55, 1 },
  { 56, 1 },
  { 56, 1 },
  { 56, 1 },
  { 56, 1 },
  { 56, 1 },
  { 56, 1 },
  { 56, 1 },
  { 48, 2 },
  { 59, 2 },
  { 60, 3 },
  { 60, 2 },
  { 60, 2 },
  { 61, 2 },
  { 61, 1 },
  { 62, 4 },
  { 49, 4 },
  { 49, 3 },
  { 49, 3 },
  { 63, 2 },
  { 63, 1 },
  { 64, 1 },
  { 65, 2 },
  { 66, 2 },
  { 67, 3 },
  { 67, 2 },
  { 67, 2 },
  { 68, 2 },
  { 68, 1 },
  { 69, 3 },
  { 69, 4 },
  { 58, 2 },
  { 70, 2 },
  { 71, 3 },
  { 71, 2 },
  { 71, 2 },
  { 72, 2 },
  { 72, 1 },
  { 73, 3 },
  { 73, 1 },
  { 74, 2 },
  { 75, 2 },
  { 76, 3 },
  { 76, 2 },
  { 76, 2 },
  { 77, 2 },
  { 77, 1 },
  { 78, 1 },
  { 78, 3 },
  { 78, 6 },
  { 47, 2 },
  { 80, 2 },
  { 81, 3 },
  { 81, 2 },
  { 81, 2 },
  { 82, 2 },
  { 82, 1 },
  { 83, 3 },
  { 46, 2 },
  { 84, 2 },
  { 85, 3 },
  { 85, 2 },
  { 85, 2 },
  { 86, 2 },
  { 86, 1 },
  { 87, 1 },
  { 88, 5 },
  { 50, 2 },
  { 89, 2 },
  { 90, 3 },
  { 90, 2 },
  { 91, 3 },
  { 91, 2 },
  { 92, 2 },
  { 93, 3 },
  { 93, 2 },
  { 94, 2 },
  { 94, 1 },
  { 95, 3 },
  { 95, 6 },
  { 96, 2 },
  { 97, 2 },
  { 98, 3 },
  { 98, 2 },
  { 98, 2 },
  { 99, 2 },
  { 99, 1 },
  { 100, 1 },
  { 100, 3 },
  { 100, 1 },
  { 100, 1 },
  { 100, 1 },
  { 100, 1 },
  { 100, 1 },
  { 101, 2 },
  { 102, 2 },
  { 103, 3 },
  { 103, 2 },
  { 103, 2 },
  { 104, 2 },
  { 104, 1 },
  { 105, 1 },
  { 105, 3 },
  { 105, 4 },
  { 105, 5 },
  { 105, 6 },
  { 105, 7 },
  { 45, 2 },
  { 106, 2 },
  { 107, 3 },
  { 107, 2 },
  { 107, 2 },
  { 108, 2 },
  { 108, 1 },
  { 109, 1 },
  { 109, 1 },
  { 109, 3 },
  { 79, 4 },
  { 79, 5 },
  { 57, 2 },
  { 110, 2 },
  { 111, 3 },
  { 111, 2 },
  { 111, 2 },
  { 112, 2 },
  { 112, 1 },
  { 113, 1 },
  { 113, 1 },
  { 113, 1 },
  { 113, 1 },
  { 113, 1 },
  { 113, 1 },
  { 113, 1 },
  { 113, 1 },
  { 113, 1 },
  { 113, 1 },
  { 113, 1 },
  { 113, 1 },
  { 113, 1 },
  { 115, 3 },
  { 115, 4 },
  { 114, 2 },
  { 116, 2 },
  { 117, 3 },
  { 117, 2 },
  { 118, 2 },
  { 118, 1 },
  { 119, 1 },
  { 119, 1 },
  { 119, 1 },
  { 119, 1 },
  { 119, 1 },
  { 119, 1 },
  { 119, 1 },
  { 119, 1 },
  { 119, 1 },
  { 119, 1 },
  { 119, 1 },
  { 121, 4 },
  { 120, 2 },
  { 122, 2 },
  { 123, 3 },
  { 123, 2 },
  { 124, 2 },
  { 124, 1 },
  { 125, 1 },
  { 126, 2 },
  { 127, 1 },
  { 128, 3 },
  { 128, 2 },
  { 128, 2 },
  { 129, 2 },
  { 129, 1 },
  { 130, 3 },
  { 130, 5 },
  { 125, 1 },
  { 125, 1 },
  { 125, 1 },
  { 125, 1 },
  { 125, 1 },
  { 125, 1 },
  { 125, 1 },
  { 125, 1 },
  { 125, 1 },
  { 125, 1 },
  { 131, 2 },
  { 132, 2 },
  { 133, 3 },
  { 133, 2 },
  { 133, 2 },
  { 134, 2 },
  { 134, 1 },
  { 135, 1 },
  { 135, 1 },
  { 135, 1 },
  { 135, 1 },
  { 135, 1 },
  { 135, 1 },
  { 135, 1 },
  { 135, 1 },
  { 135, 1 },
  { 136, 3 },
  { 136, 4 },
  { 136, 6 },
  { 135, 1 },
  { 137, 2 },
  { 138, 1 },
  { 139, 3 },
  { 139, 2 },
  { 139, 2 },
  { 140, 2 },
  { 140, 1 },
  { 141, 1 },
  { 142, 4 },
  { 135, 1 },
  { 143, 2 },
  { 144, 1 },
  { 145, 3 },
  { 145, 2 },
  { 145, 2 },
  { 146, 2 },
  { 146, 1 },
  { 147, 3 },
  { 135, 1 },
  { 148, 2 },
  { 149, 2 },
  { 150, 3 },
  { 150, 2 },
  { 150, 2 },
  { 151, 2 },
  { 151, 1 },
  { 152, 3 },
  { 152, 6 },
  { 152, 1 },
  { 152, 1 },
  { 152, 1 },
  { 135, 1 },
  { 153, 2 },
  { 154, 1 },
  { 155, 3 },
  { 155, 2 },
  { 155, 2 },
  { 156, 2 },
  { 156, 1 },
  { 157, 3 },
  { 157, 4 },
  { 157, 3 },
  { 157, 3 },
  { 158, 2 },
  { 158, 1 },
  { 159, 4 },
  { 157, 4 },
  { 157, 3 },
  { 157, 3 },
  { 160, 2 },
  { 160, 1 },
  { 161, 4 },
  { 135, 1 },
  { 162, 2 },
  { 163, 1 },
  { 164, 3 },
  { 164, 2 },
  { 164, 2 },
  { 165, 2 },
  { 165, 1 },
  { 166, 5 },
  { 166, 4 },
  { 166, 3 },
  { 135, 1 },
  { 167, 2 },
  { 168, 1 },
  { 169, 3 },
  { 169, 2 },
  { 169, 2 },
  { 170, 2 },
  { 170, 1 },
  { 171, 4 },
  { 171, 3 },
  { 135, 1 },
  { 172, 2 },
  { 173, 1 },
  { 174, 3 },
  { 174, 2 },
  { 174, 2 },
  { 175, 2 },
  { 175, 1 },
  { 176, 3 },
  { 176, 1 },
  { 177, 2 },
  { 178, 1 },
  { 179, 3 },
  { 179, 2 },
  { 179, 2 },
  { 180, 2 },
  { 180, 1 },
  { 181, 3 },
  { 135, 1 },
  { 135, 1 },
  { 182, 2 },
  { 183, 2 },
  { 184, 1 },
  { 186, 1 },
  { 185, 3 },
  { 185, 2 },
  { 185, 2 },
  { 187, 2 },
  { 187, 1 },
  { 188, 4 },
  { 188, 3 },
  { 125, 1 },
  { 189, 3 },
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  int yyruleno                 /* Number of the rule by which to reduce */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  YYMINORTYPE yygotominor;        /* The LHS of the rule reduced */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  edje_parser_ARG_FETCH;
  yymsp = &yypParser->yystack[yypParser->yyidx];
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno>=0 
        && yyruleno<(int)(sizeof(yyRuleName)/sizeof(yyRuleName[0])) ){
    fprintf(yyTraceFILE, "%sReduce [%s].\n", yyTracePrompt,
      yyRuleName[yyruleno]);
  }
#endif /* NDEBUG */

  /* Silence complaints from purify about yygotominor being uninitialized
  ** in some cases when it is copied into the stack after the following
  ** switch.  yygotominor is uninitialized when a rule reduces that does
  ** not set the value of its left-hand side nonterminal.  Leaving the
  ** value of the nonterminal uninitialized is utterly harmless as long
  ** as the value is never used.  So really the only thing this code
  ** accomplishes is to quieten purify.  
  **
  ** 2007-01-16:  The wireshark project (www.wireshark.org) reports that
  ** without this code, their parser segfaults.  I'm not sure what there
  ** parser is doing to make this happen.  This is the second bug report
  ** from wireshark this week.  Clearly they are stressing Lemon in ways
  ** that it has not been previously stressed...  (SQLite ticket #2172)
  */
  memset(&yygotominor, 0, sizeof(yygotominor));


  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
      case 0:
      case 1:
      case 2:
      case 13:
      case 17:
      case 18:
      case 26:
      case 31:
      case 32:
      case 37:
      case 38:
      case 39:
      case 40:
      case 45:
      case 46:
      case 49:
      case 54:
      case 55:
      case 57:
      case 58:
      case 63:
      case 64:
      case 65:
      case 68:
      case 73:
      case 74:
      case 76:
      case 81:
      case 82:
      case 85:
      case 89:
      case 90:
      case 94:
      case 95:
      case 98:
      case 103:
      case 104:
      case 112:
      case 117:
      case 118:
      case 125:
      case 130:
      case 131:
      case 137:
      case 142:
      case 143:
      case 156:
      case 159:
      case 163:
      case 164:
      case 175:
      case 177:
      case 181:
      case 182:
      case 183:
      case 184:
      case 189:
      case 190:
      case 203:
      case 208:
      case 209:
      case 218:
      case 222:
      case 223:
      case 228:
      case 229:
      case 230:
      case 232:
      case 233:
      case 238:
      case 239:
      case 241:
      case 242:
      case 247:
      case 248:
      case 254:
      case 255:
      case 260:
      case 261:
      case 266:
      case 267:
      case 272:
      case 273:
      case 275:
      case 276:
      case 281:
      case 282:
      case 286:
      case 287:
      case 292:
      case 293:
      case 296:
      case 297:
      case 302:
      case 303:
      case 305:
      case 306:
      case 311:
      case 312:
      case 316:
      case 317:
      case 323:
      case 324:
      case 327:
#line 6736 "edje_parser.y"
{
}
#line 8442 "edje_parser.c"
        break;
      case 3:
#line 6741 "edje_parser.y"
{
   EDJE_CREATE;
   EDJE_APPEND(collections, COLLECTION, Edje_Collection);
   COLLECTION = NULL;
}
#line 8451 "edje_parser.c"
        break;
      case 4:
#line 6747 "edje_parser.y"
{
   EDJE_CREATE;
   EDJE_APPEND(imageses, IMAGES, Edje_Images);
   IMAGES = NULL;
}
#line 8460 "edje_parser.c"
        break;
      case 5:
#line 6753 "edje_parser.y"
{
   EDJE_CREATE;
   EDJE_APPEND(fontses, FONTS, Edje_Fonts);
   FONTS = NULL;
}
#line 8469 "edje_parser.c"
        break;
      case 6:
#line 6759 "edje_parser.y"
{
   EDJE_CREATE;
   EDJE_APPEND(externals, EXTERNAL, Edje_External);
   EXTERNAL = NULL;
}
#line 8478 "edje_parser.c"
        break;
      case 7:
#line 6765 "edje_parser.y"
{
   EDJE_CREATE;
   EDJE_APPEND(data, DATA, Edje_Data);
   DATA = NULL;
}
#line 8487 "edje_parser.c"
        break;
      case 8:
#line 6771 "edje_parser.y"
{
   EDJE_CREATE;
   EDJE_APPEND(styles, STYLE, Edje_Style);
   STYLE = NULL;
}
#line 8496 "edje_parser.c"
        break;
      case 9:
#line 6777 "edje_parser.y"
{
   EDJE_CREATE;
   EDJE_APPEND(color_classes, COLOR_CLASS, Edje_Color_Class);
   COLOR_CLASS = NULL;
}
#line 8505 "edje_parser.c"
        break;
      case 10:
#line 6784 "edje_parser.y"
{yygotominor.yy144 = NULL;}
#line 8510 "edje_parser.c"
        break;
      case 11:
#line 6786 "edje_parser.y"
{
   DBG("Comment: %s", yymsp[0].minor.yy0->text);
   yygotominor.yy144 = yymsp[0].minor.yy0->text;
   yymsp[0].minor.yy0->text = NULL;
   edje_parser_token_free(yymsp[0].minor.yy0);
}
#line 8520 "edje_parser.c"
        break;
      case 12:
#line 6795 "edje_parser.y"
{
   const char *p, *q;

   p = yymsp[0].minor.yy0->text + 7;
   while (p && (isspace(*p) || (*p == '{')))
     p++;
   q = yymsp[0].minor.yy0->text + strlen(yymsp[0].minor.yy0->text) - 1;
   while (q && (isspace(*q) || (*q == '}')))
     q--;

   yygotominor.yy144 = eina_stringshare_add_length(p, q - p + 1);
   DBG("Script:\n%s", yygotominor.yy144);
   edje_parser_token_free(yymsp[0].minor.yy0);
}
#line 8538 "edje_parser.c"
        break;
      case 14:
#line 6812 "edje_parser.y"
{
   Edje_Collection *c;
   
   EDJE_CREATE;
   c = edje_collection_new();
   PARSER_APPEND(COLLECTION, c, Edje_Collection);
   DBG("Collection: %p", c);
   c->doc = yymsp[-1].minor.yy144;
  yy_destructor(3,&yymsp[0].minor);
}
#line 8552 "edje_parser.c"
        break;
      case 15:
      case 28:
      case 42:
      case 51:
      case 60:
      case 70:
      case 78:
      case 87:
      case 92:
      case 100:
      case 114:
      case 127:
      case 139:
      case 161:
      case 179:
      case 186:
      case 205:
      case 225:
      case 235:
      case 244:
      case 257:
      case 278:
      case 289:
      case 299:
      case 308:
      case 320:
#line 6821 "edje_parser.y"
{
  yy_destructor(4,&yymsp[-2].minor);
  yy_destructor(5,&yymsp[0].minor);
}
#line 8585 "edje_parser.c"
        break;
      case 16:
      case 29:
      case 43:
      case 52:
      case 61:
      case 71:
      case 79:
      case 88:
      case 93:
      case 101:
      case 115:
      case 128:
      case 140:
      case 162:
      case 180:
      case 187:
      case 206:
      case 226:
      case 236:
      case 245:
      case 258:
      case 279:
      case 290:
      case 300:
      case 309:
      case 321:
#line 6822 "edje_parser.y"
{
  yy_destructor(4,&yymsp[-1].minor);
  yy_destructor(5,&yymsp[0].minor);
}
#line 8618 "edje_parser.c"
        break;
      case 19:
#line 6827 "edje_parser.y"
{
   Edje_Collection *c;
   c = PARSER_CURRENT(COLLECTION, Edje_Collection);
   EDJE_STRUCT_APPEND(c, groups, GROUP, Edje_Group);
   GROUP = NULL;
}
#line 8628 "edje_parser.c"
        break;
      case 20:
#line 6834 "edje_parser.y"
{
   Edje_Collection *c;
   c = PARSER_CURRENT(COLLECTION, Edje_Collection);
   EDJE_STRUCT_APPEND(c, sets, SET, Edje_Set);
   SET = NULL;
}
#line 8638 "edje_parser.c"
        break;
      case 21:
#line 6841 "edje_parser.y"
{
   Edje_Collection *c;
   c = PARSER_CURRENT(COLLECTION, Edje_Collection);
   EDJE_STRUCT_APPEND(c, color_classes, COLOR_CLASS, Edje_Color_Class);
   COLOR_CLASS = NULL;
}
#line 8648 "edje_parser.c"
        break;
      case 22:
#line 6848 "edje_parser.y"
{
   Edje_Collection *c;
   c = PARSER_CURRENT(COLLECTION, Edje_Collection);
   EDJE_STRUCT_APPEND(c, imageses, IMAGES, Edje_Images);
   IMAGES = NULL;
}
#line 8658 "edje_parser.c"
        break;
      case 23:
#line 6855 "edje_parser.y"
{
   Edje_Collection *c;
   c = PARSER_CURRENT(COLLECTION, Edje_Collection);
   EDJE_STRUCT_APPEND(c, externals, EXTERNAL, Edje_External);
   EXTERNAL = NULL;
}
#line 8668 "edje_parser.c"
        break;
      case 24:
#line 6862 "edje_parser.y"
{
   Edje_Collection *c;
   c = PARSER_CURRENT(COLLECTION, Edje_Collection);
   EDJE_STRUCT_APPEND(c, styles, STYLE, Edje_Style);
   STYLE = NULL;
}
#line 8678 "edje_parser.c"
        break;
      case 25:
#line 6869 "edje_parser.y"
{
   Edje_Collection *c;
   c = PARSER_CURRENT(COLLECTION, Edje_Collection);
   EDJE_STRUCT_APPEND(c, fontses, FONTS, Edje_Fonts);
   FONTS = NULL;
}
#line 8688 "edje_parser.c"
        break;
      case 27:
#line 6878 "edje_parser.y"
{
  yy_destructor(6,&yymsp[0].minor);
}
#line 8695 "edje_parser.c"
        break;
      case 30:
      case 44:
      case 53:
      case 62:
      case 72:
      case 80:
      case 102:
      case 116:
      case 129:
      case 141:
      case 188:
      case 207:
      case 227:
      case 237:
      case 246:
      case 259:
      case 280:
      case 291:
      case 301:
      case 310:
      case 322:
#line 6881 "edje_parser.y"
{
  yy_destructor(7,&yymsp[-1].minor);
}
#line 8722 "edje_parser.c"
        break;
      case 33:
#line 6886 "edje_parser.y"
{
   Edje_Data *d;

   if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "item"))
     {
        d = edje_data_new();
        eina_hash_add(d->items, STRDUPA_NOQUOTES(yymsp[-2].minor.yy0->text), STRINGSHARE_ADD_NOQUOTES(yymsp[-1].minor.yy0->text));
        PARSER_APPEND(DATA, d, Edje_Data);
     }
   else if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "file"))
     {
        d = edje_data_new();
        eina_hash_add(d->files, STRDUPA_NOQUOTES(yymsp[-2].minor.yy0->text), STRINGSHARE_ADD_NOQUOTES(yymsp[-1].minor.yy0->text));
        PARSER_APPEND(DATA, d, Edje_Data);
     }
   else ERROR_SYNTAX(yymsp[-3].minor.yy0);

   edje_parser_token_free(yymsp[-3].minor.yy0);
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 8748 "edje_parser.c"
        break;
      case 34:
#line 6909 "edje_parser.y"
{
  yy_destructor(11,&yymsp[-3].minor);
  yy_destructor(4,&yymsp[-2].minor);
  yy_destructor(5,&yymsp[0].minor);
}
#line 8757 "edje_parser.c"
        break;
      case 35:
#line 6910 "edje_parser.y"
{
  yy_destructor(11,&yymsp[-2].minor);
  yy_destructor(4,&yymsp[-1].minor);
  yy_destructor(5,&yymsp[0].minor);
}
#line 8766 "edje_parser.c"
        break;
      case 36:
#line 6911 "edje_parser.y"
{
  yy_destructor(11,&yymsp[-2].minor);
  yy_destructor(7,&yymsp[-1].minor);
}
#line 8774 "edje_parser.c"
        break;
      case 41:
#line 6918 "edje_parser.y"
{
   Edje_Style *s;

   EDJE_CREATE;
   s = edje_style_new();
   PARSER_APPEND(STYLE, s, Edje_Style);
   DBG("Style: %p", s);
   s->doc = yymsp[-1].minor.yy144;
  yy_destructor(12,&yymsp[0].minor);
}
#line 8788 "edje_parser.c"
        break;
      case 47:
#line 6933 "edje_parser.y"
{
   Edje_Style *s;

   s = PARSER_CURRENT(STYLE, Edje_Style);
   if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "name") && (!s->name))
     STRINGSHARE_REPLACE_NOQUOTES(s->name, yymsp[-1].minor.yy0->text);
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "base") && (!s->base))
     STRINGSHARE_REPLACE_NOQUOTES(s->base, yymsp[-1].minor.yy0->text);
   else ERROR_SYNTAX(yymsp[-2].minor.yy0);

   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 8806 "edje_parser.c"
        break;
      case 48:
#line 6947 "edje_parser.y"
{
   Edje_Style *s;

   s = PARSER_CURRENT(STYLE, Edje_Style);
   if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "tag"))
     eina_hash_add(s->tags, STRDUPA_NOQUOTES(yymsp[-2].minor.yy0->text),
       STRINGSHARE_ADD_NOQUOTES(yymsp[-1].minor.yy0->text));
   else ERROR_SYNTAX(yymsp[-3].minor.yy0);

   edje_parser_token_free(yymsp[-3].minor.yy0);
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 8824 "edje_parser.c"
        break;
      case 50:
#line 6964 "edje_parser.y"
{
   Edje_Set *s;

   EDJE_CREATE;
   s = edje_set_new();
   PARSER_APPEND(SET, s, Edje_Set);
   DBG("Set: %p", s);
   s->doc = yymsp[-1].minor.yy144;
  yy_destructor(13,&yymsp[0].minor);
}
#line 8838 "edje_parser.c"
        break;
      case 56:
#line 6980 "edje_parser.y"
{
   Edje_Set *s;

   if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "name"))
     STRINGSHARE_REPLACE_NOQUOTES(s->name, yymsp[-1].minor.yy0->text);
   else ERROR_SYNTAX(yymsp[-2].minor.yy0);

   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 8853 "edje_parser.c"
        break;
      case 59:
#line 6993 "edje_parser.y"
{
   Edje_Set_Image *s;

   s = edje_set_image_new();
   PARSER_APPEND(SET_IMAGE, s, Edje_Set_Image);
   DBG("Set_Image: %p", s);
   s->doc = yymsp[-1].minor.yy144;
  yy_destructor(14,&yymsp[0].minor);
}
#line 8866 "edje_parser.c"
        break;
      case 66:
#line 7008 "edje_parser.y"
{
   if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "image"))
     {
        if (strchr(yymsp[-1].minor.yy0->text, ':')) // invalid filename characters
          ERROR_SYNTAX(yymsp[-1].minor.yy0);
        else
          {
             Edje_Set_Image *s;
             s = PARSER_CURRENT(SET_IMAGE, Edje_Set_Image);
             if ((!s) || (s->filename))
               { /* play nice with Edje_Set objects */
                  s = edje_set_image_new();
                  PARSER_APPEND(SET_IMAGE, s, Edje_Set_Image);
                  DBG("Set image: %p", s);
               }
             STRINGSHARE_REPLACE_NOQUOTES(s->filename, yymsp[-1].minor.yy0->text);
             s->comp = EDJE_COMPRESSION_TYPE_COMP;
          }
     }
   else ERROR_SYNTAX(yymsp[-2].minor.yy0);

   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 8895 "edje_parser.c"
        break;
      case 67:
#line 7033 "edje_parser.y"
{
   Edje_Set_Image *s;
   Edje_Parser_Token *t[4];
   unsigned int i;

   t[0] = yymsp[-4].minor.yy0;
   t[1] = yymsp[-3].minor.yy0;
   t[2] = yymsp[-2].minor.yy0;
   t[3] = yymsp[-1].minor.yy0;

   s = PARSER_CURRENT(SET_IMAGE, Edje_Set_Image);

   for (i = 0; i < 4; i++)
     {
        if (!edje_parser_strtol(t[i]->text, &s->size[i]))
          {
             ERROR_RANGE(t[i]);
             break;
          }
     }
     
   edje_parser_token_free(yymsp[-5].minor.yy0);
   edje_parser_token_free(yymsp[-4].minor.yy0);
   edje_parser_token_free(yymsp[-3].minor.yy0);
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 8927 "edje_parser.c"
        break;
      case 69:
#line 7063 "edje_parser.y"
{
  yy_destructor(15,&yymsp[0].minor);
}
#line 8934 "edje_parser.c"
        break;
      case 75:
#line 7071 "edje_parser.y"
{
   Edje_External *e;

   if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "external"))
     {
        e = edje_external_new();
        STRINGSHARE_REPLACE_NOQUOTES(e->external, yymsp[-1].minor.yy0->text);
        PARSER_APPEND(EXTERNAL, e, Edje_External);
     }
   else ERROR_SYNTAX(yymsp[-2].minor.yy0);

   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 8953 "edje_parser.c"
        break;
      case 77:
#line 7089 "edje_parser.y"
{
   Edje_Fonts *f;

   EDJE_CREATE;
   f = edje_fonts_new();
   PARSER_APPEND(FONTS, f, Edje_Fonts);
   DBG("Fonts: %p", f);
   f->doc = yymsp[-1].minor.yy144;
  yy_destructor(16,&yymsp[0].minor);
}
#line 8967 "edje_parser.c"
        break;
      case 83:
#line 7105 "edje_parser.y"
{
   Edje_Fonts *f;

   f = PARSER_CURRENT(FONTS, Edje_Fonts);
   EDJE_STRUCT_APPEND(f, fonts, FONT, Edje_Font);
   FONT = NULL;
}
#line 8978 "edje_parser.c"
        break;
      case 84:
#line 7115 "edje_parser.y"
{
   Edje_Font *f;

   f = edje_font_new();
   PARSER_APPEND(FONT, f, Edje_Font);
   DBG("Font: %p", f);
   f->doc = yymsp[-4].minor.yy144;

   STRINGSHARE_REPLACE_NOQUOTES(f->name, yymsp[-2].minor.yy0->text);
   STRINGSHARE_REPLACE_NOQUOTES(f->alias, yymsp[-1].minor.yy0->text);

   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(17,&yymsp[-3].minor);
  yy_destructor(10,&yymsp[0].minor);
}
#line 8998 "edje_parser.c"
        break;
      case 86:
#line 7132 "edje_parser.y"
{
  yy_destructor(18,&yymsp[0].minor);
}
#line 9005 "edje_parser.c"
        break;
      case 91:
#line 7139 "edje_parser.y"
{
   Edje_Color_Class *c;

   c = edje_color_class_new();
   PARSER_APPEND(COLOR_CLASS, c, Edje_Color_Class);
   c->doc = yymsp[-1].minor.yy144;
  yy_destructor(19,&yymsp[0].minor);
}
#line 9017 "edje_parser.c"
        break;
      case 96:
#line 7151 "edje_parser.y"
{
   Edje_Color_Class *c;

   c = PARSER_CURRENT(COLOR_CLASS, Edje_Color_Class);
   DBG("Color class name: %s", yymsp[-1].minor.yy0->text);
   c->name = eina_stringshare_add(yymsp[-1].minor.yy0->text);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(8,&yymsp[-2].minor);
  yy_destructor(10,&yymsp[0].minor);
}
#line 9031 "edje_parser.c"
        break;
      case 97:
#line 7160 "edje_parser.y"
{

   Edje_Color_Class *c;
   int *colors[3];
   Edje_Parser_Token *tokens[4];

   c = PARSER_CURRENT(COLOR_CLASS, Edje_Color_Class);

   tokens[0] = yymsp[-4].minor.yy0;
   tokens[1] = yymsp[-3].minor.yy0;
   tokens[2] = yymsp[-2].minor.yy0;
   tokens[3] = yymsp[-1].minor.yy0;
   colors[0] = c->color;
   colors[1] = c->color2;
   colors[2] = c->color3;
   if (!edje_parser_parse_color(ep, yymsp[-5].minor.yy0, (Edje_Parser_Token**)tokens, (int**)colors) && (!ep->error))
     ERROR_SYNTAX(yymsp[-5].minor.yy0);

   edje_parser_token_free(yymsp[-5].minor.yy0);
   edje_parser_token_free(yymsp[-4].minor.yy0);
   edje_parser_token_free(yymsp[-3].minor.yy0);
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 9060 "edje_parser.c"
        break;
      case 99:
#line 7188 "edje_parser.y"
{
   Edje_Programs *p;

   EDJE_CREATE;   
   p = edje_programs_new();
   PARSER_APPEND(PROGRAMS, p, Edje_Programs);
   DBG("Programs: %p", p);
   p->doc = yymsp[-1].minor.yy144;
  yy_destructor(20,&yymsp[0].minor);
}
#line 9074 "edje_parser.c"
        break;
      case 105:
#line 7204 "edje_parser.y"
{
   Edje_Programs *p;

   p = PARSER_CURRENT(PROGRAMS, Edje_Programs);
   EDJE_STRUCT_APPEND(p, programs, PROGRAM, Edje_Program);
   PROGRAM = NULL;
}
#line 9085 "edje_parser.c"
        break;
      case 106:
#line 7212 "edje_parser.y"
{
   if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "image"))
     {
        if (strchr(yymsp[-1].minor.yy0->text, ':')) // invalid filename characters
          ERROR_SYNTAX(yymsp[-1].minor.yy0);
        else
          {
             Edje_Set_Image *s;
             Edje_Programs *p;

             p = PARSER_CURRENT(PROGRAMS, Edje_Programs);
             s = PARSER_CURRENT(SET_IMAGE, Edje_Set_Image);
             if ((!s) || (s->filename))
               { /* play nice with Edje_Set objects */
                  s = edje_set_image_new();
                  PARSER_APPEND(SET_IMAGE, s, Edje_Set_Image);
                  DBG("Set image: %p", s);
               }
             STRINGSHARE_REPLACE_NOQUOTES(s->filename, yymsp[-1].minor.yy0->text);
             s->comp = EDJE_COMPRESSION_TYPE_COMP;
             EDJE_STRUCT_APPEND(p, images, SET_IMAGE, Edje_Set_Image);
             SET_IMAGE = NULL;
          }
     }
   else ERROR_SYNTAX(yymsp[-2].minor.yy0);

   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 9119 "edje_parser.c"
        break;
      case 107:
#line 7242 "edje_parser.y"
{
   Edje_Programs *p;

   p = PARSER_CURRENT(PROGRAMS, Edje_Programs);
   EDJE_STRUCT_APPEND(p, images, SET_IMAGE, Edje_Set_Image);
   SET_IMAGE = NULL;
}
#line 9130 "edje_parser.c"
        break;
      case 108:
#line 7250 "edje_parser.y"
{
   Edje_Programs *p;

   p = PARSER_CURRENT(PROGRAMS, Edje_Programs);
   EDJE_STRUCT_APPEND(p, imageses, IMAGES, Edje_Images);
   IMAGES = NULL;
}
#line 9141 "edje_parser.c"
        break;
      case 109:
#line 7258 "edje_parser.y"
{
   Edje_Programs *p;

   p = PARSER_CURRENT(PROGRAMS, Edje_Programs);
   EDJE_STRUCT_APPEND(p, fonts, FONT, Edje_Font);
   FONT = NULL;
}
#line 9152 "edje_parser.c"
        break;
      case 110:
#line 7266 "edje_parser.y"
{
   Edje_Programs *p;

   p = PARSER_CURRENT(PROGRAMS, Edje_Programs);
   EDJE_STRUCT_APPEND(p, fontses, FONTS, Edje_Fonts);
   FONTS = NULL;
}
#line 9163 "edje_parser.c"
        break;
      case 111:
#line 7274 "edje_parser.y"
{
   Edje_Programs *p;

   p = PARSER_CURRENT(PROGRAMS, Edje_Programs);
   EDJE_STRUCT_APPEND(p, sets, SET, Edje_Set);
   SET = NULL;

}
#line 9175 "edje_parser.c"
        break;
      case 113:
#line 7286 "edje_parser.y"
{
   Edje_Program *p;

   EDJE_CREATE;   
   p = edje_program_new();
   PARSER_APPEND(PROGRAM, p, Edje_Program);
   DBG("Program: %p", p);
   p->doc = yymsp[-1].minor.yy144;
  yy_destructor(21,&yymsp[0].minor);
}
#line 9189 "edje_parser.c"
        break;
      case 119:
#line 7302 "edje_parser.y"
{
   Edje_Program *p;

   p = PARSER_CURRENT(PROGRAM, Edje_Program);
   p->script = yymsp[0].minor.yy144;
}
#line 9199 "edje_parser.c"
        break;
      case 120:
#line 7310 "edje_parser.y"
{
   Edje_Program *p;

   p = PARSER_CURRENT(PROGRAM, Edje_Program);
   if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "name"))
     STRINGSHARE_REPLACE_NOQUOTES(p->name, yymsp[-1].minor.yy0->text);
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "signal"))
     STRINGSHARE_REPLACE_NOQUOTES(p->signal, yymsp[-1].minor.yy0->text);
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "source"))
     STRINGSHARE_REPLACE_NOQUOTES(p->source, yymsp[-1].minor.yy0->text);
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "target"))
     p->targets = eina_list_append(p->targets, STRINGSHARE_ADD_NOQUOTES(yymsp[-1].minor.yy0->text));
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "after"))
     p->targets = eina_list_append(p->after, STRINGSHARE_ADD_NOQUOTES(yymsp[-1].minor.yy0->text));
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "action") && (!p->action.type))
     {
         if (!strcmp(yymsp[-1].minor.yy0->text, "ACTION_STOP"))
           p->action.type = EDJE_PROGRAM_ACTION_ACTION_STOP;
         else if (!strcmp(yymsp[-1].minor.yy0->text, "FOCUS_SET"))
           p->action.type = EDJE_PROGRAM_ACTION_FOCUS_SET;
         else if (!strcmp(yymsp[-1].minor.yy0->text, "FOCUS_OBJECT"))
           p->action.type = EDJE_PROGRAM_ACTION_FOCUS_OBJECT;
         else ERROR_SYNTAX(yymsp[-1].minor.yy0);
     }
   else ERROR_SYNTAX(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 9232 "edje_parser.c"
        break;
      case 121:
#line 7339 "edje_parser.y"
{
   Edje_Program *p;

   p = PARSER_CURRENT(PROGRAM, Edje_Program);
   if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "api"))
     {
        STRINGSHARE_REPLACE_NOQUOTES(p->api.name, yymsp[-2].minor.yy0->text);
        STRINGSHARE_REPLACE_NOQUOTES(p->api.description, yymsp[-1].minor.yy0->text);
     }
   else if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "filter"))
     {
        STRINGSHARE_REPLACE_NOQUOTES(p->filter.part, yymsp[-2].minor.yy0->text);
        STRINGSHARE_REPLACE_NOQUOTES(p->filter.state, yymsp[-1].minor.yy0->text);
     }
   else if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "in"))
     {
        if (!edje_parser_strtod(yymsp[-2].minor.yy0->text, &p->in.from) || (p->in.from < 0.0))
          ERROR_RANGE(yymsp[-2].minor.yy0);
        else if (!edje_parser_strtod(yymsp[-1].minor.yy0->text, &p->in.range) || (p->in.range < 0.0))
          ERROR_RANGE(yymsp[-1].minor.yy0);
     }
   else if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "transition") && (!p->action.transition.length) && (!p->action.transition.type)) /* error on double set */
     {
#define EDJE_ACTION_CHECK(NAME) \
  if (!strcmp(yymsp[-2].minor.yy0->text, #NAME)) \
    p->action.transition.type = EDJE_PROGRAM_TRANSITION_##NAME

        EDJE_ACTION_CHECK(LINEAR);
        else EDJE_ACTION_CHECK(SINUSOIDAL);
        else EDJE_ACTION_CHECK(ACCELERATE);
        else EDJE_ACTION_CHECK(DECELERATE);
        
        if (!p->action.transition.type)
          ERROR_SYNTAX(yymsp[-2].minor.yy0);
        else if (!edje_parser_strtod(yymsp[-1].minor.yy0->text, &p->action.transition.length))
          ERROR_RANGE(yymsp[-2].minor.yy0);
#undef EDJE_ACTION_CHECK
     }
   else ERROR_SYNTAX(yymsp[-3].minor.yy0);
   edje_parser_token_free(yymsp[-3].minor.yy0);
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 9280 "edje_parser.c"
        break;
      case 122:
#line 7383 "edje_parser.y"
{
   Edje_Program *p;

   p = PARSER_CURRENT(PROGRAM, Edje_Program);
   if (!edje_parser_property_check(yymsp[-4].minor.yy0->text, "action") && (!p->action.type))
     {
         if (!strcmp(yymsp[-3].minor.yy0->text, "DRAG_VAL_SET"))
           {
              p->action.type = EDJE_PROGRAM_ACTION_DRAG_VAL_SET;
              if (!edje_parser_strtod(yymsp[-2].minor.yy0->text, &p->action.params.numbers[0]))
                ERROR_RANGE(yymsp[-2].minor.yy0);
              else if (!edje_parser_strtod(yymsp[-1].minor.yy0->text, &p->action.params.numbers[1]))
                ERROR_RANGE(yymsp[-1].minor.yy0);
           }
         else if (!strcmp(yymsp[-3].minor.yy0->text, "DRAG_VAL_STEP"))
           {
              p->action.type = EDJE_PROGRAM_ACTION_DRAG_VAL_STEP;
              if (!edje_parser_strtod(yymsp[-2].minor.yy0->text, &p->action.params.numbers[0]))
                ERROR_RANGE(yymsp[-2].minor.yy0);
              else if (!edje_parser_strtod(yymsp[-1].minor.yy0->text, &p->action.params.numbers[1]))
                ERROR_RANGE(yymsp[-1].minor.yy0);
           }
         else if (!strcmp(yymsp[-3].minor.yy0->text, "DRAG_VAL_PAGE"))
           {
              p->action.type = EDJE_PROGRAM_ACTION_DRAG_VAL_PAGE;
              if (!edje_parser_strtod(yymsp[-2].minor.yy0->text, &p->action.params.numbers[0]))
                ERROR_RANGE(yymsp[-2].minor.yy0);
              else if (!edje_parser_strtod(yymsp[-1].minor.yy0->text, &p->action.params.numbers[1]))
                ERROR_RANGE(yymsp[-1].minor.yy0);
           }
         else if (!strcmp(yymsp[-3].minor.yy0->text, "STATE_SET"))
           {
              p->action.type = EDJE_PROGRAM_ACTION_STATE_SET;
              if (!edje_parser_strtod(yymsp[-1].minor.yy0->text, &p->action.params.numbers[0]))
                ERROR_RANGE(yymsp[-1].minor.yy0);
              else
                p->action.params.strings =
                  eina_list_append(p->action.params.strings, STRINGSHARE_ADD_NOQUOTES(yymsp[-2].minor.yy0->text));
           }
         else if (!strcmp(yymsp[-3].minor.yy0->text, "SIGNAL_EMIT"))
           {
              p->action.type = EDJE_PROGRAM_ACTION_SIGNAL_EMIT;
              p->action.params.strings =
                eina_list_append(p->action.params.strings, STRINGSHARE_ADD_NOQUOTES(yymsp[-2].minor.yy0->text));
              p->action.params.strings =
                eina_list_append(p->action.params.strings, STRINGSHARE_ADD_NOQUOTES(yymsp[-1].minor.yy0->text));  
           }
         else ERROR_SYNTAX(yymsp[-3].minor.yy0);
     }
   else ERROR_SYNTAX(yymsp[-4].minor.yy0);
   edje_parser_token_free(yymsp[-4].minor.yy0);
   edje_parser_token_free(yymsp[-3].minor.yy0);
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 9340 "edje_parser.c"
        break;
      case 123:
#line 7439 "edje_parser.y"
{
   Edje_Program *p;

   p = PARSER_CURRENT(PROGRAM, Edje_Program);
   if (!edje_parser_property_check(yymsp[-5].minor.yy0->text, "action") && (!p->action.type))
     {
         if (!strcmp(yymsp[-4].minor.yy0->text, "PARAM_SET"))
           p->action.type = EDJE_PROGRAM_ACTION_PARAM_SET;

         if (!p->action.type) ERROR_SYNTAX(yymsp[-4].minor.yy0);
         else
           {
              p->action.params.strings =
                eina_list_append(p->action.params.strings,
                  STRINGSHARE_ADD_NOQUOTES(yymsp[-3].minor.yy0->text)
                );
              p->action.params.strings =
                eina_list_append(p->action.params.strings,
                  STRINGSHARE_ADD_NOQUOTES(yymsp[-2].minor.yy0->text)
                );
              p->action.params.strings =
                eina_list_append(p->action.params.strings,
                  STRINGSHARE_ADD_NOQUOTES(yymsp[-1].minor.yy0->text)
                );
           }
     }
   else ERROR_SYNTAX(yymsp[-5].minor.yy0);
   edje_parser_token_free(yymsp[-5].minor.yy0);
   edje_parser_token_free(yymsp[-4].minor.yy0);
   edje_parser_token_free(yymsp[-3].minor.yy0);
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 9378 "edje_parser.c"
        break;
      case 124:
#line 7473 "edje_parser.y"
{
   Edje_Program *p;

   p = PARSER_CURRENT(PROGRAM, Edje_Program);
   if (!edje_parser_property_check(yymsp[-6].minor.yy0->text, "action") && (!p->action.type))
     {
         if (!strcmp(yymsp[-5].minor.yy0->text, "PARAM_COPY"))
           p->action.type = EDJE_PROGRAM_ACTION_PARAM_COPY;

         if (!p->action.type) ERROR_SYNTAX(yymsp[-5].minor.yy0);
         else
           {
              p->action.params.strings =
                eina_list_append(p->action.params.strings, STRINGSHARE_ADD_NOQUOTES(yymsp[-4].minor.yy0->text));
              p->action.params.strings =
                eina_list_append(p->action.params.strings, STRINGSHARE_ADD_NOQUOTES(yymsp[-3].minor.yy0->text));
              p->action.params.strings =
                eina_list_append(p->action.params.strings, STRINGSHARE_ADD_NOQUOTES(yymsp[-2].minor.yy0->text));
              p->action.params.strings =
                eina_list_append(p->action.params.strings, STRINGSHARE_ADD_NOQUOTES(yymsp[-1].minor.yy0->text));
           }
     }
   else ERROR_SYNTAX(yymsp[-6].minor.yy0);
   edje_parser_token_free(yymsp[-6].minor.yy0);
   edje_parser_token_free(yymsp[-5].minor.yy0);
   edje_parser_token_free(yymsp[-4].minor.yy0);
   edje_parser_token_free(yymsp[-3].minor.yy0);
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 9413 "edje_parser.c"
        break;
      case 126:
#line 7508 "edje_parser.y"
{
   Edje_Images *i;

   EDJE_CREATE;   
   i = edje_images_new();
   PARSER_APPEND(IMAGES, i, Edje_Images);
   DBG("Images: %p", i);
   i->doc = yymsp[-1].minor.yy144;
  yy_destructor(22,&yymsp[0].minor);
}
#line 9427 "edje_parser.c"
        break;
      case 132:
#line 7524 "edje_parser.y"
{
   Edje_Images *i;

   i = PARSER_CURRENT(IMAGES, Edje_Images);
   EDJE_STRUCT_APPEND(i, images, SET_IMAGE, Edje_Set_Image);
   SET_IMAGE = NULL;
}
#line 9438 "edje_parser.c"
        break;
      case 133:
#line 7532 "edje_parser.y"
{
   Edje_Images *i;

   i = PARSER_CURRENT(IMAGES, Edje_Images);
   EDJE_STRUCT_APPEND(i, sets, SET, Edje_Set);
   SET = NULL;
}
#line 9449 "edje_parser.c"
        break;
      case 134:
#line 7541 "edje_parser.y"
{
   if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "image"))
     {
        if (strchr(yymsp[-1].minor.yy0->text, ':')) // invalid filename characters
          ERROR_SYNTAX(yymsp[-1].minor.yy0);
        else
          {
             Edje_Set_Image *s;
             Edje_Images *i;

             i = PARSER_CURRENT(IMAGES, Edje_Images);
             s = PARSER_CURRENT(SET_IMAGE, Edje_Set_Image);
             if ((!s) || (s->filename))
               { /* play nice with Edje_Set objects */
                  s = edje_set_image_new();
                  PARSER_APPEND(SET_IMAGE, s, Edje_Set_Image);
                  DBG("Set image: %p", s);
               }
             STRINGSHARE_REPLACE_NOQUOTES(s->filename, yymsp[-1].minor.yy0->text);
             s->comp = EDJE_COMPRESSION_TYPE_COMP;
             EDJE_STRUCT_APPEND(i, images, SET_IMAGE, Edje_Set_Image);
             SET_IMAGE = NULL;
          }
     }
   else ERROR_SYNTAX(yymsp[-2].minor.yy0);

   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 9483 "edje_parser.c"
        break;
      case 135:
#line 7572 "edje_parser.y"
{
   if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "image"))
     {
        if (strchr(yymsp[-2].minor.yy0->text, ':')) // invalid filename characters
          ERROR_SYNTAX(yymsp[-2].minor.yy0);
        else
          {
             Edje_Set_Image *s;
             s = PARSER_CURRENT(SET_IMAGE, Edje_Set_Image);
             if ((!s) || (s->filename))
               { /* play nice with Edje_Set objects */
                  s = edje_set_image_new();
                  PARSER_APPEND(SET_IMAGE, s, Edje_Set_Image);
                  DBG("Set image: %p", s);
               }
             STRINGSHARE_REPLACE_NOQUOTES(s->filename, yymsp[-2].minor.yy0->text);
             if (!strcmp(yymsp[-1].minor.yy0->text, "RAW"))
               s->comp = EDJE_COMPRESSION_TYPE_RAW;
             else if (!strcmp(yymsp[-1].minor.yy0->text, "COMP"))
               s->comp = EDJE_COMPRESSION_TYPE_COMP;
             else if (!strcmp(yymsp[-1].minor.yy0->text, "USER"))
               s->comp = EDJE_COMPRESSION_TYPE_USER;
          }
     }
   else ERROR_SYNTAX(yymsp[-3].minor.yy0);

   edje_parser_token_free(yymsp[-3].minor.yy0);
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 9518 "edje_parser.c"
        break;
      case 136:
#line 7603 "edje_parser.y"
{
   if (!edje_parser_property_check(yymsp[-4].minor.yy0->text, "image"))
     {
        if (strchr(yymsp[-3].minor.yy0->text, ':')) // invalid filename characters
          ERROR_SYNTAX(yymsp[-3].minor.yy0);
        else
          {
             Edje_Set_Image *s;
             int num;

             s = PARSER_CURRENT(SET_IMAGE, Edje_Set_Image);
             if ((!s) || (s->filename))
               { /* play nice with Edje_Set objects */
                  s = edje_set_image_new();
                  PARSER_APPEND(SET_IMAGE, s, Edje_Set_Image);
                  DBG("Set image: %p", s);
               }
             STRINGSHARE_REPLACE_NOQUOTES(s->filename, yymsp[-3].minor.yy0->text);
             if (strchr(yymsp[-1].minor.yy0->text, '.') || (!edje_parser_strtol(yymsp[-1].minor.yy0->text, &num)) || (num < 0) || (num > 100))
               ERROR_RANGE(yymsp[-1].minor.yy0);
             else
               {
                  s->comp = EDJE_COMPRESSION_TYPE_LOSSY;
                  s->comp_amount = num;
               }
          }
     }

   edje_parser_token_free(yymsp[-4].minor.yy0);
   edje_parser_token_free(yymsp[-3].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(23,&yymsp[-2].minor);
  yy_destructor(10,&yymsp[0].minor);
}
#line 9556 "edje_parser.c"
        break;
      case 138:
#line 7641 "edje_parser.y"
{
   Edje_Group *g;

   EDJE_CREATE;   
   g = edje_group_new();
   PARSER_APPEND(GROUP, g, Edje_Group);
   DBG("Group: %p", g);
   g->doc = yymsp[-1].minor.yy144;
  yy_destructor(24,&yymsp[0].minor);
}
#line 9570 "edje_parser.c"
        break;
      case 144:
#line 7659 "edje_parser.y"
{
   Edje_Group *g;

   g = PARSER_CURRENT(GROUP, Edje_Group);
   EDJE_STRUCT_APPEND(g, color_classes, COLOR_CLASS, Edje_Color_Class);
   COLOR_CLASS = NULL;
}
#line 9581 "edje_parser.c"
        break;
      case 145:
#line 7667 "edje_parser.y"
{
   Edje_Group *g;

   g = PARSER_CURRENT(GROUP, Edje_Group);
   EDJE_STRUCT_APPEND(g, imageses, IMAGES, Edje_Images);
   IMAGES = NULL;
}
#line 9592 "edje_parser.c"
        break;
      case 146:
#line 7675 "edje_parser.y"
{
   Edje_Group *g;

   g = PARSER_CURRENT(GROUP, Edje_Group);
   EDJE_STRUCT_APPEND(g, sets, SET, Edje_Set);
   SET = NULL;
}
#line 9603 "edje_parser.c"
        break;
      case 147:
#line 7683 "edje_parser.y"
{
   Edje_Group *g;

   g = PARSER_CURRENT(GROUP, Edje_Group);
   EDJE_STRUCT_APPEND(g, fontses, FONTS, Edje_Fonts);
   FONTS = NULL;
}
#line 9614 "edje_parser.c"
        break;
      case 148:
#line 7691 "edje_parser.y"
{
   Edje_Group *g;

   g = PARSER_CURRENT(GROUP, Edje_Group);
   EDJE_STRUCT_APPEND(g, fonts, FONT, Edje_Font);
   FONT = NULL;
}
#line 9625 "edje_parser.c"
        break;
      case 149:
#line 7699 "edje_parser.y"
{
   Edje_Group *g;

   g = PARSER_CURRENT(GROUP, Edje_Group);
   EDJE_STRUCT_APPEND(g, programses, PROGRAMS, Edje_Programs);
   PROGRAMS = NULL;
}
#line 9636 "edje_parser.c"
        break;
      case 150:
#line 7707 "edje_parser.y"
{
   Edje_Group *g;

   g = PARSER_CURRENT(GROUP, Edje_Group);
   EDJE_STRUCT_APPEND(g, programs, PROGRAM, Edje_Program);
   PROGRAM = NULL;
}
#line 9647 "edje_parser.c"
        break;
      case 151:
#line 7715 "edje_parser.y"
{
   Edje_Group *g;

   g = PARSER_CURRENT(GROUP, Edje_Group);
   EDJE_STRUCT_APPEND(g, externals, EXTERNAL, Edje_External);
   EXTERNAL = NULL;
}
#line 9658 "edje_parser.c"
        break;
      case 152:
#line 7723 "edje_parser.y"
{
   Edje_Group *g;

   g = PARSER_CURRENT(GROUP, Edje_Group);
   EDJE_STRUCT_APPEND(g, data, DATA, Edje_Data);
   DATA = NULL;
}
#line 9669 "edje_parser.c"
        break;
      case 153:
#line 7731 "edje_parser.y"
{
   Edje_Group *g;

   g = PARSER_CURRENT(GROUP, Edje_Group);
   EDJE_STRUCT_APPEND(g, styles, STYLE, Edje_Style);
   STYLE = NULL;
}
#line 9680 "edje_parser.c"
        break;
      case 154:
#line 7739 "edje_parser.y"
{
   Edje_Group *g;

   g = PARSER_CURRENT(GROUP, Edje_Group);
   g->script = yymsp[0].minor.yy144;
}
#line 9690 "edje_parser.c"
        break;
      case 155:
#line 7746 "edje_parser.y"
{
   Edje_Group *g;

   g = PARSER_CURRENT(GROUP, Edje_Group);
   g->parts = PARTS; /* there can only be one parts{} in a group */
   PARTS = NULL;
}
#line 9701 "edje_parser.c"
        break;
      case 157:
#line 7758 "edje_parser.y"
{
   Edje_Group *g;

   g = PARSER_CURRENT(GROUP, Edje_Group);
   if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "alias"))
     STRINGSHARE_REPLACE_NOQUOTES(g->alias, yymsp[-1].minor.yy0->text);
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "name"))
     STRINGSHARE_REPLACE_NOQUOTES(g->name, yymsp[-1].minor.yy0->text);
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "script_only"))
     {
        int i;
        if (!edje_parser_strtobool(yymsp[-1].minor.yy0->text, &i))
          ERROR_RANGE(yymsp[-1].minor.yy0);
        else
          g->script_only = !!i;
     }

   else
     ERROR_SYNTAX(yymsp[-2].minor.yy0);

   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 9729 "edje_parser.c"
        break;
      case 158:
#line 7782 "edje_parser.y"
{
   Edje_Group *g;

   g = PARSER_CURRENT(GROUP, Edje_Group);
   if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "min"))
     {
        if (!edje_parser_parse_min(ep, yymsp[-2].minor.yy0, yymsp[-1].minor.yy0, g->min, g->max))
          goto group_property_int_out;
     }
   else if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "max"))
     {
        if (!edje_parser_parse_max(ep, yymsp[-2].minor.yy0, yymsp[-1].minor.yy0, g->min, g->max))
          goto group_property_int_out;
     }
   else
     ERROR_SYNTAX(yymsp[-3].minor.yy0);
     
group_property_int_out:
   edje_parser_token_free(yymsp[-3].minor.yy0);
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 9756 "edje_parser.c"
        break;
      case 160:
#line 7809 "edje_parser.y"
{
   Edje_Parts *p;

   EDJE_CREATE;
   p = edje_parts_new();
   PARTS = p; // there can only be one at a time
   DBG("Parts: %p", p);
   p->doc = yymsp[-1].minor.yy144;
  yy_destructor(25,&yymsp[0].minor);
}
#line 9770 "edje_parser.c"
        break;
      case 165:
#line 7825 "edje_parser.y"
{
   EDJE_STRUCT_APPEND(PARTS, fontses, FONTS, Edje_Fonts);
   FONTS = NULL;
}
#line 9778 "edje_parser.c"
        break;
      case 166:
#line 7830 "edje_parser.y"
{
   EDJE_STRUCT_APPEND(PARTS, fonts, FONT, Edje_Font);
   FONT = NULL;
}
#line 9786 "edje_parser.c"
        break;
      case 167:
#line 7835 "edje_parser.y"
{
   EDJE_STRUCT_APPEND(PARTS, programses, PROGRAMS, Edje_Programs);
   PROGRAMS = NULL;
}
#line 9794 "edje_parser.c"
        break;
      case 168:
#line 7840 "edje_parser.y"
{
   EDJE_STRUCT_APPEND(PARTS, programs, PROGRAM, Edje_Program);
   PROGRAM = NULL;
}
#line 9802 "edje_parser.c"
        break;
      case 169:
#line 7845 "edje_parser.y"
{
   EDJE_STRUCT_APPEND(PARTS, color_classes, COLOR_CLASS, Edje_Color_Class);
   COLOR_CLASS = NULL;
}
#line 9810 "edje_parser.c"
        break;
      case 170:
#line 7850 "edje_parser.y"
{
   EDJE_STRUCT_APPEND(PARTS, imageses, IMAGES, Edje_Images);
   IMAGES = NULL;
}
#line 9818 "edje_parser.c"
        break;
      case 171:
#line 7855 "edje_parser.y"
{
   EDJE_STRUCT_APPEND(PARTS, sets, SET, Edje_Set);
   SET = NULL;
}
#line 9826 "edje_parser.c"
        break;
      case 172:
#line 7860 "edje_parser.y"
{
   EDJE_STRUCT_APPEND(PARTS, styles, STYLE, Edje_Style);
   STYLE = NULL;
}
#line 9834 "edje_parser.c"
        break;
      case 173:
#line 7865 "edje_parser.y"
{
   EDJE_STRUCT_APPEND(PARTS, parts, PART, Edje_Part);
   PART = NULL;
}
#line 9842 "edje_parser.c"
        break;
      case 174:
#line 7870 "edje_parser.y"
{
   PARTS->script = yymsp[0].minor.yy144;
}
#line 9849 "edje_parser.c"
        break;
      case 176:
#line 7875 "edje_parser.y"
{
   if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "alias"))
     eina_hash_add(PARTS->aliases, STRDUPA_NOQUOTES(yymsp[-2].minor.yy0->text), STRINGSHARE_ADD_NOQUOTES(yymsp[-1].minor.yy0->text));
   else ERROR_SYNTAX(yymsp[-3].minor.yy0);
   
   edje_parser_token_free(yymsp[-3].minor.yy0);
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 9863 "edje_parser.c"
        break;
      case 178:
#line 7890 "edje_parser.y"
{
   Edje_Part *p;

   EDJE_CREATE;
   p = edje_part_new();
   PARSER_APPEND(PART, p, Edje_Part);
   DBG("Part: %p", p);
   p->doc = yymsp[-1].minor.yy144;
  yy_destructor(26,&yymsp[0].minor);
}
#line 9877 "edje_parser.c"
        break;
      case 185:
#line 7907 "edje_parser.y"
{
  yy_destructor(27,&yymsp[0].minor);
}
#line 9884 "edje_parser.c"
        break;
      case 191:
#line 7915 "edje_parser.y"
{
   Edje_Part *p;

   p = PARSER_CURRENT(PART, Edje_Part);
   if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "confine"))
     STRINGSHARE_REPLACE_NOQUOTES(p->dragable.confine, yymsp[-1].minor.yy0->text);
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "events"))
     STRINGSHARE_REPLACE_NOQUOTES(p->dragable.events, yymsp[-1].minor.yy0->text);
   else ERROR_SYNTAX(yymsp[-2].minor.yy0);

   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 9902 "edje_parser.c"
        break;
      case 192:
#line 7929 "edje_parser.y"
{
   Edje_Part *p;
   Edje_Parser_Token *t[3];
   int i;

   t[0] = yymsp[-3].minor.yy0;
   t[1] = yymsp[-2].minor.yy0;
   t[2] = yymsp[-1].minor.yy0;

   p = PARSER_CURRENT(PART, Edje_Part);
   if (!edje_parser_property_check(yymsp[-4].minor.yy0->text, "x"))
     {
        for (i = 0; i < 3; i++)
          {
             if (!edje_parser_strtol(t[i]->text, &p->dragable.x[i]))
               ERROR_RANGE(t[i]);
          }
        if ((p->dragable.x[0] > 1) || (p->dragable.x[0] < -1)) /* first param is bool */
          ERROR_RANGE(yymsp[-3].minor.yy0);
     }
   else if (!edje_parser_property_check(yymsp[-4].minor.yy0->text, "y"))
     {
        for (i = 0; i < 3; i++)
          if (!edje_parser_strtol(t[i]->text, &p->dragable.y[i]))
            ERROR_RANGE(t[i]);
        if ((p->dragable.y[0] > 1) || (p->dragable.y[0] < -1))  /* first param is bool */
          ERROR_RANGE(yymsp[-3].minor.yy0);
     }
   else ERROR_SYNTAX(yymsp[-4].minor.yy0);
   
   edje_parser_token_free(yymsp[-4].minor.yy0);
   edje_parser_token_free(yymsp[-3].minor.yy0);
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 9942 "edje_parser.c"
        break;
      case 193:
#line 7965 "edje_parser.y"
{
   Edje_Part *p;

   p = PARSER_CURRENT(PART, Edje_Part);
   EDJE_STRUCT_APPEND(p, fontses, FONTS, Edje_Fonts);
   FONTS = NULL;
}
#line 9953 "edje_parser.c"
        break;
      case 194:
#line 7973 "edje_parser.y"
{
   Edje_Part *p;

   p = PARSER_CURRENT(PART, Edje_Part);
   EDJE_STRUCT_APPEND(p, fonts, FONT, Edje_Font);
   FONT = NULL;
}
#line 9964 "edje_parser.c"
        break;
      case 195:
#line 7981 "edje_parser.y"
{
   Edje_Part *p;

   p = PARSER_CURRENT(PART, Edje_Part);
   EDJE_STRUCT_APPEND(p, programses, PROGRAMS, Edje_Programs);
   PROGRAMS = NULL;
}
#line 9975 "edje_parser.c"
        break;
      case 196:
#line 7989 "edje_parser.y"
{
   Edje_Part *p;

   p = PARSER_CURRENT(PART, Edje_Part);
   EDJE_STRUCT_APPEND(p, programs, PROGRAM, Edje_Program);
   PROGRAM = NULL;
}
#line 9986 "edje_parser.c"
        break;
      case 197:
#line 7997 "edje_parser.y"
{
   Edje_Part *p;

   p = PARSER_CURRENT(PART, Edje_Part);
   EDJE_STRUCT_APPEND(p, sets, SET, Edje_Set);
   SET = NULL;
}
#line 9997 "edje_parser.c"
        break;
      case 198:
#line 8005 "edje_parser.y"
{
   Edje_Part *p;

   p = PARSER_CURRENT(PART, Edje_Part);
   EDJE_STRUCT_APPEND(p, styles, STYLE, Edje_Style);
   STYLE = NULL;
}
#line 10008 "edje_parser.c"
        break;
      case 199:
#line 8013 "edje_parser.y"
{
   Edje_Part *p;

   p = PARSER_CURRENT(PART, Edje_Part);
   EDJE_STRUCT_APPEND(p, color_classes, COLOR_CLASS, Edje_Color_Class);
   COLOR_CLASS = NULL;
}
#line 10019 "edje_parser.c"
        break;
      case 200:
#line 8021 "edje_parser.y"
{
   Edje_Part *p;

   p = PARSER_CURRENT(PART, Edje_Part);
   EDJE_STRUCT_APPEND(p, imageses, IMAGES, Edje_Images);
   IMAGES = NULL;
}
#line 10030 "edje_parser.c"
        break;
      case 201:
#line 8029 "edje_parser.y"
{
   Edje_Part *p;

   p = PARSER_CURRENT(PART, Edje_Part);
   p->script = yymsp[0].minor.yy144;
}
#line 10040 "edje_parser.c"
        break;
      case 202:
#line 8036 "edje_parser.y"
{
   Edje_Part *p;

   p = PARSER_CURRENT(PART, Edje_Part);
   EDJE_STRUCT_APPEND(p, descriptions, DESCRIPTION, Edje_Part_Description);
   DESCRIPTION = NULL;
}
#line 10051 "edje_parser.c"
        break;
      case 204:
#line 8047 "edje_parser.y"
{
   Edje_Part_Description *p;

   EDJE_CREATE;
   p = edje_part_description_new();
   PARSER_APPEND(DESCRIPTION, p, Edje_Part_Description);
   DBG("Part_Description: %p", p);
   p->doc = yymsp[-1].minor.yy144;
  yy_destructor(28,&yymsp[0].minor);
}
#line 10065 "edje_parser.c"
        break;
      case 210:
#line 8064 "edje_parser.y"
{
   Edje_Part_Description *p;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);
   EDJE_STRUCT_APPEND(p, color_classes, COLOR_CLASS, Edje_Color_Class);
   COLOR_CLASS = NULL;
}
#line 10076 "edje_parser.c"
        break;
      case 211:
#line 8072 "edje_parser.y"
{
   Edje_Part_Description *p;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);
   EDJE_STRUCT_APPEND(p, imageses, IMAGES, Edje_Images);
   IMAGES = NULL;
}
#line 10087 "edje_parser.c"
        break;
      case 212:
#line 8080 "edje_parser.y"
{
   Edje_Part_Description *p;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);
   EDJE_STRUCT_APPEND(p, fontses, FONTS, Edje_Fonts);
   FONTS = NULL;
}
#line 10098 "edje_parser.c"
        break;
      case 213:
#line 8088 "edje_parser.y"
{
   Edje_Part_Description *p;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);
   EDJE_STRUCT_APPEND(p, fonts, FONT, Edje_Font);
   FONT = NULL;
}
#line 10109 "edje_parser.c"
        break;
      case 214:
#line 8096 "edje_parser.y"
{
   Edje_Part_Description *p;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);
   EDJE_STRUCT_APPEND(p, styles, STYLE, Edje_Style);
   STYLE = NULL;
}
#line 10120 "edje_parser.c"
        break;
      case 215:
#line 8104 "edje_parser.y"
{
   Edje_Part_Description *p;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);
   EDJE_STRUCT_APPEND(p, programses, PROGRAMS, Edje_Programs);
   PROGRAMS = NULL;
}
#line 10131 "edje_parser.c"
        break;
      case 216:
#line 8112 "edje_parser.y"
{
   Edje_Part_Description *p;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);
   EDJE_STRUCT_APPEND(p, programs, PROGRAM, Edje_Program);
   PROGRAM = NULL;
}
#line 10142 "edje_parser.c"
        break;
      case 217:
#line 8120 "edje_parser.y"
{
   Edje_Part_Description *p;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);
   p->script = yymsp[0].minor.yy144;
}
#line 10152 "edje_parser.c"
        break;
      case 219:
#line 8129 "edje_parser.y"
{
    Edje_Part_Description *p;

    p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);

    if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "color_class"))
      STRINGSHARE_REPLACE_NOQUOTES(p->color_class, yymsp[-1].minor.yy0->text);
    else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "aspect_preference"))
      {
         if (!strcmp(yymsp[-1].minor.yy0->text, "BOTH"))
           p->aspect_preference = EDJE_PART_DESCRIPTION_ASPECT_PREF_BOTH;
         else if (!strcmp(yymsp[-1].minor.yy0->text, "VERTICAL"))
           p->aspect_preference = EDJE_PART_DESCRIPTION_ASPECT_PREF_VERTICAL;
         else if (!strcmp(yymsp[-1].minor.yy0->text, "HORIZONTAL"))
           p->aspect_preference = EDJE_PART_DESCRIPTION_ASPECT_PREF_HORIZONTAL;
         else if (!strcmp(yymsp[-1].minor.yy0->text, "NONE"))
           p->aspect_preference = EDJE_PART_DESCRIPTION_ASPECT_PREF_NONE;
         else ERROR_SYNTAX(yymsp[-1].minor.yy0);
      }
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "visible"))
     {
        int i;
        if (!edje_parser_strtobool(yymsp[-1].minor.yy0->text, &i))
          ERROR_RANGE(yymsp[-1].minor.yy0);
        else
          p->visible = !!i;
     }
   else ERROR_SYNTAX(yymsp[-2].minor.yy0);

   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 10189 "edje_parser.c"
        break;
      case 220:
#line 8162 "edje_parser.y"
{
    Edje_Part_Description *p;
    int i, j;

    p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);

    if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "inherit"))
      {
         STRINGSHARE_REPLACE_NOQUOTES(p->inherit.name, yymsp[-2].minor.yy0->text);
         if (!edje_parser_strtod(yymsp[-1].minor.yy0->text, &p->inherit.index))
           ERROR_RANGE(yymsp[-1].minor.yy0);
      }
    else if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "state"))
      {
         STRINGSHARE_REPLACE_NOQUOTES(p->state.name, yymsp[-2].minor.yy0->text);
         if (!edje_parser_strtod(yymsp[-1].minor.yy0->text, &p->state.index))
           ERROR_RANGE(yymsp[-1].minor.yy0);
      }
   else if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "fixed"))
     {
        if (!edje_parser_strtobool(yymsp[-2].minor.yy0->text, &i))
          ERROR_RANGE(yymsp[-2].minor.yy0);
        else if (!edje_parser_strtobool(yymsp[-1].minor.yy0->text, &j))
          ERROR_RANGE(yymsp[-1].minor.yy0);
        else
          {
             p->fixed[0] = !!i;
             p->fixed[1] = !!j;
          }
     }
   else if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "align"))
     {
        if (!edje_parser_strtod(yymsp[-2].minor.yy0->text, &p->align[0]))
          ERROR_RANGE(yymsp[-2].minor.yy0);
        if (!edje_parser_strtod(yymsp[-1].minor.yy0->text, &p->align[1]))
          ERROR_RANGE(yymsp[-1].minor.yy0);
     }
   else if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "aspect"))
     {
        if (!edje_parser_strtod(yymsp[-2].minor.yy0->text, &p->aspect[0]))
          ERROR_RANGE(yymsp[-2].minor.yy0);
        if (!edje_parser_strtod(yymsp[-1].minor.yy0->text, &p->aspect[1]))
          ERROR_RANGE(yymsp[-1].minor.yy0);
     }
   else if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "min"))
     edje_parser_parse_min(ep, yymsp[-2].minor.yy0, yymsp[-1].minor.yy0, p->min, p->max);
   else if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "max"))
     edje_parser_parse_max(ep, yymsp[-2].minor.yy0, yymsp[-1].minor.yy0, p->min, p->max);
   else if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "step"))
     {
        if (strchr(yymsp[-2].minor.yy0->text, '.') || (!edje_parser_strtol(yymsp[-2].minor.yy0->text, &i) || (i < 0)))
          ERROR_RANGE(yymsp[-2].minor.yy0);
        else if (strchr(yymsp[-1].minor.yy0->text, '.') || !edje_parser_strtol(yymsp[-1].minor.yy0->text, &j) || (j < 0))
          ERROR_RANGE(yymsp[-1].minor.yy0);
        else
          {
             p->step[0] = i;
             p->step[1] = j;
          }
     }
    else ERROR_SYNTAX(yymsp[-3].minor.yy0);

    edje_parser_token_free(yymsp[-3].minor.yy0);
    edje_parser_token_free(yymsp[-2].minor.yy0);
    edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 10260 "edje_parser.c"
        break;
      case 221:
#line 8229 "edje_parser.y"
{
   Edje_Part_Description *p;
   int *colors[3];
   Edje_Parser_Token *tokens[4];

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);
   tokens[0] = yymsp[-4].minor.yy0;
   tokens[1] = yymsp[-3].minor.yy0;
   tokens[2] = yymsp[-2].minor.yy0;
   tokens[3] = yymsp[-1].minor.yy0;
   colors[0] = p->color;
   colors[1] = p->color2;
   colors[2] = p->color3;
   if (!edje_parser_parse_color(ep, yymsp[-5].minor.yy0, (Edje_Parser_Token**)tokens, (int**)colors) && (!ep->error))
     ERROR_SYNTAX(yymsp[-5].minor.yy0);

   edje_parser_token_free(yymsp[-5].minor.yy0);
   edje_parser_token_free(yymsp[-4].minor.yy0);
   edje_parser_token_free(yymsp[-3].minor.yy0);
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 10287 "edje_parser.c"
        break;
      case 224:
#line 8254 "edje_parser.y"
{
  yy_destructor(29,&yymsp[0].minor);
}
#line 10294 "edje_parser.c"
        break;
      case 231:
#line 8263 "edje_parser.y"
{
   Edje_Part_Description *p;
   Edje_Param *e;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);
   if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "string"))
     {
        e = edje_param_new(EDJE_PARAM_TYPE_STRING);
        STRINGSHARE_REPLACE_NOQUOTES(e->data.s, yymsp[-1].minor.yy0->text);
     }
   else if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "choice"))
     {
        e = edje_param_new(EDJE_PARAM_TYPE_CHOICE);
        STRINGSHARE_REPLACE_NOQUOTES(e->data.s, yymsp[-1].minor.yy0->text);
     }
   else if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "int"))
     {
        e = edje_param_new(EDJE_PARAM_TYPE_INT);
        if (!edje_parser_strtol(yymsp[-1].minor.yy0->text, &e->data.i))
          {
             free(e);
             ERROR_RANGE(yymsp[-1].minor.yy0);
          }
     }
   else if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "double"))
     {
        e = edje_param_new(EDJE_PARAM_TYPE_DOUBLE);
        if (!edje_parser_strtod(yymsp[-1].minor.yy0->text, &e->data.d))
          {
             free(e);
             ERROR_RANGE(yymsp[-1].minor.yy0);
          }
     }
   else if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "bool"))
     {
        int i;
        e = edje_param_new(EDJE_PARAM_TYPE_BOOL);
        if (!edje_parser_strtobool(yymsp[-1].minor.yy0->text, &i))
          {
             free(e);
             ERROR_RANGE(yymsp[-1].minor.yy0);
          }
        else
          e->data.b = !!i;
     }
   else ERROR_SYNTAX(yymsp[-3].minor.yy0);
   
   if (!ep->error) eina_hash_add(p->params, yymsp[-2].minor.yy0->text, e);


   STRINGSHARE_REPLACE_NOQUOTES(e->data.s, yymsp[-1].minor.yy0->text);
   eina_hash_add(p->params, yymsp[-2].minor.yy0->text, e);

   edje_parser_token_free(yymsp[-3].minor.yy0);
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 10356 "edje_parser.c"
        break;
      case 234:
#line 8323 "edje_parser.y"
{
  yy_destructor(30,&yymsp[0].minor);
}
#line 10363 "edje_parser.c"
        break;
      case 240:
#line 8330 "edje_parser.y"
{
   Edje_Part_Description *p;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);
   if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "zplane"))
     {
        if (strchr(yymsp[-1].minor.yy0->text, '.') || (!edje_parser_strtol(yymsp[-1].minor.yy0->text, &p->perspective.zplane)))
          ERROR_RANGE(yymsp[-1].minor.yy0);
     }
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "focal"))
     {
        if (strchr(yymsp[-1].minor.yy0->text, '.') || (!edje_parser_strtol(yymsp[-1].minor.yy0->text, &p->perspective.focal)))
          ERROR_RANGE(yymsp[-1].minor.yy0);
     }
   else ERROR_SYNTAX(yymsp[-2].minor.yy0);

   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 10387 "edje_parser.c"
        break;
      case 243:
#line 8353 "edje_parser.y"
{
   Edje_Part_Description *p;
   Edje_Image *i;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);
   if (!p->image)
     p->image = edje_image_new();
   i = p->image;
   DBG("Image: %p", i);
   i->doc = yymsp[-1].minor.yy144;
  yy_destructor(14,&yymsp[0].minor);
}
#line 10403 "edje_parser.c"
        break;
      case 249:
#line 8370 "edje_parser.y"
{
   Edje_Part_Description *p;
   Edje_Image *i;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);
   i = p->image;
   if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "normal"))
     STRINGSHARE_REPLACE_NOQUOTES(i->normal, yymsp[-1].minor.yy0->text);
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "tween"))
     i->tween = eina_list_append(i->tween, STRINGSHARE_ADD_NOQUOTES(yymsp[-1].minor.yy0->text));
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "image"))
     {
        if (strchr(yymsp[-1].minor.yy0->text, ':')) // invalid filename characters
          ERROR_SYNTAX(yymsp[-1].minor.yy0);
        else
          {
             Edje_Set_Image *s;

             s = PARSER_CURRENT(SET_IMAGE, Edje_Set_Image);
             if ((!s) || (s->filename))
               { /* play nice with Edje_Set objects */
                  s = edje_set_image_new();
                  PARSER_APPEND(SET_IMAGE, s, Edje_Set_Image);
                  DBG("Set image: %p", s);
               }
             STRINGSHARE_REPLACE_NOQUOTES(s->filename, yymsp[-1].minor.yy0->text);
             s->comp = EDJE_COMPRESSION_TYPE_COMP;
             EDJE_STRUCT_APPEND(i, images, SET_IMAGE, Edje_Set_Image);
             SET_IMAGE = NULL;
          }
     }
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "middle"))
     {
        if ((!strcmp(yymsp[-1].minor.yy0->text, "NONE")) || (!strcmp(yymsp[-1].minor.yy0->text, "0")))
          i->middle = EDJE_IMAGE_MIDDLE_NONE;
        else if ((!strcmp(yymsp[-1].minor.yy0->text, "DEFAULT")) || (!strcmp(yymsp[-1].minor.yy0->text, "1")))
          i->middle = EDJE_IMAGE_MIDDLE_DEFAULT;
        else if (!strcmp(yymsp[-1].minor.yy0->text, "SOLID"))
          i->middle = EDJE_IMAGE_MIDDLE_SOLID;
        else ERROR_SYNTAX(yymsp[-1].minor.yy0);
     }
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "scale_hint"))
     {
        if ((!strcmp(yymsp[-1].minor.yy0->text, "NONE")) || (!strcmp(yymsp[-1].minor.yy0->text, "0")))
          i->middle = EDJE_IMAGE_SCALE_HINT_NONE;
        else if (!strcmp(yymsp[-1].minor.yy0->text, "DYNAMIC"))
          i->middle = EDJE_IMAGE_SCALE_HINT_DYNAMIC;
        else if (!strcmp(yymsp[-1].minor.yy0->text, "STATIC"))
          i->middle = EDJE_IMAGE_SCALE_HINT_STATIC;
     }
   else ERROR_SYNTAX(yymsp[-2].minor.yy0);

   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 10463 "edje_parser.c"
        break;
      case 250:
#line 8426 "edje_parser.y"
{
   Edje_Part_Description *p;
   Edje_Image *i;
   Edje_Parser_Token *t[4];
   unsigned int x;

   t[0] = yymsp[-4].minor.yy0;
   t[1] = yymsp[-3].minor.yy0;
   t[2] = yymsp[-2].minor.yy0;
   t[3] = yymsp[-1].minor.yy0;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);
   i = p->image;
   if (!edje_parser_property_check(yymsp[-5].minor.yy0->text, "border"))
     {
        for (x = 0; x < 4; x++)
          {
             if (!edje_parser_strtol(t[x]->text, &i->border[x]))
               ERROR_RANGE(t[x]);
          }
     }
   else ERROR_SYNTAX(yymsp[-5].minor.yy0);

   edje_parser_token_free(yymsp[-5].minor.yy0);
   edje_parser_token_free(yymsp[-4].minor.yy0);
   edje_parser_token_free(yymsp[-3].minor.yy0);
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 10497 "edje_parser.c"
        break;
      case 251:
#line 8456 "edje_parser.y"
{
   Edje_Part_Description *p;
   Edje_Image *i;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);
   i = p->image;
   EDJE_STRUCT_APPEND(i, images, SET_IMAGE, Edje_Set_Image);
   SET_IMAGE = NULL;
}
#line 10510 "edje_parser.c"
        break;
      case 252:
#line 8466 "edje_parser.y"
{
   Edje_Part_Description *p;
   Edje_Image *i;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);
   i = p->image;
   EDJE_STRUCT_APPEND(i, sets, SET, Edje_Set);
   SET = NULL;
}
#line 10523 "edje_parser.c"
        break;
      case 253:
#line 8476 "edje_parser.y"
{
   Edje_Part_Description *p;
   Edje_Image *i;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);
   i = p->image;
   EDJE_STRUCT_APPEND(i, imageses, IMAGES, Edje_Images);
   IMAGES = NULL;
}
#line 10536 "edje_parser.c"
        break;
      case 256:
#line 8488 "edje_parser.y"
{
  yy_destructor(31,&yymsp[0].minor);
}
#line 10543 "edje_parser.c"
        break;
      case 262:
#line 8496 "edje_parser.y"
{
   Edje_Part_Description *p;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);
   if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "smooth"))
     {
        int x;
        if (!edje_parser_strtobool(yymsp[-1].minor.yy0->text, &x))
          ERROR_RANGE(yymsp[-1].minor.yy0);
        p->fill.smooth = !!x;
     }
   else ERROR_SYNTAX(yymsp[-2].minor.yy0);

   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 10564 "edje_parser.c"
        break;
      case 263:
#line 8512 "edje_parser.y"
{
  yy_destructor(32,&yymsp[-3].minor);
  yy_destructor(4,&yymsp[-2].minor);
  yy_destructor(5,&yymsp[0].minor);
}
#line 10573 "edje_parser.c"
        break;
      case 264:
#line 8513 "edje_parser.y"
{
  yy_destructor(32,&yymsp[-2].minor);
  yy_destructor(4,&yymsp[-1].minor);
  yy_destructor(5,&yymsp[0].minor);
}
#line 10582 "edje_parser.c"
        break;
      case 265:
#line 8514 "edje_parser.y"
{
  yy_destructor(32,&yymsp[-2].minor);
  yy_destructor(7,&yymsp[-1].minor);
}
#line 10590 "edje_parser.c"
        break;
      case 268:
#line 8518 "edje_parser.y"
{
   Edje_Part_Description *p;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);

   if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "relative"))
     {
        if (!edje_parser_strtod(yymsp[-2].minor.yy0->text, &p->fill.origin.relative[0]))
          ERROR_RANGE(yymsp[-2].minor.yy0);
        else if (!edje_parser_strtod(yymsp[-1].minor.yy0->text, &p->fill.origin.relative[1]))
          ERROR_RANGE(yymsp[-1].minor.yy0);
     }
   else if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "offset"))
     {
        if (!edje_parser_strtol(yymsp[-2].minor.yy0->text, &p->fill.origin.offset[0]))
          ERROR_RANGE(yymsp[-2].minor.yy0);
        else if (!edje_parser_strtol(yymsp[-1].minor.yy0->text, &p->fill.origin.offset[1]))
          ERROR_RANGE(yymsp[-1].minor.yy0);
     }
   else ERROR_SYNTAX(yymsp[-3].minor.yy0);

   edje_parser_token_free(yymsp[-3].minor.yy0);
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 10620 "edje_parser.c"
        break;
      case 269:
#line 8543 "edje_parser.y"
{
  yy_destructor(33,&yymsp[-3].minor);
  yy_destructor(4,&yymsp[-2].minor);
  yy_destructor(5,&yymsp[0].minor);
}
#line 10629 "edje_parser.c"
        break;
      case 270:
#line 8544 "edje_parser.y"
{
  yy_destructor(33,&yymsp[-2].minor);
  yy_destructor(4,&yymsp[-1].minor);
  yy_destructor(5,&yymsp[0].minor);
}
#line 10638 "edje_parser.c"
        break;
      case 271:
#line 8545 "edje_parser.y"
{
  yy_destructor(33,&yymsp[-2].minor);
  yy_destructor(7,&yymsp[-1].minor);
}
#line 10646 "edje_parser.c"
        break;
      case 274:
#line 8549 "edje_parser.y"
{
   Edje_Part_Description *p;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);

   if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "relative"))
     {
        if (!edje_parser_strtod(yymsp[-2].minor.yy0->text, &p->fill.size.relative[0]))
          ERROR_RANGE(yymsp[-2].minor.yy0);
        else if (!edje_parser_strtod(yymsp[-1].minor.yy0->text, &p->fill.size.relative[1]))
          ERROR_RANGE(yymsp[-1].minor.yy0);
     }
   else if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "offset"))
     {
        if (!edje_parser_strtol(yymsp[-2].minor.yy0->text, &p->fill.size.offset[0]))
          ERROR_RANGE(yymsp[-2].minor.yy0);
        else if (!edje_parser_strtol(yymsp[-1].minor.yy0->text, &p->fill.size.offset[1]))
          ERROR_RANGE(yymsp[-1].minor.yy0);
     }
   else ERROR_SYNTAX(yymsp[-3].minor.yy0);

   edje_parser_token_free(yymsp[-3].minor.yy0);
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 10676 "edje_parser.c"
        break;
      case 277:
#line 8578 "edje_parser.y"
{
   Edje_Part *p;

   p = PARSER_CURRENT(PART, Edje_Part);
   if ((p->type != EDJE_PART_TYPE_TEXT) && (p->type != EDJE_PART_TYPE_TEXTBLOCK))
     ERROR_TYPE(yymsp[0].minor.yy0);
   edje_parser_token_free(yymsp[0].minor.yy0);
}
#line 10688 "edje_parser.c"
        break;
      case 283:
#line 8592 "edje_parser.y"
{
   Edje_Part_Description *p;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);
   if (!edje_parser_property_check(yymsp[-4].minor.yy0->text, "text"))
     {
        eina_stringshare_del(p->type.text.text);
        p->type.text.text = eina_stringshare_printf("%s%s%s", STRDUPA_NOQUOTES(yymsp[-3].minor.yy0->text), STRDUPA_NOQUOTES(yymsp[-2].minor.yy0->text), STRDUPA_NOQUOTES(yymsp[-1].minor.yy0->text));
     }
   else ERROR_SYNTAX(yymsp[-4].minor.yy0);

   edje_parser_token_free(yymsp[-4].minor.yy0);
   edje_parser_token_free(yymsp[-3].minor.yy0);
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 10709 "edje_parser.c"
        break;
      case 284:
#line 8610 "edje_parser.y"
{
   Edje_Part_Description *p;
   int i, j;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);
#define EDJE_DESC_CHECK(NAME) \
   if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, #NAME)) \
     do { \
        if (!edje_parser_strtobool(yymsp[-2].minor.yy0->text, &i)) \
          ERROR_RANGE(yymsp[-2].minor.yy0); \
        if (!edje_parser_strtobool(yymsp[-1].minor.yy0->text, &j)) \
          ERROR_RANGE(yymsp[-1].minor.yy0); \
        p->type.text.NAME[0] = !!i; \
        p->type.text.NAME[1] = !!j; \
     } while (0)

   EDJE_DESC_CHECK(fit);
   else EDJE_DESC_CHECK(min);
   else EDJE_DESC_CHECK(max);
   else if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "align"))
     {
        if (!edje_parser_strtod(yymsp[-2].minor.yy0->text, &p->type.text.align[0]))
          ERROR_RANGE(yymsp[-2].minor.yy0);
        if (!edje_parser_strtod(yymsp[-1].minor.yy0->text, &p->type.text.align[1]))
          ERROR_RANGE(yymsp[-1].minor.yy0);
     }
   else if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "text"))
     {

        eina_stringshare_del(p->type.text.text);
        p->type.text.text = eina_stringshare_printf("%s%s", STRDUPA_NOQUOTES(yymsp[-2].minor.yy0->text), STRDUPA_NOQUOTES(yymsp[-1].minor.yy0->text));
     }
   else ERROR_SYNTAX(yymsp[-3].minor.yy0);
#undef EDJE_DESC_CHECK
   edje_parser_token_free(yymsp[-3].minor.yy0);
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 10752 "edje_parser.c"
        break;
      case 285:
#line 8649 "edje_parser.y"
{
   Edje_Part_Description *p;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);
#define EDJE_DESC_CHECK(NAME) \
   if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, #NAME)) \
     STRINGSHARE_REPLACE_NOQUOTES(p->type.text.NAME, yymsp[-1].minor.yy0->text)
     
   EDJE_DESC_CHECK(text_class);
   else EDJE_DESC_CHECK(text);
   else EDJE_DESC_CHECK(font);
   else EDJE_DESC_CHECK(source);
   else EDJE_DESC_CHECK(text_source);
   else EDJE_DESC_CHECK(style);
   else EDJE_DESC_CHECK(repch);
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "size"))
     {
        if (strchr(yymsp[-1].minor.yy0->text, '.') || (!edje_parser_strtol(yymsp[-1].minor.yy0->text, &p->type.text.size) || (p->type.text.size < 1)))
          ERROR_RANGE(yymsp[-1].minor.yy0);
     }
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "elipsis"))
     {
        if (!edje_parser_strtod(yymsp[-1].minor.yy0->text, &p->type.text.elipsis))
          ERROR_RANGE(yymsp[-1].minor.yy0);
     }
   else ERROR_SYNTAX(yymsp[-2].minor.yy0);
   
#undef EDJE_DESC_CHECK
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 10788 "edje_parser.c"
        break;
      case 288:
#line 8684 "edje_parser.y"
{
   Edje_Part *p;

   p = PARSER_CURRENT(PART, Edje_Part);
   if (p->type != EDJE_PART_TYPE_BOX)
     ERROR_TYPE(yymsp[0].minor.yy0);
   edje_parser_token_free(yymsp[0].minor.yy0);
}
#line 10800 "edje_parser.c"
        break;
      case 294:
#line 8698 "edje_parser.y"
{
   Edje_Part_Description *p;
   int i, j;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);

   if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "min"))
     {
        if (!edje_parser_strtobool(yymsp[-2].minor.yy0->text, &i))
          ERROR_RANGE(yymsp[-2].minor.yy0);
        else if (!edje_parser_strtobool(yymsp[-1].minor.yy0->text, &j))
          ERROR_RANGE(yymsp[-1].minor.yy0);
        p->type.box.min[0] = !!i;
        p->type.box.min[1] = !!j;
     }
   else if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "padding"))
     {
        if (!edje_parser_strtol(yymsp[-2].minor.yy0->text, &p->type.box.padding[0]))
          ERROR_RANGE(yymsp[-2].minor.yy0);
        else if (!edje_parser_strtol(yymsp[-1].minor.yy0->text, &p->type.box.padding[1]))
          ERROR_RANGE(yymsp[-1].minor.yy0);
     }
   else if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "align"))
     {
        if (!edje_parser_strtod(yymsp[-2].minor.yy0->text, &p->type.box.align[0]))
          ERROR_RANGE(yymsp[-2].minor.yy0);
        if (!edje_parser_strtod(yymsp[-1].minor.yy0->text, &p->type.box.align[1]))
          ERROR_RANGE(yymsp[-1].minor.yy0);
     }
   else if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "layout"))
     {
#define EDJE_DESC_CHECK(TEXT, VALUE) \
  if (!strcmp(yymsp[-2].minor.yy0->text, #TEXT)) \
    p->type.box.primary = EDJE_BOX_LAYOUT_##VALUE

        EDJE_DESC_CHECK(horizontal, HORIZONTAL);
        else EDJE_DESC_CHECK(vertical, VERTICAL);
        else EDJE_DESC_CHECK(horizontal_homogeneous, HORIZONTAL_HOMOGENEOUS);
        else EDJE_DESC_CHECK(vertical_homogeneous, VERTICAL_HOMOGENEOUS);
        else EDJE_DESC_CHECK(horizontal_max, HORIZONTAL_MAX);
        else EDJE_DESC_CHECK(vertical_max, VERTICAL_MAX);
        else EDJE_DESC_CHECK(horizontal_flow, HORIZONTAL_FLOW);
        else EDJE_DESC_CHECK(stack, STACK);
        else p->type.box.primary = EDJE_BOX_LAYOUT_CUSTOM;
#undef EDJE_DESC_CHECK
#define EDJE_DESC_CHECK(TEXT, VALUE) \
  if (!strcmp(yymsp[-1].minor.yy0->text, #TEXT)) \
    p->type.box.fallback = EDJE_BOX_LAYOUT_##VALUE
    
        EDJE_DESC_CHECK(horizontal, HORIZONTAL);
        else EDJE_DESC_CHECK(vertical, VERTICAL);
        else EDJE_DESC_CHECK(horizontal_homogeneous, HORIZONTAL_HOMOGENEOUS);
        else EDJE_DESC_CHECK(vertical_homogeneous, VERTICAL_HOMOGENEOUS);
        else EDJE_DESC_CHECK(horizontal_max, HORIZONTAL_MAX);
        else EDJE_DESC_CHECK(vertical_max, VERTICAL_MAX);
        else EDJE_DESC_CHECK(horizontal_flow, HORIZONTAL_FLOW);
        else EDJE_DESC_CHECK(stack, STACK);
        else p->type.box.fallback = EDJE_BOX_LAYOUT_CUSTOM;
     }
   else ERROR_SYNTAX(yymsp[-3].minor.yy0);
#undef EDJE_DESC_CHECK
   edje_parser_token_free(yymsp[-3].minor.yy0);
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 10870 "edje_parser.c"
        break;
      case 295:
#line 8764 "edje_parser.y"
{
   Edje_Part_Description *p;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);
   if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "layout"))
     {
#define EDJE_DESC_CHECK(TEXT, VALUE) \
  if (!strcmp(yymsp[-1].minor.yy0->text, #TEXT)) \
    p->type.box.primary = EDJE_BOX_LAYOUT_##VALUE

        EDJE_DESC_CHECK(horizontal, HORIZONTAL);
        else EDJE_DESC_CHECK(vertical, VERTICAL);
        else EDJE_DESC_CHECK(horizontal_homogeneous, HORIZONTAL_HOMOGENEOUS);
        else EDJE_DESC_CHECK(vertical_homogeneous, VERTICAL_HOMOGENEOUS);
        else EDJE_DESC_CHECK(horizontal_max, HORIZONTAL_MAX);
        else EDJE_DESC_CHECK(vertical_max, VERTICAL_MAX);
        else EDJE_DESC_CHECK(horizontal_flow, HORIZONTAL_FLOW);
        else EDJE_DESC_CHECK(stack, STACK);
        else p->type.box.primary = EDJE_BOX_LAYOUT_CUSTOM;
     }
   else ERROR_SYNTAX(yymsp[-2].minor.yy0);
#undef EDJE_DESC_CHECK
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 10900 "edje_parser.c"
        break;
      case 298:
#line 8792 "edje_parser.y"
{
  yy_destructor(36,&yymsp[0].minor);
}
#line 10907 "edje_parser.c"
        break;
      case 304:
#line 8800 "edje_parser.y"
{
   Edje_Part_Description *p;
   int i;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);
#define EDJE_MAP_CHECK(NAME) \
   if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, #NAME)) \
     do { \
        if (!edje_parser_strtobool(yymsp[-1].minor.yy0->text, &i)) \
          ERROR_RANGE(yymsp[-1].minor.yy0); \
        else \
          p->type.map.NAME = !!i; \
     } while (0)

   EDJE_MAP_CHECK(on);
   else EDJE_MAP_CHECK(smooth);
   else EDJE_MAP_CHECK(alpha);
   else EDJE_MAP_CHECK(backface_cull);
   else EDJE_MAP_CHECK(perspective_on);
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "perspective"))
     STRINGSHARE_REPLACE_NOQUOTES(p->type.map.perspective, yymsp[-1].minor.yy0->text);
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "light"))
     STRINGSHARE_REPLACE_NOQUOTES(p->type.map.light, yymsp[-1].minor.yy0->text);
   else ERROR_SYNTAX(yymsp[-2].minor.yy0);
#undef EDJE_MAP_CHECK
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 10940 "edje_parser.c"
        break;
      case 307:
#line 8830 "edje_parser.y"
{
  yy_destructor(37,&yymsp[0].minor);
}
#line 10947 "edje_parser.c"
        break;
      case 313:
#line 8837 "edje_parser.y"
{
   Edje_Part_Description *p;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);
#define EDJE_MAP_CHECK(NAME) \
   if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, #NAME)) \
     do { \
        if (!edje_parser_strtod(yymsp[-1].minor.yy0->text, &p->type.map.rotation.NAME)) \
          ERROR_RANGE(yymsp[-1].minor.yy0); \
     } while (0)

   EDJE_MAP_CHECK(x);
   else EDJE_MAP_CHECK(y);
   else EDJE_MAP_CHECK(z);
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "center"))
     STRINGSHARE_REPLACE_NOQUOTES(p->type.map.rotation.center, yymsp[-1].minor.yy0->text);
   else ERROR_SYNTAX(yymsp[-2].minor.yy0);
#undef EDJE_MAP_CHECK
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 10973 "edje_parser.c"
        break;
      case 314:
      case 315:
#line 8859 "edje_parser.y"
{ REL = NULL; }
#line 10979 "edje_parser.c"
        break;
      case 318:
#line 8864 "edje_parser.y"
{
   Edje_Part_Description *p;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);
   DBG("Rel1");
   REL = &p->rel[0];
  yy_destructor(38,&yymsp[0].minor);
}
#line 10991 "edje_parser.c"
        break;
      case 319:
#line 8872 "edje_parser.y"
{
   Edje_Part_Description *p;

   p = PARSER_CURRENT(DESCRIPTION, Edje_Part_Description);
   DBG("Rel2");
   REL = &p->rel[1];
  yy_destructor(39,&yymsp[0].minor);
}
#line 11003 "edje_parser.c"
        break;
      case 325:
#line 8885 "edje_parser.y"
{

   if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "offset"))
     {
        if (!edje_parser_strtol(yymsp[-2].minor.yy0->text, &REL->offset[0]))
          ERROR_RANGE(yymsp[-2].minor.yy0);
        else if (!edje_parser_strtol(yymsp[-1].minor.yy0->text, &REL->offset[1]))
          ERROR_RANGE(yymsp[-1].minor.yy0);
     }
   else if (!edje_parser_property_check(yymsp[-3].minor.yy0->text, "relative"))
     {
        if (!edje_parser_strtod(yymsp[-2].minor.yy0->text, &REL->relative[0]))
          ERROR_RANGE(yymsp[-2].minor.yy0);
        else if (!edje_parser_strtod(yymsp[-1].minor.yy0->text, &REL->relative[1]))
          ERROR_RANGE(yymsp[-1].minor.yy0);
     }
   else ERROR_SYNTAX(yymsp[-3].minor.yy0);
   
   edje_parser_token_free(yymsp[-3].minor.yy0);
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 11030 "edje_parser.c"
        break;
      case 326:
#line 8908 "edje_parser.y"
{
   if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "to"))
     STRINGSHARE_REPLACE_NOQUOTES(REL->to, yymsp[-1].minor.yy0->text);
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "to_x"))
     STRINGSHARE_REPLACE_NOQUOTES(REL->to_x, yymsp[-1].minor.yy0->text);
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "to_y"))
     STRINGSHARE_REPLACE_NOQUOTES(REL->to_y, yymsp[-1].minor.yy0->text);
   else ERROR_SYNTAX(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 11046 "edje_parser.c"
        break;
      case 328:
#line 8925 "edje_parser.y"
{
   Edje_Part *p;
   int i;

   p = PARSER_CURRENT(PART, Edje_Part);
#define EDJE_PART_CHECK(TYPE) \
if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, #TYPE)) \
  STRINGSHARE_REPLACE_NOQUOTES(p->TYPE, yymsp[-1].minor.yy0->text)
  
   EDJE_PART_CHECK(clip_to);
   else EDJE_PART_CHECK(name);
   else EDJE_PART_CHECK(source);
   else EDJE_PART_CHECK(source2);
   else EDJE_PART_CHECK(source3);
   else EDJE_PART_CHECK(source4);
   else EDJE_PART_CHECK(source5);
   else EDJE_PART_CHECK(source6);
#undef EDJE_PART_CHECK
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "type"))
     {
#define EDJE_PART_CHECK(TYPE) \
if (!strcmp(yymsp[-1].minor.yy0->text, #TYPE)) \
  p->type = EDJE_PART_TYPE_##TYPE

        EDJE_PART_CHECK(RECT);
        else EDJE_PART_CHECK(TEXT);
        else EDJE_PART_CHECK(IMAGE);
        else EDJE_PART_CHECK(SWALLOW);
        else EDJE_PART_CHECK(TEXTBLOCK);
        else EDJE_PART_CHECK(GROUP);
        else EDJE_PART_CHECK(BOX);
        else EDJE_PART_CHECK(TABLE);
        else EDJE_PART_CHECK(EXTERNAL);
        else EDJE_PART_CHECK(PROXY);
        else ERROR_SYNTAX(yymsp[-1].minor.yy0);
#undef EDJE_PART_CHECK
     }
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "ignore_flags"))
     {
        if (!strcmp(yymsp[-1].minor.yy0->text, "NONE"))
          p->ignore_flags = EDJE_IGNORE_FLAG_NONE;
        else if (!strcmp(yymsp[-1].minor.yy0->text, "ON_HOLD"))
          p->ignore_flags = EDJE_IGNORE_FLAG_ON_HOLD;
        else
          ERROR_SYNTAX(yymsp[-1].minor.yy0);
     }
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "pointer_mode"))
     {
        if (!strcmp(yymsp[-1].minor.yy0->text, "AUTOGRAB"))
          p->pointer_mode = EDJE_POINTER_MODE_AUTOGRAB;
        else if (!strcmp(yymsp[-1].minor.yy0->text, "NOGRAB"))
          p->pointer_mode = EDJE_POINTER_MODE_NOGRAB;
        else
          ERROR_SYNTAX(yymsp[-1].minor.yy0);
     }
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "effect"))
     {
#define EDJE_PART_CHECK(TYPE) \
if (!strcmp(yymsp[-1].minor.yy0->text, #TYPE)) \
  p->effect = EDJE_PART_EFFECT_##TYPE

        EDJE_PART_CHECK(NONE);
        else EDJE_PART_CHECK(PLAIN);
        else EDJE_PART_CHECK(OUTLINE);
        else EDJE_PART_CHECK(SOFT_OUTLINE);
        else EDJE_PART_CHECK(SHADOW);
        else EDJE_PART_CHECK(SOFT_SHADOW);
        else EDJE_PART_CHECK(OUTLINE_SHADOW);
        else EDJE_PART_CHECK(OUTLINE_SOFT_SHADOW);
        else EDJE_PART_CHECK(FAR_SHADOW);
        else EDJE_PART_CHECK(FAR_SOFT_SHADOW);
        else EDJE_PART_CHECK(GLOW);
        else ERROR_SYNTAX(yymsp[-1].minor.yy0);
#undef EDJE_PART_CHECK
     }
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "entry_mode"))
     {
#define EDJE_PART_CHECK(TYPE) \
if (!strcmp(yymsp[-1].minor.yy0->text, #TYPE)) \
  p->entry_mode = EDJE_PART_ENTRY_MODE_##TYPE

        EDJE_PART_CHECK(NONE);
        else EDJE_PART_CHECK(PLAIN);
        else EDJE_PART_CHECK(EDITABLE);
        else EDJE_PART_CHECK(PASSWORD);
        else ERROR_SYNTAX(yymsp[-1].minor.yy0);
#undef EDJE_PART_CHECK
     }
   else if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, "select_mode"))
     {
        if (!strcmp(yymsp[-1].minor.yy0->text, "DEFAULT"))
          p->select_mode = EDJE_PART_SELECT_MODE_DEFAULT;
        else if (!strcmp(yymsp[-1].minor.yy0->text, "EXPLICIT_DEFAULT"))
          p->select_mode = EDJE_PART_SELECT_MODE_EXPLICIT_DEFAULT;
        else
          ERROR_SYNTAX(yymsp[-1].minor.yy0);
     }
#define EDJE_PART_CHECK(TYPE) \
if (!edje_parser_property_check(yymsp[-2].minor.yy0->text, #TYPE)) \
  if (!edje_parser_strtobool(yymsp[-1].minor.yy0->text, &i)) \
    ERROR_RANGE(yymsp[-1].minor.yy0); \
  else \
  p->TYPE = !!i

   else EDJE_PART_CHECK(mouse_events);
   else EDJE_PART_CHECK(repeat_events);
   else EDJE_PART_CHECK(scale);
   else EDJE_PART_CHECK(precise_is_inside);
   else EDJE_PART_CHECK(use_alternate_font_metrics);
   else EDJE_PART_CHECK(multiline);
#undef EDJE_PART_CHECK

   else
     ERROR_SYNTAX(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-2].minor.yy0);
   edje_parser_token_free(yymsp[-1].minor.yy0);
  yy_destructor(10,&yymsp[0].minor);
}
#line 11168 "edje_parser.c"
        break;
  };
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yypParser->yyidx -= yysize;
  yyact = yy_find_reduce_action(yymsp[-yysize].stateno,yygoto);
  if( yyact < YYNSTATE ){
#ifdef NDEBUG
    /* If we are not debugging and the reduce action popped at least
    ** one element off the stack, then we can push the new element back
    ** onto the stack here, and skip the stack overflow test in yy_shift().
    ** That gives a significant speed improvement. */
    if( yysize ){
      yypParser->yyidx++;
      yymsp -= yysize-1;
      yymsp->stateno = yyact;
      yymsp->major = yygoto;
      yymsp->minor = yygotominor;
    }else
#endif
    {
      yy_shift(yypParser,yyact,yygoto,&yygotominor);
    }
  }else if( yyact == YYNSTATE + YYNRULE + 1 ){
    yy_accept(yypParser);
  }
}

/*
** The following code executes when the parse fails
*/
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  edje_parser_ARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
  edje_parser_ARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  YYMINORTYPE yyminor            /* The minor type of the error token */
){
  edje_parser_ARG_FETCH;
#define TOKEN (yyminor.yy0)
#line 24 "edje_parser.y"
ERROR_SYNTAX(TOKEN);
#line 11228 "edje_parser.c"
  edje_parser_ARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  edje_parser_ARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
  edje_parser_ARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "edje_parser_Alloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void edje_parser_(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  edje_parser_TOKENTYPE yyminor       /* The value for the token */
  edje_parser_ARG_PDECL               /* Optional %extra_argument parameter */
){
  YYMINORTYPE yyminorunion;
  int yyact;            /* The parser action. */
  int yyendofinput;     /* True if we are at the end of input */
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
  yyParser *yypParser;  /* The parser */

  /* (re)initialize the parser, if necessary */
  yypParser = (yyParser*)yyp;
  if( yypParser->yyidx<0 ){
#if YYSTACKDEPTH<=0
    if( yypParser->yystksz <=0 ){
      memset(&yyminorunion, 0, sizeof(yyminorunion));
      yyStackOverflow(yypParser, &yyminorunion);
      return;
    }
#endif
    yypParser->yyidx = 0;
    yypParser->yyerrcnt = -1;
    yypParser->yystack[0].stateno = 0;
    yypParser->yystack[0].major = 0;
  }
  yyminorunion.yy0 = yyminor;
  yyendofinput = (yymajor==0);
  edje_parser_ARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sInput %s\n",yyTracePrompt,yyTokenName[yymajor]);
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,yymajor);
    if( yyact<YYNSTATE ){
      yy_shift(yypParser,yyact,yymajor,&yyminorunion);
      yypParser->yyerrcnt--;
      if( yyendofinput && yypParser->yyidx>=0 ){
        yymajor = 0;
      }else{
        yymajor = YYNOCODE;
      }
    }else if( yyact < YYNSTATE + YYNRULE ){
      yy_reduce(yypParser,yyact-YYNSTATE);
    }else if( yyact == YY_ERROR_ACTION ){
      int yymx;
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".  
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( yypParser->yyerrcnt<0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yymx = yypParser->yystack[yypParser->yyidx].major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yymajor,&yyminorunion);
        yymajor = YYNOCODE;
      }else{
         while(
          yypParser->yyidx >= 0 &&
          yymx != YYERRORSYMBOL &&
          (yyact = yy_find_reduce_action(
                        yypParser->yystack[yypParser->yyidx].stateno,
                        YYERRORSYMBOL)) >= YYNSTATE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yyidx < 0 || yymajor==0 ){
          yy_destructor(yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          YYMINORTYPE u2;
          u2.YYERRSYMDT = 0;
          yy_shift(yypParser,yyact,YYERRORSYMBOL,&u2);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
      }
      yymajor = YYNOCODE;
#endif
    }else{
      yy_accept(yypParser);
      yymajor = YYNOCODE;
    }
  }while( yymajor!=YYNOCODE && yypParser->yyidx>=0 );
  return;
}
