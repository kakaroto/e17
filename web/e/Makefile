configfiles =    config/bottom \
		 config/config \
		 config/end \
		 config/nav \
		 config/start \
		 config/title \
		 config/top

srcdatafiles = data/*.html
dstdatafiles = $(subst data,pages,$(srcdatafiles))

all: $(dstdatafiles)

$(dstdatafiles):: pages/%.html: data/%.html
	s/make $(subst pages/,,$@)

$(dstdatafiles):: $(configfiles)
	s/make
