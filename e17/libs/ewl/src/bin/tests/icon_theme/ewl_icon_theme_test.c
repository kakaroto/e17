/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_freebox.h"
#include "ewl_icon.h"
#include "ewl_icon_theme.h"
#include "ewl_scrollpane.h"

#include <stdio.h>

#define EWL_ICON_THEME_PER_LOOP 20

static int ewl_icon_theme_pos = 0;
const char *icons[] = {
                        EWL_ICON_ADDRESS_BOOK_NEW,
                        EWL_ICON_APPLICATION_EXIT,
                        EWL_ICON_APPOINTMENT_NEW,
                        EWL_ICON_CONTACT_NEW,
                        EWL_ICON_DIALOG_APPLY,
                        EWL_ICON_DIALOG_CANCEL,
                        EWL_ICON_DIALOG_CLOSE,
                        EWL_ICON_DIALOG_OK,
                        EWL_ICON_DOCUMENT_NEW,
                        EWL_ICON_DOCUMENT_OPEN,
                        EWL_ICON_DOCUMENT_OPEN_RECENT,
                        EWL_ICON_DOCUMENT_PAGE_SETUP,
                        EWL_ICON_DOCUMENT_PRINT,
                        EWL_ICON_DOCUMENT_PRINT_PREVIEW,
                        EWL_ICON_DOCUMENT_PROPERTIES,
                        EWL_ICON_DOCUMENT_REVERT,
                        EWL_ICON_DOCUMENT_SAVE,
                        EWL_ICON_DOCUMENT_SAVE_AS,
                        EWL_ICON_EDIT_COPY,
                        EWL_ICON_EDIT_CUT,
                        EWL_ICON_EDIT_DELETE,
                        EWL_ICON_EDIT_FIND,
                        EWL_ICON_EDIT_FIND_REPLACE,
                        EWL_ICON_EDIT_PASTE,
                        EWL_ICON_EDIT_REDO,
                        EWL_ICON_EDIT_SELECT_ALL,
                        EWL_ICON_EDIT_UNDO,
                        EWL_ICON_FORMAT_INDENT_LESS,
                        EWL_ICON_FORMAT_INDENT_MORE,
                        EWL_ICON_FORMAT_JUSTIFY_CENTER,
                        EWL_ICON_FORMAT_JUSTIFY_FILL,
                        EWL_ICON_FORMAT_JUSTIFY_LEFT,
                        EWL_ICON_FORMAT_JUSTIFY_RIGHT,
                        EWL_ICON_FORMAT_TEXT_DIRECTION_LTR,
                        EWL_ICON_FORMAT_TEXT_DIRECTION_RTL,
                        EWL_ICON_FORMAT_TEXT_BOLD,
                        EWL_ICON_FORMAT_TEXT_ITALIC,
                        EWL_ICON_FORMAT_TEXT_UNDERLINE,
                        EWL_ICON_FORMAT_TEXT_STRIKETHROUGH,
                        EWL_ICON_GO_BOTTOM,
                        EWL_ICON_GO_DOWN,
                        EWL_ICON_GO_FIRST,
                        EWL_ICON_GO_HOME,
                        EWL_ICON_GO_JUMP,
                        EWL_ICON_GO_LAST,
                        EWL_ICON_GO_NEXT,
                        EWL_ICON_GO_PREVIOUS,
                        EWL_ICON_GO_TOP,
                        EWL_ICON_GO_UP,
                        EWL_ICON_HELP_ABOUT,
                        EWL_ICON_HELP_CONTENTS,
                        EWL_ICON_HELP_FAQ,
                        EWL_ICON_INSERT_IMAGE,
                        EWL_ICON_INSERT_LINK,
                        EWL_ICON_INSERT_OBJECT,
                        EWL_ICON_INSERT_TEXT,
                        EWL_ICON_LIST_ADD,
                        EWL_ICON_LIST_REMOVE,
                        EWL_ICON_MAIL_FORWARD,
                        EWL_ICON_MAIL_MARK_IMPORTANT,
                        EWL_ICON_MAIL_MARK_JUNK,
                        EWL_ICON_MAIL_MARK_NOTJUNK,
                        EWL_ICON_MAIL_MARK_READ,
                        EWL_ICON_MAIL_MARK_UNREAD,
                        EWL_ICON_MAIL_MESSAGE_NEW,
                        EWL_ICON_MAIL_REPLY_ALL,
                        EWL_ICON_MAIL_REPLY_SENDER,
                        EWL_ICON_MAIL_SEND_RECEIVE,
                        EWL_ICON_MEDIA_EJECT,
                        EWL_ICON_MEDIA_PLAYBACK_PAUSE,
                        EWL_ICON_MEDIA_PLAYBACK_START,
                        EWL_ICON_MEDIA_PLAYBACK_STOP,
                        EWL_ICON_MEDIA_RECORD,
                        EWL_ICON_MEDIA_SEEK_BACKWARD,
                        EWL_ICON_MEDIA_SEEK_FORWARD,
                        EWL_ICON_MEDIA_SKIP_BACKWARD,
                        EWL_ICON_MEDIA_SKIP_FORWARD,
                        EWL_ICON_SYSTEM_LOCK_SCREEN,
                        EWL_ICON_SYSTEM_LOG_OUT,
                        EWL_ICON_SYSTEM_RUN,
                        EWL_ICON_SYSTEM_SEARCH,
                        EWL_ICON_TOOLS_CHECK_SPELLING,
                        EWL_ICON_VIEW_FULLSCREEN,
                        EWL_ICON_VIEW_REFRESH,
                        EWL_ICON_VIEW_SORT_ASCENDING,
                        EWL_ICON_VIEW_SORT_DESCENDING,
                        EWL_ICON_WINDOW_CLOSE,
                        EWL_ICON_WINDOW_NEW,
                        EWL_ICON_ZOOM_BEST_FIT,
                        EWL_ICON_ZOOM_IN,
                        EWL_ICON_ZOOM_ORIGINAL,
                        EWL_ICON_ZOOM_OUT,
                        EWL_ICON_PROCESS_WORKING,
                        EWL_ICON_ACCESSORIES_CALCULATOR,
                        EWL_ICON_ACCESSORIES_CHARACTER_MAP,
                        EWL_ICON_ACCESSORIES_DICTIONARY,
                        EWL_ICON_ACCESSORIES_TEXT_EDITOR,
                        EWL_ICON_HELP_BROWSER,
                        EWL_ICON_MULTIMEDIA_VOLUME_CONTROL,
                        EWL_ICON_PREFERENCES_DESKTOP_ACCESSIBILITY,
                        EWL_ICON_PREFERENCES_DESKTOP_FONT,
                        EWL_ICON_PREFERENCES_DESKTOP_KEYBOARD,
                        EWL_ICON_PREFERENCES_DESKTOP_LOCALE,
                        EWL_ICON_PREFERENCES_DESKTOP_MULTIMEDIA,
                        EWL_ICON_PREFERENCES_DESKTOP_SCREENSAVER,
                        EWL_ICON_PREFERENCES_DESKTOP_THEME,
                        EWL_ICON_PREFERENCES_DESKTOP_WALLPAPER,
                        EWL_ICON_SYSTEM_FILE_MANAGER,
                        EWL_ICON_SYSTEM_SOFTWARE_UPDATE,
                        EWL_ICON_UTILITIES_TERMINAL,
                        EWL_ICON_APPLICATIONS_ACCESSORIES,
                        EWL_ICON_APPLICATIONS_DEVELOPMENT,
                        EWL_ICON_APPLICATIONS_GAMES,
                        EWL_ICON_APPLICATIONS_GRAPHICS,
                        EWL_ICON_APPLICATIONS_INTERNET,
                        EWL_ICON_APPLICATIONS_MULTIMEDIA,
                        EWL_ICON_APPLICATIONS_OFFICE,
                        EWL_ICON_APPLICATIONS_OTHER,
                        EWL_ICON_APPLICATIONS_SYSTEM,
                        EWL_ICON_APPLICATIONS_UTILITIES,
                        EWL_ICON_PREFERENCES_DESKTOP,
                        EWL_ICON_PREFERENCES_DESKTOP_ACCESSIBILITY,
                        EWL_ICON_PREFERENCES_DESKTOP_PERIPHERALS,
                        EWL_ICON_PREFERENCES_DESKTOP_PERSONAL,
                        EWL_ICON_PREFERENCES_OTHER,
                        EWL_ICON_PREFERENCES_SYSTEM,
                        EWL_ICON_PREFERENCES_SYSTEM_NETWORK,
                        EWL_ICON_SYSTEM_HELP,
                        EWL_ICON_AUDIO_CARD,
                        EWL_ICON_AUDIO_INPUT_MICROPHONE,
                        EWL_ICON_BATTERY,
                        EWL_ICON_CAMERA_PHOTO,
                        EWL_ICON_CAMERA_VIDEO,
                        EWL_ICON_COMPUTER,
                        EWL_ICON_DRIVE_CDROM,
                        EWL_ICON_DRIVE_HARDDISK,
                        EWL_ICON_DRIVE_REMOVABLE_MEDIA,
                        EWL_ICON_INPUT_GAMING,
                        EWL_ICON_INPUT_KEYBOARD,
                        EWL_ICON_INPUT_MOUSE,
                        EWL_ICON_MEDIA_CDROM,
                        EWL_ICON_MEDIA_FLOPPY,
                        EWL_ICON_MULTIMEDIA_PLAYER,
                        EWL_ICON_NETWORK_WIRED,
                        EWL_ICON_NETWORK_WIRELESS,
                        EWL_ICON_PRINTER,
                        EWL_ICON_EMBLEM_DEFAULT,
                        EWL_ICON_EMBLEM_DOCUMENTS,
                        EWL_ICON_EMBLEM_DOWNLOADS,
                        EWL_ICON_EMBLEM_FAVORITE,
                        EWL_ICON_EMBLEM_IMPORTANT,
                        EWL_ICON_EMBLEM_MAIL,
                        EWL_ICON_EMBLEM_PHOTOS,
                        EWL_ICON_EMBLEM_READONLY,
                        EWL_ICON_EMBLEM_SHARED,
                        EWL_ICON_EMBLEM_SYMBOLIC_LINK,
                        EWL_ICON_EMBLEM_SYNCHRONIZED,
                        EWL_ICON_EMBLEM_SYSTEM,
                        EWL_ICON_EMBLEM_UNREADABLE,
                        EWL_ICON_FACE_ANGEL,
                        EWL_ICON_FACE_CRYING,
                        EWL_ICON_FACE_DEVIL_GRIN,
                        EWL_ICON_FACE_DEVIL_SAD,
                        EWL_ICON_FACE_GLASSES,
                        EWL_ICON_FACE_KISS,
                        EWL_ICON_FACE_MONKEY,
                        EWL_ICON_FACE_PLAIN,
                        EWL_ICON_FACE_SAD,
                        EWL_ICON_FACE_SMILE,
                        EWL_ICON_FACE_SMILE_BIG,
                        EWL_ICON_FACE_SMIRK,
                        EWL_ICON_FACE_SURPRISE,
                        EWL_ICON_FACE_WINK,
                        EWL_ICON_APPLICATION_X_EXECUTABLE,
                        EWL_ICON_AUDIO_X_GENERIC,
                        EWL_ICON_FONT_X_GENERIC,
                        EWL_ICON_IMAGE_X_GENERIC,
                        EWL_ICON_PACKAGE_X_GENERIC,
                        EWL_ICON_TEXT_HTML,
                        EWL_ICON_TEXT_X_GENERIC,
                        EWL_ICON_TEXT_X_GENERIC_TEMPLATE,
                        EWL_ICON_TEXT_X_SCRIPT,
                        EWL_ICON_VIDEO_X_GENERIC,
                        EWL_ICON_X_OFFICE_ADDRESS_BOOK,
                        EWL_ICON_X_OFFICE_CALENDAR,
                        EWL_ICON_X_OFFICE_DOCUMENT,
                        EWL_ICON_X_OFFICE_PRESENTATION,
                        EWL_ICON_X_OFFICE_SPREADSHEET,
                        EWL_ICON_FOLDER,
                        EWL_ICON_FOLDER_NEW,
                        EWL_ICON_FOLDER_REMOTE,
                        EWL_ICON_NETWORK_SERVER,
                        EWL_ICON_NETWORK_WORKGROUP,
                        EWL_ICON_START_HERE,
                        EWL_ICON_USER_DESKTOP,
                        EWL_ICON_USER_HOME,
                        EWL_ICON_USER_TRASH,
                        EWL_ICON_APPOINTMENT_MISSED,
                        EWL_ICON_APPOINTMENT_SOON,
                        EWL_ICON_AUDIO_VOLUME_HIGH,
                        EWL_ICON_AUDIO_VOLUME_LOW,
                        EWL_ICON_AUDIO_VOLUME_MEDIUM,
                        EWL_ICON_AUDIO_VOLUME_MUTED,
                        EWL_ICON_BATTERY_CAUTION,
                        EWL_ICON_BATTERY_LOW,
                        EWL_ICON_DIALOG_ERROR,
                        EWL_ICON_DIALOG_INFORMATION,
                        EWL_ICON_DIALOG_PASSWORD,
                        EWL_ICON_DIALOG_QUESTION,
                        EWL_ICON_DIALOG_WARNING,
                        EWL_ICON_FOLDER_DRAG_ACCEPT,
                        EWL_ICON_FOLDER_OPEN,
                        EWL_ICON_FOLDER_VISITING,
                        EWL_ICON_IMAGE_LOADING,
                        EWL_ICON_IMAGE_MISSING,
                        EWL_ICON_MAIL_ATTACHMENT,
                        EWL_ICON_MAIL_UNREAD,
                        EWL_ICON_MAIL_READ,
                        EWL_ICON_MAIL_REPLIED,
                        EWL_ICON_MAIL_SIGNED,
                        EWL_ICON_MAIL_SIGNED_VERIFIED,
                        EWL_ICON_MEDIA_PLAYLIST_REPEAT,
                        EWL_ICON_MEDIA_PLAYLIST_SHUFFLE,
                        EWL_ICON_NETWORK_ERROR,
                        EWL_ICON_NETWORK_IDLE,
                        EWL_ICON_NETWORK_OFFLINE,
                        EWL_ICON_NETWORK_RECEIVE,
                        EWL_ICON_NETWORK_TRANSMIT,
                        EWL_ICON_NETWORK_TRANSMIT_RECEIVE,
                        EWL_ICON_PRINTER_ERROR,
                        EWL_ICON_PRINTER_PRINTING,
                        EWL_ICON_SOFTWARE_UPDATE_AVAILABLE,
                        EWL_ICON_SOFTWARE_UPDATE_URGENT,
                        EWL_ICON_SYNC_ERROR,
                        EWL_ICON_SYNC_SYNCHRONIZING,
                        EWL_ICON_TASK_DUE,
                        EWL_ICON_TASK_PASSED_DUE,
                        EWL_ICON_USER_AWAY,
                        EWL_ICON_USER_IDLE,
                        EWL_ICON_USER_OFFLINE,
                        EWL_ICON_USER_ONLINE,
                        EWL_ICON_USER_TRASH_FULL,
                        EWL_ICON_WEATHER_CLEAR,
                        EWL_ICON_WEATHER_CLEAR_NIGHT,
                        EWL_ICON_WEATHER_FEW_CLOUDS,
                        EWL_ICON_WEATHER_FEW_CLOUDS_NIGHT,
                        EWL_ICON_WEATHER_FOG,
                        EWL_ICON_WEATHER_OVERCAST,
                        EWL_ICON_WEATHER_SEVERE_ALERT,
                        EWL_ICON_WEATHER_SHOWERS,
                        EWL_ICON_WEATHER_SHOWERS_SCATTERED,
                        EWL_ICON_WEATHER_SNOW,
                        EWL_ICON_WEATHER_STORM,
                        NULL
};

