/*
 * Copyright (C) 2001 Ben Martin.
 *
 * Original author: Ben Martin
 *
 * See COPYING for full details of copying & use of this software.
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *
 */


#include <gevas_util.h>

char* gevas_trim_prefix( const char* prefix, const char* s )
{
    char* ret = s;
    
    if( !g_strncasecmp(s, prefix, strlen(prefix)))
    {
        ret+=strlen(prefix);
    }
    return ret;
}


char* url_file_name_part_new( const char* _full_url )
{
    char* start = g_strdup(_full_url);
    char* s     = start;
    
    while( *s && *s != '?' && *s != '#' ) s++;
    *s='\0';

    printf("url_file_name_part_new() _full_url:%s\n",_full_url);
    printf("url_file_name_part_new() start    :%s\n",start);
    
    return start;
}



//
// Parse args (in the form of .*\?key=value&key2=value2&...
// into a hash of key -> value
//
GHashTable* url_args_to_hash( const char* _args )
{
    char*   args        = (char*)_args;
    GHashTable* ret = g_hash_table_new( g_str_hash, g_str_equal );
    gchar*  key         = 0; //
    gchar*  value       = 0; // 
    gchar** argsv       = 0; // key1=value1, key2=value2, ... 0
    gchar** key_value_v = 0; // keyX, valueX, 0
    gchar** p           = 0; // temp

    // ensure we start on '?' char + 1
    while( *args && *args != '?' )
        args++;
    while(  *args == '?' )  args++;
    if   ( !*args )         return ret;

    printf("url_args_to_hash() args:%s\n",args);
    
    
    argsv = g_strsplit( args, "&", 0 );

    for(p=argsv; p && *p; p++ )
    {
        printf("got an arg:%s\n",*p);
        key_value_v = g_strsplit( *p, "=", 3 );

        printf("key_value_v[0]:%s\n",key_value_v[0]);
        printf("key_value_v[1]:%s\n",key_value_v[1]);
        if( (*(key_value_v[0])) && (*(key_value_v[1])) )
        {
            key   = key_value_v[0];
            value = key_value_v[1];

            g_hash_table_insert(ret, g_strdup(key), g_strdup(value) );
        }
        g_strfreev(key_value_v);
    }

    g_strfreev(argsv);
    return ret;
}


char* url_args_lookup_str( GHashTable* h, const char* key, const char* def )
{
    char* ret = g_hash_table_lookup(h,key);
    
    if(!ret) ret = g_hash_table_lookup(h,"_");
    if(!ret) ret = def;

    return ret;
}

gint url_args_lookup_int( GHashTable* h, const char* key, gint def )
{
    char* ret = g_hash_table_lookup(h,key);
    
    if(!ret) ret = g_hash_table_lookup(h,"_");
    if(!ret) ret = "";

    return atoi(ret);
}


gint edb_lookup_int( E_DB_File* edb, gint def, const char* fmt, ... )
{
    gint rc = def;
    va_list args;
    gchar* key = NULL;

    va_start (args, fmt);
    key = g_strdup_vprintf( fmt, args );
    va_end (args);

    if(1 != e_db_int_get(edb, key, &rc))
    {
        rc = def;
    }
    g_free(key);
    
    return rc;
}

// Free return val with g_free()
char* edb_lookup_str( E_DB_File* edb, const char* def, const char* fmt, ... )
{
    char* rc = 0;
    va_list args;
    gchar* key = NULL;

    va_start (args, fmt);
    key = g_strdup_vprintf( fmt, args );
    va_end (args);

    if(!(rc = e_db_str_get(edb, key)))
    {
        rc = g_strdup(def);
    }
    else
    {
        char* p = rc;
        rc = g_strdup(p);
        free(p);
    }
    
    g_free(key);
    
    return rc;
}




void edb_to_hash_int( E_DB_File* edb, GHashTable* hash,
                      const char* key, const char* prefix, gint def)
{
    gint n=def;
    n = edb_lookup_int( edb, def, "%s/%s", prefix, key);
    g_hash_table_insert(hash, g_strdup(key), g_strdup_printf("%d",n,0));
}

static void hash_str_str_clean_f(gpointer key, gpointer value, gpointer user_data)
{
    g_free(key);
    g_free(value);
}

void hash_str_str_clean(GHashTable *hash)
{
//    g_hash_table_foreach( hash, hash_str_str_clean, 0 );
    g_hash_table_destroy( hash );
}


