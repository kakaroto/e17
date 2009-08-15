Basic site map
##############

#. home (brochure)
    #. main page
#. about (brochure)
    #. main page
    #. desktop shell
    #. application development
    #. embedded development
    #. artist resources
#. development (trac)
    #. main page
    #. manuals & API references
    #. user guide
    #. contributing
    #. ..other user generated pages.
#. community (exchange)
    #. ..more user generated pages

Header Links
############
This header will appear in every page of the site including the automatically
generated documentation (which is a pain to customize) thus we must come make
sure -to the greatest extent possible- that we will not be changing it later.

#. about
#. news
#. development
#. community

*Note: there is no direct link to the home page in the header as it has no
unique data featured in it. Perhaps the logo could be used but is not really
important.*


Website sections
################
The titles underlined with "=" represent a section, which is a collection of
one ore more pages. The titles underlined with "-" represents a single page
inside the current section.


home
=====
For those completely new to everything enlightenment.


main page
---------
This page introduces E as a way to "create lean and beautiful software" and then
answers the two obvious questions "Why is it lean?" and "Why is it beautiful?"
with small lists of features for developers and artists and links to the pages
"enlightenment development" and "enlghtenment art" in the about section.

Almost as important is featuring the Enlightenment Desktop Shell in a graphical
way and linking to the "enlightenment desktop shell" page in the about section.

The last priority is to feature the official and planet news along with the
exchange art and application repository.


about
=====
The "about" section consists of 5 brochure-like pages if we count the main page
along the pages for enlightenment artists, the desktop shell, desktop
development and embedded development.

These pages are intended to be static portals to the rest of the project,
basically an adverstiment that provides an overview for each wing of the
project.

Every page in this section should include a div with a list of links to the
other "about" pages with the link of the current page played down.


main page
---------
The main objetive of this page is to present the enlightenment project as a
whole with a brief history of the project and the stated objetives along with an
introduction of the developers and collaborators.

But it is important to feature the three faces of the project -desktop shell,
software developer and art- like the home page does before we delve into long
lists and extense paragraphs.


enlightenment desktop shell
---------------------------
This page is the only brochure page oriented to the user. The main objetive is
to present the poster child of the EFL -the E17 desktop shell- with screenshots,
feature lists, videos... the usual works.

Due to the protagonism in the main page it is possible that many developers
get this as their first page. The wording has to be very careful to present
the desktop shell as a product of the EFL. Like an application that its
currently being featured. A poster-child if you will.

A promiment link to the official packages and installation instructions (if
any) is required.

The secondary objetive is to introduce the user to the community, this includes
the support channels and the eXchange repository along with the user guide in
the wiki.

The last objetive is to present the user metods to contribute to the desktop
shell including references to the enlightenment artists page along with links to
the debugging and translating wiki pages. The developers that might be
interested in collaborating with E17 should be considered and pointed to the
wiki as well.


enlightenment development
-------------------------
This is the main developer oriented brochure page. Its focus is desktop software
development altough embedded development should be featured in a important spot
along with a link to the embedded development page.

The main objetive is to establish the benefits of the EFL as a group in a way
that highlights the design philosphies behind them. An updated diagram of the
EFL that will link to the EFL overview wiki page needs to be included.

The fact that development with the EFL is completely separate from the desktop
shell must be clearly noted to prevent the developer from thinking that the
EFL are dependent of the desktop shell like the kde and gnome development
libraries are tied to their desktop environments.

The secondary objetive is to introduce the developer to the community with links
to the available support channels (currently mailing lists and IRC) and the
development related trac pages:
- main trac page
- EFL overview
- manuals & apis
- contributing


enlightenment embedded development
----------------------------------
This secondary developer brochure-like page complements the regular development
page constructing over it to explain why and how these initially desktop
technologies are benefitial for embedded development. Resource consumption
should be emphasized.

A list of supported architectures and operating systems -the latter with links
to installation instructions- is required.

Due to its dinamic nature a supported device list -with instructions to install
the EFL in them- should be relegated to a wiki page for easy updating. While the
brochure page should mantain a small list of the most popular devices along with
a link to complete device list in the wiki page.


enlightenment artists
---------------------
Art with the EFL means Edje so Edje shall be the focus of this page along with
resources to create E related artwork (svg logo for example).

The first objetive is to introduce the artist to Edje and compare EDC with
similar technologies like html/css and Embryo/Lua with JavaScript. But it should
not dwell into details that could often change like extensive syntax analisis.
It should present the Edje design process write -> compile -> use in a friendly
manner, probably consisting of a simple graphic and a little text. The detailed
explanations of the previously mentioned subjects should be written in the wiki
and linked from here. A link to the web version EDC Reference from here is quite
important as a designer would never wander throught the path required to get
there (unless he is also a dev).

The Edje Editor should be promimently featured BUT it should be noted its status
as a research project until an stable release is available. As alternatives to
he Edje Editor the inkscape and gimp plugins should be introduced with a small
explanation of usage, tips and limitations.

The last objetive is to introduce the means to aquire and collaborate with
artwork specifically for the e17 desktop shell. It should mention the
technologies used like SVG, inkscape and the gimp. And the places to send
artwork like icons for the main theme to the maintainers or adding new themes
to the eXchange repository.


news
====
This section also consists of a single page that contains lists of the official
and planet news with more additional information than the lists displayed in the
main page.


development
===========
This is trac. This section deals with documentation and project developent, both
dinamically changing information. There is no way to know how many pages will
be dinamically created and what their contents will be but I want to outline the
structure of the main page and some additional wiki pages that should exist.

Its important to state that trac is about HOW to use the EFL and point to the
"enlightenment development" about page for the WHY.

I plan to do some small changes to trac (python is not an issue):
- Remove the "screenshots" option.
- The "news" option should explain that is limited to internal development news
and link to the news page on the site for public announcements.
- Correct some alingment issues.
- A more verbose and accesible roadmap page.
- Include SVN instructions in the source page.


main page
---------
the main page should be split in 4 areas, the project developers,
desktop shell users, software developers and artists.

In the project developers area we should link to:
- Release Plan
- Release Schedule
- Binary packaging
- FAQ
- Development Ideas
- E-Coding
- Debugging
- Translating
- Guidelines
- E17 Module developent

In the desktop shell users area we should link to the User Guide and to each
section in the user guide, if there are too many link only to the most important
sections.

In the software developers area we should link to:
- EFL overview (which would link to each library page)
- manuals & apis
- tutorials & examples
- supported OSes and architectures
- supported devices

In the artists area we should link to:
- Book About EDC (outside link)
- EDC Reference (outside link)
- Tutorials
- Resources


mauals & APIs
-------------
Basically links to all the manuals available with a small description and the
API references.


user guide
----------
An introductory text welcoming the user and list of links for each section with
perhaps a small explanation of what the section covers. The current user guide
is unnecesarly big and should be trimmed down, for example, all the chapters
explaining a configuration dialog should be removed or be moved to a tooltip
in the gui itself.


contributing
------------
A mash of the contributing page from the old site, an explanation of the
technologies used FOR developent (svn, trac) and IN development (C, autoblah)
and the guidelines along with links to the E-Coding, Debugging and
Translating pages.


community
=========
This is basically eXchange with a few modifications to make it a community page.

- A small area linking to the User Guide and a few links to the main sections
  of the guide.
- A small area linking to the desktop shell about page and a few links to
  the most relevant anchors inside that page, namely support, download, etc.
- Modify the style to fit the rest of the website including the vertical rythm.
  This will require some modifications to the PHP code in order to force the
  thumbnails to be a multiple of 18 -or image height + margin == multiple of
  18-, etc.
