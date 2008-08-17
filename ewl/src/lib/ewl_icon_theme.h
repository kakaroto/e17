/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_ICON_THEME_H
#define EWL_ICON_THEME_H

/**
 * @addtogroup Ewl_Icon_Theme Ewl_Icon_Theme: The icon theme code
 * @brief Provides code to retrieve the path to icon theme icons
 *
 * @{
 */

int              ewl_icon_theme_init(void);
void             ewl_icon_theme_shutdown(void);

void             ewl_icon_theme_theme_change(void);

const char      *ewl_icon_theme_icon_path_get(const char *icon, int size);

#define EWL_ICON_SIZE_SMALL 16        /**< 16x16 icons */
#define EWL_ICON_SIZE_MEDIUM 22        /**< 22x22 icons */
#define EWL_ICON_SIZE_LARGE 24        /**< 24x24 icons */

#define EWL_ICON_ADDRESS_BOOK_NEW "address-book-new"        /**< address book icon */
#define EWL_ICON_APPLICATION_EXIT "application-exit"        /**< application exit */
#define EWL_ICON_APPOINTMENT_NEW "appointment-new"        /**< appointment new */
#define EWL_ICON_CONTACT_NEW "contact-new"        /**< contact new */
#define EWL_ICON_DIALOG_APPLY "dialog-apply"        /**< dialog apply */
#define EWL_ICON_DIALOG_CANCEL "dialog-cancel"        /**< dialog cancel */
#define EWL_ICON_DIALOG_CLOSE "dialog-close"        /**< dialog close */
#define EWL_ICON_DIALOG_OK "dialog-ok"        /**< dialog ok */
#define EWL_ICON_DOCUMENT_NEW "document-new"        /**< document new */
#define EWL_ICON_DOCUMENT_OPEN "document-open"        /**< document open */
#define EWL_ICON_DOCUMENT_OPEN_RECENT "document-open-recent"        /**< document open recent */
#define EWL_ICON_DOCUMENT_PAGE_SETUP "document-page-setup"        /**< document page setup */
#define EWL_ICON_DOCUMENT_PRINT "document-print"        /**< document print */
#define EWL_ICON_DOCUMENT_PRINT_PREVIEW "document-print-preview"        /**< document print preview */
#define EWL_ICON_DOCUMENT_PROPERTIES "document-properties"        /**< document properities */
#define EWL_ICON_DOCUMENT_REVERT "document-revert"        /**< document revent */
#define EWL_ICON_DOCUMENT_SAVE "document-save"        /**< document save */
#define EWL_ICON_DOCUMENT_SAVE_AS "document-save-as"        /**< document save as */
#define EWL_ICON_EDIT_COPY "edit-copy"        /**< edit copy */
#define EWL_ICON_EDIT_CUT "edit-cut"        /**< edit cut */
#define EWL_ICON_EDIT_DELETE "edit-delete"        /**< edit delete */
#define EWL_ICON_EDIT_FIND "edit-find"        /**< edit find */
#define EWL_ICON_EDIT_FIND_REPLACE "edit-find-replace"        /**< edit find replace */
#define EWL_ICON_EDIT_PASTE "edit-paste"        /**< edit paste */
#define EWL_ICON_EDIT_REDO "edit-redo"        /**< edit redo */
#define EWL_ICON_EDIT_SELECT_ALL "edit-select-all"        /**< edit select all */
#define EWL_ICON_EDIT_UNDO "edit-undo"        /**< edit undo */
#define EWL_ICON_FORMAT_INDENT_LESS "format-indent-less"        /**< format indent less */
#define EWL_ICON_FORMAT_INDENT_MORE "format-indent-more"        /**< format indent more */
#define EWL_ICON_FORMAT_JUSTIFY_CENTER "format-justify-center"        /**< format justify center */
#define EWL_ICON_FORMAT_JUSTIFY_FILL "format-justify-fill"        /**< format justify fill */
#define EWL_ICON_FORMAT_JUSTIFY_LEFT "format-justify-left"        /**< format justify left */
#define EWL_ICON_FORMAT_JUSTIFY_RIGHT "format-justify-right"        /**< format justify right */
#define EWL_ICON_FORMAT_TEXT_DIRECTION_LTR "format-text-direction-ltr"        /**< format text direction left to right */
#define EWL_ICON_FORMAT_TEXT_DIRECTION_RTL "format-text-direction-rtl"        /**< format text direction right to left */
#define EWL_ICON_FORMAT_TEXT_BOLD "format-text-bold"        /**< format text bold */
#define EWL_ICON_FORMAT_TEXT_ITALIC "format-text-italic"        /**< format text italic */
#define EWL_ICON_FORMAT_TEXT_UNDERLINE "format-text-underline"        /**< format text underline */
#define EWL_ICON_FORMAT_TEXT_STRIKETHROUGH "format-text-strikethrough"        /**< format text strikethrough */
#define EWL_ICON_GO_BOTTOM "go-bottom"        /**< go bottom */
#define EWL_ICON_GO_DOWN "go-down"        /**< go down */
#define EWL_ICON_GO_FIRST "go-first"        /**< go first */
#define EWL_ICON_GO_HOME "go-home"        /**< go home */
#define EWL_ICON_GO_JUMP "go-jump"        /**< go jump */
#define EWL_ICON_GO_LAST "go-last"        /**< go last */
#define EWL_ICON_GO_NEXT "go-next"        /**< go next */
#define EWL_ICON_GO_PREVIOUS "go-previous"        /**< go previous */
#define EWL_ICON_GO_TOP "go-top"        /**< go top */
#define EWL_ICON_GO_UP "go-up"        /**< go up */
#define EWL_ICON_HELP_ABOUT "help-about"        /**< help about */
#define EWL_ICON_HELP_CONTENTS "help-contents"        /**< help contents */
#define EWL_ICON_HELP_FAQ "help-faq"        /**< help faq */
#define EWL_ICON_INSERT_IMAGE "insert-image"        /**< insert image */
#define EWL_ICON_INSERT_LINK "insert-link"        /**< insert link */
#define EWL_ICON_INSERT_OBJECT "insert-object"        /**< insert object */
#define EWL_ICON_INSERT_TEXT "insert-text"        /**< insert text */
#define EWL_ICON_LIST_ADD "list-add"        /**< list add */
#define EWL_ICON_LIST_REMOVE "list-remove"        /**< list remove */
#define EWL_ICON_MAIL_FORWARD "mail-forward"        /**< mail forward */
#define EWL_ICON_MAIL_MARK_IMPORTANT "mail-mark-important"        /**< mail mark important */
#define EWL_ICON_MAIL_MARK_JUNK "mail-mark-junk"        /**< mail mark junk */
#define EWL_ICON_MAIL_MARK_NOTJUNK "mail-mark-notjunk"        /**< mail mark not junk */
#define EWL_ICON_MAIL_MARK_READ "mail-mark-read"        /**< mail mark read */
#define EWL_ICON_MAIL_MARK_UNREAD "mail-mark-unread"        /**< mail mark unread */
#define EWL_ICON_MAIL_MESSAGE_NEW "mail-message-new"        /**< mail message new */
#define EWL_ICON_MAIL_REPLY_ALL "mail-reply-all"        /**< mail reply all */
#define EWL_ICON_MAIL_REPLY_SENDER "mail-reply-sender"        /**< mail reply sender */
#define EWL_ICON_MAIL_SEND_RECEIVE "mail-send-receive"        /**< mail send receive */
#define EWL_ICON_MEDIA_EJECT "media-eject"        /**< media eject */
#define EWL_ICON_MEDIA_PLAYBACK_PAUSE "media-playback-pause"        /**< media playback pause */
#define EWL_ICON_MEDIA_PLAYBACK_START "media-playback-start"        /**< media playback start */
#define EWL_ICON_MEDIA_PLAYBACK_STOP "media-playback-stop"        /**< media playback stop */
#define EWL_ICON_MEDIA_RECORD "media-record"        /**< media record */
#define EWL_ICON_MEDIA_SEEK_BACKWARD "media-seek-backward"        /**< media seek backward */
#define EWL_ICON_MEDIA_SEEK_FORWARD "media-seek-forward"        /**< media seek forward */
#define EWL_ICON_MEDIA_SKIP_BACKWARD "media-skip-backward"        /**< media skip backward */
#define EWL_ICON_MEDIA_SKIP_FORWARD "media-skip-forward"        /**< media skip forward */
#define EWL_ICON_SYSTEM_LOCK_SCREEN "system-lock-screen"        /**< system lock screen */
#define EWL_ICON_SYSTEM_LOG_OUT "system-log-out"        /**< system log out */
#define EWL_ICON_SYSTEM_RUN "system-run"        /**< system run */
#define EWL_ICON_SYSTEM_SEARCH "system-search"        /**< system search */
#define EWL_ICON_TOOLS_CHECK_SPELLING "tools-check-spelling"        /**< tools check spelling */
#define EWL_ICON_VIEW_FULLSCREEN "view-fullscreen"        /**< view fullscreen */
#define EWL_ICON_VIEW_REFRESH "view-refresh"        /**< view refresh */
#define EWL_ICON_VIEW_SORT_ASCENDING "view-sort-ascending"        /**< view sort assending */
#define EWL_ICON_VIEW_SORT_DESCENDING "view-sort-descending"        /**< view sort descending */
#define EWL_ICON_WINDOW_CLOSE "window-close"        /**< window close */
#define EWL_ICON_WINDOW_NEW "window-new"        /**< window new */
#define EWL_ICON_ZOOM_BEST_FIT "zoom-best-fit"        /**< zoom best fit */
#define EWL_ICON_ZOOM_IN "zoom-in"        /**< zoom in */
#define EWL_ICON_ZOOM_ORIGINAL "zoom-original"        /**< zoom original */
#define EWL_ICON_ZOOM_OUT "zoom-out"        /**< zoom out */

