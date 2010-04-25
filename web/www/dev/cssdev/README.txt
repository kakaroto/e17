This is a CSS generatior known as SASS that allows you design websites using
amenities like variables, functions, mixings, mathematical operations
supporting color codes and size formats (px, em, etc).

I use a new syntax in the beta version of SASS known as SCSS, this syntax is
extremely similar to CSS and its actually CSS3 compilant.


The directory structure
=======================

    old_sass    contains the old format SASS files used to create the design you
                currently see in the e.org/dev/

    src/lib     should contain SCSS files that DO NOT ADD ANY CSS code when
                included, only mixins and variables.

    src/website should contain files that use the facilities in src/lib to
                create the necessary files for the BROCHURE website, I suggest
                using screen as the file included in every page and a separate
                file for the specific design of each page.

    /src/...    The other folders in src are for the different targets just
                like website is.

    grid.svg    The grid file for understanding the system, and includes the
                column sizes to setup in gridfox version 1 (also add 16px
                height rows).


Installing SASS
===============
To install sass you must simply install Ruby and ""gem install haml --pre".
To learn the basics visit http://sass-lang.com/
To leanr about SCSS and new features and use the links in posts refering to
the "beta" version at http://nex-3.com/?tag=sass
Specially cool feature is the --watch option.


Typography
==========
For typography I suggest reading http://www.awayback.com/revised-font-stack/
for a font stack that will work well with the typical linux, windows and mac
installations.


Grid
====
My intention with the definitive grid system (not the one currently on the svn
dev folder) was to reduce the number of columns to give more freedom while
still mantaining some kind of logic in the alginment. See the comments on
src/lib/grid.scss for more details. The screen file here will not produce any
meaningful result.

This system allows extremely flexibile design with a lot shared between the
different webs services and doxygen by simply changing a variable or two.

You could trun it into a flexible (% based) or flid (em or ex based) design
with little effort.


