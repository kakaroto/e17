MAKE = make

all: slets slethe

clean:
	${MAKE} -C src/slets clean
	${MAKE} -C src/slethe clean
	rm -rf slets slethe

slets:
	${MAKE} -C src/slets all
	cp src/slets/slets slets

slethe:
	${MAKE} -C src/slethe all
	cp src/slethe/slethe slethe
