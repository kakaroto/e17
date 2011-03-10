/*
 * @brief parse an elt file and create a Eyelight tree (eyelight_compiler, eyelight_node)
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <locale.h>

#include "eyelight_compiler_parser.h"

static void _eyelight_save(Eyelight_Node *node, FILE *file, char *tabs);


/*
 * @brief get the name of a token
 */
Eyelight_Node_Name eyelight_name_get(char* p)
{
    unsigned int i;
    for (i = 0; i < sizeof (eyelight_name_keys) / sizeof (Eyelight_Name_Key); ++i)
        if (strcmp(p, eyelight_name_keys[i].keyname) == 0)
        {
            return eyelight_name_keys[i].name;
        }

    return EYELIGHT_NAME_NONE;
}

/*
 * @brief Get the string which correspond to the id
 */
const char *eyelight_string_name_get(Eyelight_Node_Name name)
{
    unsigned int i;
    for (i = 0; i < sizeof (eyelight_name_keys) / sizeof (Eyelight_Name_Key); ++i)
        if (eyelight_name_keys[i].name == name)
        {
            return eyelight_name_keys[i].keyname;
        }

    return NULL;
}

/*
 * @brief remove the first and last " of a string
 */
char* eyelight_remove_quote(char* p)
{
    if(*p=='"')
    {
        char* new = malloc(sizeof(char)*(strlen(p)+1-2));
        *new = '\0';
        strncat(new,p+1,strlen(p)-2);
        new[strlen(p)-2] = '\0';
        return new;
    }
    else
        return strdup(p);
}

/*
 * @brief replace \n by <br>
 */
char* eyelight_remove_slashn(char* p)
{
    int count = 0;
    unsigned int i,j;

    for(i=0; i<strlen(p);i++)
    {
        if(p[i]=='\n')
            count++;
    }

    char* new = malloc(sizeof(char)*(strlen(p)+1+count*3));

    i=0;
    j=0;
    while(p[i]!='\0')
    {
        if(p[i]=='\n')
        {
            new[j++] = '<';
            new[j++] = 'b';
            new[j++] = 'r';
            new[j++] = '>';
            i++;
        }
        else
            new[j++] = p[i++];
    }
    new[j]='\0';
    return new;
}


/*
 * @brief test if a char is a token (; : ....)
 */
int eyelight_is_char_token(char* p)
{
    if(*p == '{' || *p=='}' || *p==';' || *p==':')
        return 1;
    else
        return 0;
}

/*
 * @brief test if a char is the end of a token
 */
int eyelight_is_end_token(char* p)
{
    if(eyelight_is_char_token(p) || *p == '\n' || *p==' ')
        return 1;
    else
        return 0;
}

/*
 * @brief test if we start/end a token with spaces
 */
int eyelight_is_string_token(char* p)
{
    return *p=='"';
}

/*
 * @brief test if a char is an open block ( { )
 */
void eyelight_if_open_block(Eyelight_Compiler *compiler,char* p)
{
    if(eyelight_is_open_block_token(p))
    {
        compiler->last_open_block++;
        if(compiler->last_open_block>=EYELIGHT_BUFLEN)
        {
            ERR("You can't have more than %d block imbricate ",EYELIGHT_BUFLEN);
            exit (EXIT_FAILURE);
        }

        compiler->open_block[compiler->last_open_block] = compiler->line;
    }
}

/*
 * @brief test if a char is a close block ( } )
 */
void eyelight_if_close_block(Eyelight_Compiler *compiler,char* p)
{
    if(eyelight_is_close_block_token(p))
    {
        compiler->last_open_block--;
        if(compiler->last_open_block<-1)
        {
            ERR("(line %d): you close a block never open ",compiler->line);
            exit (EXIT_FAILURE);
        }
    }
}

/*
 * @brief test if the token start a property
 */
int eyelight_is_start_prop_token(char* p)
{
    return *p==':';
}
/*
 * @brief test if the token end a property
 */