#define EWL_ICON_PROCESS_WORKING "process-working"        /**< process working */

#define EWL_ICON_ACCESSORIES_CALCULATOR "accessories-calculator"        /**< accessories calculator */
#define EWL_ICON_ACCESSORIES_CHARACTER_MAP "accessories-character-map"        /**< accessories character map */
#define EWL_ICON_ACCESSORIES_DICTIONARY "accessories-dictionary"        /**< accessories dictionary */
#define EWL_ICON_ACCESSORIES_TEXT_EDITOR "accessories-text-editor"        /**< accessories text editor */
#define EWL_ICON_HELP_BROWSER "help-browser"        /**< help browser */
#define EWL_ICON_MULTIMEDIA_VOLUME_CONTROL "multimedia-volume-control"        /**< multimedia volume control */
#define EWL_ICON_PREFERENCES_DESKTOP_ACCESSIBILITY "preferences-desktop-accessibility"        /**< pref desktop
                                                                                                accessibility */
#define EWL_ICON_PREFERENCES_DESKTOP_FONT "preferences-desktop-font"        /**< pref desktop font */
#define EWL_ICON_PREFERENCES_DESKTOP_KEYBOARD "preferences-desktop-keyboard" /**< pref desktop keyboard */
#define EWL_ICON_PREFERENCES_DESKTOP_LOCALE "preferences-desktop-locale" /**< pref desktop locale */
#define EWL_ICON_PREFERENCES_DESKTOP_MULTIMEDIA "preferences-desktop-multimedia" /**< pref desktop multimedia */
#define EWL_ICON_PREFERENCES_DESKTOP_SCREENSAVER "preferences-desktop-screensaver" /**< pref desktop scrensaver*/
#define EWL_ICON_PREFERENCES_DESKTOP_THEME "preferences-desktop-theme"        /**< pref desktop theme */
#define EWL_ICON_PREFERENCES_DESKTOP_WALLPAPER "preferences-desktop-wallpaper" /**< pref desktop wallpaper */
#define EWL_ICON_SYSTEM_FILE_MANAGER "system-file-manager"        /**< system file manager */
#define EWL_ICON_SYSTEM_SOFTWARE_UPDATE "system-software-update"        /**< system software update */
#define EWL_ICON_UTILITIES_TERMINAL "utilities-terminal"        /**< utilities terminal */

