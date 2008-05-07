/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_text.h"
#include "ewl_private.h"
#include "ewl_debug.h"
#include "ewl_macros.h"

static void setup_hash();
static int string_is_keyword(Ecore_Hash *keys, const char * string);
static void text_set(Ewl_Text *t, char *text);

enum text_mode
{
        COMMAND,
        C_COMMENT,
        CPP_COMMENT,
        STRING,
        NUMBER,
        PREPROCESSOR,
        REST
};

static Ecore_Hash *key1 = NULL;
static Ecore_Hash *key2 = NULL;

static char *keywords2[] = {
        "auto",
        "char",
        "const",
        "double",
        "enum",
        "extern",
        "float",
        "int",
        "long",
        "register",
        "short",
        "signed",
        "static",
        "struct",
        "typedef",
        "union",
        "unsigned",
        "void",
        "volatile",
        NULL
};

static char *keywords1[] = {
        "break",
        "case",
        "continue",
        "default",
        "do",
        "else",
        "for",
        "goto",
        "if",
        "return",
        "sizeof",
        "switch",
        "while",
        NULL
};

static void
setup_hash()
{

        int i;

        key1 = ecore_hash_new(ecore_str_hash, ecore_str_compare);
        key2 = ecore_hash_new(ecore_str_hash, ecore_str_compare);

        for (i = 0; keywords1[i] != NULL; i++)
                ecore_hash_set(key1, keywords1[i], keywords1[i]);

        for (i = 0; keywords2[i] != NULL; i++)
                ecore_hash_set(key2, keywords2[i], keywords2[i]);
}

Ewl_Widget *
ewl_io_manager_plugin_uri_read(const char *uri)
{
        Ewl_Widget *ret = NULL;
        FILE *file;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(uri, NULL);

        if (!key1)
                setup_hash();


        file = fopen(uri, "r");
        if (file)
        {
                struct stat buf;
                char *str;

                ret = ewl_text_new();
                ewl_text_font_set(EWL_TEXT(ret), "ewl/monospace");

                stat(uri, &buf);
                str = malloc(sizeof(char) * (buf.st_size + 1));
                fread(str, buf.st_size, 1, file);
                str[buf.st_size] = '\0';
                fclose(file);

                text_set(EWL_TEXT(ret), str);
                FREE(str);
        }

        DRETURN_PTR(ret, DLEVEL_STABLE);
}

Ewl_Widget *
ewl_io_manager_plugin_string_read(const char *string)
{
        Ewl_Widget *ret = NULL;
        char *txt;

        DENTER_FUNCTION(DLEVEL_STABLE);

        if (!key1)
                setup_hash();

        ret = ewl_text_new();
        ewl_text_font_set(EWL_TEXT(ret), "ewl/monospace");
        txt = strdup(string);
        text_set(EWL_TEXT(ret), txt);
        free(txt);

        DRETURN_PTR(ret, DLEVEL_STABLE);
}

int
ewl_io_manager_plugin_uri_write(Ewl_Widget *data, const char *uri)
{
        FILE *file;
        int ret = FALSE;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(data, FALSE);
        DCHECK_PARAM_PTR_RET(uri, FALSE);
        DCHECK_TYPE_RET(data, EWL_TEXT_TYPE, FALSE);

        file = fopen(uri, "w");
        if (file)
        {
                char *txt;

                txt = ewl_text_text_get(EWL_TEXT(data));
                fwrite(txt, sizeof(char), strlen(txt), file);

                FREE(txt);
                fclose(file);

                ret = TRUE;
        }

        DRETURN_INT(ret, DLEVEL_STABLE);
}

int
ewl_io_manager_plugin_string_write(Ewl_Widget *data, const char **string)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(data, FALSE);
        DCHECK_TYPE_RET(data, EWL_TEXT_TYPE, FALSE);

        *string = ewl_text_text_get(EWL_TEXT(data));

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static int
string_is_keyword(Ecore_Hash *keys, const char *string)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(string, FALSE);

        DRETURN_INT(((ecore_hash_get(keys, string) != NULL) ? TRUE : FALSE),
                                                        DLEVEL_STABLE);
}

static void
text_set(Ewl_Text *t, char *text)
{
        char *tx1, *tx2;

        tx1 = tx2 = text;
        while (*tx1)
        {
                int mode;
                char remc = '\0';

                /* decide what case we are dealing with */
                if (isalpha(*tx1) || (*tx1 == '_'))
                        mode = COMMAND;

                else if ((*tx1 == '/') && (*(tx1 + 1) == '*'))
                        mode = C_COMMENT;

                else if ((*tx1 == '/') && (*(tx1 + 1) == '/'))
                        mode = CPP_COMMENT;

                else if (*tx1 == '#')
                        mode = PREPROCESSOR;

                else if (isdigit(*tx1))
                        mode = NUMBER;

                else if (*tx1 == '\"' && !( tx1 != text && *(tx1 - 1) == '\\'))
                        mode = STRING;

                else
                        mode = REST;

                /*
                 * finde the end of the mode
                 */
                tx2++;
                switch (mode)
                {
                        case NUMBER:
                        case COMMAND:
                                while(isalnum(*tx2) || (*tx2 == '_'))
                                        tx2++;
                                break;

                        case C_COMMENT:
                                while((*tx2 != '\0') &&
                                                 (!((*tx2 == '*')
                                                        && (*(tx2 + 1) == '/'))))
                                        tx2++;

                                /* skip over the * and / characters */
                                if (*tx2) tx2 += 2;
                                break;

                        case PREPROCESSOR:
                        case CPP_COMMENT:
                                while((*tx2 != '\0') && (*tx2 != '\n'))
                                        tx2++;
                                break;

                        case STRING:
                                while((*tx2 != '\0')
                                                && !((*tx2 == '\"')
                                                        && (*(tx2 - 1) != '\\')))
                                        tx2++;

                                if (*tx2) tx2++;
                                break;

                        case REST:
                                while(!isalnum(*tx2) && (*tx2 != '\0')
                                                && (*tx2 != '#')
                                                && (*tx2 != '/')
                                                && (*tx2 != '\"'))
                                        tx2++;
                                break;

                        default:
                                break;
                }

                /* append the text */
                remc = *tx2;
                *tx2 = '\0';

                if (mode == COMMAND && ((tx2 - tx1) < 8) &&
                                        string_is_keyword(key1, tx1))
                        ewl_text_color_set(t, 68, 144, 169, 255);

                else if ((mode == COMMAND) && ((tx2 - tx1) < 8) &&
                                        string_is_keyword(key2, tx1))
                        ewl_text_color_set(t, 26, 117, 3, 255);

                else if (mode == STRING)
                        ewl_text_color_set(t, 217, 0, 0, 255);

                else if ((mode == C_COMMENT) || (mode == CPP_COMMENT))
                        ewl_text_color_set(t, 106, 15, 159, 255);

                else if (mode == PREPROCESSOR)
                        ewl_text_color_set(t, 153, 85, 29, 255);

                else if (mode == NUMBER)
                        ewl_text_color_set(t, 255, 114, 0, 255);

                else
                        ewl_text_color_set(t, 0, 0, 0, 255);

                ewl_text_text_append(t, tx1);

                *tx2 = remc;
                tx1 = tx2;
        }
}

