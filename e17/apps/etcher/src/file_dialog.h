#ifndef ETCHER_FILEDIALOG_H
#define ETCHER_FILEDIALOG_H

typedef enum filedialog_mode
{
  EtchingMode,
  NewImageMode,
  NormalImageMode,
  HilitedImageMode,
  ClickedImageMode,
  SelectedImageMode,
  DisabledImageMode,
  GridImageMode,
  SaveAsMode
}
FileDialogMode;

void    filedialog_show(FileDialogMode mode);
void    filedialog_hide(GtkButton *button);

#endif