#define EWL_ICON_APPLICATIONS_ACCESSORIES "applications-accessories"        /**< apps accessories */
#define EWL_ICON_APPLICATIONS_DEVELOPMENT "applications-development"        /**< apps development */
#define EWL_ICON_APPLICATIONS_GAMES "applications-games"        /**< apps games*/
#define EWL_ICON_APPLICATIONS_GRAPHICS "applications-graphics"        /**< apps graphics */
#define EWL_ICON_APPLICATIONS_INTERNET "applications-internet"        /**< apps internet */
#define EWL_ICON_APPLICATIONS_MULTIMEDIA "applications-multimedia"        /**< apps multimedia */
#define EWL_ICON_APPLICATIONS_OFFICE "applications-office"        /**< apps office */
#define EWL_ICON_APPLICATIONS_OTHER "applications-other"        /**< apps other */
#define EWL_ICON_APPLICATIONS_SYSTEM "applications-system"        /**< apps system */
#define EWL_ICON_APPLICATIONS_UTILITIES "applications-utilities"        /**< apps utilities */
#define EWL_ICON_PREFERENCES_DESKTOP "preferences-desktop"        /**< prefs desktop */
#define EWL_ICON_PREFERENCES_DESKTOP_ACCESSIBILITY "preferences-desktop-accessibility"        /**< prefs desktop
                                                                                                accessibility */
