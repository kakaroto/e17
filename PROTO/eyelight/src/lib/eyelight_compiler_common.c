/*
 * =====================================================================================
 *
 *       Filename:  eyelight_compiler_common.c
 *
 *    Description:  some functions use during the compilation
 *        Version:  1.0
 *        Created:  11/07/08 12:54:05 UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#include "eyelight_compiler_common.h"


/*
 * @brief create and edj file from an edc file
 */
Eyelight_Compiler* eyelight_elt_load(char *input_file)
{
    FILE* output;
    char* end;
    char* p;
    char buf[EYELIGHT_BUFLEN];
    char* output_file;

    Eyelight_Compiler* compiler = eyelight_compiler_new(input_file, 0);

    char* path = ecore_file_dir_get(input_file);
    snprintf(buf,EYELIGHT_BUFLEN,"%s/%s",path,ecore_file_file_get(input_file));
    buf[strlen(buf)-2] = 'd';
    buf[strlen(buf)-1] = 'c';

    compiler->output_file = strdup(buf);
    output_file = strdup(buf);

    p = eyelight_source_fetch(compiler->input_file,&end);
    eyelight_parse(compiler,p,end);

    return compiler;
}

/*
 * @brief get the name of a token
 */
Eyelight_Node_Name eyelight_name_get(char* p)
{
    int i;
    for (i = 0; i < sizeof (eyelight_name_keys) / sizeof (Eyelight_Name_Key); ++i)
        if (strcmp(p, eyelight_name_keys[i].keyname) == 0)
        {
            return eyelight_name_keys[i].name;
        }

    return EYELIGHT_NAME_NONE;
}

/*
 * @brief create a new compiler
 */
Eyelight_Compiler* eyelight_compiler_new(char* input_file, int display_areas)
{
    Eyelight_Compiler* compiler;
    compiler = calloc(1,sizeof(Eyelight_Compiler));
    compiler->line = 1;
    compiler->last_open_block = -1;
    compiler->root = eyelight_node_new(EYELIGHT_NODE_TYPE_BLOCK,EYELIGHT_NAME_ROOT,NULL);

    //special summary slide
    compiler->node_summary = eyelight_node_new(EYELIGHT_NODE_TYPE_BLOCK,EYELIGHT_NAME_SLIDE,NULL);
    Eyelight_Node *node = eyelight_node_new(EYELIGHT_NODE_TYPE_PROP,EYELIGHT_NAME_LAYOUT,
            compiler->node_summary);
    node = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE,EYELIGHT_NAME_NONE,node);
    node->value = strdup("summary");

    node = eyelight_node_new(EYELIGHT_NODE_TYPE_PROP,EYELIGHT_NAME_TITLE,
            compiler->node_summary);
    node = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE,EYELIGHT_NAME_NONE,node);
    node->value = strdup(" Summary ");

    node = eyelight_node_new(EYELIGHT_NODE_TYPE_PROP,EYELIGHT_NAME_SUBTITLE,
            compiler->node_summary);
    node = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE,EYELIGHT_NAME_NONE,node);
    node->value = strdup("");

    Eyelight_Node *node_area = eyelight_node_new(EYELIGHT_NODE_TYPE_BLOCK,EYELIGHT_NAME_AREA,
            compiler->node_summary);
    node = eyelight_node_new(EYELIGHT_NODE_TYPE_PROP,EYELIGHT_NAME_NAME,
            node_area);
    node = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE,EYELIGHT_NAME_NONE,node);
    node->value = strdup("summary");

    compiler->node_summary_items = eyelight_node_new(EYELIGHT_NODE_TYPE_BLOCK,EYELIGHT_NAME_ITEMS,
            node_area);
    //

    compiler->input_file = strdup(input_file);
    compiler-> display_areas = display_areas;

    return compiler;
}

/*
 * @brief free a compiler
 */
void eyelight_compiler_free(Eyelight_Compiler **p_compiler)
{
    Eyelight_Compiler* compiler = *p_compiler;
    char* str;

    eyelight_node_free(&(compiler->root),compiler->node_summary);
    eyelight_node_free(&(compiler->node_summary),NULL);

    EYELIGHT_FREE(compiler->input_file);
    EYELIGHT_FREE(compiler->output_file);

    EYELIGHT_FREE(compiler);
}

/*
 * @brief create a new node
 */
Eyelight_Node *eyelight_node_new(int type,Eyelight_Node_Name name, Eyelight_Node* father)
{
    Eyelight_Node* node = calloc(1,sizeof(Eyelight_Node));
    node->type = type;
    node->father = father;
    node->name = name;
    if(father)
    {
        father->l = eina_list_append(father->l,node);
    }

    return node;
}

/*
 * @brief free a node
 */
