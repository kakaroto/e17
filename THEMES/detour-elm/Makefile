# Makefile for detour-elm

CC = edje_cc
CAT = /bin/cat
CP = /bin/cp
HEAD = /usr/bin/head
RM = /bin/rm -f
TAIL = /usr/bin/tail
COMPILE = $(CC) $(EDJE_CC_FLAGS)
SRC = main.edc entry_text.edc
OUT = detour-elm.edj
PATH_INSTALL = $(HOME)/.elementary/themes

EDJE_CC_FLAGS = -id images \
-id icons \
-id custom \
-id clock

.SILENT :

all: version clean detour-elm.edj

clean:
	@echo
	@echo "Checking..."
	if [ -e $(OUT) ]; then $(RM) $(OUT); fi

detour-elm.edj: main.edc
	@echo "Running make..."
	$(COMPILE) $(SRC) -o $(OUT)

install: detour-elm.edj
	@echo "Running make install..."
	if [ -e $(OUT) ]; then $(CP) $(OUT) $(PATH_INSTALL)/; fi
	@echo
	@echo --------------------------------------------------
	@echo $(OUT) was installed in your
	@echo $(PATH_INSTALL)/ directory.
	@echo --------------------------------------------------
	@echo
	@echo

version:
	@echo
	$(CAT) $(SRC) | $(HEAD) -14 | $(TAIL) -1
	$(CAT) $(SRC) | $(HEAD) -15 | $(TAIL) -1

verbose: main.edc
	$(COMPILE) -v $(SRC) -o $(OUT)

uninstall: detour-elm.edj
	@echo
	@echo "Removing" $(OUT)
	#$(RM) $(PATH_INSTALL)/$(OUT)
	@echo
	@echo --------------------------------------------------
	@echo $(OUT) was removed from your
	@echo $(PATH_INSTALL)/ directory.
	@echo --------------------------------------------------
	@echo

