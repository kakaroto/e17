#Simple makefile

CC=gcc
LD=ld
EDJE_CC=edje_cc
PROGNAME=test_volume_knob
THEME=slider.edj

THEME_SRC=default.edc
THEME_DEPS=*.edc
THEME_INCLUDE=-id images

SOURCES=main.c
INCLUDE=-I. `pkg-config --cflags elementary`
LDFLAGS=`pkg-config --libs elementary`
CFLAGS=-g -Wall

OBJS=$(subst .c,.o,$(SOURCES))

all: $(PROGNAME) $(THEME)

.c.o:
	@echo "CC $<"
	@$(CC) -c $< -o $@ $(INCLUDE) $(CFLAGS)

$(PROGNAME): $(OBJS)
	@echo "LINK $(PROGNAME)"
	@$(CXX) $^ -o $(PROGNAME) $(LDFLAGS)

$(THEME): $(THEME_DEPS)
	@echo "EDJE_CC $(THEME)"
	@$(EDJE_CC)      $(THEME_INCLUDE) $(THEME_SRC) $(THEME)

clean:
	@echo "Clean..."
	@rm -f $(THEME) $(OBJS) $(PROGNAME) src/*~
