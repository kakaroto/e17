EDC = artifakt.edc
EET = $(EDC:%.edc=%.eet)
SRC = $(EDC) images.edc ./images/*.png \
	fonts.edc ./fonts/*.ttf \
	parts/*.edc \
	programs/*.edc

all: $(EET)

$(EET): $(SRC)
	edje_cc -v -id ./images -fd ./fonts $(EDC) $(EET)

clean:
	rm -f ${EET}
