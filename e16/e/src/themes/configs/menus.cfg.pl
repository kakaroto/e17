#include <definitions>

__E_CFG_VERSION 0

BEGIN_NEW_FILE_MENU("APPS_SUBMENU", "ROOT", ECONFDIR"/file.menu")
END_MENU
/*
BEGIN_NEW_MENU("APPS_SUBMENU", "ROOT")
ADD_MENU_TITLE("Apps")
ADD_MENU_TEXT_ITEM("Eterm",         __A_EXEC, "Eterm")
ADD_MENU_TEXT_ITEM("Xterm",         __A_EXEC, "xterm")
ADD_MENU_TEXT_ITEM("RXVT" ,         __A_EXEC, "rxvt")
ADD_MENU_TEXT_ITEM("Terminal Gnome",__A_EXEC, "gnome-terminal")
ADD_MENU_TEXT_ITEM("TkRat",         __A_EXEC, "tkrat")
ADD_MENU_TEXT_ITEM("Netscape Mail", __A_EXEC, "netscape -mail -no-about-splash")
ADD_MENU_TEXT_ITEM("Exmh",          __A_EXEC, "exmh")
ADD_MENU_TEXT_ITEM("Balsa",         __A_EXEC, "balsa")
ADD_MENU_TEXT_ITEM("Netscape",      __A_EXEC, "netscape -no-about-splash")
ADD_MENU_TEXT_ITEM("Gimp",          __A_EXEC, "gimp")
ADD_MENU_TEXT_ITEM("XV",            __A_EXEC, "xv")
ADD_MENU_TEXT_ITEM("Electric Eyes", __A_EXEC, "ee")
ADD_MENU_TEXT_ITEM("GQview",        __A_EXEC, "gqview")
ADD_MENU_TEXT_ITEM("XMag",          __A_EXEC, "xmag")
ADD_MENU_TEXT_ITEM("Xeyes",         __A_EXEC, "xeyes")
ADD_MENU_TEXT_ITEM("XawTV",         __A_EXEC, "xawtv")
ADD_MENU_TEXT_ITEM("Ustawienia Imlib",__A_EXEC, "imlib_config")
ADD_MENU_TEXT_ITEM("TkMan",         __A_EXEC, "tkman")
ADD_MENU_TEXT_ITEM("Xman",          __A_EXEC, "xman")
ADD_MENU_TEXT_ITEM("X-Chat",        __A_EXEC, "xchat")
ADD_MENU_TEXT_ITEM("GnomeICU",      __A_EXEC, "gnomeicu -a")
ADD_MENU_TEXT_ITEM("eMusic",        __A_EXEC, "emusic")
ADD_MENU_TEXT_ITEM("GQmpeg",        __A_EXEC, "gqmpeg")
ADD_MENU_TEXT_ITEM("X11Amp",        __A_EXEC, "x11amp")
ADD_MENU_TEXT_ITEM("XMMS",          __A_EXEC, "xmms")
ADD_MENU_TEXT_ITEM("Civilization",  __A_EXEC, "civctp")
ADD_MENU_TEXT_ITEM("Myth 2",        __A_EXEC, "myth2")
ADD_MENU_TEXT_ITEM("Myth 2 Loathing",__A_EXEC, "loathing")
ADD_MENU_TEXT_ITEM("Myth 2 Fear",   __A_EXEC, "fear")
END_MENU
*/

/* recursively scan $HOME/.enlightenment.backgrounds and build menu if images
 * are there */
BEGIN_NEW_DIRSCAN_MENU("BACKGROUNDS_MENU", "ROOT_IMAGE", ECONFDIR"/backgrounds")
ADD_MENU_TITLE("T³a")
END_MENU

BEGIN_NEW_MENU("DESKTOP_SUBMENU", "ROOT")
ADD_MENU_TITLE("Operacje Pulpitu")
ADD_MENU_SUBMENU_TEXT_ITEM("T³a",        "BACKGROUNDS_MENU")
ADD_MENU_TEXT_ITEM("Wyczy¶æ Pulpit ",       __A_CLEANUP,       "")
ADD_MENU_TEXT_ITEM("Nastêpny Pulpit",       __A_DESKTOP_NEXT,  "")
ADD_MENU_TEXT_ITEM("Poprzedni Pulpit",      __A_DESKTOP_PREV,  "")
ADD_MENU_TEXT_ITEM("Nowe Pude³ko na Ikony", __A_CREATE_ICONBOX, "")
ADD_MENU_TEXT_ITEM("FX - Zmarszczki",       __A_FX,            "ripples")
/* this leaks shared memory segemnts like a seive - disable */
/*ADD_MENU_TEXT_ITEM("FX - Raindrops",      __A_FX,            "raindrops")*/
ADD_MENU_TEXT_ITEM("FX - Fale",             __A_FX,            "waves")
END_MENU