#define EWL_ICON_PREFERENCES_DESKTOP_PERIPHERALS "preferences-desktop-peripherals" /**< prefs desktop
                                                                                                peripherals */
#define EWL_ICON_PREFERENCES_DESKTOP_PERSONAL "preferences-desktop-personal" /**< prefs desktop personal */
#define EWL_ICON_PREFERENCES_OTHER "preferences-other"        /**< prefs other */
#define EWL_ICON_PREFERENCES_SYSTEM "preferences-system"        /**< prefs system */
#define EWL_ICON_PREFERENCES_SYSTEM_NETWORK "preferences-system-network" /**< prefs system network */
#define EWL_ICON_SYSTEM_HELP "system-help"        /**< system help */

#define EWL_ICON_AUDIO_CARD "audio-card"        /**< audio card */
#define EWL_ICON_AUDIO_INPUT_MICROPHONE "audio-input-microphone" /**< audio input microphone */
#define EWL_ICON_BATTERY "battery"        /**< battery */
#define EWL_ICON_CAMERA_PHOTO "camera-photo"        /**< camera photo */
#define EWL_ICON_CAMERA_VIDEO "camera-video"        /**< camera video */
#define EWL_ICON_COMPUTER "computer"        /**< computer */
#define EWL_ICON_DRIVE_CDROM "drive-cdrom"        /**< drive cdrom */
#define EWL_ICON_DRIVE_HARDDISK "drive-harddisk"        /**< drive harddisk */
#define EWL_ICON_DRIVE_REMOVABLE_MEDIA "drive-removable-media"        /**< drive removable media */
#define EWL_ICON_INPUT_GAMING "input-gaming"        /**< input gaming */
#define EWL_ICON_INPUT_KEYBOARD "input-keyboard"        /**< input keyboard */
#define EWL_ICON_INPUT_MOUSE "input-mouse"        /**< input mouse */
#define EWL_ICON_MEDIA_CDROM "media-cdrom"        /**< media cdrom */
#define EWL_ICON_MEDIA_FLOPPY "media-floppy"        /**< media floppy */
#define EWL_ICON_MULTIMEDIA_PLAYER "multimedia-player"        /**< multimedia player */
#define EWL_ICON_NETWORK_WIRED "network-wired"        /**< network wired */
#define EWL_ICON_NETWORK_WIRELESS "network-wireless"        /**< network wireless */
#define EWL_ICON_PRINTER "printer"        /**< printer */

