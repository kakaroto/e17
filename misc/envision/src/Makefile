# gcc `ecore-config --libs --cflags` `emotion-config --libs --cflags` `edje-config --libs --cflags`  envision.c -o envision
EDJE = `edje-config --libs --cflags`
ECORE = `ecore-config --libs --cflags`
EMOTION = `emotion-config --libs --cflags`

all: envision

envision: envision.c
	gcc $(EDJE) $(ECORE) $(EMOTION) envision.c -o envision

clean: 
	rm envision
