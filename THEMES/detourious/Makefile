.PHONY: default illume dark elm

all: default illume dark elm

# XXX shouldn't be phony, but managing all dependence edc is a pain
default:
	edje_cc -id . -fd . main.edc -o detourious.edj

illume:
	edje_cc -DBUILD_ILLUME=1 -DUSE_BOLD_FONT=1 -id . -fd . main.edc -o detourious-illume.edj

dark:
	edje_cc -DCOLORS=1 -id . -fd . main.edc -o detourious-dark.edj

elm:
	edje_cc -id ./elm -fd ./elm ./elm/default.edc -o detourious-elm.edj

illume-mine:
	edje_cc -DILLUME_KBD_TERMINAL_ONLY=1 -DBUILD_ILLUME=1 -DUSE_BOLD_FONT=0 -id . -fd . main.edc -o dtrs-illume.edj


install: detourious
	rm -f ~/.e/e/themes/detourious.edj
	cp detourious.edj ~/.e/e/themes
