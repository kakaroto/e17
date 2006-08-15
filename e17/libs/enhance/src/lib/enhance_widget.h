#ifndef EN_WIDGET_H
#define EN_WIDGET_H

typedef struct _E_Widget E_Widget;
typedef struct _E_Widget_Packing E_Widget_Packing;

struct _E_Widget
{
   Etk_Widget *wid;
   EXML_Node  *node;
   E_Widget_Packing *packing;
};

struct _E_Widget_Packing
{
   /* Box packing */
   int      padding;
   Etk_Bool expand;
   Etk_Bool fill;
   Etk_Box_Group box_group;

   /* Table packing */
   int   left_attach;
   int   right_attach;
   int   top_attach;
   int   bottom_attach;
   int   x_padding;
   int   y_padding;
   char *x_options;
   char *y_options;
   
   /* Frame packing */
   char *type;
   
   /* Paned packing */
   Etk_Bool shrink;
};
    
E_Widget *_e_widget_handle(Enhance *en, EXML_Node *node);
void      _e_widget_parent_add(E_Widget *parent, E_Widget *child);

#endif
