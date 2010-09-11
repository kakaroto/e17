ECORE_EVAS_FLAGS = `pkg-config --libs --cflags ecore-evas ecore evas`

TESTS = eina-tiler-visualizer

.PHONY: all clean

all: $(TESTS)
clean:
	rm -f $(TESTS)

eina-tiler-visualizer: eina-tiler-visualizer.c
	$(CC) $(ECORE_EVAS_FLAGS) $< -o $@
