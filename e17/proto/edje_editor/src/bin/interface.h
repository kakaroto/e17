#ifndef _EDJE_EDITOR_INTERFACE_H_
#define _EDJE_EDITOR_INTERFACE_H_

#include "main.h"

/* Etk_Tree Helper */
#define TREE_COL_NAME 0
#define TREE_COL_VIS 1
#define TREE_COL_TYPE 2
#define TREE_COL_PARENT 3
#define COL_NAME   etk_tree_nth_col_get(ETK_TREE(UI_PartsTree), TREE_COL_NAME)
#define COL_VIS    etk_tree_nth_col_get(ETK_TREE(UI_PartsTree), TREE_COL_VIS)
#define COL_TYPE   etk_tree_nth_col_get(ETK_TREE(UI_PartsTree), TREE_COL_TYPE)
#define COL_PARENT etk_tree_nth_col_get(ETK_TREE(UI_PartsTree), TREE_COL_PARENT)

Evas *UI_evas;
/* main window objects */
Ecore_Evas *UI_ecore_MainWin;
Etk_Widget *UI_GroupsComboBox;
Etk_Widget *UI_Toolbar;
Etk_Widget *UI_PartsTree;
Etk_Widget *UI_AddMenu;
Etk_Widget *UI_RemoveMenu;
Etk_Widget *UI_OptionsMenu;
/* extra window objects */
Etk_Widget *UI_ColorPickerWin;
Etk_Widget *UI_AlertDialog;
Etk_Widget *UI_FileChooser;
Etk_Widget *UI_FileChooserDialog;
Etk_Widget *UI_FilechooserSaveButton;
Etk_Widget *UI_FilechooserLoadButton;
Etk_Widget *UI_ColorWin;
Etk_Widget *UI_ColorPicker;
/* toolbar objects */
Etk_Widget *UI_PlayButton;
Etk_Widget *UI_PlayImage;
Etk_Widget *UI_PauseImage;
Etk_Widget *UI_AddStateButton;
Etk_Widget *UI_RemoveStateButton;
Etk_Widget *UI_RemovePartButton;
Etk_Widget *UI_RemoveProgramButton;

