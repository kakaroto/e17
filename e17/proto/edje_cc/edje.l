%{
#include <stdio.h>
#include "Etcher.h"
#include "edje_cc.tab.h"
    
    int yylex(void);
    int lnum = 1;
    int col = 0;
    static int comment = 0, cpp_comment = 0;

    #define COMMENT_START() BEGIN(c_comment); comment++; 
    #define COMMENT_END()   comment--; \
                            if (comment <= 0) { comment = 0; BEGIN(INITIAL); }
    #define CPP_COMMENT()   comment = 1; cpp_comment = 1;

    #define WHITE_SPACE()   col += yyleng;

    #define NEW_LINE()      lnum ++; \
                            if (cpp_comment) { \
                                cpp_comment = 0; \
                                comment = 0; \
                            } \
                            col = 0;

    #define KEYWORD_RETURN(x)   col += yyleng; \
                                if (!comment) return (x);

    #define VAL_RETURN(x)   col += yyleng; \
                            if (!comment) return (x);

    #define STRING_RETURN() yylval.string = (char *)malloc(yyleng - 1); \
                            { \
                                char *c = yytext; \
                                strncpy(yylval.string, ++c, yyleng - 2); \
                            } \
                            VAL_RETURN(STRING);

    #define FLOAT_RETURN()  yylval.val = atof(yytext); \
                            VAL_RETURN(FLOAT);

    #define CHAR_RETURN()   VAL_RETURN(yytext[0]);
%}

%option noyywrap
%option nounput

start_part [-+]?
exponent_part [eE][-+]?[0-9]+
fractional_constant ([0-9]*"."[0-9]+)|([0-9]+".")
floating_constant ((({start_part}{fractional_constant}{exponent_part}?)|({start_part}[0-9]+{exponent_part}))[FfLl]?)|({start_part}[0-9][0-9]*)

%x c_comment

%%
ACCELERATE			{ KEYWORD_RETURN(ACCELERATE); }
action				{ KEYWORD_RETURN(ACTION); }
ACTION_STOP			{ KEYWORD_RETURN(ACTION_STOP); }
after				{ KEYWORD_RETURN(AFTER); }
align				{ KEYWORD_RETURN(ALIGN); }
aspect				{ KEYWORD_RETURN(ASPECT); }
aspect_preference	{ KEYWORD_RETURN(ASPECT_PREFERENCE); }
border				{ KEYWORD_RETURN(BORDER); }
clip_to				{ KEYWORD_RETURN(CLIP_TO); }
collections			{ KEYWORD_RETURN(COLLECTIONS); }
color				{ KEYWORD_RETURN(COLOR); }
color2				{ KEYWORD_RETURN(COLOR2); }
color3				{ KEYWORD_RETURN(COLOR3); }
color_class			{ KEYWORD_RETURN(COLOR_CLASS); }
COMP				{ KEYWORD_RETURN(COMP); }
confine				{ KEYWORD_RETURN(CONFINE); }
data				{ KEYWORD_RETURN(DATA); }
DECELERATE			{ KEYWORD_RETURN(DECELERATE); }
description			{ KEYWORD_RETURN(DESCRIPTION); }
DRAG_VAL_PAGE	    { KEYWORD_RETURN(DRAG_VAL_PAGE); }
DRAG_VAL_SET		{ KEYWORD_RETURN(DRAG_VAL_SET); }
DRAG_VAL_STEP		{ KEYWORD_RETURN(DRAG_VAL_STEP); }
dragable			{ KEYWORD_RETURN(DRAGABLE); }
effect				{ KEYWORD_RETURN(EFFECT); }
fill				{ KEYWORD_RETURN(FILL); }
fit					{ KEYWORD_RETURN(FIT); }
font				{ KEYWORD_RETURN(FONT); }
fonts				{ KEYWORD_RETURN(FONTS); }
group				{ KEYWORD_RETURN(GROUP); }
image				{ KEYWORD_RETURN(IMAGE); }
images				{ KEYWORD_RETURN(IMAGES); }
in					{ KEYWORD_RETURN(IN); }
item				{ KEYWORD_RETURN(ITEM); }
LINEAR				{ KEYWORD_RETURN(LINEAR); }
LOSSY				{ KEYWORD_RETURN(LOSSY); }
max					{ KEYWORD_RETURN(MAX); }
min					{ KEYWORD_RETURN(MIN); }
mouse_events		{ KEYWORD_RETURN(MOUSE_EVENTS); }
name				{ KEYWORD_RETURN(NAME); }
normal				{ KEYWORD_RETURN(NORMAL); }
offset				{ KEYWORD_RETURN(OFFSET); }
origin				{ KEYWORD_RETURN(ORIGIN); }
part				{ KEYWORD_RETURN(PART); }
parts				{ KEYWORD_RETURN(PARTS); }
program				{ KEYWORD_RETURN(PROGRAM); }
programs			{ KEYWORD_RETURN(PROGRAMS); }
RAW					{ KEYWORD_RETURN(RAW); }
rel1				{ KEYWORD_RETURN(REL1); }
rel2				{ KEYWORD_RETURN(REL2); }
relative			{ KEYWORD_RETURN(RELATIVE); }
repeat_events		{ KEYWORD_RETURN(REPEAT_EVENTS); }
script				{ KEYWORD_RETURN(SCRIPT); }
signal				{ KEYWORD_RETURN(SIGNAL); }
SIGNAL_EMIT			{ KEYWORD_RETURN(SIGNAL_EMIT); }
SINUSOIDAL			{ KEYWORD_RETURN(SINUSOIDAL); }
size				{ KEYWORD_RETURN(SIZE); }
smooth				{ KEYWORD_RETURN(SMOOTH); }
source				{ KEYWORD_RETURN(SOURCE); }
state				{ KEYWORD_RETURN(STATE); }
STATE_SET			{ KEYWORD_RETURN(STATE_SET); }
step				{ KEYWORD_RETURN(STEP); }
target				{ KEYWORD_RETURN(TARGET); }
text				{ KEYWORD_RETURN(TEXT); }
text_class			{ KEYWORD_RETURN(TEXT_CLASS); }
to					{ KEYWORD_RETURN(TO); }
to_x				{ KEYWORD_RETURN(TO_X); }
to_y				{ KEYWORD_RETURN(TO_Y); }
transition			{ KEYWORD_RETURN(TRANSITION); }
tween				{ KEYWORD_RETURN(TWEEN); }
type				{ KEYWORD_RETURN(TYPE); }
visible				{ KEYWORD_RETURN(VISIBLE); }
x					{ KEYWORD_RETURN(X); }
y					{ KEYWORD_RETURN(Y); }
(\"[^\"]*\")*       { STRING_RETURN(); }
{floating_constant} { FLOAT_RETURN(); }
\}                  { KEYWORD_RETURN(CLOSE_BRACE); }
\{                  { KEYWORD_RETURN(OPEN_BRACE); }
[:,]		    { KEYWORD_RETURN(COLON); }
;		    { KEYWORD_RETURN(SEMICOLON); }
\"		    { KEYWORD_RETURN(QUOTE); }
[ \t]+              { WHITE_SPACE(); }
\n                  { NEW_LINE(); }

\/\/				{ CPP_COMMENT(); }
#                   { CPP_COMMENT(); }

"/*"                { COMMENT_START(); }

<c_comment>{
  "/*"           { COMMENT_START(); }
  \n             { NEW_LINE(); }

  "*/"           { COMMENT_END(); }
  .
}



.                   { CHAR_RETURN(); }


%%


