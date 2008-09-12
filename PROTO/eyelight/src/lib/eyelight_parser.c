/*
 * @brief parse an elt file and create a Eyelight tree (eyelight_compiler, eyelight_node)
 */


#include "eyelight_parser.h"

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
            fprintf(stderr,"You can't have more than %d block imbricate \n",EYELIGHT_BUFLEN);
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
            fprintf(stderr,"(line %d): you close a block never open \n",compiler->line);
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
            p++;
    }

    if(!tok_start)
        return NULL;


    token = malloc(sizeof(char)*((tok_end-tok_start)+2));
    strncpy(token, tok_start, tok_end - tok_start +1);
    token[tok_end - tok_start + 1] = '\0';
    *new_p = p;
    new_token = eyelight_remove_quote(token);
    EYELIGHT_FREE(token);
    return new_token;
}


/*
 * @brief test if a block "p" is valid in a block "block"
 */
int eyelight_is_valid_block_in_block(Eyelight_Node_Name p, Eyelight_Node_Name block)
{
    int i;
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
    int i;
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
    int i;
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

    int i;
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

    while( (token=eyelight_next_token(compiler,p,end,&p)) )
    {
        if(eyelight_is_open_block_token(token))
        {
            if(!current_node->type==EYELIGHT_NODE_TYPE_BLOCK)
            {
                fprintf(stderr,"(line %d) A block can not be declare here. \n",compiler->line);
                exit(EXIT_FAILURE);
            }
            name = eyelight_name_get(previous_token);
            if(eyelight_is_valid_block_in_block(name, current_node->name))
            {
                current_node = eyelight_node_new(EYELIGHT_NODE_TYPE_BLOCK,name,current_node);
            }
            else
            {
                fprintf(stderr,"(line %d) The type of the block is not specified or invalid\n",compiler->line);
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
                fprintf(stderr,"(line %d) A property can not be declare here. \n",compiler->line);
                exit(EXIT_FAILURE);
            }
            name = eyelight_name_get(previous_token);
            if(eyelight_is_valid_prop_in_block(name,current_node->name))
            {
                current_node = eyelight_node_new(EYELIGHT_NODE_TYPE_PROP, name, current_node);
                value_size = 0;
            }
            else
            {
                fprintf(stderr,"(line %d) The property %s is invalid in a block %d\n",compiler->line,previous_token,current_node->name);
                exit(EXIT_FAILURE);
            }
            EYELIGHT_FREE(previous_token);
        }
        else if(eyelight_is_end_prop_token(token))
        {
            int i;
            if(!current_node->type == EYELIGHT_NODE_TYPE_PROP)
            {
                fprintf(stderr,"(line %d) A value can not be declare here. \n",compiler->line);
                exit (EXIT_FAILURE);
            }

            if(value_size!=eyelight_get_nb_value_of_prop(current_node->name))
            {
                fprintf(stderr,"(line %d) Expected %d values for the property %d but got %d. \n", compiler->line, eyelight_get_nb_value_of_prop(current_node->name),current_node->name,value_size);
                exit(EXIT_FAILURE);
            }
            for(i=0;i<value_size;i++)
            {
                Eyelight_Value_Type type = eyelight_get_type_value_of_prop(current_node->name,i);
                char* end_float=NULL;
                double v=0;
                switch(type)
                {
                    case EYELIGHT_VALUE_TYPE_NONE:
                        fprintf(stderr,"Internal error.\n");
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
                            fprintf(stderr,"(line %d) The %d st value of the property %d has to be a float\n",compiler->line,i,current_node->name);
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
                            fprintf(stderr,"(line %d) The %d st value of the property %d has to be an integer\n",compiler->line,i,current_node->name);
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
                fprintf(stderr,"(line %d) You can't have more than %d values. \n",compiler->line, EYELIGHT_BUFLEN);
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
        fprintf(stderr,"(line %d), you forgot to close this block (line %d).\n",compiler->line, compiler->open_block[i]);
        error = 1;
    }

    if(error)
        exit (EXIT_FAILURE);
}

