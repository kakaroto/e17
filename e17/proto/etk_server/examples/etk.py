#!/usr/bin/python

import os
import time

def etk(str) :
    out_file = open("/tmp/etk_server_fifo","w")
    out_file.write("etk_" + str + "\0")
    out_file.close()    
    
    in_file = open("/tmp/etk_server_fifo","r")
    text = in_file.read()
    in_file.close()
    
    return text

os.system("etk_server /tmp/etk_server_fifo &")
time.sleep(0.5)

etk("init")

win = etk("window_new")

button = etk("button_new_with_label \"Python rules!\"")

etk("container_add " + win + " " + button)
etk("window_title_set " + win + " \"Etk-Python Demo\"")
etk("widget_show_all " + win)

etk("server_signal_connect \"clicked\" " + button + " \"button_click\"")
etk("server_signal_connect \"delete_event\" " + win + " \"window_delete\"")

while 1:
    
    event = etk("server_callback")
    
    if event == "window_delete":
	print "Bye!"
	break    
    
    if event == "button_click":
	print "Button clicked!"
    
etk("main_quit")
etk("server_shutdown")

