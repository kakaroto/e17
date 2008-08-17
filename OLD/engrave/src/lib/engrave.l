%{
#include <string.h>
#include <stdio.h>
#include "Engrave.h"
#include "libengrave_la-engrave.h"
    
    int yylex(void);
    int engrave_lnum = 1;
    int engrave_col = 0;
    char *engrave_cur_file = NULL;
    extern char *engrave_filename;

    static int comment = 0, cpp_comment = 0;
    static int script_level = 0;
    static char *script = NULL;

    #define COMMENT_START() BEGIN(SC_COMMENT); comment++; 

    #define COMMENT_END()   comment--; \
                            if (comment <= 0) { comment = 0; BEGIN(INITIAL); }

    #define CPP_COMMENT()   comment = 1; cpp_comment = 1;

    #define SCRIPT_START()  if (!comment && !cpp_comment) BEGIN(SC_SCRIPT);

    #define SCRIPT_END()    BEGIN(INITIAL); \
			        if (script) { \
				yylval.string = strdup(script); \
				free(script); \
				} \
				script = NULL; \
				VAL_RETURN(SCRIPT);

    #define SCRIPT_PIECE()  if (script == NULL) { \
                                script = strdup(yytext); \
			    } else { \
				char *t = NULL; \
				int len = strlen(script) + yyleng + 1; \
				t = (char *)malloc(sizeof(char) * len); \
				snprintf(t, len, "%s%s", script, yytext); \
				free(script); \
				script = t; \
			    } 

    #define SCRIPT_NEW_LINE() engrave_lnum ++; SCRIPT_PIECE(); 

    #define SCRIPT_OPEN_BRACE() script_level++; \
                                if (script_level > 1) { SCRIPT_PIECE(); }

    #define SCRIPT_CLOSE_BRACE() script_level--; \
                                 if (script_level > 0) { SCRIPT_PIECE();}\
                                 else {script_level = 0; SCRIPT_END();}\

    #define WHITE_SPACE()   engrave_col += yyleng;

    #define NEW_LINE()      engrave_lnum ++; \
                            if (cpp_comment) { \
                                cpp_comment = 0; \
                                comment = 0; \
                            } \
                            engrave_col = 0;

    #define KEYWORD_RETURN(x)   engrave_col += yyleng; \
                                if (!comment) return (x);

    #define VAL_RETURN(x)   engrave_col += yyleng; \
                            if (!comment) return (x);

    #define STRING_RETURN() VAL_RETURN(STRING);

    #define FLOAT_RETURN()  yylval.val = atof(yytext); \
                            VAL_RETURN(FLOAT);

    #define CHAR_RETURN()   VAL_RETURN(yytext[0]); 
%}

%option noyywrap
%option nounput

start_part [-+]?
exponent_part [eE][-+]?[0-9]+
fractional_constant ([0-9]*"."[0-9]+)|([0-9]+".")
floating_constant ((({fractional_constant}{exponent_part}?)|({start_part}[0-9]+{exponent_part}))[FfLl]?)|([0-9][0-9]*)

%x SC_COMMENT
%x SC_SCRIPT

