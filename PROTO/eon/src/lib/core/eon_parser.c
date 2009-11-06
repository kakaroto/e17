/* EON - Canvas and Toolkit library
 * Copyright (C) 2008-2009 Jorge Luis Zapata
 * Thanks to pancake <http://nopcode.org> for the code
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#include "Eon.h"
#include "eon_private.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/* The new parser will parse something like this
 * object {
 *   prop = object | value
 * }
 *
 */
#define PARSER_MAX_LEVEL 256

typedef struct _Parser Parser;
typedef struct _Parser_State Parser_State;

struct _Parser
{
	Ekeko_Object *parent;
	Eon_Document *doc;
	char last[PARSER_MAX_LEVEL];
};

typedef enum _Parser_Mode {
	MODE_PARSE,
	MODE_OPERATOR,
	MODE_COMMENT,
	MODE_STRING,
} Parser_Mode;

struct _Parser_State {
	Parser_Mode mode;
	int line;
	int level;
	int levels[PARSER_MAX_LEVEL];
	int lastchar;
	char skipuntil;
	char endch;
	char curchar;
	char str[PARSER_MAX_LEVEL];
	int stridx;
	void (*cb_word)(Parser_State *st);
	void (*cb_level)(Parser_State *st, int delta, char ch);
	int (*cb_error)(Parser_State *st, const char *str);
	Parser parser;
};

static int _parse_is_operator(char ch)
{
	return (ch == '+'|| ch == '-' || ch == '&' || ch =='=' || ch == '&' ||
			ch == '~' || ch == '>'|| ch == '|'|| ch == '^' || 
			ch == '/' || ch =='%' || ch == '*' || ch == '!' ||
			ch == '<');
}

static void _parse_pushword(Parser_State *st)
{
	if (st->stridx == 0 && st->mode != MODE_STRING)
		return;

	st->str[st->stridx] = 0;
	st->cb_word(st);
	memset(&st->str, 0, sizeof(st->str));
	st->stridx = 0;
}

static int _parse_concatchar(Parser_State *st, char ch)
{
	st->str[st->stridx++] = ch;
	if (st->stridx >= PARSER_MAX_LEVEL)
		return st->cb_error(st, "Too long string"); //, st->str);
	return 1; // return 0 makes it fail
}

static int parse_char(Parser_State *st, char ch)
{
	int ret = 1;

	if (ch == 0)
		return 0;

	if (st->skipuntil) {
		if (ch == st->skipuntil)
			st->skipuntil = 0;
		else return 1;
	}
	st->curchar = ch;
	switch (st->mode) {
	case MODE_PARSE:
		switch (ch) {
		case '"':
		case '\'':
			if (st->lastchar == '\\')
				_parse_concatchar(st, ch);
			else st->mode = MODE_STRING;
			st->endch = ch;
			break;
		case '#':
			st->skipuntil = '\n';
			break;
		case '[':
		case '(':
		case '{':
			_parse_pushword(st);
			st->cb_level(st, 1, ch);
			switch(ch) {
			case '{': st->levels[st->level] = '}'; break;
			case '(': st->levels[st->level] = ')'; break;
			case '[': st->levels[st->level] = ']'; break;
			}
			st->level++;
			if (st->level > PARSER_MAX_LEVEL)
				return st->cb_error (st, "Too much recursivity");
			break;
		case ']':
		case ')':
		case '}':
			_parse_pushword(st);
			st->level--;
			if (st->level < 0)
				return st->cb_error (st, "Level underflow\n");
			if (st->levels[st->level] != ch)
				return st->cb_error (st,
					"Unexpected closing parentesis.\n");
			st->cb_level(st, -1, ch);
			break;
		case '\n':
			st->line++;
		case ' ':
		case ',':
		case ';':
		case ':':
		//case '\'':
		case '\t':
		case '\r':
			_parse_pushword(st);
			break;
		default:
			if (st->lastchar == '/') {
				switch(ch) {
				case '*':
					st->mode = MODE_COMMENT;
					break;
				case '/':
					st->skipuntil = '\n';
					break;
				}
			} else {
				if (_parse_is_operator(ch)) {
					_parse_pushword(st);
					st->mode = MODE_OPERATOR; // XXX dupped
					return parse_char(st, ch);
				} else _parse_concatchar(st, ch);
			}
			break;
		}
		break;
	case MODE_OPERATOR:
		if (st->lastchar == '/') {
			switch(ch) {
			case '*':
				st->mode = MODE_COMMENT;
				st->stridx = 0;
				break;
			case '/':
				st->skipuntil = '\n';
				st->mode = MODE_COMMENT;
				st->stridx = 0;
				break;
			}
		}
		if (st->mode != MODE_OPERATOR || !_parse_is_operator(ch)) {
			st->mode = MODE_PARSE;
			_parse_pushword(st);
			// XXX: check if return here is ok
			return parse_char(st, ch);
		} else st->str[st->stridx++] = ch; //return parse_char (st, ch);
		break;
	case MODE_COMMENT:
		if (ch == '/' && st->lastchar == '*') {
			st->mode = MODE_PARSE;
			st->stridx = 0;
			ch = 0; // workaround for default->lastchar==/
		}
		break;
	case MODE_STRING:
		if (st->lastchar == '\\') {
			switch (ch) {
			case 'n': ch = '\n'; break;
			case 't': ch = '\t'; break;
			case 'r': ch = '\r'; break;
			case 'e': ch = '\x1b'; break;
			case 'x': ch = 'x'; break; // TODO: fix support for binary stuff
			case '\\': ch = '\\'; break;
			case '\'': ch = '\''; break;
			case '\"': ch = '\"'; break;
			default: //return st->cb_error(st,
				printf("Invalid escaped char '%c'\n", ch);
			}
			st->str[st->stridx-1] = ch;
			if (ch == '\\') ch = 0;
		} else if (ch == st->endch) {
			_parse_pushword(st);
			st->mode = MODE_PARSE;
		} else ret = _parse_concatchar(st, ch);
		break;
	}
	st->lastchar = ch;
	return ret;
}

static void _cb_word(Parser_State *st)
{
	printf("word %s\n", st->str);
}

static void _cb_level(Parser_State *st, int delta, char ch)
{
	if (delta > 0)
		printf("level %s {\n", st->str);
	else
		printf("level }\n");
}

static int _cb_error(Parser_State *st, const char *str)
{
	printf("parser error %s\n", str);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * @param doc The Document
 * @param o The root object
 * @param file The file to parse
 */
EAPI Eina_Bool eon_parser_load(Eon_Document *doc, Ekeko_Object **o, const char *file)
{
	Eon_Parser *p;
	Parser_State st;
	int fd;
	int ret;
	char buf[1024];

	if (!o || !*o || !file || !*file)
		return EINA_FALSE;

	fd = open(file, O_RDONLY);
	if (fd < 0)
		return EINA_FALSE;

	memset (&st, 0, sizeof (Parser_State));
	st.cb_error = _cb_error;
	st.cb_word = _cb_word;
	st.cb_level = _cb_level;
	st.parser.doc = doc;
	st.parser.parent = o;

	do {
		int i;

		ret = read(fd, buf, 1024);
		for (i = 0; i < ret; i++)
			if (!parse_char(&st, buf[i]))
				break;
	} while (ret > 0);

	ekeko_object_dump(*o, ekeko_object_dump_printf);
	close(fd);

	return EINA_TRUE;
}
