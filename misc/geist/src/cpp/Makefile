
CPPFLAGS = -O2 -Wall -g
OBJS = GeistCompositeObject.o GeistDebug.o GeistDocument.o GeistImage.o GeistObject.o main.o

geist: $(OBJS)
	g++ $(CPPFLAGS) -o geist  $(OBJS)

%.o: %.cpp *.h
	g++ $(CPPFLAGS) -c $<

clean:
	rm *.o