#define EWL_ICON_EMBLEM_DEFAULT "emblem-default"        /**< emblem default */
#define EWL_ICON_EMBLEM_DOCUMENTS "emblem-documents"        /**< emblem documents */
#define EWL_ICON_EMBLEM_DOWNLOADS "emblem-downloads"        /**< emblem downloads */
#define EWL_ICON_EMBLEM_FAVORITE "emblem-favorite"        /**< emblem favorite */
#define EWL_ICON_EMBLEM_IMPORTANT "emblem-important"        /**< emblem important */
#define EWL_ICON_EMBLEM_MAIL "emblem-mail"        /**< emblem mail */
#define EWL_ICON_EMBLEM_PHOTOS "emblem-photos"        /**< emblem photos */
#define EWL_ICON_EMBLEM_READONLY "emblem-readonly"        /**< emblem readonly */
#define EWL_ICON_EMBLEM_SHARED "emblem-shared"        /**< emblem shared */
#define EWL_ICON_EMBLEM_SYMBOLIC_LINK "emblem-symbolic-link"        /**< emblem symbolic link */
#define EWL_ICON_EMBLEM_SYNCHRONIZED "emblem-synchronized"        /**< emblem synchronized */
#define EWL_ICON_EMBLEM_SYSTEM "emblem-system"        /**< emblem system */
#define EWL_ICON_EMBLEM_UNREADABLE "emblem-unreadable"        /**< emblem unreadable */

#define EWL_ICON_FACE_ANGEL "face-angel"        /**< face angel */
#define EWL_ICON_FACE_CRYING "face-crying"        /**< face crying */
#define EWL_ICON_FACE_DEVIL_GRIN "face-devil-grin"        /**< face devil grin */
#define EWL_ICON_FACE_DEVIL_SAD "face-devil-sad"        /**< face devil sad */
#define EWL_ICON_FACE_GLASSES "face-glasses"        /**< face glasses */
#define EWL_ICON_FACE_KISS "face-kiss"        /**< face kiss */
#define EWL_ICON_FACE_MONKEY "face-monkey"        /**< face monkey */
#define EWL_ICON_FACE_PLAIN "face-plain"        /**< face plain */
#define EWL_ICON_FACE_SAD "face-sad"        /**< face sad */
#define EWL_ICON_FACE_SMILE "face-smile"        /**< face smile */
#define EWL_ICON_FACE_SMILE_BIG "face-smile-big"        /**< face smile big */
#define EWL_ICON_FACE_SMIRK "face-smirk"        /**< face smirk */
#define EWL_ICON_FACE_SURPRISE "face-surprise"        /**< face surprise */
#define EWL_ICON_FACE_WINK "face-wink"        /**< face wink */

