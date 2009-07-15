#include <eflsys.h>
#include <evas.h>
#include <stdio.h>

typedef efl::EvasList<int> integerList;
typedef efl::EvasListIterator<int> integerListIterator;

void showlist( const integerList& list )
{
    printf( "number of elements = %d\n", list.count() );

    for ( int i = 0; i < list.count(); ++i )
    {
        printf( "element[%d] = %d\n", i, *(list[i]) );
    }
}

int main( int argc, char** argv )
{
    printf( "evas list test\n" );

    integerList list;
    showlist( list );

    int* theAnswer = new int( 42 );

    list.append( new int( 10 ) );
    list.append( new int( 10 ) );
    list.append( new int( 10 ) );
    showlist(list);
    list.append( new int( 10 ) );
    list.append( theAnswer );
    list.prepend( new int( 20 ) );
    list.append( new int( 10 ) );
    list.append( new int( 10 ) );
    showlist( list );
    list.append( new int( 20 ) );
    showlist( list );
    list.append( new int( 30 ) );
    showlist( list );
    list.prepend( new int( -10 ) );
    showlist( list );

    printf( "listfind false = %p\n", list.find( new int( 10 ) ) );
    printf( "listfind true = %p\n", list.find( theAnswer ) );

    for ( integerListIterator it( list ); *it; ++it )
    {
        printf( "data is now %d\n", **it );
    }

    integerListIterator fourtyTwo = list.iter( theAnswer );
    printf( "fourtyTwo = %d\n", **fourtyTwo );
    ++fourtyTwo;
    printf( "value after fourtyTwo = %d\n", **fourtyTwo );

    return 0;
}
