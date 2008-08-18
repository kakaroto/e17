INCLUDES = @ELITAIRE_CFLAGS@

AM_CFLAGS    = -Wall -O2 -g
AM_CXXFLAGS  = -Wall -O2 -g

bin_PROGRAMS = elitaire

elitaire_SOURCES = \
  main.cpp\
  \
  Eli_App.h\
  eli_app.cpp\
  \
  intl.h\
  points.h\
  points.c\
  \
  edje_frontend.h\
  edje_frontend_gui.cpp\
  edje_frontend_game.cpp\
  edje_frontend_menu.cpp\
  \
  ewl_dialogs.h\
  ewl_dialogs.cpp\
  ewl_dialog_config.cpp\
  ewl_dialog_about.cpp\
  ewl_dialog_hiscore.cpp\
  \
  Rules.h\
  rules.cpp\
  rulesBakersDozen.h\
  rulesFreeCell.h\
  rulesGolf.h\
  rulesKlondike.h\
  rulesSpider.h\
  \
  Elitaire.h\
  elitaire_job.h\
  elitaire_private.h\
  elitaire_card.h\
  elitaire_card_private.h\
  elitaire_card.cpp\
  elitaire_card_jobs.cpp\
  elitaire.cpp\
  elitaire_job.c\
  elitaire_private.cpp\
  \
  eli_hiscore.h\
  eli_hiscore.c\
  \
  eli_statistics.h\
  eli_statistics.c\
  \
  eli_help_reader.h\
  eli_help_reader.c\
  ewl_help_reader.h\
  ewl_help_reader.c

elitaire_LDFLAGS = @ELITAIRE_LIBS@

top_srcdir = @top_srcdir@
