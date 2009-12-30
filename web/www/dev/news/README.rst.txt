####
News
####

This is a repository of news files for each language, when a news article or
list is requested, the available news for the user's language will be merged
with the list of the fallback language.

The markup for the news page is in `pages/language/news.html.php` while the
code that parses, caches them and display them is in `lib/news.php`.

Writing news
============
Quite simple actually, the format is the same as before, the first line is used
for the news title, the second line for the author's name and the third for
the authors contact data.

The rest of the article uses html tags to format the content. I could easly
implmement a Markdown or reStructuredText (my favorite) alternative if
requested.

The filename follows the format `YYYYMMDD-HHMMSS` it can have any extension but
all will be trated the same way.

TODO
====
When a news article is requested, the file is parsed and the resulting html is
tidyed up, hopefully fixing any broken tags and then cached for future
displays.