static int create_test(Ewl_Container *box);
static int ewl_icon_theme_idle(void *data);

void
test_info(Ewl_Test *test)
{
        test->name = "Icon Theme";
        test->tip = "Testing the Icon Theme";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_SIMPLE;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *fbox, *scroll;

        scroll = ewl_scrollpane_new();
        ewl_container_child_append(box, scroll);
        ewl_object_minimum_size_set(EWL_OBJECT(scroll), 240, 180);
        ewl_widget_show(scroll);

        fbox = ewl_vfreebox_new();
        ewl_container_child_append(EWL_CONTAINER(scroll), fbox);
        ewl_widget_show(fbox);

        ewl_icon_theme_pos = 0;

        ecore_idle_enterer_add(ewl_icon_theme_idle, fbox);

        return 1;
}

static int
ewl_icon_theme_idle(void *data)
{
        Ewl_Widget *o, *fbox;
        int count = 0, ret = 0;

        fbox = data;
        for ( ; icons[ewl_icon_theme_pos] != NULL;
                        ewl_icon_theme_pos++, count ++)
        {
                const char *path;

                if (count >= EWL_ICON_THEME_PER_LOOP)
                {
                        ret = 1;
                        break;
                }

                path = ewl_icon_theme_icon_path_get(icons[ewl_icon_theme_pos],
                                                        EWL_ICON_SIZE_MEDIUM);
                o = ewl_icon_simple_new();
                if (path) ewl_icon_image_set(EWL_ICON(o), path,
                                        icons[ewl_icon_theme_pos]);
                ewl_icon_label_set(EWL_ICON(o), icons[ewl_icon_theme_pos]);
                ewl_container_child_append(EWL_CONTAINER(fbox), o);
                ewl_widget_show(o);
        }

        return ret;
}



