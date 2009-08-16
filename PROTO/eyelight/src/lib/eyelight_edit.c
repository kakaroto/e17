
#include "Eyelight_Edit.h"
#include "eyelight_compiler_parser.h"


const char* eyelight_edit_slide_title_get(Eyelight_Viewer *pres, int id_slide)
{
    const char *title = NULL;
    Eyelight_Node *node_slide, *node;
    Eina_List *l;
    Eyelight_Compiler *compiler = pres->compiler;
    int nb_slides = eyelight_nb_slides_get(compiler);

    //retrieve the default title of the slide
    //it is the title define outside a slide block
    int i_slide = -1;
    l = compiler->root->l;
    while( l && i_slide<id_slide)
    {
        node = eina_list_data_get(l);
        switch(node->type)
        {
            case EYELIGHT_NODE_TYPE_PROP:
                switch(node->name)
                {
                    case EYELIGHT_NAME_TITLE:
                        title = eyelight_retrieve_value_of_prop(node,0);
                        break;
                    default: ;
                }
                break;
            case EYELIGHT_NODE_TYPE_BLOCK:
                switch(node->name)
                {
                    case EYELIGHT_NAME_SLIDE:
                        node_slide = node;
                        i_slide++;
                        break;
                }
                break;
        }
        l = eina_list_next(l);
    }

    //retrieve the title defines in the slide
    Eyelight_Node *node_title = eyelight_retrieve_node_prop(node, EYELIGHT_NAME_TITLE);
    if(node_title)
        title = eyelight_retrieve_value_of_prop(node_title,0);

    return title;
}

Eyelight_Thumb* eyelight_edit_thumbnails_get_new(Eyelight_Viewer* pres, int pos)
{
    return (Eyelight_Thumb*)eyelight_viewer_thumbnails_get_new(pres, pos);
}