int eyelight_is_end_prop_token(char* p)
{
    return *p == ';';
}

/*
 * @brief test if a char is an open block token
 */
int eyelight_is_open_block_token(char* p)
{
    return *p=='{';
}

/*
 * @brief test if a char is a close block token
 */
int eyelight_is_close_block_token(char* p)
{
    return *p=='}';
}

/*
 * @brief browse in the file and return a token
 */
char* eyelight_next_token(Eyelight_Compiler* compiler,char *p, char *end, char **new_p)
{
    char* tok_start= NULL;
    char* tok_end = NULL;
    char* token,*new_token;
    int done = 0;
    int string = 0;
    int in_comment = 0;

    if(p>=end)
        return NULL;
    while(!done && p<end)
    {
        if(!in_comment && tok_start && tok_end - tok_start == 1 && *tok_start=='/' && *tok_end=='/')
        {
            in_comment = 1;
            p++;
        }
        else if(in_comment && *p=='\n')
        {
            in_comment = 0;
            tok_start = NULL;
            compiler->line++;
            p++;
        }
        else if(!in_comment && !tok_start && (*p ==' ' || *p == '\t'))
            p++;
        else if(!in_comment && !tok_start && *p=='\n')
        {
            compiler->line++;
            p++;
        }
        else if( !in_comment && !tok_start || !eyelight_is_end_token(p) || string)
        {
            if(!tok_start)
            {
                tok_start = p;
                tok_end = p;
                if(eyelight_is_char_token(p))
                {
                    eyelight_if_open_block(compiler,p);
                    eyelight_if_close_block(compiler,p);
                    done = 1;
                }
                else if(eyelight_is_string_token(p))
                    string = 1;
            }
            else
            {
                tok_end = p;
                if(string && eyelight_is_string_token(p))
                    done = 1;
            }
            p++;
        }
        else if(!in_comment)
            done = 1;
        else
	{
	   if (*p == '\n')
	     compiler->line++;

	   p++;
	}
    }

    if(!tok_start)
        return NULL;


    token = malloc(sizeof(char)*((tok_end-tok_start)+2));
    strncpy(token, tok_start, tok_end - tok_start +1);
    token[tok_end - tok_start + 1] = '\0';
    *new_p = p;
    new_token = eyelight_remove_quote(token);
    EYELIGHT_FREE(token);
    token = new_token;
    new_token = eyelight_remove_slashn(token);
    EYELIGHT_FREE(token);

    return new_token;
}


/*
 * @brief test if a block "p" is valid in a block "block"
 */
int eyelight_is_valid_block_in_block(Eyelight_Node_Name p, Eyelight_Node_Name block)
{
    unsigned int i;
    for (i = 0; i < sizeof (eyelight_valid_block_block) / sizeof (Eyelight_Valid_Prop_Block); ++i)
        if (block ==  eyelight_valid_block_block[i].block
                && p == eyelight_valid_block_block[i].block_prop)
        {
            return 1;
        }

    return 0;
}

/*
 * @brief test if a property "prop" is valid in a block "block"
 */
int eyelight_is_valid_prop_in_block(Eyelight_Node_Name prop, Eyelight_Node_Name block)
{
    unsigned int i;
    for (i = 0; i < sizeof (eyelight_valid_prop_block) / sizeof (Eyelight_Valid_Prop_Block); ++i)
        if (block ==  eyelight_valid_prop_block[i].block
                && prop == eyelight_valid_prop_block[i].block_prop)
        {
            return 1;
        }

    return 0;
}

/*
 * @brief return the number of values of the property "prop"
 */
int eyelight_get_nb_value_of_prop(Eyelight_Node_Name prop)
{
    unsigned int i;
    for (i = 0; i < sizeof (eyelight_prop_nb_value) / sizeof (Eyelight_Prop_Nb_Value); ++i)
        if (prop ==  eyelight_prop_nb_value[i].prop)
        {
            return eyelight_prop_nb_value[i].nb;
        }

    return 1;
}


