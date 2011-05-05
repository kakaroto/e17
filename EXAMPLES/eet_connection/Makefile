CFLAGS  +=`pkg-config --cflags eina ecore ecore-con eet`
LDFLAGS +=`pkg-config --libs eina ecore ecore-con eet`

all: tailor stock

clean:
	rm -f tailor stock *.o

tailor: client.o serialisation.o
	gcc ${LDFLAGS} -o tailor client.o serialisation.o

stock: server.o serialisation.o
	gcc ${LDFLAGS} -o stock server.o serialisation.o
