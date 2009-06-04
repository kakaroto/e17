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
char* eyelight_create_edj_from_edc(char* input_file, char* theme_file)
{
    const char* file_name;
    char* folder_edc;
    char* folder_theme;
    char new_edc_file[EYELIGHT_BUFLEN];
    char buf[EYELIGHT_BUFLEN];
    char command[EYELIGHT_BUFLEN];
    Ecore_Exe* exe;
    int status;
    FILE* f_read,*f_write;
    int i;
    char* output_file;

    file_name = ecore_file_file_get(input_file);
    char* path = ecore_file_dir_get(input_file);
    snprintf(buf,EYELIGHT_BUFLEN,"%s/%s",path,file_name);
    buf[strlen(buf)-1] = 'j';
    output_file = strdup(buf);

    //copy the edc file and include the theme file
    i = 1;
    do
    {
        snprintf(new_edc_file,EYELIGHT_BUFLEN,"%s/%s_tmp_%d",path,file_name,i);
        i++;
    }while(ecore_file_exists(new_edc_file));
    f_read = fopen(input_file,"r");
    f_write = fopen(new_edc_file,"w");

    buf[0] = '\0';
    for(i=0;i<strlen(input_file);i++)
        if(input_file[i]=='/')
            strcat(buf,"../");
    fprintf(f_write,"#include \"%s%s\"\n",buf,theme_file);
    while(fgets(buf,EYELIGHT_BUFLEN,f_read))
        fprintf(f_write,"%s",buf);
    fclose(f_read);
    fclose(f_write);

    //create the edje file
    folder_edc= ecore_file_dir_get(input_file);
    folder_theme = ecore_file_dir_get(theme_file);

    snprintf(command,EYELIGHT_BUFLEN,"edje_cc -id %s -id %s -fd %s -fd %s %s -o %s",folder_edc,folder_theme,folder_edc,folder_theme,new_edc_file,output_file);

    EYELIGHT_FREE(folder_theme);
    EYELIGHT_FREE(folder_edc);

    exe = ecore_exe_run(command,NULL);
    waitpid(ecore_exe_pid_get(exe),&status,0);
    remove(new_edc_file);

    EYELIGHT_FREE(path);
    return output_file;
}

/*
 * @brief create an edc file from an elt file
 */
char* eyelight_create_edc_from_elt(char* input_file,int display_areas)
{
    FILE* output;
    char* end;
    char* p;
    char buf[EYELIGHT_BUFLEN];
    char* output_file;

    Eyelight_Compiler* compiler = eyelight_compiler_new(input_file, display_areas);

    char* path = ecore_file_dir_get(input_file);
    snprintf(buf,EYELIGHT_BUFLEN,"%s/%s",path,ecore_file_file_get(input_file));
    buf[strlen(buf)-2] = 'd';
    buf[strlen(buf)-1] = 'c';

    compiler->output_file = strdup(buf);
    output_file = strdup(buf);

    p = eyelight_source_fetch(compiler->input_file,&end);
    eyelight_parse(compiler,p,end);

    output = fopen(buf,"w");
    eyelight_compile(compiler,output);
    fclose(output);

    eyelight_compiler_free(&compiler);
    EYELIGHT_FREE(path);
    EYELIGHT_FREE(p);
    return output_file;
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
    compiler = malloc(sizeof(Eyelight_Compiler));
    compiler->line = 1;
    compiler->image_list = ecore_list_new();
    compiler->edc_files = ecore_list_new();
    compiler->last_open_block = -1;
    compiler->root = eyelight_node_new(EYELIGHT_NODE_TYPE_BLOCK,EYELIGHT_NAME_ROOT,NULL);
    compiler->input_file = strdup(input_file);
    compiler->output_file = NULL;
    compiler->default_foot_text = NULL;
    compiler->default_foot_image = NULL;
    compiler->default_layout = NULL;
    compiler->default_header_image = NULL;
    compiler->default_title = NULL;
    compiler->default_subtitle = NULL;
    compiler->default_transition_previous = NULL;
    compiler->default_transition_next = NULL;
    compiler->default_transition = NULL;
    compiler -> display_areas = display_areas;

    return compiler;
}

/*
 * @brief free a compiler
 */
