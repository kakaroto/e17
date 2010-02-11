/* 
 * File Name: eina_list.vala
 * Creation Date: 22-03-2009
 * Last Modified:
 *
 * Authored by Frederik 'playya' Sdun <Frederik.Sdun@googlemail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 */
using Eina;

public static int main( string[] args )
{
    Eina.init();
    eina_list_example();
    eina_file_example();
    eina_error_example();
    Eina.shutdown();
    return 0;
}

//=================================================================
public static void eina_list_example()
{
    Eina.List<string> a = null;
    Eina.List<string> b = null;
    a.prepend( "Alfred" );
    a.prepend( "Anton" );
    b.prepend( "Batman" );
    b.prepend( "Bar" );
    b.prepend( "aaaa" );
    debug( "list a:" );
    a.iterator_new(  ).foreach( print_str,null);
    debug( "list b:" );
    b.iterator_new(  ).foreach( print_str, null);
    a.reverse();
    b.merge( a );
    debug( "list merged:" );
    b.iterator_new(  ).foreach( print_str, null);
    uint c = b.count();
    b.sort( c ,sort_str );
    debug( "list sorted:" );
    b.iterator_new().foreach( print_str, null);
}

public static bool print_str( void* container, void* data, void* fdata )
{
    string s = ( string )data;
    debug( "\tdata: %s", s  );
    return true;
}
public static int sort_str( void* a, void* b )
{
    string s1=( string )a;
    string s2=( string )b;
    return GLib.strcmp( s1,s2 );
}
//=================================================================
public static void eina_file_example()
{
    string path = GLib.Environment.get_variable( "PWD" );

    stdout.printf("Dir \tName\n");
    File.dir_list( path, true, dir_print, null );
}
public void dir_print( string? name, string? path, void* data )
{
    string p = "%s/%s".printf( path, name );
    var v = FileUtils.test( p, GLib.FileTest.IS_DIR ).to_string();
    stdout.printf( "%s\t%s\n", v, p );
}
//=================================================================
public static void eina_error_example()
{
    var e1 = Eina.Error( "Foobar" );
    var e2 = Eina.Error( "Moep Meop" );
    var e3 = Eina.Error( "Chooo Choo" );
    e1.set();

}
