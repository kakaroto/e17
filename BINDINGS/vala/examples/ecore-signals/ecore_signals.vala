using Ecore;


bool sig_user(int type, void *event)
{
    stdout.printf("USR%d\n", ((EventSignalUser *) event)->number);
    return false;
}

bool sig_realtime(int type, void *event)
{
    stdout.printf("num: %d\n", ((EventSignalRealtime *) event)->num);
    return true;
}

bool sig_exit(int type, void *event)
{
    var ev = (EventSignalExit *) event;
    stdout.printf("[i: %u, q: %u, t: %u]\n",
		  ev->interrupt, ev->quit, ev->terminate);
    if (ev->terminate != 0)
	Ecore.MainLoop.quit();
    return false;
}

void main()
{
    Ecore.init();
    var x = new EventHandler(EventType.SIGNAL_USER, sig_user);
    var y = new EventHandler(EventType.SIGNAL_REALTIME, sig_realtime);
    var z = new EventHandler(EventType.SIGNAL_EXIT, sig_exit);
    Ecore.MainLoop.begin();
    x = null; y = null; z = null; // required to avoid double free of the event
    Ecore.shutdown();
}
