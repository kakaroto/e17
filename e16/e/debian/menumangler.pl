$R1 = qq{BEGIN_NEW_FILE_MENU("DEBIAN_MENU","ROOT","/var/lib/enlightenment/debian.menu")};
$R2 = qq{ADD_MENU_SUBMENU_TEXT_ITEM("Debian","DEBIAN_MENU")};
s/(^BEGIN_NEW_FILE_MENU..APPS_SUBMENU.*$)/$1\nEND_MENU\n$R1/;
s/(^ADD_MENU_SUBMENU_TEXT_ITEM..User Menus.*$)/$1\n$R2/;
