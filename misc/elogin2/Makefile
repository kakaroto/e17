ELOGIN_PATH="/usr/local/e17"
FLAGS=`ecore-config --cflags` `evas-config --cflags` `ebits-config --cflags`
LIBS=`ecore-config --libs` `evas-config --libs` `ebits-config --libs` -lpam
CC=gcc -g

OBJS=object.o box.o label.o entry.o button.o auth.o user.o 

all: clean $(OBJS)
	$(CC) $(FLAGS) $(OBJS) test.c -o test $(LIBS)
	$(CC) $(FLAGS) $(OBJS) main.c -o elogin $(LIBS)
	$(CC) $(FLAGS) $(OBJS) spawner.c -o elogind $(LIBS)	

clean:
	rm -f *~ *.o elogin elogind test

object.o:
	$(CC) $(FLAGS) -c object.c
box.o:
	$(CC) $(FLAGS) -c box.c
label.o: 
	$(CC) $(FLAGS) -c label.c
entry.o: 
	$(CC) $(FLAGS) -c entry.c
button.o:
	$(CC) $(FLAGS) -c button.c
auth.o:
	$(CC) $(FLAGS) -c auth.c
user.o:
	$(CC) $(FLAGS) -c user.c

	
install:
	cp elogin elogind $(ELOGIN_PATH)/bin
	cp config/elogin /etc/pam.d
	rm -fr $(ELOGIN_PATH)/share/elogin
	mkdir $(ELOGIN_PATH)/share/elogin
	cp -r data $(ELOGIN_PATH)/share/elogin
#	/usr/sbin/userdel elogin 
#	/usr/sbin/useradd -s /bin/false elogin
