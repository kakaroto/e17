/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

/** @file etk_stock.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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
   { NULL, NULL, NULL, NULL },
   { "actions/address-book-new_16", "actions/address-book-new_22", "actions/address-book-new_48", "New Address Book" },
   { "actions/appointment-new_16", "actions/appointment-new_22", "actions/appointment-new_48", "New Appointment" },
   { "actions/bookmark-new_16", "actions/bookmark-new_22", "actions/bookmark-new_48", "New Bookmark" },
   { "actions/contact-new_16", "actions/contact-new_22", "actions/contact-new_48", "New Contact" },
   { "actions/dialog-apply_16", "actions/dialog-apply_22", "actions/dialog-apply_48", "Apply" },
   { "actions/dialog-ok_16", "actions/dialog-ok_22", "actions/dialog-ok_48", "OK" },
   { "actions/dialog-cancel_16", "actions/dialog-cancel_22", "actions/dialog-cancel_48", "Cancel" },
   { "actions/dialog-yes_16", "actions/dialog-yes_22", "actions/dialog-yes_48", "Yes" },
   { "actions/dialog-no_16", "actions/dialog-no_22", "actions/dialog-no_48", "No" },
   { "actions/dialog-close_16", "actions/dialog-close_22", "actions/dialog-close_48", "Close" },
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
   { "actions/edit-delete_16", "actions/edit-delete_22", "actions/edit-delete_48", "Delete" },
   { "actions/edit-find-replace_16", "actions/edit-find-replace_22", "actions/edit-find-replace_48", "Replace" },
   { "actions/folder-new_16", "actions/folder-new_22", "actions/folder-new_48", "New Folder" },
   { "actions/format-indent-less_16", "actions/format-indent-less_22", "actions/format-indent-less_48", "Indent Less" },
   { "actions/format-indent-more_16", "actions/format-indent-more_22", "actions/format-indent-more_48", "Indent More" },
   { "actions/format-justify-center_16", "actions/format-justify-center_22", "actions/format-justify-center_48", "Center" },
   { "actions/format-justify-fill_16", "actions/format-justify-fill_22", "actions/format-justify-fill_48", "Justify" },
   { "actions/format-justify-left_16", "actions/format-justify-left_22", "actions/format-justify-left_48", "Align Left" },
   { "actions/format-justify-right_16", "actions/format-justify-right_22", "actions/format-justify-right_48", "Align Right" },
   { "actions/format-text-bold_16", "actions/format-text-bold_22", "actions/format-text-bold_48", "Bold" },
   { "actions/format-text-italic_16", "actions/format-text-italic_22", "actions/format-text-italic_48", "Italic" },
   { "actions/format-text-strikethrough_16", "actions/format-text-strikethrough_22", "actions/format-text-strikethrough_48", "Strike Through" },
   { "actions/format-text-underline_16", "actions/format-text-underline_22", "actions/format-text-underline_48", "Underline" },
   { "actions/go-bottom_16", "actions/go-bottom_22", "actions/go-bottom_48", "Bottom" },
   { "actions/go-down_16", "actions/go-down_22", "actions/go-down_48", "Down" },
   { "actions/go-first_16", "actions/go-first_22", "actions/go-first_48", "First" },
   { "actions/go-home_16", "actions/go-home_22", "actions/go-home_48", "Home" },
   { "actions/go-jump_16", "actions/go-jump_22", "actions/go-jump_48", "Jump" },
   { "actions/go-last_16", "actions/go-last_22", "actions/go-last_48", "Last" },
   { "actions/go-next_16", "actions/go-next_22", "actions/go-next_48", "Next" },
   { "actions/go-previous_16", "actions/go-previous_22", "actions/go-previous_48", "Previous" },
   { "actions/go-top_16", "actions/go-top_22", "actions/go-top_48", "Top" },
   { "actions/go-up_16", "actions/go-up_22", "actions/go-up_48", "Up" },
   { "actions/list-add_16", "actions/list-add_22", "actions/list-add_48", "Add" },
   { "actions/list-remove_16", "actions/list-remove_22", "actions/list-remove_48", "Remove" },
   { "actions/mail-message-new_16", "actions/mail-message-new_22", "actions/mail-message-new_48", "New Message" },
   { "actions/mail-forward_16", "actions/mail-forward_22", "actions/mail-forward_48", "Forward" },
   { "actions/mail-mark-junk_16", "actions/mail-mark-junk_22", "actions/mail-mark-junk_48", "Mark As Junk" },
   { "actions/mail-reply-all_16", "actions/mail-reply-all_22", "actions/mail-reply-all_48", "Reply To All" },
   { "actions/mail-reply-sender_16", "actions/mail-reply-sender_22", "actions/mail-reply-sender_48", "Reply" },
   { "actions/mail-send-receive_16", "actions/mail-send-receive_22", "actions/mail-send-receive_48", "Send And Receive" },
   { "actions/media-eject_16", "actions/media-eject_22", "actions/media-eject_48", "Eject" },
   { "actions/media-playback-pause_16", "actions/media-playback-pause_22", "actions/media-playback-pause_48", "Pause" },
   { "actions/media-playback-start_16", "actions/media-playback-start_22", "actions/media-playback-start_48", "Play" },
   { "actions/media-playback-stop_16", "actions/media-playback-stop_22", "actions/media-playback-stop_48", "Stop" },
   { "actions/media-record_16", "actions/media-record_22", "actions/media-record_48", "Record" },
   { "actions/media-seek-backward_16", "actions/media-seek-backward_22", "actions/media-seek-backward_48", "Seek Backward" },
   { "actions/media-seek-forward_16", "actions/media-seek-forward_22", "actions/media-seek-forward_48", "Seek Forward" },
   { "actions/media-skip-backward_16", "actions/media-skip-backward_22", "actions/media-skip-backward_48", "Skip Backward" },
   { "actions/media-skip-forward_16", "actions/media-skip-forward_22", "actions/media-skip-forward_48", "Skip Forward" },
   { "actions/process-stop_16", "actions/process-stop_22", "actions/process-stop_48", "Stop" },
   { "actions/system-lock-screen_16", "actions/system-lock-screen_22", "actions/system-lock-screen_48", "Lock The Screen" },
   { "actions/system-log-out_16", "actions/system-log-out_22", "actions/system-log-out_48", "Log out" },
   { "actions/system-search_16", "actions/system-search_22", "actions/system-search_48", "Search" },
   { "actions/system-shutdown_16", "actions/system-shutdown_22", "actions/system-shutdown_48", "Shutdown" },
   { "actions/tab-new_16", "actions/tab-new_22", "actions/tab-new_48", "New Tab" },
   { "actions/view-refresh_16", "actions/view-refresh_22", "actions/view-refresh_48", "Refresh" },
   { "actions/window-new_16", "actions/window-new_22", "actions/window-new_48", "New Window" },
   { "apps/accessories-calculator_16", "apps/accessories-calculator_22", "apps/accessories-calculator_48", "" },
   { "apps/accessories-character-map_16", "apps/accessories-character-map_22", "apps/accessories-character-app_48", "" },
   { "apps/accessories-text-editor_16", "apps/accessories-text-editor_22", "apps/accessories-text-editor_48", "" },
   { "apps/help-browser_16", "apps/help-browser_22", "apps/help-browser_48", "" },
   { "apps/internet-group-chat_16", "apps/internet-group-chat_22", "apps/internet-group-chat_48", "" },
   { "apps/internet-mail_16", "apps/internet-mail_22", "apps/internet-mail_48", "" },
   { "apps/internet-news-reader_16", "apps/internet-news-reader_22", "apps/internet-news-reader_48", "" },
   { "apps/internet-web-browser_16", "apps/internet-web-browser_22", "apps/internet-web-browser_48", "" },
   { "apps/multimedia-volume-control_16", "apps/multimedia-volume-control_22", "apps/multimedia-volume-control_48", "" },
   { "apps/office-calendar_16", "apps/office-calendar_22", "apps/office-calendar_48", "" },
   { "apps/preferences-desktop-accessibility_16", "apps/preferences-desktop-accessibility_22", "apps/preferences-desktop-accessibility_48", "" },
   { "apps/preferences-desktop-assistive-technology_16", "apps/preferences-desktop-assistive-technology_22", "apps/preferences-desktop-assistive-technology_48", "" },
   { "apps/preferences-desktop-font_16", "apps/preferences-desktop-font_22", "apps/preferences-desktop-font_48", "" },
   { "apps/preferences-desktop-keyboard-shortcuts_16", "apps/preferences-desktop-keyboard-shortcuts_22", "apps/preferences-desktop-keyboard-shortcuts_48", "" },
   { "apps/preferences-desktop-locale_16", "apps/preferences-desktop-locale_22", "apps/preferences-desktop-locale_48", "" },
   { "apps/preferences-desktop-remote-desktop_16", "apps/preferences-desktop-remote-desktop_22", "apps/preferences-desktop-remote-desktop_48", "" },
   { "apps/preferences-desktop-sound_16", "apps/preferences-desktop-sound_22", "apps/preferences-desktop-sound_48", "" },
   { "apps/preferences-desktop-screensaver_16", "apps/preferences-desktop-screensaver_22", "apps/preferences-desktop-screensaver_48", "" },
   { "apps/preferences-desktop-theme_16", "apps/preferences-desktop-theme_22", "apps/preferences-desktop-theme_48", "" },
   { "apps/preferences-desktop-wallpaper_16", "apps/preferences-desktop-wallpaper_22", "apps/preferences-desktop-wallpaper_48", "" },
   { "apps/preferences-system-network-proxy_16", "apps/preferences-system-network-proxy_22", "apps/preferences-system-network-proxy_48", "" },
   { "apps/preferences-system-session_16", "apps/preferences-system-session_22", "apps/preferences-system-session_48", "" },
   { "apps/preferences-system-windows_16", "apps/preferences-system-windows_22", "apps/preferences-system-windows_48", "" },
   { "apps/system-file-manager_16", "apps/system-file-manager_22", "apps/system-file-manager_48", "" },
   { "apps/system-installer_16", "apps/system-installer_22", "apps/system-installer_48", "" },
   { "apps/system-software-update_16", "apps/system-software-update_22", "apps/system-software-update_48", "" },
   { "apps/system-users_16", "apps/system-users_22", "apps/system-users_48", "" },
   { "apps/utilities-system-monitor_16", "apps/utilities-system-monitor_22", "apps/utilities-system-monitor_48", "" },
   { "apps/utilities-terminal_16", "apps/utilities-terminal_22", "apps/utilities-terminal_48", "" },
   { "categories/applications-accessories_16", "categories/applications-accessories_22", "categories/applications-accessories_48", "" },
   { "categories/applications-development_16", "categories/applications-development_22", "categories/applications-development_48", "" },
   { "categories/applications-games_16", "categories/applications-games_22", "categories/applications-games_48", "" },
   { "categories/applications-graphics_16", "categories/applications-graphics_22", "categories/applications-graphics_48", "" },
   { "categories/applications-internet_16", "categories/applications-internet_22", "categories/applications-internet_48", "" },
   { "categories/applications-multimedia_16", "categories/applications-multimedia_22", "categories/applications-multimedia_48", "" },
   { "categories/applications-office_16", "categories/applications-office_22", "categories/applications-office_48", "" },
   { "categories/applications-other_16", "categories/applications-other_22", "categories/applications-other_48", "" },
   { "categories/applications-system_16", "categories/applications-system_22", "categories/applications-system_48", "" },
   { "categories/preferences-desktop-peripherals_16", "categories/preferences-desktop-peripherals_22", "categories/preferences-desktop-peripherals_48", "" },
   { "categories/preferences-desktop_16", "categories/preferences-desktop_22", "categories/preferences-desktop_48", "" },
   { "categories/preferences-system_16", "categories/preferences-system_22", "categories/preferences-system_48", "" },
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
   { "devices/multimedia-player_16", "devices/multimedia-player_22", "devices/multimedia-player_48", "" },
   { "devices/network_16", "devices/network_22", "devices/network_48", "" },
   { "devices/network-wireless_16", "devices/network-wireless_22", "devices/network-wireless_48", "" },
   { "devices/network-wired_16", "devices/network-wired_22", "devices/network-wired_48", "" },
   { "devices/printer_16", "devices/printer_22", "devices/printer_48", "" },
   { "devices/printer-remote_16", "devices/printer-remote_22", "devices/printer-remote_48", "" },
   { "devices/video-display_16", "devices/video-display_22", "devices/video-display_48", "" },
   { "emblems/emblem-favorite_16", "emblems/emblem-favorite_22", "emblems/emblem-favorite_48", "" },
   { "emblems/emblem-important_16", "emblems/emblem-important_22", "emblems/emblem-important_48", "" },
   { "emblems/emblem-photos_16", "emblems/emblem-photos_22", "emblems/emblem-photos_48", "" },
   { "emblems/emblem-readonly_16", "emblems/emblem-readonly_22", "emblems/emblem-readonly_48", "" },
   { "emblems/emblem-symbolic-link_16", "emblems/emblem-symbolic-link_22", "emblems/emblem-symbolic-link_48", "" },
   { "emblems/emblem-system_16", "emblems/emblem-system_22", "emblems/emblem-system_48", "" },
   { "emblems/emblem-unreadable_16", "emblems/emblem-unreadable_22", "emblems/emblem-unreadable_48", "" },
   { "emotes/face-angel_16", "emotes/face-angel_22", "emotes/face-angel_48", "" },
   { "emotes/face-crying_16", "emotes/face-crying_22", "emotes/face-crying_48", "" },
   { "emotes/face-devil-grin_16", "emotes/face-devil-grin_22", "emotes/face-devil-grin_48", "" },
   { "emotes/face-glasses_16", "emotes/face-glasses_22", "emotes/face-glasses_48", "" },
   { "emotes/face-grin_16", "emotes/face-grin_22", "emotes/face-grin_48", "" },
   { "emotes/face-kiss_16", "emotes/face-kiss_22", "emotes/face-kiss_48", "" },
   { "emotes/face-plain_16", "emotes/face-plain_22", "emotes/face-plain_48", "" },
   { "emotes/face-sad_16", "emotes/face-sad_22", "emotes/face-sad_48", "" },
   { "emotes/face-smile-big_16", "emotes/face-smile-big_22", "emotes/face-smile-big_48", "" },
   { "emotes/face-smile_16", "emotes/face-smile_22", "emotes/face-smile_48", "" },
   { "emotes/face-surprise_16", "emotes/face-surprise_22", "emotes/face-surprise_48", "" },
   { "emotes/face-wink_16", "emotes/face-wink_22", "emotes/face-wink_48", "" },
   { "mimetypes/application-certificate_16", "mimetypes/application-certificate_22", "mimetypes/application-certificate_48", "" },
   { "mimetypes/application-x-executable_16", "mimetypes/application-x-executable_22", "mimetypes/application-x-executable_48", "" },
   { "mimetypes/audio-x-generic_16", "mimetypes/audio-x-generic_22", "mimetypes/audio-x-generic_48", "" },
   { "mimetypes/font-x-generic_16", "mimetypes/font-x-generic_22", "mimetypes/font-x-generic_48", "" },
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
   { "mimetypes/x-office-calendar_16", "mimetypes/x-office-calendar_22", "mimetypes/x-office-calendar_48", "" },
   { "mimetypes/x-office-document_16", "mimetypes/x-office-document_22", "mimetypes/x-office-document_48", "" },
   { "mimetypes/x-office-presentation_16", "mimetypes/x-office-presentation_22", "mimetypes/x-office-presentation_48", "" },
   { "mimetypes/x-office-spreadsheet_16", "mimetypes/x-office-spreadsheet_22", "mimetypes/x-office-spreadsheet_48", "" },
   { "places/folder_16", "places/folder_22", "places/folder_48", "" },
   { "places/folder-remote_16", "places/folder-remote_22", "places/folder-remote_48", "" },
   { "places/folder-saved-search_16", "places/folder-saved-search_22", "places/folder-saved-search_48", "" },
   { "places/network-server_16", "places/network-server_22", "places/network-server_48", "" },
   { "places/network-workgroup_16", "places/network-workgroup_22", "places/network-workgroup_48", "" },
   { "places/start-here_16", "places/start-here_22", "places/start-here_48", "" },
   { "places/user-desktop_16", "places/user-desktop_22", "places/user-desktop_48", "" },
   { "places/user-home_16", "places/user-home_22", "places/user-home_48", "" },
   { "places/user-trash_16", "places/user-trash_22", "places/user-trash_48", "" },
   { "status/audio-volume-high_16", "status/audio-volume-high_22", "status/audio-volume-high_48", "" },
   { "status/audio-volume-low_16", "status/audio-volume-low_22", "status/audio-volume-low_48", "" },
   { "status/audio-volume-medium_16", "status/audio-volume-medium_22", "status/audio-volume-medium_48", "" },
   { "status/audio-volume-muted_16", "status/audio-volume-muted_22", "status/audio-volume-muted_48", "" },
   { "status/battery-caution_16", "status/battery-caution_22", "status/battery-caution_48", "" },
   { "status/dialog-error_16", "status/dialog-error_22", "status/dialog-error_48", "" },
   { "status/dialog-information_16", "status/dialog-information_22", "status/dialog-information_48", "" },
   { "status/dialog-warning_16", "status/dialog-warning_22", "status/dialog-warning_48", "" },
   { "status/dialog-question_16", "status/dialog-question_22", "status/dialog-question_48", "" },
   { "status/folder-drag-accept_16", "status/folder-drag-accept_22", "status/folder-drag-accept_48", "" },
   { "status/folder-open_16", "status/folder-open_22", "status/folder-open_48", "" },
   { "status/folder-visiting_16", "status/folder-visiting_22", "status/folder-visiting_48", "" },
   { "status/image-loading_16", "status/image-loading_22", "status/image-loading_48", "" },
   { "status/image-missing_16", "status/image-missing_22", "status/image-missing_48", "" },
   { "status/mail-attachment_16", "status/mail-attachment_22", "status/mail-attachment_48", "" },
   { "status/network-error_16", "status/network-error_22", "status/network-error_48", "" },
   { "status/network-idle_16", "status/network-idle_22", "status/network-idle_48", "" },
   { "status/network-offline_16", "status/network-offline_22", "status/network-offline_48", "" },
   { "status/network-online_16", "status/network-online_22", "status/network-online_48", "" },
   { "status/network-receive_16", "status/network-receive_22", "status/network-receive_48", "" },
   { "status/network-transmit_16", "status/network-transmit_22", "status/network-transmit_48", "" },
   { "status/network-transmit-receive_16", "status/network-transmit-receive_22", "status/network-transmit-receive_48", "" },
   { "status/network-wireless-encrypted_16", "status/network-wireless-encrypted_22", "status/network-wireless-encrypted_48", "" },
   { "status/printer-error_16", "status/printer-error_22", "status/printer-error_48", "" },
   { "status/user-trash-full_16", "status/user-trash-full_22", "status/user-trash-full_48", "" },
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
const char *etk_stock_key_get(Etk_Stock_Id stock_id, Etk_Stock_Size size)
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
const char *etk_stock_label_get(Etk_Stock_Id stock_id)
{
   if (stock_id < 0 || stock_id >= ETK_NUM_STOCK_IDS)
      return NULL;
   return _etk_stock_items[stock_id].label;
}

/** @} */