#define EWL_ICON_APPLICATION_X_EXECUTABLE "application-x-executable"        /**< application/x-executable */
#define EWL_ICON_AUDIO_X_GENERIC "audio-x-generic"        /**< audio/x-generic */
#define EWL_ICON_FONT_X_GENERIC "font-x-generic"        /**< font/x-generic */
#define EWL_ICON_IMAGE_X_GENERIC "image-x-generic"        /**< image/x-generic */
#define EWL_ICON_PACKAGE_X_GENERIC "package-x-generic"        /**< package/x-generic */
#define EWL_ICON_TEXT_HTML "text-html"        /**< text/html */
#define EWL_ICON_TEXT_X_GENERIC "text-x-generic"        /**< text/x-generic */
#define EWL_ICON_TEXT_X_GENERIC_TEMPLATE "text-x-generic-template" /**< text/x-generic-template */
#define EWL_ICON_TEXT_X_SCRIPT "text-x-script"        /**< text/x-script*/
#define EWL_ICON_VIDEO_X_GENERIC "video-x-generic"        /**< video/x-generic */
#define EWL_ICON_X_OFFICE_ADDRESS_BOOK "x-office-address-book"        /**< x-office/address-book */
#define EWL_ICON_X_OFFICE_CALENDAR "x-office-calendar"        /**< x-office/calendar */
#define EWL_ICON_X_OFFICE_DOCUMENT "x-office-document"        /**< x-office/document */
#define EWL_ICON_X_OFFICE_PRESENTATION "x-office-presentation"        /**< x-office/presentation */
#define EWL_ICON_X_OFFICE_SPREADSHEET "x-office-spreadsheet"        /**< x-office/spreadsheet */

#define EWL_ICON_FOLDER "folder"        /**< folder */
#define EWL_ICON_FOLDER_NEW "folder-new"        /**< new folder */
#define EWL_ICON_FOLDER_REMOTE "folder-remote"        /**< folder remote */
#define EWL_ICON_NETWORK_SERVER "network-server"        /**< network server */
#define EWL_ICON_NETWORK_WORKGROUP "network-workgroup"        /**< network workgroup */
#define EWL_ICON_START_HERE "start-here"        /**< start here */
#define EWL_ICON_USER_DESKTOP "user-desktop"        /**< user desktop */
#define EWL_ICON_USER_HOME "user-home"        /**< user home */
#define EWL_ICON_USER_TRASH "user-trash"        /**< user trash */

#define EWL_ICON_APPOINTMENT_MISSED "appointment-missed"        /**< appointment missed */
#define EWL_ICON_APPOINTMENT_SOON "appointment-soon"        /**< appointment soon */
#define EWL_ICON_AUDIO_VOLUME_HIGH "audio-volume-high"        /**< audio volume high */
#define EWL_ICON_AUDIO_VOLUME_LOW "audio-volume-low"        /**< audio volume low */
#define EWL_ICON_AUDIO_VOLUME_MEDIUM "audio-volume-medium"        /**< audio volume medium */
#define EWL_ICON_AUDIO_VOLUME_MUTED "audio-volume-muted"        /**< audio volume muted */
#define EWL_ICON_BATTERY_CAUTION "battery-caution"        /**< battery caution */
#define EWL_ICON_BATTERY_LOW "battery-low"        /**< battery low */
#define EWL_ICON_DIALOG_ERROR "dialog-error"        /**< dialog error */
#define EWL_ICON_DIALOG_INFORMATION "dialog-information"        /**< dialog information */
#define EWL_ICON_DIALOG_PASSWORD "dialog-password"        /**< dialog password */
#define EWL_ICON_DIALOG_QUESTION "dialog-question"        /**< dialog question */
#define EWL_ICON_DIALOG_WARNING "dialog-warning"        /**< dialog warning */
#define EWL_ICON_FOLDER_DRAG_ACCEPT "folder-drag-accept"        /**< folder drag accept */
#define EWL_ICON_FOLDER_OPEN "folder-open"        /**< folder open */
#define EWL_ICON_FOLDER_VISITING "folder-visiting"        /**< folder visiting */
#define EWL_ICON_IMAGE_LOADING "image-loading"        /**< image loading */
#define EWL_ICON_IMAGE_MISSING "image-missing"        /**< image missing */
#define EWL_ICON_MAIL_ATTACHMENT "mail-attachment"        /**< mail attachment */
#define EWL_ICON_MAIL_UNREAD "mail-unread"        /**< mail unread */
#define EWL_ICON_MAIL_READ "mail-read"        /**< mail read */
#define EWL_ICON_MAIL_REPLIED "mail-replied"        /**< mail replied */
#define EWL_ICON_MAIL_SIGNED "mail-signed"        /**< mail signed */
#define EWL_ICON_MAIL_SIGNED_VERIFIED "mail-signed-verified"        /**< mail signed verified */
#define EWL_ICON_MEDIA_PLAYLIST_REPEAT "media-playlist-repeat"        /**< media playlist repeat */
#define EWL_ICON_MEDIA_PLAYLIST_SHUFFLE "media-playlist-shuffle" /**< media playlist shuffle */
#define EWL_ICON_NETWORK_ERROR "network-error"        /**< network error */
#define EWL_ICON_NETWORK_IDLE "network-idle"        /**< network idle */

