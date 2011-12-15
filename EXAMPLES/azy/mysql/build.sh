
azy_parser services.azy

gcc -o server server.c server_code.c Demo_Message.azy_server.c Demo_Common_Azy.c Demo_Common.c `pkg-config --libs --cflags azy esskyuehl ecore eina` -g

gcc -o client client.c Demo_Message.azy_client.c Demo_Common.c Demo_Common_Azy.c `pkg-config --libs --cflags azy ecore eina` -g

