/** @file etk_stock.c */
#include <stdlib.h>
#include <string.h>
#include "etk_stock.h"

/**
 * @addtogroup Etk_Stock
 * @{
 */

typedef struct _Etk_Stock_Item
{
   char *small_icon_key;
   char *medium_icon_key;
   char *big_icon_key;
   char *label;
} Etk_Stock_Item;

static const Etk_Stock_Item _etk_stock_items[] =
{
/*   
   { "actions/process-stop_16", "actions/process-stop_22", "actions/process-stop_48", "Stop" },
   { "actions/document-open_16", "actions/document-open_22", "actions/document-open_48", "Open" },
   { "actions/document-save_16", "actions/document-save_22", "actions/document-save_48", "Save" },
   { "actions/document-save-as_16", "actions/document-save-as_22", "actions/document-save-as_48", "Save As" },
   { "actions/edit-cut_16", "actions/edit-cut_22", "actions/edit-cut_48", "Cut" },
   { "actions/edit-copy_16", "actions/edit-copy_22", "actions/edit-copy_48", "Copy" },
   { "actions/edit-paste_16", "actions/edit-paste_22", "actions/edit-paste_48", "Paste" },
   { "actions/process-stop_16", "actions/process-stop_22", "actions/process-stop_48", "Cancel" },
   { "actions/go-up_16", "actions/go-up_22", "actions/go-up_48", "Go Up" },
   { "actions/go-down_16", "actions/go-down_22", "actions/go-down_48", "Go Down" },
*/   
   { NULL, NULL },
   { "actions/address-book-new_16", "actions/address-book-new_22", "actions/address-book-new_48", "" },
   { "actions/appointment-new_16", "actions/appointment-new_22", "actions/appointment-new_48", "" },
   { "actions/bookmark-new_16", "actions/bookmark-new_22", "actions/bookmark-new_48", "" },
   { "actions/contact-new_16", "actions/contact-new_22", "actions/contact-new_48", "" },
   { "actions/dialog-cancel_16", "actions/dialog-cancel_22", "actions/dialog-cancel_48", "Cancel" },
   { "actions/directory-new_16", "actions/directory-new_22", "actions/directory-new_48", "" },
   { "actions/document-new_16", "actions/document-new_22", "actions/document-new_48", "New" },
   { "actions/document-open_16", "actions/document-open_22", "actions/document-open_48", "Open" },
   { "actions/document-print_16", "actions/document-print_22", "actions/document-print_48", "Print" },
   { "actions/document-print-preview_16", "actions/document-print-preview_22", "actions/document-print-preview_48", "Print Preview" },
   { "actions/document-properties_16", "actions/document-properties_22", "actions/document-properties_48", "Properties" },
   { "actions/document-save-as_16", "actions/document-save-as_22", "actions/document-save-as_48", "Save as" },
   { "actions/document-save_16", "actions/document-save_22", "actions/document-save_48", "Save" },
   { "actions/edit-clear_16", "actions/edit-clear_22", "actions/edit-clear_48", "Clear" },
   { "actions/edit-copy_16", "actions/edit-copy_22", "actions/edit-copy_48", "Copy" },
   { "actions/edit-cut_16", "actions/edit-cut_22", "actions/edit-cut_48", "Cut" },
   { "actions/edit-find_16", "actions/edit-find_22", "actions/edit-find_48", "Find" },
   { "actions/edit-paste_16", "actions/edit-paste_22", "actions/edit-paste_48", "Paste" },
   { "actions/edit-redo_16", "actions/edit-redo_22", "actions/edit-redo_48", "Redo" },
   { "actions/edit-undo_16", "actions/edit-undo_22", "actions/edit-undo_48", "Undo" },
   { "actions/format-indent-less_16", "actions/format-indent-less_22", "actions/format-indent-less_48", "" },
   { "actions/format-indent-more_16", "actions/format-indent-more_22", "actions/format-indent-more_48", "" },
   { "actions/format-justify-center_16", "actions/format-justify-center_22", "actions/format-justify-center_48", "" },
   { "actions/format-justify-fill_16", "actions/format-justify-fill_22", "actions/format-justify-fill_48", "" },
   { "actions/format-justify-left_16", "actions/format-justify-left_22", "actions/format-justify-left_48", "" },
   { "actions/format-justify-right_16", "actions/format-justify-right_22", "actions/format-justify-right_48", "" },
   { "actions/format-text-bold_16", "actions/format-text-bold_22", "actions/format-text-bold_48", "Bold" },
   { "actions/format-text-italic_16", "actions/format-text-italic_22", "actions/format-text-italic_48", "Italic" },
   { "actions/format-text-strikethrough_16", "actions/format-text-strikethrough_22", "actions/format-text-strikethrough_48", "Strike Through" },
   { "actions/format-text-underline_16", "actions/format-text-underline_22", "actions/format-text-underline_48", "Underline" },
   { "actions/go-bottom_16", "actions/go-bottom_22", "actions/go-bottom_48", "Bottom" },
   { "actions/go-down_16", "actions/go-down_22", "actions/go-down_48", "Go Down" },
   { "actions/go-first_16", "actions/go-first_22", "actions/go-first_48", "First" },
   { "actions/go-home_16", "actions/go-home_22", "actions/go-home_48", "Home" },
   { "actions/go-jump_16", "actions/go-jump_22", "actions/go-jump_48", "Jump" },
   { "actions/go-last_16", "actions/go-last_22", "actions/go-last_48", "Last" },
   { "actions/go-next_16", "actions/go-next_22", "actions/go-next_48", "Next" },
   { "actions/go-previous_16", "actions/go-previous_22", "actions/go-previous_48", "Previous" },
   { "actions/go-top_16", "actions/go-top_22", "actions/go-top_48", "Top" },
   { "actions/go-up_16", "actions/go-up_22", "actions/go-up_48", "Go Up" },
   { "actions/help-contents_16", "actions/help-contents_22", "actions/help-contents_48", "" },
   { "actions/mail-message-new_16", "actions/mail-message-new_22", "actions/mail-message-new_48", "" },
   { "actions/media-playback-pause_16", "actions/media-playback-pause_22", "actions/media-playback-pause_48", "" },
   { "actions/media-playback-start_16", "actions/media-playback-start_22", "actions/media-playback-start_48", "" },
   { "actions/media-playback-stop_16", "actions/media-playback-stop_22", "actions/media-playback-stop_48", "" },
   { "actions/media-record_16", "actions/media-record_22", "actions/media-record_48", "" },
   { "actions/media-seek-backward_16", "actions/media-seek-backward_22", "actions/media-seek-backward_48", "" },
   { "actions/media-seek-forward_16", "actions/media-seek-forward_22", "actions/media-seek-forward_48", "" },
   { "actions/media-skip-backward_16", "actions/media-skip-backward_22", "actions/media-skip-backward_48", "" },
   { "actions/media-skip-forward_16", "actions/media-skip-forward_22", "actions/media-skip-forward_48", "" },
   { "actions/process-stop_16", "actions/process-stop_22", "actions/process-stop_48", "Stop" },
   { "actions/system-log-out_16", "actions/system-log-out_22", "actions/system-log-out_48", "Log out" },
   { "actions/system-search_16", "actions/system-search_22", "actions/system-search_48", "Search" },
   { "actions/system-shutdown_16", "actions/system-shutdown_22", "actions/system-shutdown_48", "Shutdown" },
   { "actions/tab-new_16", "actions/tab-new_22", "actions/tab-new_48", "New Tab" },
   { "actions/view-refresh_16", "actions/view-refresh_22", "actions/view-refresh_48", "Refresh" },
   { "actions/window-new_16", "actions/window-new_22", "actions/window-new_48", "New Window" },
   { "apps/accessories-calculator_16", "apps/accessories-calculator_22", "apps/accessories-calculator_48", "" },
   { "apps/accessories-text-editor_16", "apps/accessories-text-editor_22", "apps/accessories-text-editor_48", "" },
   { "apps/help-browser_16", "apps/help-browser_22", "apps/help-browser_48", "" },
   { "apps/internet-group-chat_16", "apps/internet-group-chat_22", "apps/internet-group-chat_48", "" },
   { "apps/internet-mail_16", "apps/internet-mail_22", "apps/internet-mail_48", "" },
   { "apps/internet-news-reader_16", "apps/internet-news-reader_22", "apps/internet-news-reader_48", "" },
   { "apps/internet-web-browser_16", "apps/internet-web-browser_22", "apps/internet-web-browser_48", "" },
   { "apps/multimedia-volume-control_16", "apps/multimedia-volume-control_22", "apps/multimedia-volume-control_48", "" },
   { "apps/office-calendar_16", "apps/office-calendar_22", "apps/office-calendar_48", "" },
   { "apps/preferences-desktop-accessibility_16", "apps/preferences-desktop-accessibility_22", "apps/preferences-desktop-accessibility_48", "" },
   { "apps/preferences-desktop-font_16", "apps/preferences-desktop-font_22", "apps/preferences-desktop-font_48", "" },
   { "apps/preferences-desktop-screensaver_16", "apps/preferences-desktop-screensaver_22", "apps/preferences-desktop-screensaver_48", "" },
   { "apps/preferences-desktop-theme_16", "apps/preferences-desktop-theme_22", "apps/preferences-desktop-theme_48", "" },
   { "apps/preferences-desktop-wallpaper_16", "apps/preferences-desktop-wallpaper_22", "apps/preferences-desktop-wallpaper_48", "" },
   { "apps/preferences-system-session_16", "apps/preferences-system-session_22", "apps/preferences-system-session_48", "" },
   { "apps/preferences-system-windows_16", "apps/preferences-system-windows_22", "apps/preferences-system-windows_48", "" },
   { "apps/system-file-manager_16", "apps/system-file-manager_22", "apps/system-file-manager_48", "" },
   { "apps/system-installer_16", "apps/system-installer_22", "apps/system-installer_48", "" },
   { "apps/system-lock-screen_16", "apps/system-lock-screen_22", "apps/system-lock-screen_48", "" },
   { "apps/system-software-update_16", "apps/system-software-update_22", "apps/system-software-update_48", "" },
   { "apps/system-users_16", "apps/system-users_22", "apps/system-users_48", "" },
   { "apps/utilities-system-monitor_16", "apps/utilities-system-monitor_22", "apps/utilities-system-monitor_48", "" },
   { "apps/utilities-terminal_16", "apps/utilities-terminal_22", "apps/utilities-terminal_48", "" },
   { "categories/applications-development_16", "categories/applications-development_22", "categories/applications-development_48", "" },
   { "categories/applications-games_16", "categories/applications-games_22", "categories/applications-games_48", "" },
   { "categories/preferences-desktop_16", "categories/preferences-desktop_22", "categories/preferences-desktop_48", "" },
   { "devices/audio-card_16", "devices/audio-card_22", "devices/audio-card_48", "" },
   { "devices/audio-input-microphone_16", "devices/audio-input-microphone_22", "devices/audio-input-microphone_48", "" },
   { "devices/battery_16", "devices/battery_22", "devices/battery_48", "" },
   { "devices/camera-photo_16", "devices/camera-photo_22", "devices/camera-photo_48", "" },
   { "devices/camera-video_16", "devices/camera-video_22", "devices/camera-video_48", "" },
   { "devices/computer_16", "devices/computer_22", "devices/computer_48", "" },
   { "devices/drive-cdrom_16", "devices/drive-cdrom_22", "devices/drive-cdrom_48", "" },
   { "devices/drive-harddisk_16", "devices/drive-harddisk_22", "devices/drive-harddisk_48", "" },
   { "devices/drive-removable-media_16", "devices/drive-removable-media_22", "devices/drive-removable-media_48", "" },
   { "devices/input-gaming_16", "devices/input-gaming_22", "devices/input-gaming_48", "" },
   { "devices/input-keyboard_16", "devices/input-keyboard_22", "devices/input-keyboard_48", "" },
   { "devices/input-mouse_16", "devices/input-mouse_22", "devices/input-mouse_48", "" },
   { "devices/media-cdrom_16", "devices/media-cdrom_22", "devices/media-cdrom_48", "" },
   { "devices/media-floppy_16", "devices/media-floppy_22", "devices/media-floppy_48", "" },
   { "devices/network_16", "devices/network_22", "devices/network_48", "" },
   { "devices/network-wireless_16", "devices/network-wireless_22", "devices/network-wireless_48", "" },
   { "devices/printer_16", "devices/printer_22", "devices/printer_48", "" },
   { "devices/printer-remote_16", "devices/printer-remote_22", "devices/printer-remote_48", "" },
   { "devices/video-display_16", "devices/video-display_22", "devices/video-display_48", "" },
   { "emblems/emblem-favorites_16", "emblems/emblem-favorites_22", "emblems/emblem-favorites_48", "" },
   { "emblems/emblem-important_16", "emblems/emblem-important_22", "emblems/emblem-important_48", "" },
   { "emblems/emblem-photos_16", "emblems/emblem-photos_22", "emblems/emblem-photos_48", "" },
   { "emblems/emblem-symbolic-link_16", "emblems/emblem-symbolic-link_22", "emblems/emblem-symbolic-link_48", "" },
   { "emblems/emblem-system_16", "emblems/emblem-system_22", "emblems/emblem-system_48", "" },
   { "emotes/face-angel_16", "emotes/face-angel_22", "emotes/face-angel_48", "" },
   { "emotes/face-crying_16", "emotes/face-crying_22", "emotes/face-crying_48", "" },
   { "emotes/face-devil-grin_16", "emotes/face-devil-grin_22", "emotes/face-devil-grin_48", "" },
   { "emotes/face-glasses_16", "emotes/face-glasses_22", "emotes/face-glasses_48", "" },
   { "emotes/face-grin_16", "emotes/face-grin_22", "emotes/face-grin_48", "" },
   { "emotes/face-sad_16", "emotes/face-sad_22", "emotes/face-sad_48", "" },
   { "emotes/face-smile-big_16", "emotes/face-smile-big_22", "emotes/face-smile-big_48", "" },
   { "emotes/face-smile_16", "emotes/face-smile_22", "emotes/face-smile_48", "" },
   { "emotes/face-surprise_16", "emotes/face-surprise_22", "emotes/face-surprise_48", "" },
   { "emotes/face-wink_16", "emotes/face-wink_22", "emotes/face-wink_48", "" },
   { "mimetypes/application-certificate_16", "mimetypes/application-certificate_22", "mimetypes/application-certificate_48", "" },
   { "mimetypes/application-x-executable_16", "mimetypes/application-x-executable_22", "mimetypes/application-x-executable_48", "" },
   { "mimetypes/audio-x-generic_16", "mimetypes/audio-x-generic_22", "mimetypes/audio-x-generic_48", "" },
   { "mimetypes/image-x-generic_16", "mimetypes/image-x-generic_22", "mimetypes/image-x-generic_48", "" },
   { "mimetypes/package-x-generic_16", "mimetypes/package-x-generic_22", "mimetypes/package-x-generic_48", "" },
   { "mimetypes/text-html_16", "mimetypes/text-html_22", "mimetypes/text-html_48", "" },
   { "mimetypes/text-x-generic_16", "mimetypes/text-x-generic_22", "mimetypes/text-x-generic_48", "" },
   { "mimetypes/text-x-generic-template_16", "mimetypes/text-x-generic-template_22", "mimetypes/text-x-generic-template_48", "" },
   { "mimetypes/text-x-script_16", "mimetypes/text-x-script_22", "mimetypes/text-x-script_48", "" },
   { "mimetypes/video-x-generic_16", "mimetypes/video-x-generic_22", "mimetypes/video-x-generic_48", "" },
   { "mimetypes/x-directory-desktop_16", "mimetypes/x-directory-desktop_22", "mimetypes/x-directory-desktop_48", "" },
   { "mimetypes/x-directory-normal-drag-accept_16", "mimetypes/x-directory-normal-drag-accept_22", "mimetypes/x-directory-normal-drag-accept_48", "" },
   { "mimetypes/x-directory-normal-home_16", "mimetypes/x-directory-normal-home_22", "mimetypes/x-directory-normal-home_48", "" },
   { "mimetypes/x-directory-normal-open_16", "mimetypes/x-directory-normal-open_22", "mimetypes/x-directory-normal-open_48", "" },
   { "mimetypes/x-directory-normal_16", "mimetypes/x-directory-normal_22", "mimetypes/x-directory-normal_48", "" },
   { "mimetypes/x-directory-normal-visiting_16", "mimetypes/x-directory-normal-visiting_22", "mimetypes/x-directory-normal-visiting_48", "" },
   { "mimetypes/x-directory-remote_16", "mimetypes/x-directory-remote_22", "mimetypes/x-directory-remote_48", "" },
   { "mimetypes/x-directory-remote-server_16", "mimetypes/x-directory-remote-server_22", "mimetypes/x-directory-remote-server_48", "" },
   { "mimetypes/x-directory-remote-workgroup_16", "mimetypes/x-directory-remote-workgroup_22", "mimetypes/x-directory-remote-workgroup_48", "" },
   { "mimetypes/x-directory-trash-full_16", "mimetypes/x-directory-trash-full_22", "mimetypes/x-directory-trash-full_48", "" },
   { "mimetypes/x-directory-trash_16", "mimetypes/x-directory-trash_22", "mimetypes/x-directory-trash_48", "" },
   { "mimetypes/x-office-address-book_16", "mimetypes/x-office-address-book_22", "mimetypes/x-office-address-book_48", "" },
   { "mimetypes/x-office-document_16", "mimetypes/x-office-document_22", "mimetypes/x-office-document_48", "" },
   { "mimetypes/x-office-spreadsheet_16", "mimetypes/x-office-spreadsheet_22", "mimetypes/x-office-spreadsheet_48", "" },
   { "status/battery-caution_16", "status/battery-caution_22", "status/battery-caution_48", "" },
   { "status/dialog-error_16", "status/dialog-error_22", "status/dialog-error_48", "" },
   { "status/dialog-information_16", "status/dialog-information_22", "status/dialog-information_48", "" },
   { "status/dialog-warning_16", "status/dialog-warning_22", "status/dialog-warning_48", "" },
   { "status/image-loading_16", "status/image-loading_22", "status/image-loading_48", "" },
   { "status/image-missing_16", "status/image-missing_22", "status/image-missing_48", "" },
   { "status/locked_16", "status/locked_22", "status/locked_48", "" },
   { "status/mail-attachment_16", "status/mail-attachment_22", "status/mail-attachment_48", "" },
   { "status/unlocked_16", "status/unlocked_22", "status/unlocked_48", "" }
};

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the edje key corresponding to the stock id
 * @param stock_id a stock id
 * @param size the size of the stock icon
 * @return Returns the edje key corresponding to the stock id
 */
char *etk_stock_key_get(Etk_Stock_Id stock_id, Etk_Stock_Size size)
{
   if (stock_id < 0 || stock_id >= ETK_NUM_STOCK_IDS)
      return NULL;
   
   if (size == ETK_STOCK_SMALL)
      return _etk_stock_items[stock_id].small_icon_key;
   else if (size == ETK_STOCK_MEDIUM)
      return _etk_stock_items[stock_id].medium_icon_key;
   else
      return _etk_stock_items[stock_id].big_icon_key;
}

/**
 * @brief Gets the label corresponding to the stock id
 * @param stock_id a stock id
 * @return Returns the label corresponding to the stock id
 */
char *etk_stock_label_get(Etk_Stock_Id stock_id)
{
   if (stock_id < 0 || stock_id >= ETK_NUM_STOCK_IDS)
      return NULL;
   return _etk_stock_items[stock_id].label;
}

/** @} */
