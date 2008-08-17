include sources.inc
include platform.inc

RM = rm -rf

##############################################################################
#
# Build rules
#
##############################################################################

# Their Objects
BIN_EXORCIST_OBJECTS  = $(GENERIC_BIN_EXORCIST_OBJECTS)
OBJECTS               = $(BIN_EXORCIST_OBJECTS)

# The VPATH mechanism could use a "per target" build directory
# To keep it simple at the moment, the directory is fixed to "build"
BUILD_DIR = build
VPATH     = $(SRC_DIR):$(BUILD_DIR)

# The variables used for the dist rule
distdir=$(PACKAGE)-$(VERSION)
distfile=$(distdir).tar

#-----------------------------------------------------------------------------
# The default rule
#-----------------------------------------------------------------------------

.SUFFIXES: .$(OBJECT_EXTENSION) .c

all: $(PROG_EXORCIST)
	@echo
	@echo "---------------------------------------------------------------"
	@echo " Exorcist has been successfully built."
	@echo
	@echo " * Binaries are currently located in the '$(BUILD_DIR)' directory"
	@echo " * To install them on your system, you can run '# make install'"
	@echo "   as root."
	@echo "---------------------------------------------------------------"
	@echo
	@echo " * Installation......: make install"
	@echo " * Install path......: $(prefix)"
	@echo

$(OBJECTS): platform.inc

$(BUILD_DIR):
	@echo "  D: $(BUILD_DIR)"
	@$(INSTALL) -d $(BUILD_DIR)

#-----------------------------------------------------------------------------
# Generic C rule
#-----------------------------------------------------------------------------

.c.$(OBJECT_EXTENSION):
	@echo "  C: $(@D)/$(<F)"
	@$(INSTALL) -d $(BUILD_DIR)/$(@D)
	@$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $(BUILD_DIR)/$@

#-----------------------------------------------------------------------------
# Programs
#-----------------------------------------------------------------------------
$(PROG_EXORCIST): .depend $(BIN_EXORCIST_OBJECTS)
	@echo "  P: $(@F)"
	@cd $(BUILD_DIR) && $(CC) -o $(PROG_EXORCIST) $(BIN_EXORCIST_OBJECTS) $(LDFLAGS) && chmod u+x $(PROG_EXORCIST)

#-----------------------------------------------------------------------------
# Installation
#-----------------------------------------------------------------------------

install: $(BUILD_DIR)/$(PROG_EXORCIST)
	@echo "  D: $(bindir)"
	@$(INSTALL) -d $(DEST_DIR)$(bindir)
	@echo "  I: $(bindir)/$(BIN_OBJECT)"
	@$(INSTALL) -m 755 $(BUILD_DIR)/$(PROG_EXORCIST) $(DEST_DIR)$(bindir)/$(PROG_EXORCIST)

#-----------------------------------------------------------------------------
# Platorm specific file -- dumb rules for people executing make before
# ./configure or even ./bootstrap.sh
#-----------------------------------------------------------------------------

platform.inc: configure
	./configure

#-----------------------------------------------------------------------------
# .depend targets
#-----------------------------------------------------------------------------

.depend: $(BUILD_DIR)
	@echo "  H: headers dependances"
	@$(RM) $(BUILD_DIR)/.depend
	@touch $(BUILD_DIR)/.depend
	@cd $(SRC_DIR) && \
	find . -name '*.c' | while read i; do \
	  filename="`echo $$i | sed 's:^\./::'`"; \
	  dir="`echo $$filename | sed 's:\(.*\)/.*:\1:'`"; \
	  (echo -n "$$dir/" && $(CC) -MM -g0 $(CPPFLAGS) $$filename) 1>> ../build/$(BUILD_DIR)/.depend; \
	done

#-----------------------------------------------------------------------------
# .PHONY targets
#-----------------------------------------------------------------------------

.PHONY: distclean clean dist info \
	list-objects list-targets list-install-path list-flags

clean:
	@echo "  Cl: Build directory"
	@$(RM) $(BUILD_DIR)

distclean: clean
	@echo "  Cl: Generated build files"
	@$(RM) platform.inc
	@$(RM) $(distfile).gz
	@$(RM) $(distfile).bz2

distdir:
	@cd ../ && \
	test ! -d $(distdir) \
	|| { find $(distdir) -type d ! -perm -200 -exec chmod u+w {} ';' \
	&& $RM $(distdir); };
	@mkdir $(distdir)
	@cp -pR \
	build/config_check.inc \
	build/config_msg.inc \
	build/config_output.inc \
	build/configure \
	build/Makefile \
	build/sources.inc \
	src/bin/*.c \
	src/bin/*.h \
	$(SRC_DIR)/ $(distdir)

dist-tar: distdir
	@tar cf $(distfile) $(distdir)

dist-gzip: dist-tar
	@echo "  T: $(distfile).gz"
	@gzip -9 -c $(distfile) > $(distfile).gz

dist-bzip2: dist-tar
	@echo "  T: $(distfile).bz2"
	@bzip2 -9 -c $(distfile) > $(distfile).bz2

dist: dist-gzip dist-bzip2
	@$(RM) $(distdir) $(distfile)

list-objects:
	@echo
	@echo "---------------------------------------------------------------"
	@echo "Object files used for this build"
	@echo "---------------------------------------------------------------"
	@echo
	@echo $(LIB_OBJECTS)
	@echo
	@echo $(BIN_EET_OBJECTS)
	@echo
	@echo $(BIN_EET_BENCH_OBJECTS)
	@echo

list-targets:
	@echo
	@echo "---------------------------------------------------------------"
	@echo "Target Libraries"
	@echo "---------------------------------------------------------------"
	@echo
	@echo Shared library: $(SHARED_LIB)
	@echo Static library: $(STATIC_LIB)
	@echo

list-install-path:
	@echo
	@echo "---------------------------------------------------------------"
	@echo "Install Paths"
	@echo "---------------------------------------------------------------"
	@echo
	@echo Binary  Directory: $(bindir)
	@echo Include Directory: $(includedir)
	@echo Library Directory: $(libdir)
	@echo

list-flags:
	@echo
	@echo "---------------------------------------------------------------"
	@echo "Using FLAGS"
	@echo "---------------------------------------------------------------"
	@echo
	@echo CFLAGS=$(CFLAGS)
	@echo

info: list-objects list-flags list-targets list-install-path

ifneq ($(wildcard $(BUILD_DIR)/.depend),)
include $(BUILD_DIR)/.depend
endif
