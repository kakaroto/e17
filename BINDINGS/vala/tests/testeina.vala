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

using Eina;

//===========================================================================
public void test_eina_list()
{
    int[] data = { 6, 9, 42, 1, 7, 9, 81, 1664, 1337 };
    int[] res = { 81, 9, 9, 7, 1 };

    Eina.init();
    Eina.List<int> l = null;

    l.append( data[0] );
    assert ( l != null );
    assert ( l.count() == 1 );
    assert ( l[0] == data[0] );
    assert ( data[0] in l );
    assert ( !( -1 in l ) );

    l.prepend( data[1] );
    assert ( l != null );
    assert ( l.count() == 2 );

    l.append( data[2] );
    assert ( l != null );
    assert ( l.count() == 3 );

    l.remove( data[0] );
    assert ( l != null );
    assert ( l.count() == 2 );

    l.remove( data[0] );
    assert ( l != null );
    assert ( l.count() == 2 );

    var l2 = l.data_find_list( data[2] );
    assert ( l2 != null );
/*
    debug( "l2 size = %u", l2.count() );
    assert ( l2.count() == 1 );
*/
    var item = l.data_find( data[2] );
    assert ( item == data[2] );

    l = null;

    
    Eina.shutdown();
    
}
public void test_eina_hash()
{
    Eina.init();

    Test.message("Generating hashmap");
    Eina.Hash<string,string> hash = new Eina.Hash<string,string>((Eina.Hash.KeyLength)string_len,(Eina.Hash.KeyCmp)string_key_cmp, (Eina.Hash.KeyHash)string_key_hash,(Eina.FreeCb)string_free, 5);
    debug("Add member foo as foo %p", "foo");
    assert( hash.population() == 0 );
    hash["foo"] = "foo";
    assert( hash.population() == 1 );
    hash["bar"] = "bar";
    assert( hash.population() == 2 );
    hash["baz"] = "baz";
    assert( hash.population() == 3 );
    hash["foobar"] = "foobar";
    assert( hash.population() == 4 );
    debug("foo: %s", hash["foo"]);

    assert ( hash["foo"] == "foo" );
    assert ( hash["bar"] == "bar" );
    assert ( hash["baz"] == "baz" );
    assert ( hash["foobar"] == "foobar" );

    string foo = hash.modify( "foo", "oof" );
    debug("modify: %s", foo);
    assert ( hash["foo"] == "oof" );
    assert ( foo == "foo" );

    Eina.shutdown();
}
internal uint string_len(string s)
{
    debug("str_len: %i for %p '%s'", (int)s.size(), s, s);
    return (uint)s.size() + 1;
}
internal int string_key_cmp(string key1, int key1_length, string key2, int key2_length)
{
    debug ("comparing: %s %i %s %i", key1, key1_length, key2, key2_length);
    if(key1_length != key2_length)
         return key1_length < key2_length? -1 : 1;
    return GLib.strcmp(key1, key2);
}
internal uint string_key_hash(string data)
{
    uint result = 0x8000;
    char last = 5;
    char * d = (char*)data;
    while(d[0]!=0)
    {
        result = result + d[0];
        result *= last;
        last = d[0];
        d++;
    }
    return result;

}
internal void string_free(string s)
{
    //free isn't mapped
    s = null;
}

void test_eina_error()
{
    var e_foo = Eina.Error("foo");
    var e_bar = Eina.Error("bar");

    e_foo.set();

    assert( Eina.Error.get() == e_foo );

    e_bar.set();
    assert( Eina.Error.get() != e_foo );
    assert( Eina.Error.get() == e_bar );
}
[CCode (has_target = false)]
void spec_one (int request)
{
    for (int i = 0; i < request; i++ )
    {
        //FIXME: do something ;)
    }
}

void test_eina_benchmark()
{
    assert( Eina.Module.init() );
    assert( Eina.List.init() );
    assert( Eina.Array.init() );
    assert( Eina.Benchmark.init() );
    assert( Eina.Mempool.init() );
    var b = new Eina.Benchmark( "test", "test");
    assert( b.register( "bogus", spec_one, 0, 100, 5 ) );
    unowned Eina.Array<string> result = b.run();
    assert( result.count_get() == 2 );
    result = null;
    b = null;
    assert( Eina.Mempool.shutdown() );
    assert( Eina.Benchmark.shutdown() );
    assert( Eina.Array.shutdown() );
    assert( Eina.List.shutdown() );
    assert( Eina.Module.shutdown() );
}

//===========================================================================
void main( string[] args )
{
    Eina.init();
    Test.init( ref args );

    Test.add_func( "/Eina/Hash/String", test_eina_hash );
    Test.add_func( "/Eina/List/Int", test_eina_list );
    Test.add_func( "/Eina/Error", test_eina_error );
    //XXX: segaults in Eina.List.Flush
    Test.add_func( "/Eina/Benchmark", test_eina_benchmark);

    Test.run();
    Eina.shutdown();
}
