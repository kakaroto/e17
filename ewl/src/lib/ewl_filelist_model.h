#ifndef EWL_FILELIST_MODEL_H
#define EWL_FILELIST_MODEL_H
#include "ewl_filelist.h"

/**
 * @addtogroup Ewl_Filelist_Model Ewl_Filelist_Model: The model for the filelist
 * @brief A model for the filelist
 *
 * @{
 */

Ewl_Filelist_Directory	*ewl_filelist_model_directory_new(const char *path,
                                                unsigned char show_dot,
                                                unsigned int show_dot_dot,
                                                Ewl_Filelist_Filter *filter);
unsigned int 		 ewl_filelist_model_data_count(void *data);
void 			*ewl_filelist_model_data_fetch(void *data,
						unsigned int row,
                                                unsigned int column);
void 			 ewl_filelist_model_data_sort(void *data,
						unsigned int column,
                                                Ewl_Sort_Direction sort);
int 			 ewl_filelist_model_data_expandable_get(void *data, 
                                                unsigned int row);
void 			*ewl_filelist_model_data_expansion_data_fetch(void *data, 
                                                unsigned int parent);
unsigned int 		 ewl_filelist_model_data_unref(void *data);
int 			 ewl_filelist_model_column_sortable(void *data, 
                                                unsigned int column);

unsigned int 		 ewl_filelist_model_show_dot_files_set
						(Ewl_Filelist_Directory *dir,
                                                unsigned int show_dot);
unsigned int 		 ewl_filelist_model_show_dot_files_get
						(Ewl_Filelist_Directory *dir);

unsigned int 		 ewl_filelist_model_filter_set
					(Ewl_Filelist_Directory *dir,
                                        Ewl_Filelist_Filter *filter);
Ewl_Filelist_Filter 	*ewl_filelist_model_filter_get
					(Ewl_Filelist_Directory *dir);

/**
 * @}
 */

#endif

