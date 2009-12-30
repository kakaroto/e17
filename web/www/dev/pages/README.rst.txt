#####
PAGES
#####

To add a new language simply create a new folder with your language name in its
own language.

But only valid language folders will be considered, a valid language folder
consists of at least a:

    *   home.html.php
    *   messages.inc.php

Keep the filenames intact, the english page names are used as the ID, you can
localize your page name in messages.inc.php I created a minimal spanish
translation to serve as an example.

When someone visits a page a list of languages available for that page will be
displayed. When someone selects it the language will be added to the vistor's
session and used as default, when the visitor enters a page that hasn't been
translated to his language yet the fallback language will be used and a notice
asking the user if he wishes to collaborate with the translation will be
displayed.

Adding new pages
================
Besides adding the `.html.php` file in your language's folder you have to
update `messages.inc.php` since its used to display the page name, even in the
fallback language.


Writing and translating content
===============================
Every `.html.php` page will have the least amount of php code possible. This
code is (or should be) always language independent. Do not touch anything
inside the <?php and ?> tags, sometimes these tags are used around visible
markup. If you see unlocalized text there feel free to translate it.

Right now there is no automatic html tidy tool that its wildly available that
does not screw php code in some way. So try to keep the text wrapping around
79 characters. The <tags> might go over that limit but the text that precedes
them should not, or some translator/writer might miss it.

Also try to keep a certain level of indentation. The contents of block level
tags like <div>, <table> and <tr> should be indented 4 spaces.


Default variables
=================
These are avialable to all pages except fatal errors. You shouldn't need to
touch them unless you are creating new content.

*   $motto : The (localized) project's motto, ex. "beauty at your fingertips"
*   $name  : The (localized) page name, ex. "acerca de" instead of about
*   $page  : The english page name, used for links and other internal uses.
*   $language : The localized name of the language used in the page.
*   $pages    : A list of pages merged from the fallback and current language.
*   $heder    : Internal variable, do not use.
*   $footer   : Internal variable, do not use.


Default helper functions
=========================
These are listed in lib/html.php you shouldn't need them if you are only
translating content.

Other helper functions relating to developer or news are out of limits for both
translators and content writers.
