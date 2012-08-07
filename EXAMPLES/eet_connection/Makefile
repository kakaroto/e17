CFLAGS  +=`pkg-config --cflags eina ecore ecore-con eet`
LDFLAGS +=`pkg-config --libs eina ecore ecore-con eet`

all: tailor stock

clean:
	rm -f tailor stock *.o

tailor: client.o serialisation.o
	gcc client.o serialisation.o ${LDFLAGS} -o tailor

stock: server.o serialisation.o
	gcc server.o serialisation.o ${LDFLAGS} -o stock