/*
 * @brief return the type of the ith value of the property "prop"
 */
Eyelight_Value_Type eyelight_get_type_value_of_prop(Eyelight_Node_Name prop, int pos)
{
    Eyelight_Value_Type type = EYELIGHT_VALUE_TYPE_STRING;

    unsigned int i;
    for (i = 0; i < sizeof (eyelight_prop_value_type) / sizeof (Eyelight_Prop_Value_Type); ++i)
        if (prop ==  eyelight_prop_value_type[i].name
                && pos == eyelight_prop_value_type[i].arg_pos)
        {
            type = eyelight_prop_value_type[i].type;
            break;
        }
    return type;
}

/*
 * @brief main function, parse an elt file
 */
void eyelight_parse(Eyelight_Compiler* compiler,char *p, char* end)
{
    char* token;
    int i;
    int error = 0;

    Eyelight_Node_Name name;
    char* previous_token = NULL;
    Eyelight_Node* current_node = compiler->root;

    int value_size = -1;
    char* value_list[EYELIGHT_BUFLEN];

    //just to be sure we use the '.' as decimal separator
    setlocale(LC_NUMERIC,"C");
    while( (token=eyelight_next_token(compiler,p,end,&p)) )
    {
        if(eyelight_is_open_block_token(token))
        {
            if(!current_node->type==EYELIGHT_NODE_TYPE_BLOCK)
            {
                ERR("(line %d) A block can not be declare here. ",compiler->line);
                exit(EXIT_FAILURE);
            }
            name = eyelight_name_get(previous_token);
            if(eyelight_is_valid_block_in_block(name, current_node->name))
            {
                current_node = eyelight_node_new(EYELIGHT_NODE_TYPE_BLOCK,name,current_node);
            }
            else
            {
                ERR("(line %d) The type of the block is not specified or invalid",compiler->line);
                exit(EXIT_FAILURE);
            }
            EYELIGHT_FREE(previous_token);
        }
        else if(eyelight_is_close_block_token(token))
        {
            current_node = current_node->father;
            EYELIGHT_FREE(previous_token);
        }
        else if(eyelight_is_start_prop_token(token))
        {
            if(!current_node->type==EYELIGHT_NODE_TYPE_BLOCK)
            {
                ERR("(line %d) A property can not be declare here.",compiler->line);
                exit(EXIT_FAILURE);
            }

            name = eyelight_name_get(previous_token);
            Eyelight_Node *old_current_node = current_node;
            current_node = eyelight_node_new(EYELIGHT_NODE_TYPE_PROP, name, current_node);
            value_size = 0;

            if(!eyelight_is_valid_prop_in_block(name,old_current_node->name))
            {
                ERR("(line %d) The property %s is invalid in a block %d",compiler->line,previous_token,current_node->name);
                exit(EXIT_FAILURE);
            }
            EYELIGHT_FREE(previous_token);
        }
        else if(eyelight_is_end_prop_token(token))
        {
            int i;
            if(!current_node->type == EYELIGHT_NODE_TYPE_PROP)
            {
                ERR("(line %d) A value can not be declare here.",compiler->line);
                exit (EXIT_FAILURE);
            }

            if(value_size!=eyelight_get_nb_value_of_prop(current_node->name))
            {
                ERR("(line %d) Expected %d values for the property %d but got %d.", compiler->line, eyelight_get_nb_value_of_prop(current_node->name),current_node->name,value_size);
                exit(EXIT_FAILURE);
            }
            for(i=0;i<value_size;i++)
            {
                Eyelight_Value_Type type = eyelight_get_type_value_of_prop(current_node->name,i);
                char* end_float=NULL;
                double v;
                switch(type)
                {
                    case EYELIGHT_VALUE_TYPE_NONE:
                        ERR("Internal error.");
                        exit(EXIT_FAILURE);
                        break;

                    case EYELIGHT_VALUE_TYPE_STRING:
                        break;//nothing to do, all string are valid
                    case EYELIGHT_VALUE_TYPE_DOUBLE:
                        errno = 0;
                        v = strtod(value_list[i],&end_float);
                        if(*end_float != '\0'
                                || (errno == ERANGE &&
                                    (v == LONG_MAX || v == LONG_MIN))
                                || (errno != 0 && v == 0))
                        {
                            ERR("(line %d) The %d st value of the property %d has to be a float",compiler->line,i,current_node->name);
                            exit(EXIT_FAILURE);
                        }
                        break;
                    case EYELIGHT_VALUE_TYPE_INT:
                        errno = 0;
                        v = strtol(value_list[i],&end_float,0);
                        if(*end_float != '\0'
                                || (errno == ERANGE &&
                                    (v == LONG_MAX || v == LONG_MIN))
                                || (errno != 0 && v == 0))
                        {
                            ERR("(line %d) The %d st value of the property %d has to be an integer",compiler->line,i,current_node->name);
                            exit(EXIT_FAILURE);
                        }
                        break;
                }

                //save the value in the tree
                char* new = value_list[i];
                Eyelight_Node* n;
                n = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE,EYELIGHT_NAME_NONE,current_node);
                n->value = strdup(new);

                EYELIGHT_FREE(value_list[i]);
            }

            current_node = current_node -> father;
            EYELIGHT_FREE(previous_token);
        }
        else if(current_node->type == EYELIGHT_NODE_TYPE_PROP)
        {
            if(value_size>=EYELIGHT_BUFLEN)
            {
                ERR("(line %d) You can't have more than %d values.",compiler->line, EYELIGHT_BUFLEN);
                exit(EXIT_FAILURE);
            }
            value_list[value_size] = strdup(token);
            value_size++;
        }
        else
        {
            EYELIGHT_FREE(previous_token);
            previous_token = strdup(token);
        }
        EYELIGHT_FREE(token);
    }
    EYELIGHT_FREE(previous_token);
    EYELIGHT_FREE(token);

    for(i=compiler->last_open_block;i>=0;i--)
    {
        ERR("(line %d), you forgot to close this block (line %d).",compiler->line, compiler->open_block[i]);
        error = 1;
    }

    if(error)
        exit (EXIT_FAILURE);
}