BEGIN_NEW_MENU("MAINT_SUBMENU", "ROOT")
ADD_MENU_TITLE("Konserwacja Enlightenment")
ADD_MENU_TEXT_ITEM("Wyczy¶æ cache plików konfiguracyjnych",                      __A_EXEC, ENLIGHTENMENT_BIN"/e_cache_clean.pl config")
ADD_MENU_TEXT_ITEM("Wyczy¶æ cache z t³ami Podgl±du Pulpitu",                     __A_EXEC, ENLIGHTENMENT_BIN"/e_cache_clean.pl pager")
ADD_MENU_TEXT_ITEM("Wyczy¶æ cache z wyborem t³a",                                __A_EXEC, ENLIGHTENMENT_BIN"/e_cache_clean.pl bgsel")
ADD_MENU_TEXT_ITEM("Wyczy¶æ wszystkie cache",                                    __A_EXEC, ENLIGHTENMENT_BIN"/e_cache_clean.pl all")
ADD_MENU_TEXT_ITEM("Podaj zu¿ycie pamiêci przez cache plików konfiguracyjnych",  __A_EXEC, ENLIGHTENMENT_BIN"/e_cache_query.pl config")
ADD_MENU_TEXT_ITEM("Podaj zu¿ycie pamiêci przez cache z t³ami Podgl±du Pulpitu", __A_EXEC, ENLIGHTENMENT_BIN"/e_cache_query.pl pager")
ADD_MENU_TEXT_ITEM("Podaj zu¿ycie pamiêci przez cache z wyborem t³a",            __A_EXEC, ENLIGHTENMENT_BIN"/e_cache_query.pl bgsel")
ADD_MENU_TEXT_ITEM("Podaj zu¿ycie pamiêci przez wszystkie cache",                __A_EXEC, ENLIGHTENMENT_BIN"/e_cache_query.pl all")
ADD_MENU_TEXT_ITEM("Regeneracja Menu",                                           __A_EXEC, ENLIGHTENMENT_BIN"/e_gen_menu.pl")
END_MENU

/* dont need this anymore - will have a scritp soon to do it all 
BEGIN_NEW_GNOME_MENU("GNOME_SUBMENU", "ROOT", "/usr/share/gnome/apps")
ADD_MENU_TITLE("Aplikacje Gnome")
END_MENU

BEGIN_NEW_GNOME_MENU("GNOME_USER_SUBMENU", "ROOT", HOME_DIR"/.gnome/apps")
ADD_MENU_TITLE("Gnome Apps")
END_MENU
*/

BEGIN_NEW_THEMES_MENU("THEMES_SUBMENU", "ROOT")
ADD_MENU_TITLE("Wystroje")
END_MENU

BEGIN_NEW_MENU("CONFIG_SUBMENU", "ROOT")
ADD_MENU_TITLE("Ustawienia")
ADD_MENU_TEXT_ITEM("Ustawienia Aktywacji ...",                    __A_CONFIG, "focus")
ADD_MENU_TEXT_ITEM("Ustawienia Przesuwania i Zmiany Rozmiaru ...",__A_CONFIG, "moveresize")
ADD_MENU_TEXT_ITEM("Ustawienia Podgl±du Pulpitu ...",             __A_CONFIG, "pager")
ADD_MENU_TEXT_ITEM("Usatwienia Rozmieszczania Okien ...",         __A_CONFIG, "placement")
ADD_MENU_TEXT_ITEM("Ustawienia Wielu Pulpitów ...",               __A_CONFIG, "desktops")
ADD_MENU_TEXT_ITEM("Ustawienia Wirtualnego Pulpitu ...",          __A_CONFIG, "area")
/*ADD_MENU_TEXT_ITEM("Icon Settings ...",                         __A_CONFIG, "icons")*/
ADD_MENU_TEXT_ITEM("Ustawienia Automatycznego Podnoszenia ...",   __A_CONFIG, "autoraise")
ADD_MENU_TEXT_ITEM("Ustawienia Podpowiedzi ...",                  __A_CONFIG, "tooltips")
ADD_MENU_TEXT_ITEM("Ustawienia Audio ...",                        __A_CONFIG, "audio")
ADD_MENU_TEXT_ITEM("Ustawienia Grup ...",                         __A_CONFIG, "group_defaults")
ADD_MENU_TEXT_ITEM("Zapamiêtywanie Ustawieñ ...",                 __A_CONFIG, "remember");
ADD_MENU_TEXT_ITEM("Ustawienia Efektów Specjalnych ...",          __A_CONFIG, "fx")
ADD_MENU_TEXT_ITEM("Ustawienia T³a Pulpitu ...",                  __A_CONFIG, "bg")
ADD_MENU_TEXT_ITEM("Ustawienia Wspierania KDE ...",               __A_CONFIG, "kde")
ADD_MENU_TEXT_ITEM("Ustawienia Ró¿ne ...",                        __A_CONFIG, "miscellaneous");
ADD_MENU_TEXT_ITEM("Przestarza³e narzêdzie E-conf ...",           __A_EXEC, "e-conf")
END_MENU

