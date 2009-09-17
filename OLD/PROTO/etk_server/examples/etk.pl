use IO::Socket;

system("etk_server &");
sleep(1);

my $sock = new IO::Socket::INET (
    PeerAddr => '127.0.0.1',
    PeerPort => '8080',
    Proto => 'tcp',
    Reuse => 1,
);
die "Could not create socket: $!\n" unless $sock;

sub etk_async
{
    print "a $_[0] $_[1]\0\n";
    print $sock "a $_[0] $_[1]\0";
    return $_[0];
}

sub etk_sync
{
    my $line;
    my $listener;
    
    print "s $_[0]\0\n";
    print $sock "s $_[0]\0";
    print "reading...\n";
    $listener = $sock->accept();
    $line = <$listener>;
    return $line;
}

my $app_id = etk_sync("0 etk_server_init");
  
my $win = etk_async("win1", "etk_window_new");
  
my $button1 = etk_async("btn1", "etk_button_new_with_label \"Perl Rules\"");

my $vbox = etk_async("vbx1", "etk_vbox_new 0 0");
etk_async("0", "etk_box_pack_start $vbox $button1 1 1 0");

etk_async("0", "etk_container_add $win $vbox");
  
etk_async("0", "etk_widget_show_all $win");

etk_async("0", "etk_server_signal_connect \"clicked\" $button1 \"button_1_click\"");
etk_async("0", "etk_server_signal_connect \"delete_event\" $win \"win_delete\"");
  
$event = "";
while ( $event ne "win_delete" )
{
    $event = etk_sync("0 etk_server_callback");
    
    if($event =~ /\s/)
    {
	($event, $tmp) = split " ", $event, 2;
	if($tmp =~ /(num|str)="(.*?)(?<!\\)"/)
	{
	    # todo, fill in args for event handler
	}
    }
    
    if($event eq "button_1_click")
    {
	print "button 1 clicked!\n";
    }
}

etk_async("0", "etk_main_quit");
etk_async("0", "etk_server_shutdown");