/* group frame objects */
Etk_Widget *UI_GroupNameEntry;
Etk_Widget *UI_GroupNameEntryImage;
Etk_Widget *UI_GroupMinWSpinner;
Etk_Widget *UI_GroupMinHSpinner;
Etk_Widget *UI_GroupMaxWSpinner;
Etk_Widget *UI_GroupMaxHSpinner;
Etk_Widget *UI_CurrentGroupSizeLabel;
/* part frame objects */
Etk_Widget *UI_PartNameEntry;
Etk_Widget *UI_PartNameEntryImage;
Etk_Widget *UI_PartTypeComboBox;
Etk_Widget *UI_PartEventsCheck;
Etk_Widget *UI_PartEventsRepeatCheck;
Etk_Widget *UI_CliptoComboBox;
Etk_Widget *UI_PartSourceComboBox;
Etk_Widget *UI_PartSourceLabel;
/* state frame objects */
Etk_Widget *UI_StateEntry;
Etk_Widget *UI_StateEntryImage;
Etk_Widget *UI_StateMinWSpinner;
Etk_Widget *UI_StateMinHSpinner;
Etk_Widget *UI_StateMaxWSpinner;
Etk_Widget *UI_StateMaxHSpinner;
Etk_Widget *UI_StateAlignVSpinner;
Etk_Widget *UI_StateAlignHSpinner;
/* text frame objects */
Etk_Widget *UI_TextEntry;
Etk_Widget *UI_FontComboBox;
Etk_Widget *UI_FontSizeSpinner;
Etk_Widget *UI_EffectComboBox;
Etk_Widget *UI_FontAddButton;
Etk_Widget *UI_FontAlignVSpinner;
Etk_Widget *UI_FontAlignHSpinner;
Etk_Widget *UI_AspectMinSpinner;
Etk_Widget *UI_AspectMaxSpinner;
Etk_Widget *UI_AspectComboBox;
/* image frame objects */
Etk_Widget *UI_ImageTweenList;
Etk_Widget *UI_ImageComboBox;
Etk_Widget *UI_ImageTweenVBox;
Etk_Widget *UI_ImageAlphaSlider;
Etk_Widget *UI_AddTweenButton;
Etk_Widget *UI_DeleteTweenButton;
Etk_Widget *UI_MoveDownTweenButton;
Etk_Widget *UI_MoveUpTweenButton;
Etk_Widget *UI_BorderTopSpinner;
Etk_Widget *UI_BorderLeftSpinner;
Etk_Widget *UI_BorderBottomSpinner;
Etk_Widget *UI_BorderRightSpinner;
Etk_Widget *UI_ImageAddButton;
/* position frame objects */
Etk_Widget *UI_Rel1XSpinner;
Etk_Widget *UI_Rel1XOffsetSpinner;
Etk_Widget *UI_Rel1YSpinner;
Etk_Widget *UI_Rel1YOffsetSpinner;
Etk_Widget *UI_Rel2XSpinner;
Etk_Widget *UI_Rel2XOffsetSpinner;
Etk_Widget *UI_Rel2YSpinner;
Etk_Widget *UI_Rel2YOffsetSpinner;
Etk_Widget *UI_Rel1ToXComboBox;
Etk_Widget *UI_Rel1ToYComboBox;
Etk_Widget *UI_Rel2ToXComboBox;
Etk_Widget *UI_Rel2ToYComboBox;
/* script frame objects */
Etk_Widget *UI_ScriptBox;
Etk_Widget *UI_ScriptSaveButton;
/* program frame objects */
Etk_Widget *UI_ProgramEntry;
Etk_Widget *UI_ProgramEntryImage;
Etk_Widget *UI_RunProgButton;
Etk_Widget *UI_SignalEntry;
Etk_Widget *UI_ActionComboBox;
Etk_Widget *UI_TransiComboBox;
Etk_Widget *UI_TransiLabel;
Etk_Widget *UI_DurationSpinner;
Etk_Widget *UI_AfterEntry;
Etk_Widget *UI_SourceEntry;
Etk_Widget *UI_TargetEntry;
Etk_Widget *UI_TargetLabel;
Etk_Widget *UI_Param1Label;
Etk_Widget *UI_Param1Entry;
Etk_Widget *UI_Param1Spinner;
Etk_Widget *UI_Param1Label;
Etk_Widget *UI_Param2Entry;
Etk_Widget *UI_Param2Spinner;
Etk_Widget *UI_Param2Label;
Etk_Widget *UI_DelayFromSpinner;
Etk_Widget *UI_DelayRangeSpinner;
/* etk_embed objects */
Etk_Widget *UI_PartsTreeEmbed;
Etk_Widget *UI_GroupEmbed;
Etk_Widget *UI_PartEmbed;
Etk_Widget *UI_DescriptionEmbed;
Etk_Widget *UI_RectEmbed;
Etk_Widget *UI_TextEmbed;
Etk_Widget *UI_PositionEmbed;
Etk_Widget *UI_ProgramEmbed;
Etk_Widget *UI_ScriptEmbed;
Etk_Widget *UI_ImageEmbed;
Etk_Widget *UI_ToolbarEmbed;
/* colors objects*/
Evas_Object *RectColorObject;
Evas_Object *TextColorObject;
Evas_Object *ShadowColorObject;
Evas_Object *OutlineColorObject;

Evas_Object *edje_ui;



void create_main_window(void);

Etk_Tree_Row *AddPartToTree(const char *part_name, Etk_Tree_Row *after);
Etk_Tree_Row *AddStateToTree(const char *part_name, const char *state_name);
Etk_Tree_Row *AddProgramToTree(const char* prog);

void PopulateGroupsComboBox(void);
void PopulateTree(void);
void PopulateRelComboBoxes(void);
void PopulateImagesComboBox(void);
void PopulateFontsComboBox(void);
void PopulateSourceComboBox(void);
void PopulateTweenList(void);
void PopulateSourceComboEntry(void);
void PopulateSignalComboEntry(void);

void UpdateGroupFrame(void);
void UpdatePositionFrame(void);
void UpdateComboPositionFrame(void);
void UpdateImageFrame(void);
void UpdateTextFrame(void);
void UpdatePartFrame(void);
void UpdateRectFrame(void);
void UpdateDescriptionFrame(void);
void UpdateProgFrame(void);
void UpdateScriptFrame(void);
void UpdateWindowTitle(void);

void ShowFilechooser(int FileChooserType);
void ShowAlert(char* text);
void ConsolleLog(char *text);
void ConsolleClear(void);
void TogglePlayButton(int set);
char* GetPartTypeImage(int part_type);

#endif
