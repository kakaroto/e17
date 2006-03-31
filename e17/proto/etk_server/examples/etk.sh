
# Sample script using Etk + Etkd and Bash

echo 'Depricated for now!'
exit;

FIFO=/tmp/etk_server_fifo
i=1
id=0

etk()
{
  echo a $id $1
  echo a $id $1 > $FIFO
  echo done
  sleep 0.001
}

etk_r()
{
  echo s $id $1
  echo s $id $1 > $FIFO
  RESULT=`cat $FIFO`
  echo done
  sleep 0.001
}

etk_l()
{
  i=`expr $i \+ 1`
  RESULT="$id"_$i
  echo a $RESULT $1  
  echo a $RESULT $1 > $FIFO
  echo done
  sleep 0.001
}

ETK_TRUE=1
ETK_FALSE=0

# run the Etk server
#etk_server $FIFO &
#sleep 0.5

# initialize Etk
etk_r "etk_server_init"
id=$RESULT

# create a new window and save the result
etk_l "etk_window_new"
WIN=$RESULT;

# create a new button + label and save the result
etk_l "etk_button_new_with_label \"Bash Owns\""
BUTTON=$RESULT

# create another button + label and save the result
etk_l "etk_button_new_with_label \"Bash Rules\""
BUTTON2=$RESULT

# create a vertical box and save the result
etk_l "etk_vbox_new $ETK_FALSE 0"
VBOX=$RESULT

# pack the buttons into the vbox
etk "etk_box_pack_start $VBOX $BUTTON $ETK_TRUE $ETK_TRUE 0"
etk "etk_box_pack_start $VBOX $BUTTON2 $ETK_TRUE $ETK_TRUE 0"

# create a new table and save the result
etk_l "etk_table_new 1 3 0"
TABLE=$RESULT

# create a new entry and save the result
etk_l "etk_entry_new"
ENTRY=$RESULT

# create a new progressbar, save the result, set its text
etk_l "etk_progress_bar_new"
PBAR=$RESULT
etk "etk_progress_bar_text_set $PBAR \"Downloading...\""

# create another button + label and save the result
etk_l "etk_button_new_with_label \"Get Text\""
BUTTON3=$RESULT

# attach the progressbar and the entry to the table
etk "etk_table_attach_defaults $TABLE $PBAR 0 0 0 0"
etk "etk_table_attach_defaults $TABLE $ENTRY 0 0 1 1"
etk "etk_table_attach_defaults $TABLE $BUTTON3 0 0 2 2"
etk "etk_box_pack_start $VBOX $TABLE $ETK_TRUE $ETK_TRUE 0"

# add the vbox to the window, set its title, and show it
etk "etk_container_add $WIN $VBOX"
etk "etk_window_title_set $WIN \"Etkd-Bash\""
etk "etk_widget_show_all $WIN"

etk "etk_server_signal_connect \"clicked\" $BUTTON \"button_1_click\""
etk "etk_server_signal_connect \"clicked\" $BUTTON2 \"button_2_click\""
etk "etk_server_signal_connect \"clicked\" $BUTTON3 \"button_3_click\""
etk "etk_server_signal_connect \"delete_event\" $WIN \"win_delete\""

# start main loop
while [ "$CB" != "win_delete" ]; do

  etk_r "etk_server_callback"
  CB=$RESULT
  case $CB in
  
    button_1_click)
      echo "Button 1 clicked"
    ;;
      
    button_2_click)
      echo "Button 2 clicked"
    ;;
    
    button_3_click)
      etk "etk_entry_text_get $ENTRY"
      TEXT=$RESULT
      etk "etk_server_var_get $TEXT"
      TEXT=$RESULT
      echo "Text = '$RESULT'"
    ;;
        
  esac;

done

etk "etk_main_quit"
etk "etk_shutdown"
etk "etk_server_shutdown"