BEGIN_NEW_MENU("ROOT_2", "ROOT")
ADD_MENU_SUBMENU_TEXT_ITEM("Menu U¿ytkownika",        "APPS_SUBMENU")
ADD_MENU_SUBMENU_TEXT_ITEM("Pulpit",                  "DESKTOP_SUBMENU")
ADD_MENU_SUBMENU_TEXT_ITEM("Ustawienia",              "CONFIG_SUBMENU")
ADD_MENU_SUBMENU_TEXT_ITEM("Wystroje",                "THEMES_SUBMENU")
ADD_MENU_SUBMENU_TEXT_ITEM("Konserwacja",             "MAINT_SUBMENU")
ADD_MENU_TEXT_ITEM("Pomoc",                   __A_EXEC, ENLIGHTENMENT_BIN"/dox" $EROOT"/E-docs")
ADD_MENU_TEXT_ITEM("O Enlightenment",         __A_ABOUT, "")
ADD_MENU_TEXT_ITEM("O tym wystroju",          __A_EXEC, ENLIGHTENMENT_BIN"/dox" $ETHEME"/ABOUT")
ADD_MENU_TEXT_ITEM("Restartuj Enlightenment", __A_EXIT, "restart")
ADD_MENU_TEXT_ITEM("Wyloguj siê",             __A_EXIT, "logout")
END_MENU

BEGIN_NEW_MENU("WINOPS_H", "EMPTY")
ADD_MENU_TEXT_ITEM("Prze³±cznik Wysoko¶ci Maksymalnej",              __A_MAX_HEIGHT, "conservative")
ADD_MENU_TEXT_ITEM("Prze³±cznik Wysoko¶ci Maksymalnej dostêpnej",    __A_MAX_HEIGHT, "available")
ADD_MENU_TEXT_ITEM("Prze³±cznik Wysoko¶ci Maksymalnej bezwzglêdnej", __A_MAX_HEIGHT, "")
END_MENU

BEGIN_NEW_MENU("WINOPS_W", "EMPTY")
ADD_MENU_TEXT_ITEM("Prze³±cznik Szeroko¶ci Maksymalnej",              __A_MAX_WIDTH, "conservative")
ADD_MENU_TEXT_ITEM("Prze³±cznik Szeroko¶ci Maksymalnej dostêpnej",    __A_MAX_WIDTH, "available")
ADD_MENU_TEXT_ITEM("Prze³±cznik Szeroko¶ci Maksymalnej bezwzglêdnej", __A_MAX_WIDTH, "")
END_MENU

BEGIN_NEW_MENU("WINOPS_WH", "EMPTY")
ADD_MENU_TEXT_ITEM("Prze³±cznik Rozmiaru Maksymalnego",               __A_MAX_SIZE, "conservative")
ADD_MENU_TEXT_ITEM("Prze³±cznik Rozmiaru Maksymalnego dostêpnego",    __A_MAX_SIZE, "available")
ADD_MENU_TEXT_ITEM("Prze³±cznik Rozmiaru Maksymalnego bezwzglêdnego", __A_MAX_SIZE, "")
END_MENU

BEGIN_NEW_MENU("WINOPS_SIZE", "EMPTY")
ADD_MENU_SUBMENU_TEXT_ITEM("Wysoko¶æ", "WINOPS_H")
ADD_MENU_SUBMENU_TEXT_ITEM("Szeroko¶æ", "WINOPS_W")
ADD_MENU_SUBMENU_TEXT_ITEM("Rozmiar", "WINOPS_WH")
ADD_MENU_TEXT_ITEM("Pe³ny Ekran/Okno",           __A_ZOOM,       "")
END_MENU

BEGIN_NEW_MENU("WINOPS_LAYER", "EMPTY")
ADD_MENU_TEXT_ITEM("Poni¿ej",            __A_SET_LAYER,       "2")
ADD_MENU_TEXT_ITEM("Normalnie",          __A_SET_LAYER,       "4")
ADD_MENU_TEXT_ITEM("Ponad",              __A_SET_LAYER,       "6")
ADD_MENU_TEXT_ITEM("Na szczycie",        __A_SET_LAYER,       "20")
END_MENU

BEGIN_NEW_BORDERS_MENU("WINOPS_BORDERS", "EMPTY")
END_MENU