%%
ACCELERATE			{ KEYWORD_RETURN(ACCELERATE); }
action				{ KEYWORD_RETURN(ACTION); }
ACTION_STOP			{ KEYWORD_RETURN(ACTION_STOP); }
after				{ KEYWORD_RETURN(AFTER); }
align				{ KEYWORD_RETURN(ALIGN); }
aspect				{ KEYWORD_RETURN(ASPECT); }
aspect_preference	{ KEYWORD_RETURN(ASPECT_PREFERENCE); }
BOTH				{ KEYWORD_RETURN(BOTH); }
border				{ KEYWORD_RETURN(BORDER); }
middle				{ KEYWORD_RETURN(MIDDLE); }
clip_to				{ KEYWORD_RETURN(CLIP_TO); }
collections			{ KEYWORD_RETURN(COLLECTIONS); }
color				{ KEYWORD_RETURN(COLOR); }
color2				{ KEYWORD_RETURN(COLOR2); }
color3				{ KEYWORD_RETURN(COLOR3); }
color_class			{ KEYWORD_RETURN(COLOR_CLASS); }
color_classes			{ KEYWORD_RETURN(COLOR_CLASSES); }
COMP				{ KEYWORD_RETURN(COMP); }
confine				{ KEYWORD_RETURN(CONFINE); }
data				{ KEYWORD_RETURN(DATA); }
DECELERATE			{ KEYWORD_RETURN(DECELERATE); }
description			{ KEYWORD_RETURN(DESCRIPTION); }
DRAG_VAL_PAGE	    { KEYWORD_RETURN(DRAG_VAL_PAGE); }
DRAG_VAL_SET		{ KEYWORD_RETURN(DRAG_VAL_SET); }
DRAG_VAL_STEP		{ KEYWORD_RETURN(DRAG_VAL_STEP); }
SCRIPT		{ KEYWORD_RETURN(ACTION_SCRIPT); }
dragable			{ KEYWORD_RETURN(DRAGABLE); }
effect				{ KEYWORD_RETURN(EFFECT); }
fill				{ KEYWORD_RETURN(FILL); }
fit					{ KEYWORD_RETURN(FIT); }
font				{ KEYWORD_RETURN(FONT); }
fonts				{ KEYWORD_RETURN(FONTS); }
spectra				{ KEYWORD_RETURN(SPECTRA); }
spectrum			{ KEYWORD_RETURN(SPECTRUM); }
styles				{ KEYWORD_RETURN(STYLES); }
style				{ KEYWORD_RETURN(STYLE); }
base				{ KEYWORD_RETURN(SBASE); }
tag				{ KEYWORD_RETURN(TAG); }
group				{ KEYWORD_RETURN(GROUP); }
HORIZONTAL			{ KEYWORD_RETURN(HORIZONTAL); }
image				{ KEYWORD_RETURN(IMAGE); }
images				{ KEYWORD_RETURN(IMAGES); }
in					{ KEYWORD_RETURN(IN); }
inherit				{ KEYWORD_RETURN(INHERIT); }
item				{ KEYWORD_RETURN(ITEM); }
IMAGE				{ KEYWORD_RETURN(IMAGE); }
LINEAR				{ KEYWORD_RETURN(LINEAR); }
LOSSY				{ KEYWORD_RETURN(LOSSY); }
max					{ KEYWORD_RETURN(MAX); }
min					{ KEYWORD_RETURN(MIN); }
fixed					{ KEYWORD_RETURN(FIXED); }
mouse_events		{ KEYWORD_RETURN(MOUSE_EVENTS); }
name				{ KEYWORD_RETURN(NAME); }
alias				{ KEYWORD_RETURN(ALIAS); }
NONE				{ KEYWORD_RETURN(NONE); }
normal				{ KEYWORD_RETURN(NORMAL); }
offset				{ KEYWORD_RETURN(OFFSET); }
origin				{ KEYWORD_RETURN(ORIGIN); }
OUTLINE				{ KEYWORD_RETURN(OUTLINE); }
OUTLINE_SHADOW		{ KEYWORD_RETURN(OUTLINE_SHADOW); }
OUTLINE_SOFT_SHADOW { KEYWORD_RETURN(OUTLINE_SOFT_SHADOW); }
FAR_SHADOW 		{ KEYWORD_RETURN(FAR_SHADOW); }
FAR_SOFT_SHADOW		{ KEYWORD_RETURN(FAR_SOFT_SHADOW); }
GLOW			{ KEYWORD_RETURN(GLOW); }
part				{ KEYWORD_RETURN(PART); }
parts				{ KEYWORD_RETURN(PARTS); }
PLAIN				{ KEYWORD_RETURN(PLAIN); }
program				{ KEYWORD_RETURN(PROGRAM); }
programs			{ KEYWORD_RETURN(PROGRAMS); }
RAW					{ KEYWORD_RETURN(RAW); }
RECT				{ KEYWORD_RETURN(RECT); }
GROUP				{ KEYWORD_RETURN(TGROUP); }
rel1				{ KEYWORD_RETURN(REL1); }
rel2				{ KEYWORD_RETURN(REL2); }
relative			{ KEYWORD_RETURN(RELATIVE); }
repeat_events		{ KEYWORD_RETURN(REPEAT_EVENTS); }
precise_is_inside		{ KEYWORD_RETURN(PRECISE_IS_INSIDE); }
pointer_mode		{ KEYWORD_RETURN(POINTER_MODE); }
AUTOGRAB		{ KEYWORD_RETURN(AUTOGRAB); }
NOGRAB			{ KEYWORD_RETURN(NOGRAB); }
SHADOW				{ KEYWORD_RETURN(SHADOW); }
signal				{ KEYWORD_RETURN(SIGNAL); }
SIGNAL_EMIT			{ KEYWORD_RETURN(SIGNAL_EMIT); }
SINUSOIDAL			{ KEYWORD_RETURN(SINUSOIDAL); }
size				{ KEYWORD_RETURN(SIZE); }
smooth				{ KEYWORD_RETURN(SMOOTH); }
SOFT_OUTLINE		{ KEYWORD_RETURN(SOFT_OUTLINE); }
SOFT_SHADOW			{ KEYWORD_RETURN(SOFT_SHADOW); }
source				{ KEYWORD_RETURN(SOURCE); }
elipsis				{ KEYWORD_RETURN(ELIPSIS); }
state				{ KEYWORD_RETURN(STATE); }
STATE_SET			{ KEYWORD_RETURN(STATE_SET); }
step				{ KEYWORD_RETURN(STEP); }
SWALLOW				{ KEYWORD_RETURN(SWALLOW); }
GRADIENT			{ KEYWORD_RETURN(GRADIENT); }
gradient			{ KEYWORD_RETURN(GRAD); }
target				{ KEYWORD_RETURN(TARGET); }
TEXT				{ KEYWORD_RETURN(TEXT); }
TEXTBLOCK		{ KEYWORD_RETURN(TEXTBLOCK); }
TILE			{ KEYWORD_RETURN(TILE); }
SCALE			{ KEYWORD_RETURN(SCALE); }
text				{ KEYWORD_RETURN(TEXT); }
text_class			{ KEYWORD_RETURN(TEXT_CLASS); }
text_source			{ KEYWORD_RETURN(TEXT_SOURCE); }
to					{ KEYWORD_RETURN(TO); }
to_x				{ KEYWORD_RETURN(TO_X); }
to_y				{ KEYWORD_RETURN(TO_Y); }
transition			{ KEYWORD_RETURN(TRANSITION); }
tween				{ KEYWORD_RETURN(TWEEN); }
type				{ KEYWORD_RETURN(TYPE); }
VERTICAL			{ KEYWORD_RETURN(VERTICAL); }
visible				{ KEYWORD_RETURN(VISIBLE); }
x					{ KEYWORD_RETURN(X); }
y					{ KEYWORD_RETURN(Y); }

