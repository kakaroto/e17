
CPPFLAGS = -O2 -W
OBJS = GeistCompositeObject.o GeistDebug.o GeistDocument.o GeistImage.o GeistObject.o main.o

geist: $(OBJS)
	g++ $(CPPFLAGS) -o geist  $(OBJS)

.cpp.o:
	g++ $(CPPFLAGS) -c $<
.SUFFIXES: .cpp .c .o

clean:
	rm *.o
