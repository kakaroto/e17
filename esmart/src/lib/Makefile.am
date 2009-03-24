if BUILD_ESMART_CONTAINER
container_subdir = esmart_container
endif

if BUILD_ESMART_DRAGGIES
draggies_subdir = esmart_draggies
endif

if BUILD_ESMART_RESIZE
resize_subdir = esmart_resize
endif

if BUILD_ESMART_FILE_DIALOG
file_dialog_subdir = esmart_file_dialog
endif

if BUILD_ESMART_TEXT_ENTRY
text_entry_subdir = esmart_text_entry
endif

if BUILD_ESMART_TEXTAREA
textarea_subdir = esmart_textarea
endif

if BUILD_ESMART_THUMB
thumb_subdir = esmart_thumb
endif

if BUILD_ESMART_TRANS_X11
trans_subdir = esmart_trans_x11
endif

if BUILD_ESMART_CAIRO
cairo_subdir = esmart_cairo
endif

if BUILD_ESMART_XPIXMAP
xpixmap_subdir = esmart_xpixmap
endif

SUBDIRS = \
  $(container_subdir) $(draggies_subdir) $(text_entry_subdir) \
  $(file_dialog_subdir) $(thumb_subdir) $(trans_subdir) $(resize_subdir) \
  $(cairo_subdir) $(xpixmap_subdir)

# adding textarea_subdir breaks distcheck
# $(textarea_subdir)
