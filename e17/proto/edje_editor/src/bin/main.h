#ifndef INCLUSION_GUARD
#define INCLUSION_GUARD




/* DEFINES */
#define FAKEWIN_BORDER_TOP 16
#define FAKEWIN_BORDER_LEFT 7
#define FAKEWIN_BORDER_RIGHT 4
#define FAKEWIN_BORDER_BOTTOM 4

//Indentation string for the various indentation level
#define IN1 "  "
#define IN2 "    "
#define IN3 "      "
#define IN4 "        "
#define IN5 "          "
#define IN6 "            "

//All the enum used are declared here
enum various{
	COMP_RAW,
	COMP_COMP,
	COMP_LOSSY,
	PART_TYPE_IMAGE,
	PART_TYPE_RECT,
	PART_TYPE_TEXT,
	NONE,
	FX_PLAIN,
	FX_OUTLINE,
	FX_SOFT_OUTLINE,
	FX_SHADOW,
	FX_SOFT_SHADOW,
	FX_OUTLINE_SHADOW,
	FX_OUTLINE_SOFT_SHADOW,
	NEW_IMAGE,
	NEW_RECT,
	NEW_TEXT,
	NEW_DESC,
	NEW_GROUP,
	REMOVE_DESCRIPTION,
	REMOVE_PART,
	REMOVE_GROUP,
	DRAG_MINIARROW,
	DRAG_REL1,
	DRAG_REL2,
	REL_COMBO_INTERFACE,
	COLOR_OBJECT_RECT,
	COLOR_OBJECT_TEXT,
	COLOR_OBJECT_SHADOW,
	COLOR_OBJECT_OUTLINE,
	TOOLBAR_NEW,
	TOOLBAR_OPEN,
	TOOLBAR_ADD,
	TOOLBAR_REMOVE,
	TOOLBAR_DEBUG,
	TOOLBAR_SAVE,
	TOOLBAR_SAVE_AS,
	TOOLBAR_PLAY,
	TOOLBAR_MOVE_UP,
	TOOLBAR_MOVE_DOWN,
   TOOLBAR_IMAGE_FILE_ADD,
   TOOLBAR_FONT_FILE_ADD,
	REL1X_SPINNER,
	REL1Y_SPINNER,
	REL2X_SPINNER,
	REL2Y_SPINNER,
	MINX_SPINNER,
	MAXX_SPINNER,
	MINY_SPINNER,
	MAXY_SPINNER,
	BORDER_TOP,
	BORDER_LEFT,
	BORDER_RIGHT,
	BORDER_BOTTOM,
	FILECHOOSER_OPEN,
	FILECHOOSER_NEW,
	FILECHOOSER_IMAGE,
	FILECHOOSER_FONT,
	FILECHOOSER_SAVE_AS,
   ROW_GROUP,
   ROW_PART,
   ROW_DESC
};	
 

/* STRUCTURE DEFINITIONS */
typedef struct EDC_Image_		EDC_Image;
typedef struct EDC_Group_			EDC_Group;
typedef struct EDC_Description_ 	EDC_Description;
typedef struct EDC_Part_ 			EDC_Part;

struct	EDC_Image_{
	GString		*name;
	gint     		comp;		
	gint     		comp_type;		
};
struct	EDC_Group_{
	GString		*name;
	gint     		min_x,min_y;		
	gint     		max_x,max_y;	
	GList		*parts;
   Etk_Tree2_Row *tree_row;
};
struct	EDC_Part_{
	GString		*name;
	gint     		type;		
	gint     		mouse_events;
	gint			effect;
	gint			repeat_events;
	GString		*clip_to;
	GString		*color_class;
	GString		*text_class;
	GList		*descriptions;	//Childs
	EDC_Description *current_description;
	EDC_Group	*group;		//Parent
	Evas_Object	*ev_obj;
	gint			realx,realy,realw,realh;
	Etk_Tree2_Row *tree_row;
};
struct 	EDC_Description_{
	GString	*state;
	float	state_index;
	gint		visible;
	float	align_h, align_v;
	gint		min_w, min_h;
	gint		max_w, max_h;
	gint		step_h, step_v;
	float	aspect_min, aspect_max;
	int		color_r, color_g, color_b, color_a;
	int		color2_r, color2_g, color2_b, color2_a;
	int		color3_r, color3_g, color3_b, color3_a;
	GString	*inherit;
	float	inherit_index;
	float	rel1_relative_x,rel1_relative_y;
	int		rel1_offset_x,rel1_offset_y;
	GString	*rel1_to, *rel1_to_x, *rel1_to_y;
	float	rel2_relative_x,rel2_relative_y;
	int		rel2_offset_x,rel2_offset_y;
	GString	*rel2_to, *rel2_to_x, *rel2_to_y;
	GString	*image_normal;
	GList	*image_tween; //list of GString*
	int		image_border_left,image_border_right,image_border_top,image_border_bottom;
	GString	*text_text;
	GString	*text_font;
	int		text_size;
	int		text_fit_h,text_fit_v;
	int		text_min_h,text_min_v;
	float	text_align_h,text_align_v;
	EDC_Part *part;
	Etk_Tree2_Row *tree_row;
};






