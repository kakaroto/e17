/* vim: set sw=4 ts=4 sts=4 expandtab: */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "eli_help_reader.h"

#define ELI_HELP_READER_NODE_MAX 12

typedef struct Eli_Help_Reader_Stack Eli_Help_Reader_Stack;

struct Eli_Help_Reader_Stack
{
    Eli_Help_Reader_Node nodes[ELI_HELP_READER_NODE_MAX];
    size_t len;
};

static char eli_help_reader_char_get(FILE * file, unsigned int *skip_space);
static char eli_help_reader_text_get(FILE * file, char *buffer, size_t len,
                                                    unsigned int *skip_space);
static char eli_help_reader_type_get(FILE * file, char * buffer, size_t len,
                                    Eli_Help_Reader_Node_Type * type);
static char eli_help_reader_option_get(FILE * file, char * buffer, size_t len);
static char eli_help_reader_preformated_get(FILE * file, char * buffer, size_t len);

static void eli_help_reader_stack_push(Eli_Help_Reader_Stack *stack,
                                    Eli_Help_Reader_Node_Type type,
                                    const char *parameter);
static void eli_help_reader_stack_pop(Eli_Help_Reader_Stack *stack);


/*
 * External functions
 */
int
eli_help_reader_parse(const char *path, Eli_Help_Reader_Enter enter,
                        Eli_Help_Reader_Append append, 
                        Eli_Help_Reader_Leave leave, void *data)
{
    FILE *file;
    Eli_Help_Reader_Stack stack;
    char buffer[4096];
    unsigned int skip = 1;
    char last;

    /* init stack */
    stack.len = 0;

    /* open the file */
    file = fopen(path, "r");
    if (!file)
        return 0;

    do {
        last = eli_help_reader_text_get(file, buffer, sizeof(buffer), &skip);

        if (last == '{')
        {
            Eli_Help_Reader_Node_Type type;
            int param = 0;

            if (stack.len > 0)
                append(data, buffer, stack.nodes, stack.len);

            last = eli_help_reader_type_get(file, buffer, sizeof(buffer),&type);
            if (last == '\0')
                break;
            else if (last == '[')
            {
                last = eli_help_reader_option_get(file, buffer, sizeof(buffer));
                if (last == '\0')
                    break;
                param = 1;
            }
            eli_help_reader_stack_push(&stack, type, param ? buffer : NULL);
            enter(data, stack.nodes, stack.len);
            /* if there is no text */
            if (last == '}')
            {
                leave(data, stack.nodes, stack.len);
                eli_help_reader_stack_pop(&stack);
            }
            /* we need to treat the preformated text specailly to not loose
             * the white spaces */
            if (type == ELI_HELP_READER_NODE_PREFORMATED)
            {
                do {
                    last = eli_help_reader_preformated_get(file, buffer,
                                                            sizeof(buffer));
                    append(data, buffer, stack.nodes, stack.len);
                }
                while (last != '}' && last != '\0');
                leave(data, stack.nodes, stack.len);
                eli_help_reader_stack_pop(&stack);

                if (last == '\0')
                    break;
            }
            skip = 1;
        }
        else if (last == '}')
        {
            if (stack.len > 0)
            {
                append(data, buffer, stack.nodes, stack.len);
                leave(data, stack.nodes, stack.len);
            }
            eli_help_reader_stack_pop(&stack);
        }
        else if (last == ' ')
            skip = 1;
    } while (last != '\0');

    /* clear the stack */
    while (stack.len)
        eli_help_reader_stack_pop(&stack);

    return 1;
}

/* 
 * Internal functions
 */
static char 
eli_help_reader_char_get(FILE * file, unsigned int * skip_space)
{
    char ret;
    int c;

    if (!*skip_space)
    {
        c = fgetc(file);
        ret = (c != EOF) ? c : '\0';
        if (isspace(ret))
        {
            ret = ' ';
            *skip_space = 1;
        }
    }
    else
    {
        do {
            c = fgetc(file);
            ret = (c != EOF) ? c : '\0';
        } while(isspace(ret));
        *skip_space = 0;
    }

    return ret;
}

