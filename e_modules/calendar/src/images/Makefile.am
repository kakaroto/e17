MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v

# data files for the module
filesdir = $(datadir)

files_DATA = \
topimage.edj  \
backimage.edj \
today.edj \
weekend.edj \
weekday.edj \
label.edj \
calendar.edj \
arrow.edj

EXTRA_DIST = $(files_DATA) $(files_DATA:%.edj=%.edc) background.png bubbles2.png \
    nm.png ny.png other.png pm.png py.png top.png

topimage.edj: Makefile topimage.edc
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 topimage.edc \
	 topimage.edj

backimage.edj: Makefile backimage.edc
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 backimage.edc \
	 backimage.edj

today.edj: Makefile today.edc
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 today.edc \
	 today.edj

weekend.edj: Makefile weekend.edc
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 weekend.edc \
	 weekend.edj

weekday.edj: Makefile weekday.edc
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 weekday.edc \
	 weekday.edj

label.edj: Makefile label.edc
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 label.edc \
	 label.edj

calendar.edj: Makefile calendar.edc
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 calendar.edc \
	 calendar.edj

arrow.edj: Makefile arrow.edc
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 arrow.edc \
	 arrow.edj

clean-local:
	 rm -rf *.edj *~