[oO][nN]			{ KEYWORD_RETURN(ON); }
[oO][fF][fF]		{ KEYWORD_RETURN(OFF); }
[tT][rR][uU][eE] 	{ KEYWORD_RETURN(TRUE); }
[fF][aA][lL][sS][eE]	{ KEYWORD_RETURN(FALSE); }

(\"[^\"]*\")*       {{ 
                        char *tmp = NULL;
                        int i = 0;

                        yylval.string = (char *)calloc(yyleng - 1, sizeof(char));

                        for(i = 0; i < yyleng; i++) {
                            if (yytext[i] == '"') {
                                if (tmp != NULL) {
                                    yytext[i] = '\0';
                                    strcat(yylval.string, tmp);
                                    tmp = NULL;
                                }
                                continue;
                            }

                            if (tmp == NULL)
                                tmp = yytext + i;
                        }
                        if (tmp != NULL)
                            strcat(yylval.string, tmp);

                        STRING_RETURN(); 
                    }}
{floating_constant} { FLOAT_RETURN(); }
\+					{ KEYWORD_RETURN(PLUS); }
\-					{ KEYWORD_RETURN(MINUS); }
\*					{ KEYWORD_RETURN(TIMES); }
\/					{ KEYWORD_RETURN(DIVIDE); }
\(					{ KEYWORD_RETURN(OPEN_PAREN); }
\)					{ KEYWORD_RETURN(CLOSE_PAREN); }

\}                  { KEYWORD_RETURN(CLOSE_BRACE); }
\{                  { KEYWORD_RETURN(OPEN_BRACE); }
[:,]		        { KEYWORD_RETURN(COLON); }
;		    	    { KEYWORD_RETURN(SEMICOLON); }
\"		            { KEYWORD_RETURN(QUOTE); }
\.					{ KEYWORD_RETURN(DOT); }
\%                  { KEYWORD_RETURN(PERCENT); }
[ \t]+              { WHITE_SPACE(); }
\n                  { NEW_LINE(); }

\/\/				{ CPP_COMMENT(); }

#[ \t]+[0-9]+[ \t]+\".*\".* {
    /* this handles the #line <num> <name> <something>
	 * so that the lnum and cur_file hold correct 
	 * values for this file
	 */              

	int c;          
	char *txt = yytext;
	char *lpos;     
	int count = 1;
	char *tmp;
	
	c = *txt;
	while(c == '#' || c == ' ') 
		c = *++txt; 

	lpos = txt;
	while(c >= 48 && c <= 57) {
		c = *++txt;
		count ++;
	}       
	
	tmp = (char *)calloc(count + 1, sizeof(char));
	strncpy(tmp, lpos, count);

	/* subtract 1 becase gcc returns the no of the *next* line
	 * and we will add one when we hit the \n anyway */
	engrave_lnum = atoi(tmp) - 1;
	free(tmp);

	while(c == ' ' || c == '\"') 
		c = *++txt;   

	lpos = txt;
	count = 1;
	while(c != '\"') {
		c = *++txt; 
		count ++;
	}
	count --;

	if (engrave_cur_file)
		free(engrave_cur_file);

	engrave_cur_file = (char *)calloc(count + 1, sizeof(char));
	strncpy(engrave_cur_file, lpos, count);

	if (!strcmp(engrave_cur_file, "<stdin>")) {
		free(engrave_cur_file);
		engrave_cur_file = strdup(engrave_filename);
	}
}

#                   { CPP_COMMENT(); }

"/*"                { COMMENT_START(); }

<SC_COMMENT>{
  "/*"              { COMMENT_START(); }
  \n                { NEW_LINE(); }

  "*/"              { COMMENT_END(); }
  .
}

script              { SCRIPT_START(); }


[a-zA-Z][a-zA-Z0-9_]*       {{ 
		/*
		Use this as a last case to capture single-word unquoted strings
		*/
                        yylval.string = (char *)calloc(yyleng - 1, sizeof(char));
                        strcat(yylval.string, yytext);
                        STRING_RETURN(); 
		}} 

<SC_SCRIPT>{
  [^\{\}\n]*        { SCRIPT_PIECE(); }
  \n                { SCRIPT_NEW_LINE(); }
  \{                { SCRIPT_OPEN_BRACE(); }
  \}                { SCRIPT_CLOSE_BRACE(); }
}

.                   { CHAR_RETURN(); }


%%


