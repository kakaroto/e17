
#ifndef __EWL_STOCK_H__
#define __EWL_STOCK_H__


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

  typedef struct Ewl_Stock_Item Ewl_Stock_Item;
  
  struct Ewl_Stock_Item
  {
    char *stock_id;
    char *label;
  };
  
  char *ewl_stock_get_filename (const char *stock_id);
  char *ewl_stock_get_label    (const char *stock_id);
  
  /* Stock items */
  
#define EWL_STOCK_OK     "ewl-ok"
#define EWL_STOCK_APPLY  "ewl-apply"
#define EWL_STOCK_CANCEL "ewl-cancel"
#define EWL_STOCK_OPEN   "ewl-open"
#define EWL_STOCK_SAVE   "ewl-save"
  

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __EWL_STOCK_H__ */