/*
 * @brief save the presentation in an elt file
 */
int eyelight_save(Eyelight_Node *root, const char *file)
{
    FILE *f;
    char buf[EYELIGHT_BUFLEN];
    buf[0] = '\0';

    f = fopen(file,"w");
    EYELIGHT_ASSERT_RETURN(!!f);
    _eyelight_save(root,f, buf);
    fclose(f);

    return 1;
}

static void _eyelight_save(Eyelight_Node *node,FILE *file, char *tabs)
{
    Eina_List *l;
    Eyelight_Node *_node;

    switch(node->type)
    {
        case EYELIGHT_NODE_TYPE_BLOCK:
            if(node->name != EYELIGHT_NAME_ROOT)
            {
                fprintf(file, "%s%s {\n", tabs, eyelight_string_name_get(node->name), tabs);
                strcat(tabs,"\t");
            }
            EINA_LIST_FOREACH(node->l, l, _node)
                _eyelight_save(_node, file, tabs);
            if(node->name != EYELIGHT_NAME_ROOT)
            {
                tabs[strlen(tabs)-1]='\0';
                fprintf(file,"%s}\n",tabs);
            }
            break;
        case EYELIGHT_NODE_TYPE_PROP:
            if(eyelight_string_name_get(node->name))
            {
                fprintf(file, "%s%s : ", tabs, eyelight_string_name_get(node->name),tabs);
                EINA_LIST_FOREACH(node->l, l, _node)
                    _eyelight_save(_node, file, tabs);
                fprintf(file,";\n");
            }
            break;
        case EYELIGHT_NODE_TYPE_VALUE:
            fprintf(file, "\"%s\" ", node->value);
            break;
    }
}
