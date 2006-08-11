#------------------------------------- Enhance example -------------------------------
# This example sucks. Expect better examples soon. 
# You should be able to get the idea anyway, since enhance is a really small library
# See also the Etk examples.
#-------------------------------------------------------------------------------------

require 'dl/ffcall-callback'
require 'ruby-efl/enhance/ruby-enhance'

include Etk
include Enhance

# CClass.debug = true # uncomment this for lots of debug info

EnhanceBase.init
EtkBase.init(0, nil)
   
en = EnhanceBase.new      
en.signal_handling = Enhance::SIGNAL_STORE

en.file_load("window1", File.dirname(File.expand_path(__FILE__)) + "/dialog.glade")

window1 = en.var_get("window1")

class Handlers
    class << self
        def cancel_clicked(target, data)
            puts "Oooh, cancel clicked !"
        end
    end
end

en.autoconnect(Handlers)

win = Dialog.wrap(window1)
win << Etk::DeleteEventSignal.new { EtkBase.main_quit }
win.show

EtkBase.main
EtkBase.shutdown
   
en.free
EnhanceBase.shutdown
   
