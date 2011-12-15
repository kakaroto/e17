
azy_parser services.azy

gcc -g -o server server.c Demo_HowAreYou.azy_server.c Demo_Common.c Demo_Common_Azy.c `pkg-config --libs --cflags azy` `pkg-config --libs ecore eina`

gcc -g -o client client.c Demo_HowAreYou.azy_client.c Demo_Common.c Demo_Common_Azy.c `pkg-config --libs --cflags azy` `pkg-config --libs ecore eina`