/* GLOBALS */ 
GString		*EdjeFile;				//The filename of the edje_editor.edj file (witch contain all the graphics used by the program)
GString		*EDCFile;				//Name of the edc file currently open (full path)
GString		*EDCFileDir;			//Directory where the currently open file is
GList 		*EDC_Group_list;		//The main group list ... all start from here

int 			FileChooserOperation;	//The current file chooser operation (FILECHOOSER_OPEN,FILECHOOSER_NEW etc)

EDC_Description *selected_desc;		//Currently selected description (NULL if no desc selected)
EDC_Part 	*selected_part;		//Currently selected part (NULL if no part selected)
EDC_Group 	*selected_group;		//Currently selected group (NULL if no group selected)

Etk_Widget 	*ETK_canvas;			//Designer canvas 
Evas_Object 	*EV_canvas_bg;		//
Evas_Object 	*EV_canvas_shadow;	//
Evas_Object	*EV_fakewin;			//The simple window implementation
Evas_Object 	*focus_handler;		//The yellow box around the selected part
Evas_Object 	*rel1_handler;			//The red point
Evas_Object 	*rel2_handler;			//The blue point
Evas_Object 	*rel1X_parent_handler;	//The 4 line that show the container for each side of the part
Evas_Object 	*rel1Y_parent_handler;	//
Evas_Object 	*rel2X_parent_handler;	//
Evas_Object 	*rel2Y_parent_handler;	//



/* FUNCTION PROTOTYPES*/
EDC_Group*		EDC_Group_new		(char* name,int min_x, int min_y, int max_x, int max_y);	//Create a new Group Object - create tree object - update EDC_Group_list - update the GroupComboBox
void 			EDC_Group_clear		(EDC_Group *group, int skip_parent_modify);			//Clear the group object, all parts and all description inside -  skip_parent_modify set to not remove the group from EDC_Group_list
EDC_Part*		EDC_Part_new			(EDC_Group *parent_group,char* name, int type);		//Create a new Part Object attached to the parent_group
void 			EDC_Part_clear		(EDC_Part *part, int skip_parent_modify);				//Clear the Part object and all the description inside - skip_parent_modify set to: 1.dont remove the part from its parent group list and 2. dont resolv named link to the part
void 			RenamePart			(EDC_Part* part, char* new_name);					//Rename a part and all the description that point to it (the only way to rename safetly)
EDC_Description*	EDC_Description_new	(EDC_Part *parent_part, char* state, float state_index);	//Create a new Description Object setting the default value
void 			EDC_Description_clear	(EDC_Description *desc,int skip_parent_modify);			//Clear the description object - skip_parent_modify set to not remove the description from it's parent list
void 			RenameDescription		(EDC_Description* desc, char* new_name, float index);	//Rename a description  (the only way to rename safetly) - new_name could be null to change only the index - index must be given
void 			ClearAll				(void);											//Clear all EDC data 
EDC_Part* 		GetPartByName		(char* part_name);								//Get the first Part in the selected_group with that name
void			PrintDebugInformation	(int show_groups_list);								//  :)
int 				OpenEDC				(const gchar* EDCfile);								//Open 'gchr *EDCfile', parse it and update the whole interface
void 			SaveEDC				(char *save_as);									//Save the EDC data in memory to a file in the EDC format - 'char *save_as' could be NULL for a normal save operation
void 			PlayEDC				(void);											//Show the PlayDialog - Compile the edc and make a pipe from edje_cc to update the dialog
void 			CreateBlankEDC		(void);											//Create a blank edc with one group, one part and one description (the initial stat of the program)
void 			FileCopy				(char* source, char*dest);							//Normal cp operation

#endif // INCLUSION_GUARD
