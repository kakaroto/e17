import os
import time

class Etk:
    "Etk-Python base class"        

    def Init(self):
	self.Call("init")
    
    def Connect(self):
	os.system("etk_server /tmp/etk_server_fifo &")
	time.sleep(0.5)
        
    def Call(self, str):
	print("sending " + str)
	out_file = open("/tmp/etk_server_fifo","w")
	out_file.write("etk_" + str + "\0")
	out_file.close()    
	
	in_file = open("/tmp/etk_server_fifo","r")
	text = in_file.read()
	in_file.close()
	
	return text
    
    def Main(self):
	while 1:	    
	    event = self.Call("server_callback")

class Object (Etk):
    "Etk Object class, has basic object operations"
    __object = ""
    
    def Get(self):
	return self.__object
    
    def Set(self, o):
	self.__object = o
	
    def SignalConnect(self, signal, callback):
	self.Call("server_signal_connect \"" + signal + "\" " + self.Get() + " \"" + signal + "_" + object + "\"")
	self.AddCallback(signal, callback)
    
    def AddCallback(self, callback):
	print("Adding callback " + callback)

class Widget (Object):
    "Etk Widget class, has basic widget operations"
    
    def __init__(self):
	Object()
    
    def ShowAll(self):
	self.Call("widget_show_all " + self.Get())

class Button (Widget):
    "Etk Button class, has basic button operations"
    
    def __init__(self):	
	self.Set( self.Call("button_new") )
    
    def LabelSet(self, label):
	self.Call("button_label_set " + self.Get() + " \"" + label + "\"")

class Container (Widget):
    "Etk Container class, has basic container operations"
    
    def __init__(self):
	Widget()
    
    def Add(self, widget):
	self.Call("container_add " + self.Get() + " " + widget.Get())

class Window (Container):
    "Etk Window class, has basic window operations"
    
    def __init__(self):
	Container()
	self.Set( self.Call("window_new") )
    
    def TitleSet(self, title):
	self.Call("window_title_set " + self.Get() + " \"" + title + "\"")

print("depricated for now!!\n")	
#etk = Etk()
#etk.Connect()
#etk.Init()
#
#window = Window()
#print("window id is " + window.Get());
#window.TitleSet("Etk-Python")
#
#button = Button()
#button.LabelSet("Python rules!")
#
#window.Add(button)
#window.ShowAll()
#
#etk.Main()
