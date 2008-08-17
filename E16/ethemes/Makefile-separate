THEMES += BlueSteel
THEMES += ShinyMetal
THEMES += BrushedMetal-Tigert
THEMES += Ganymede

.PHONY: $(THEMES)

all: check

# Make distcheck
THEMES-CHECK = $(addprefix check-, $(THEMES))
.PHONY: $(THEMES-CHECK)
check: $(THEMES-CHECK)
$(THEMES-CHECK): check-%:
	cd $*; ./autogen.sh && make distcheck

# Make RPM's
THEMES-RPM = $(addprefix rpm-, $(THEMES))
.PHONY: $(THEMES-RPM)
rpms: $(THEMES-RPM)
$(THEMES-RPM): rpm-%:
#	cd $*; ./autogen.sh && make dist && rpmbuild -bb etheme-$*.spec
	cd $*; mzbuild || rpmbuild -ba etheme-$*.spec
