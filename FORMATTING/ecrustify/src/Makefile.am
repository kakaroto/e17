MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS = ecrustify

BUILT_SOURCES = token_names.h

ecrustify_SOURCES = uncrustify.cpp chunk_list.cpp indent.cpp align.cpp combine.cpp \
		tokenize.cpp tokenize_cleanup.cpp space.cpp newlines.cpp output.cpp \
		args.cpp options.cpp keywords.cpp punctuators.cpp parse_frame.cpp \
		logmask.cpp logger.cpp ChunkStack.cpp braces.cpp brace_cleanup.cpp \
		align_stack.cpp defines.cpp width.cpp lang_pawn.cpp md5.cpp \
		backup.cpp parens.cpp universalindentgui.cpp semicolons.cpp \
		sorting.cpp detect.cpp unc_text.cpp unicode.cpp token_names.h

ecrustify_CPPFLAGS = -I$(top_builddir)

token_names.h: token_enum.h ../make_token_names.sh
	@echo "Rebuilding token_names.h"
	@sh ../make_token_names.sh
