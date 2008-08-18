#------------------------------------- Enhance example -------------------------------
# This example sucks. Expect better examples soon. 
# You should be able to get the idea anyway, since enhance is a really small library
# See also the Etk examples.
#-------------------------------------------------------------------------------------

require 'dl/ffcall-callback'
require 'ruby-efl/enhance/ruby-enhance'
#require File.dirname(File.expand_path(__FILE__)) + '/../ruby-enhance.rb' # uncomment this if you want to use local version

include Etk
include Enhance

# CClass.debug = true          # uncomment this for lots of debug info
# CClass.debug_level = [:call] # and uncomment this too this for a bit more relevant debug info

EnhanceBase.init
EtkBase.init(0, nil)
   
en = EnhanceBase.new      
en.signal_handling = Enhance::SIGNAL_STORE

en.file_load("window1", File.dirname(File.expand_path(__FILE__)) + "/dialog.glade")

class Event_Handlers
    attr_accessor :enhance
    
    def initialize(enhance)
        @text = nil
        @enhance = enhance
        @textbox = enhance.var_get("textbox")
    end
    def save_clicked(target, data)
        @somevar = @textbox.text
    end
    def load_clicked(target, data)
        @textbox.text = @somevar unless @somevar.nil?
    end
    def on_window1_delete_event(target, data)
        EtkBase.main_quit
		  puts("coin")
    end
end
h = Event_Handlers.new(en)

en.autoconnect(h)

win = en.var_get("window1")
win.show

EtkBase.main
   
#en.free
EnhanceBase.shutdown
EtkBase.shutdown
   