void eyelight_compiler_free(Eyelight_Compiler **p_compiler)
{
    Eyelight_Compiler* compiler = *p_compiler;
    char* str;

    ecore_list_first_goto(compiler->image_list);
    while( (str = ecore_list_next(compiler->image_list)))
    {
        EYELIGHT_FREE(str);
    }
    ecore_list_destroy(compiler->image_list);
    ecore_list_first_goto(compiler->edc_files);
    while( (str = ecore_list_next(compiler->edc_files)))
    {
        EYELIGHT_FREE(str);
    }
    ecore_list_destroy(compiler->edc_files);

    eyelight_node_free(&(compiler->root));

    EYELIGHT_FREE(compiler->input_file);
    EYELIGHT_FREE(compiler->output_file);
    EYELIGHT_FREE(compiler->default_foot_text);
    EYELIGHT_FREE(compiler->default_foot_image);
    EYELIGHT_FREE(compiler->default_layout);
    EYELIGHT_FREE(compiler->default_header_image);
    EYELIGHT_FREE(compiler->default_title);
    EYELIGHT_FREE(compiler->default_subtitle);
    EYELIGHT_FREE(compiler->default_transition_previous);
    EYELIGHT_FREE(compiler->default_transition_next);
    EYELIGHT_FREE(compiler->default_transition);

    EYELIGHT_FREE(compiler);
}

/*
 * @brief create a new node
 */
Eyelight_Node *eyelight_node_new(int type,Eyelight_Node_Name name, Eyelight_Node* father)
{
    Eyelight_Node* node = malloc(sizeof(Eyelight_Node));
    node->type = type;
    node->father = father;
    node->l = ecore_list_new();
    node->name = name;
    if(father)
    {
        ecore_list_append(father->l,node);
    }

    return node;
}

/*
 * @brief free a node
 */
void eyelight_node_free(Eyelight_Node** current)
{
    Eyelight_Node* node;

    if((*current)->type==EYELIGHT_NODE_TYPE_VALUE)
        EYELIGHT_FREE((*current)->value);

    ecore_list_first_goto((*current)->l);
    while( (node=ecore_list_next((*current)->l)))
        eyelight_node_free(&node);

    ecore_list_destroy((*current)->l);

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
 * @brief add an image in the image list, then these images will be add in an edc block image
 */
int eyelight_image_add(Eyelight_Compiler* compiler,char* image)
{
    int find = 0;
    char* img;
    ecore_list_first_goto(compiler->image_list);
    while( !find && (img = ecore_list_next(compiler->image_list)))
    {
        if(strcmp(image,img)==0)
            find = 1;
    }
    if(!find)
    {
        ecore_list_append(compiler->image_list,strdup(image));
        return 1;
    }
    return 0;
}

/*
 * @brief add an edc file in the list, then these files will be include
 */
int eyelight_edc_file_add(Eyelight_Compiler* compiler,char* edc_file)
{
    int find = 0;
    char* file;
    ecore_list_first_goto(compiler->edc_files);
    while( !find && (file = ecore_list_next(compiler->edc_files)))
    {
        if(strcmp(edc_file,file)==0)
            find = 1;
    }
    if(!find)
    {
        ecore_list_append(compiler->edc_files,strdup(edc_file));
        return 1;
    }
    return 0;
}

/*
 * return the ith value of the property "node"
 */
char* eyelight_retrieve_value_of_prop(Eyelight_Node* node,int i)
{
    ecore_list_first_goto(node->l);
    Eyelight_Node *data;
    int j;
    for(j=0;j<=i;j++)
        data= (Eyelight_Node*)ecore_list_next(node->l);
    return data->value;
}

/*
 * @brief return node of the property "prop" of the block node "current"
 */
Eyelight_Node* eyelight_retrieve_node_prop(Eyelight_Node* current, Eyelight_Node_Name prop)
{
    Eyelight_Node* node = NULL;
    int find = 0;
    ecore_list_first_goto(current->l);
    while(!find && (node=ecore_list_next(current->l)))
    {
        if(node->type == EYELIGHT_NODE_TYPE_PROP && node->name == prop)
            find = 1;
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
    ecore_list_first_goto(current->l);
    while((node=ecore_list_next(current->l)))
    {
        if(node->type == EYELIGHT_NODE_TYPE_BLOCK && node->name == EYELIGHT_NAME_ITEMS)
            number+=eyelight_number_item_in_block(node);
        else if(node->type==EYELIGHT_NODE_TYPE_BLOCK &&
                  ( node->name == EYELIGHT_NAME_ITEM
                  || node->name == EYELIGHT_NAME_IMAGE
                  || node->name == EYELIGHT_NAME_EDC))
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

