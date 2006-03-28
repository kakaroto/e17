
$fifo = "/tmp/etk_server_fifo";

sub etk
{
    open(ETK, ">$fifo");
    print ETK "$_[0]\0";
    close ETK;

    open(ETK, "$fifo");
    my $line = <ETK>;
    close ETK;

    return $line;
}

system("etk_server $fifo &");
sleep(1);

etk "etk_init";
  
my $win = etk "etk_window_new";
  
my $button1 = etk "etk_button_new_with_label \"Perl Rules\"";

my $vbox = etk "etk_vbox_new 0 0";
etk "etk_box_pack_start $vbox $button1 1 1 0";

etk "etk_container_add $win $vbox";
  
etk "etk_widget_show_all $win";

etk "etk_server_signal_connect \"clicked\" $button1 \"button_1_click\"";
etk "etk_server_signal_connect \"delete_event\" $win \"win_delete\"";
  
$event = "";
while ( $event ne "win_delete" )
{
    $event = etk "etk_server_callback";
    
    if($event eq "button_1_click")
    {
	print "button 1 clicked!\n";
    }
}

etk "etk_main_quit";
etk "etk_server_shutdown";