#define EWL_ICON_NETWORK_OFFLINE "network-offline"        /**< network offline */
#define EWL_ICON_NETWORK_RECEIVE "network-receive"        /**< network receive */
#define EWL_ICON_NETWORK_TRANSMIT "network-transmit"        /**< network transmit */
#define EWL_ICON_NETWORK_TRANSMIT_RECEIVE "network-transmit-receive"        /**< network transmit receive */
#define EWL_ICON_PRINTER_ERROR "printer-error"        /**< printer error */
#define EWL_ICON_PRINTER_PRINTING "printer-printing"        /**< printer printing */
#define EWL_ICON_SOFTWARE_UPDATE_AVAILABLE "software-update-available"        /**< software update*/
#define EWL_ICON_SOFTWARE_UPDATE_URGENT "software-update-urgent" /**< software update urgent */
#define EWL_ICON_SYNC_ERROR "sync-error"        /**< sync error */
#define EWL_ICON_SYNC_SYNCHRONIZING "sync-synchronizing"        /**< sync synchronizing */
#define EWL_ICON_TASK_DUE "task-due"        /**< task due */
#define EWL_ICON_TASK_PASSED_DUE "task-passed-due"        /**< task passwd due */
#define EWL_ICON_USER_AWAY "user-away"        /**< user away */
#define EWL_ICON_USER_IDLE "user-idle"        /**< user idle */
#define EWL_ICON_USER_OFFLINE "user-offline"        /**< user offline */
#define EWL_ICON_USER_ONLINE "user-online"        /**< user online */
#define EWL_ICON_USER_TRASH_FULL "user-trash-full"        /**< user trash full */
#define EWL_ICON_WEATHER_CLEAR "weather-clear"        /**< weather clear */
#define EWL_ICON_WEATHER_CLEAR_NIGHT "weather-clear-night"        /**< weather clear night */
#define EWL_ICON_WEATHER_FEW_CLOUDS "weather-few-clouds"        /**< weather few clouds */
#define EWL_ICON_WEATHER_FEW_CLOUDS_NIGHT "weather-few-clouds-night"        /**< weather few clouds night */
#define EWL_ICON_WEATHER_FOG "weather-fog"        /**< weather fog */
#define EWL_ICON_WEATHER_OVERCAST "weather-overcast"        /**< weather overcast */
#define EWL_ICON_WEATHER_SEVERE_ALERT "weather-severe-alert"        /**< weather severe alert */
#define EWL_ICON_WEATHER_SHOWERS "weather-showers"        /**< weather showers */
#define EWL_ICON_WEATHER_SHOWERS_SCATTERED "weather-showers-scattered"        /**< weather showers scattered */
#define EWL_ICON_WEATHER_SNOW "weather-snow"        /**< weather snow */
#define EWL_ICON_WEATHER_STORM "weather-storm"        /**< weather storm */

/**
 * @}
 */

#endif

