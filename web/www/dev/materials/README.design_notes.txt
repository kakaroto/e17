Design Notes
============
**Please read this before taking any design related decisions, thank you.**


Tidy your HTML
--------------
Install tidyhtml from http://tidy.sourceforge.net/ or from your local package
manager and use the following command to tidy-it-up.

    tidy -m --doctype "strict" --drop-font-tags y \
         --drop-proprietary-attributes y --enclose-text y --escape-cdata y \
         --logical-emphasis y --indent "yes" --indent-spaces "4" --wrap "79" \
         --wrap-script-literals y --char-encoding "utf8" \
         --input-encoding "utf8" --output-encoding "utf8" YOURFILE.HTML

If you are editing the content of some page make sure you add the following
option so it doesn't insert the body and header tags.

    --show-body-only y

This serves for two things, first it makes it consistent with the rest of the
site and second it will validate your markup.

Color Scheme
-------------
I tried to stay true to the monochromatic nature of the e17 default theme so
I made a simple, as desaturated as possible, triadic color scheme. My intention
is to use the primary colors (blue-ish) to liven up the site a little while
restricting the secondary colors (red-ish and yellow-ish) to the status in trac
ticket lists and other similar uses.

You can find various palette files in ``materials/resources``. The .gpl file
can be copied to ``~/.gimp-2.4/palettes`` and ``~/.inkscape/palettes``.


Website Structure
-----------------
The site is split in four sections, header, primary-content, secondary-content
and footer. Each section needs two divs to work with full width backgrounds and
the grid at the same time.

The outter div has the name of the section (header, primary-content, etc.) as
its id while the inner div has "container" as the class. The div with the id
will strech to the 100% of the page while the container div width and position
will be dictated by the framework grid.


Grid & BlueprintCSS
-------------------
After trying different CSS frameworks -including using no framework at all- I
came across this one which seems simple enough to use and customize. There is
a cheatsheet in the resources directory. There is documentation in the
``materials/blueprint-css-0.8/`` folder but its formatting is annoying so take
a look to the quick start tutorial in the official wiki [1]_.


-   The grid consist of 12 colums of 63px width each with a gutter of 17px
    totaling 960px wide with 943px visible. These values can be changed easly to
    other fixed values or even liquid/flexible. As long as we don't alter the
    number of colums we won't have to even touch the html code.  Some
    width-concious images like banners and headers might have to be redesigned
    depending on the changes made to the grid.


A gutter is the margin applied to the right border of a colum. This margin is
automatic and can be easly overriden. It only apears when the div is
specifically labeled as a column by add span-12, span-8, etc. to its class
string. This means a span-1 div will be 63px width while a span-2 div will be
143px instead 126px.

It is possible to customize the blueprint css files using the ruby scripts
inside the ``materials/blueprint-css-0.8/``. Use and update the e.org project
entry in ``lib/settings.yml`` please.


-   Create a new blueprint css file collection running the compress.rb script
    inside the ``blueprint-css-0.8/lib`` folder as: `ruby compress.rb -p e.org`
    Then copy the files in ``blueprint-css-0.8/e.org`` inside the website css
    folder.

My only beef with blueprint is that it uses em to set the line height. This
means that changing the font size of a random element will change its line
height and screw up vertical rhythm.

-   To preserve vertical rythm when changing font sizes of any element reset
    line-height using pixels or some bizarre em value, I don't care as long as
    the line height remains a multiple of 18px.

    Another option is to simply use one of the predefined classes directly in
    the markup.


.. [1] http://wiki.github.com/joshuaclayton/blueprint-css/quick-start-tutorial


Vertical Rhythm
---------------
Newspapers and brochures present a great deal of information in a single page
but remain easy for the eye to read and follow beacause they use a concept
-often ignored in web design- known as Vertical Rhythm, visit [3]_ to
learn more about it.


-   The verical sizes of every element (images, line-heights, divs, top/bottom
    margins, etc) should be a multiple of 18px -the default value of
    blueprint-. Worry not about about zooming and custom font sizes since it
    will be dealt properly in competent browsers or in the worst case scenario
    it will look like any other website.


At the beginning it might be hard to keep track of every object and margin that
needs to be a mulple of 18 so I customized the gridfox firefox extension a
little bit to make this easier. Simply install from the ``materials/resources``
folder and go to tools -> gridfox to use it. The default values are set to work
with this design.


.. [3] http://www.alistapart.com/articles/settingtypeontheweb






























