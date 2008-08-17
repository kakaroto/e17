#!/usr/local/bin/ruby

puts("Depricated for now!\n")
exit

def etk(str)

  fifo = File.open("/tmp/etk_server_fifo", "w")
  fifo.puts("etk_#{str}")
  fifo.close()
  
  fifo = File.open("/tmp/etk_server_fifo", "r")
  ret = fifo.gets()
  fifo.close()
  
  return ret
end

system("etk_server /tmp/etk_server_fifo &")
sleep(0.5)

etk("init")

win = etk("window_new")
etk("window_title_set #{win} \"Etk-Ruby Demo\"");

vbox = etk("vbox_new 0 0")
etk("container_add #{win} #{vbox}")

button1 = etk("button_new_with_label \"Ruby rules!\"")
etk("box_pack_start #{vbox} #{button1} 1 1 0")

button2 = etk("button_new_with_label \"So does Etk!\"")
etk("box_pack_start #{vbox} #{button2} 1 1 0")

entry = etk("entry_new")
etk("box_pack_start #{vbox} #{entry} 1 1 0")

button3 = etk("button_new_with_label \"Get text\"")
etk("box_pack_start #{vbox} #{button3} 1 1 0")

etk("widget_show_all #{win}")

etk("server_signal_connect \"clicked\" #{button1} \"button_1_click\"")
etk("server_signal_connect \"clicked\" #{button2} \"button_2_click\"")
etk("server_signal_connect \"clicked\" #{button3} \"button_3_click\"")
etk("server_signal_connect \"delete_event\" #{win} \"win_delete\"")

event = ""
while event != "win_delete"

  event = etk("server_callback");
  case event
  
    when "button_1_click"
      puts "button 1 clicked"
  
    when "button_2_click"
      puts "button 2 clicked"
     
    when "button_3_click"
      text = etk("entry_text_get #{entry}")
      text = etk("server_var_get #{text}")
      puts "Text=#{text}"
     
   end
  
end

etk("main_quit");
etk("shutdown");
etk("server_shutdown");
