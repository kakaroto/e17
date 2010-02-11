/**
 * Copyright (C) 2009 Michael 'Mickey' Lauer <mlauer@vanille-media.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 */

using Ecore;

public class Delegates : GLib.Object
{
    int magic;

    public Delegates()
    {
        magic = 42;
    }

    public bool idle_callback()
    {
        assert( magic == 42 );
        Ecore.MainLoop.quit();
        return false; // don't call me again
    }

    public bool idle_enterer()
    {
        assert( magic == 42 );
        Ecore.MainLoop.quit();
        return false;
    }

    public bool idle_exiter()
    {
        assert( magic == 42 );
        Ecore.MainLoop.quit();
        return false;
    }

    public bool timeout()
    {
        assert( magic == 42 );
        Ecore.MainLoop.quit();
        return false;
    }

}

public void test_mainloop_idler()
{
    init();
    var delegates = new Delegates();
    var i = new Idler( delegates.idle_callback );
    Ecore.MainLoop.begin();
    shutdown();
}

public void test_mainloop_idle_enterer()
{
    init();
    var delegates = new Delegates();
    var i = new IdleEnterer( delegates.idle_enterer );
    Ecore.MainLoop.begin();
    shutdown();
}

public void test_mainloop_idle_exiter()
{
    init();
    var delegates = new Delegates();
    var i = new IdleExiter( delegates.idle_exiter );
    Ecore.MainLoop.begin();
    shutdown();
}

public void test_mainloop_timer_timeout()
{
    init();
    var delegates = new Delegates();
    var i = new Ecore.Timer( 1.0, delegates.timeout );
    Ecore.MainLoop.begin();
    shutdown();
}


//===========================================================================
void main (string[] args)
{
    Test.init(ref args);

    Test.add_func("/MainLoop/Idler", test_mainloop_idler);
    Test.add_func("/MainLoop/IdleEnterer", test_mainloop_idle_enterer);
    //Test.add_func("/MainLoop/IdleExiter", test_mainloop_idle_exiter);
    Test.add_func("/MainLoop/Timer/Timeout", test_mainloop_timer_timeout);

    Test.run ();
}