static char
eli_help_reader_text_get(FILE * file, char *buffer, size_t len,
                                                    unsigned int *skip_space)
{
    size_t i = 0;

    /* we need to add a '\0' at the end */
    len--;
    while (i < len)
    {
        buffer[i] = eli_help_reader_char_get(file, skip_space);
        if (buffer[i] == '}' || buffer[i] == '\0')
        {
            char ret = buffer[i];
            if (i != 0 && buffer[i-1] == ' ')
                buffer[i-1] = '\0';
            else
                buffer[i] = '\0';

            return ret;
        }
        else if (buffer[i] == '{')
        {
            buffer[i] = '\0';
            return '{';
        }
        i++;
    }
    buffer[i] = 0;
    return buffer[i-1];
}

static char
eli_help_reader_type_get(FILE * file, char * buffer, size_t len,
                            Eli_Help_Reader_Node_Type * type)
{
    size_t i = 0;
    unsigned int skip = 0;
    char ret;

    ret = eli_help_reader_char_get(file, &skip);
    if (ret != '\\') 
    {
        *type = ELI_HELP_READER_NODE_UNKNOWN;
        fprintf(stderr, "help reader: brace with out \\\n");
        return ret;
    }

    while (i < len)
    {
        skip = 0;
        buffer[i] = eli_help_reader_char_get(file, &skip);
        if (!isalpha(buffer[i]))
                break;
        i++;
    }

    ret = buffer[i];
    buffer[i] = '\0';

    if (!strcmp(buffer, "h"))
        *type = ELI_HELP_READER_NODE_HEADER;
    else if (!strcmp(buffer, "p"))
        *type = ELI_HELP_READER_NODE_PARAGRAPH;
    else if (!strcmp(buffer, "a"))
        *type = ELI_HELP_READER_NODE_LINK;
    else if (!strcmp(buffer, "em"))
        *type = ELI_HELP_READER_NODE_EMPHASIZE;
    else if (!strcmp(buffer, "st"))
        *type = ELI_HELP_READER_NODE_STRONG;
    else if (!strcmp(buffer, "noip"))
        *type = ELI_HELP_READER_NODE_BLOCK;
    else if (!strcmp(buffer, "pre"))
        *type = ELI_HELP_READER_NODE_PREFORMATED;
    else if (!strcmp(buffer, "title"))
        *type = ELI_HELP_READER_NODE_TITLE;
    else
    {
        fprintf(stderr, "help reader: Didn't find a type for: [%s]", buffer);
        *type = ELI_HELP_READER_NODE_UNKNOWN;
    }

    return ret;
}

static char
eli_help_reader_option_get(FILE * file, char * buffer, size_t len)
{
    size_t i = 0;
    unsigned int skip = 1;
    char ret;

    while (i < len)
    {
        buffer[i] = eli_help_reader_char_get(file, &skip);
        if (buffer[i] == '\0' || buffer[i] == ']')
                break;
        i++;
    }

    if (buffer[i] == ']' && i > 0 && buffer[i - 1] == ' ')
        buffer[i - 1] = '\0';

    ret = buffer[i];
    buffer[i] = '\0';

    return ret;
}

static char
eli_help_reader_preformated_get(FILE * file, char * buffer, size_t len)
{
    size_t i = 0;
    int c;
    char ret;

    while (i < len)
    {
        c = fgetc(file);
        buffer[i] = (c == EOF) ? '\0' : c;
        if (buffer[i] == '\0' || buffer[i] == '}')
                break;
        i++;
    }

    ret = buffer[i];
    buffer[i] = '\0';

    return ret;
}


/* stack functions */
static void
eli_help_reader_stack_push(Eli_Help_Reader_Stack *stack,
                                    Eli_Help_Reader_Node_Type type,
                                    const char *parameter)
{
    Eli_Help_Reader_Node *n;

    if (stack->len == ELI_HELP_READER_NODE_MAX)
        return;
    stack->len++;
    n = &stack->nodes[stack->len -1];

    n->type = type;
    if (parameter)
        n->parameter = strdup(parameter);
    else
        n->parameter = NULL;
}

static void
eli_help_reader_stack_pop(Eli_Help_Reader_Stack *stack)
{
    Eli_Help_Reader_Node *n = &stack->nodes[stack->len - 1];

    if (n->parameter)
    {
        free(n->parameter);
        n->parameter = NULL;
    }

    if (stack->len > 0)
        stack->len--;
}