BEGIN_NEW_MENU("WINOPS_GROUP", "EMPTY")
ADD_MENU_TEXT_ITEM("Konfiguruj grupê(y) tego okna",                 __A_CONFIG, "group")
ADD_MENU_TEXT_ITEM("Stwórz now± grupê",                             __A_START_GROUP,"")
ADD_MENU_TEXT_ITEM("Wybierz grupê, do której dodaæ to okno",        __A_CONFIG, "group_membership")
ADD_MENU_TEXT_ITEM("Dodaj to okno do aktualnej grupy",              __A_ADD_TO_GROUP, "");
ADD_MENU_TEXT_ITEM("Usuñ okno z grupy",                             __A_REMOVE_FROM_GROUP,"")
ADD_MENU_TEXT_ITEM("Rozbij grupê, do której nale¿y to okno",        __A_BREAK_GROUP, "")
ADD_MENU_TEXT_ITEM("Poka¿/Ukryj grupê(y) do której nale¿y to okno", __A_SHOWHIDE_GROUP, "")
END_MENU

BEGIN_NEW_MENU("WINOPS_MENU", "EMPTY")
ADD_MENU_TEXT_ITEM("Zamknij",                              __A_KILL,       "")
ADD_MENU_TEXT_ITEM("Zniszcz",                              __A_KILL_NASTY, "")
ADD_MENU_TEXT_ITEM("Zwiñ do Ikony",                        __A_ICONIFY,    "")
ADD_MENU_TEXT_ITEM("Podnie¶",                              __A_RAISE,      "")
ADD_MENU_TEXT_ITEM("Obni¿",                                __A_LOWER,      "")
ADD_MENU_TEXT_ITEM("Schowaj w Cieñ/Wyjd¿ z Cienia",        __A_SHADE,      "")
ADD_MENU_TEXT_ITEM("Przyklej/Odklej",                      __A_STICK,      "")
ADD_MENU_TEXT_ITEM("Prze³±cznik Pomijania w Li¶cie Okien", __A_SKIPLISTS,  "")
/* ADD_MENU_TEXT_ITEM("Toggle Never Focus",                __A_NEVERFOCUS ,"") */
ADD_MENU_TEXT_ITEM("Zapamiêtaj ustawienia...",             __A_SNAPSHOT,   "dialog")
ADD_MENU_SUBMENU_TEXT_ITEM("Grupy Okna",               "WINOPS_GROUP")
ADD_MENU_SUBMENU_TEXT_ITEM("Rozmiar Okna",             "WINOPS_SIZE")
ADD_MENU_SUBMENU_TEXT_ITEM("Ustaw Warstwê",            "WINOPS_LAYER")
ADD_MENU_SUBMENU_TEXT_ITEM("Ustaw Typ Obramowania",    "WINOPS_BORDERS")
/*ADD_MENU_SUBMENU_TEXT_ITEM("Zapamiêtaj Stan",    "WINOPS_SNAP")*/
END_MENU

/* Also available macros:
ADD_MENU_ITEM(text, icon_iclass, action, parameters)
ADD_MENU_ICON_ITEM(icon_iclass, action, parameters)
ADD_MENU_SUBMENU_ITEM(text, icon_iclass, submenu_name)
ADD_MENU_SUBMENU_ICON_ITEM(icon_iclass, submenu_name)
*/




/* The LONG Way */
/* A menu style - MUST provide DEFAULT */
/*
__MENU_STYLE __BGN
  __NAME "DEFAULT"
  __BORDER "MENU_BORDER"
  __TCLASS "TEXT1"
  __BG_ICLASS "TITLE_BAR_HORIZONTAL" 
  __ITEM_ICLASS "TITLE_BAR_HORIZONTAL"
  __SUBMENU_ICLASS "DEFAULT_MENU_SUB"
  __USE_ITEM_BACKGROUNDS __OFF
  __MAXIMUM_NUMBER_OF_COLUMNS 10
  __MAXIMUM_NUMBER_OF_ROWS    20
__END

__MENU __BGN
  __NAME "SUBMENU1"
  __STYLE "DEFAULT"
  __MENU_ITEM "NULL" "Text Only"
  __MENU_ITEM "DEFAULT_TOOLTIP_CLOUD1" "Text + Icon"
  __MENU_ACTION __A_EXEC "Eterm"
__END
__MENU __BGN
  __NAME "WINDOW_MENU"
  __STYLE "DEFAULT"
  __MENU_ITEM "NULL" "This menu Item does Nothing"
  __MENU_ITEM "ICON_ICLASS" "Eterm"
  __MENU_ACTION __A_EXEC "Eterm"
  __MENU_ITEM "DEFAULT_TOOLTIP_CLOUD1" 
  __MENU_ACTION __A_EXEC "xterm"
  __SUMENU "SUBMENU1" "DEFAULT_TOOLTIP_CLOUD1" "This is a submenu"
__END
*/