void eyelight_node_free(Eyelight_Node** current, Eyelight_Node *not_free)
{
    Eyelight_Node* node;
    Eina_List *l;

    if((*current)->type==EYELIGHT_NODE_TYPE_VALUE)
        EYELIGHT_FREE((*current)->value);

    EINA_LIST_FOREACH( (*current)->l ,l ,node)
        if(node != not_free)
            eyelight_node_free(&node, not_free);

    eina_list_free( (*current)->l );
    (*current)-> l = NULL;
    EYELIGHT_FREE(*current);
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
 * @brief load a file into an array
 */
char* eyelight_source_fetch(char* file, char** p_end)
{
    FILE* input;
    char buf[EYELIGHT_BUFLEN];
    int size = 2000;
    int size_read = 0;
    char * p = malloc(sizeof(char)* size);
    char * end = p;

    input = fopen(file,"r");
    if(!input)
    {
        fprintf(stderr,"Can't open the file %s\n",file);
        exit(EXIT_FAILURE);
    }

    while(fgets(buf,EYELIGHT_BUFLEN,input))
    {
        size_read += strlen(buf);
        if(size_read > size)
        {
            char* p_new;
            size+=2000;
            p_new = realloc(p,size*sizeof(char));
            end = p_new + (end-p);
            p = p_new;
        }
        strncpy(end,buf,strlen(buf));
        end += strlen(buf);
    }

    *p_end = end;

    //fclose(input);
    return p;
}

/*
 * return the ith value of the property "node"
 */
char* eyelight_retrieve_value_of_prop(Eyelight_Node* node,int i)
{
    Eyelight_Node *data;
    data = eina_list_nth(node->l,i);
    return data->value;
}

/*
 * @brief return node of the property "prop" of the block node "current"
 */
Eyelight_Node* eyelight_retrieve_node_prop(Eyelight_Node* current, Eyelight_Node_Name prop)
{
    Eyelight_Node* node = NULL;
    int find = 0;
    Eina_List *l;

    l = current->l;
    while(!find && l)
    {
        node = eina_list_data_get(l);
        if(node->type == EYELIGHT_NODE_TYPE_PROP && node->name == prop)
            find = 1;
        l=eina_list_next(l);
    }
    if(!find)
        return NULL;
    else
        return node;
}

/*
 * @brief return the number of items in a block and his sub blocks
 * this function is used to know how many items will be add in an area
 */
int eyelight_number_item_in_block(Eyelight_Node* current)
{
    int number = 0;
    Eyelight_Node* node;
    Eina_List *l;

    EINA_LIST_FOREACH(current->l, l, node)
    {
        if(node->type == EYELIGHT_NODE_TYPE_BLOCK && node->name == EYELIGHT_NAME_ITEMS)
            number+=eyelight_number_item_in_block(node);
        else if(node->type==EYELIGHT_NODE_TYPE_BLOCK &&
                  ( node->name == EYELIGHT_NAME_ITEM
                  || node->name == EYELIGHT_NAME_IMAGE
                  || node->name == EYELIGHT_NAME_EDJ))
            number++;
        else if(node->type == EYELIGHT_NODE_TYPE_PROP &&
                  ( node->name == EYELIGHT_NAME_TEXT
                  || node->name == EYELIGHT_NAME_IMAGE))
            number++;
    }
    return number;
}

/*
 * @brief Originally written by magma
 * translate a decimal value into a roman value
 * source: http://files.codes-sources.com/fichier.aspx?id=31354&f=Romain.c
 */
int eyelight_decimal_to_roman(char *dec, char *rom)
{
    int len, i;
    char *unite[] = {"", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"};
    char *dizaine[] = {"", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC"};
    char *centaine[] = {"", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"};
    char *millier[] = {"", "M", "MM", "MMM"};

    if(dec[0] == '0')
    {
        strcpy(rom, "Nombre invalide [1->3999] !");
        return 0;
    }

    len = strlen(dec);
    strcpy(rom, "");

    for(i = 0; i < len; i++)
    {
        if(dec[i] >= '0' && dec[i] <= '9')
        {
            switch(len - i)
            {
                case 1:
                    strcat(rom, unite[dec[i] - '0']);
                    break;

                case 2:
                    strcat(rom, dizaine[dec[i] - '0']);
                    break;

                case 3:
                    strcat(rom, centaine[dec[i] - '0']);
                    break;

                case 4:
                    if(dec[0] <= '3')
                        strcpy(rom, millier[dec[i] - '0']);
                    else {
                        strcpy(rom, "Nombre invalide [1->3999] !");
                        return 0;
                    }
                    break;

                default:
                    strcpy(rom, "Nombre invalide [1->3999] !");
                    return 0;
            }
        }
        else
        {
            strcpy(rom, "Nombre invalide [1->3999] !");
            return 0;
        }
    }
    return 1;
}

