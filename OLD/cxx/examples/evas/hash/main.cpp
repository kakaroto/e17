#include <eflpp_sys.h>
#include <eflpp_evas.h>
#include <stdio.h>

#include <Evas.h>

typedef efl::EvasHash<int> integerHash;
//typedef E::Vas::ListIterator<int> integerListIterator;

static Evas_Bool printhash(Evas_Hash *hash, const char *key, void *data, void *fdata)
{
    printf( "hash[%s] = %d\n", key, *(int*) data );
    return 1;
}

void showhash( const integerHash& hash )
{
    printf( "\nnumber of elements = %d\n", hash.size() );
    hash.foreach( &printhash );
    printf( "\n\n" );
}

int main( int argc, char** argv )
{
    printf( "evas hash test\n" );

    integerHash hash;
    showhash( hash );

    printf( "Queryiing a not existing value returns '%p'", hash["not existing"]);

    int* theValue = new int( 42 );
    hash.add( "The Answer", theValue );

    showhash( hash );

    int* nullValue = new int( 0 );
    hash.add( "The Null", nullValue );

    showhash( hash );

    printf( "The Answer is '%d'\n", *hash["The Answer"] );
    printf( "The Null is '%d'\n", *hash["The Null"] );

    return 0;
}
