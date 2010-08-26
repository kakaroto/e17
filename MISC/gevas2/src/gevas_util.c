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
#include <string.h>


/**
 *  Trim prefix from s if prefix starts the string s.
 * This is a case insensitive compare. If prefix doesn't start 's' then 's' is
 * simply returned.
 *
 * The return value is a pointer into 's' not a new string.
 *
 * @param prefix the prefix to check for at the start of s.
 * @param s      the string to return an index into.
 */
const char* gevas_trim_prefix( const char* prefix, const char* s )
{
    const char* ret = s;

    g_return_if_fail (s      != NULL);
    g_return_if_fail (prefix != NULL);

    if( !g_strncasecmp(s, prefix, strlen(prefix)))
    {
        ret+=strlen(prefix);
    }
    return ret;
}


/**
 *  Return a newly allocated string that contains only the file name
 * portion of a given # or ? delimited url.
 *
 * @param _full_url The URL to return the part of leading to the # or ?.
 */
char* url_file_name_part_new( const char* _full_url )
{
    char* start = g_strdup(_full_url);
    char* s     = start;

    g_return_if_fail (_full_url != NULL);
    
    while( *s && *s != '?' && *s != '#' ) s++;
    *s='\0';

/*     printf("url_file_name_part_new() _full_url:%s\n",_full_url); */
/*     printf("url_file_name_part_new() start    :%s\n",start); */
    
    return start;
}



/**
 * Parse args (in the form of .*\?key=value&key2=value2&...
 * into a hash of key -> value
 *
 * Note that the caller must free the return value.
 *
 * @param _args the string containing a full URL to parse.
 *
 */
GHashTable* url_args_to_hash( const char* _args )
{
    char*   args        = (char*)_args;
    GHashTable* ret = g_hash_table_new( g_str_hash, g_str_equal );
    gchar*  key         = 0;    /*  */
    gchar*  value       = 0;    /*  */
    gchar** argsv       = 0;    /* key1=value1, key2=value2, ... 0 */
    gchar** key_value_v = 0;    /* keyX, valueX, 0 */
    gchar** p           = 0;    /* temp */

    
    g_return_if_fail (_args != NULL);

    /* ensure we start on '?' char + 1 */
    while( *args && *args != '?' )
        args++;
    while(  *args == '?' )  args++;
    if   ( !*args )         return ret;

/*     printf("url_args_to_hash() args:%s\n",args); */
    
    
    argsv = g_strsplit( args, "&", 0 );

    for(p=argsv; p && *p; p++ )
    {
/*         printf("got an arg:%s\n",*p); */
        key_value_v = g_strsplit( *p, "=", 3 );

/*         printf("key_value_v[0]:%s\n",key_value_v[0]); */
/*         printf("key_value_v[1]:%s\n",key_value_v[1]); */
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


/**
 * Lookup a string value in the hash created with the function
 * url_args_to_hash(). Use the 'key' to find the string, if the
 * string does not exist then return 'def'.
 *
 * Note that the return value is either def or a pointer into the hash 'h'
 * The user should not free the return value directly, only the hash 'h'.
 *
 * @param h   The hash to lookup in
 * @param key The key to find the string value of
 * @param def The default value if the 'key' is not found.
 *
 */
const char* url_args_lookup_str( GHashTable* h, const char* key, const char* def )
{
    char* ret = g_hash_table_lookup(h,key);

    g_return_if_fail (h   != NULL);
    g_return_if_fail (key != NULL);
    g_return_if_fail (def != NULL);

    if(!ret) ret = g_hash_table_lookup(h,"_");
    if(!ret) ret = (char*)def;

    return ret;
}


/**
 * Lookup an int value in the hash created with the function
 * url_args_to_hash(). Use the 'key' to find the int, if the
 * int does not exist then return 'def'.
 *
 * @param h   The hash to lookup in
 * @param key The key to find the int value of
 * @param def The default value if the 'key' is not found.
 *
 */
gint url_args_lookup_int( GHashTable* h, const char* key, gint def )
{
    char* ret = g_hash_table_lookup(h,key);

    g_return_if_fail (h   != NULL);
    g_return_if_fail (key != NULL);

    if(!ret) ret = g_hash_table_lookup(h,"_");
    if(!ret) ret = "";

    return atoi(ret);
}


#ifdef BUILD_EDB_CODE
/**
 *  Lookup an int in a edb database. The key is given by the format string
 * 'fmt' which should be in printf() style with the args following fmt.
 * If the key does not have a value on disk then return def.
 *
 * @param edb  the database to read from
 * @param def  the default value to return if the edb file is not valid or
 *             if the key does not have a value in the edb file.
 * @param fmt  The printf() style format string.
 *
 */
gint edb_lookup_int( E_DB_File* edb, gint def, const char* fmt, ... )
{
    gint rc = def;
    va_list args;
    gchar* key = NULL;

    g_return_if_fail (edb   != NULL);
    g_return_if_fail (fmt   != NULL);

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

/**
 *  Lookup a string in a edb database. The key is given by the format string
 * 'fmt' which should be in printf() style with the args following fmt.
 * If the key does not have a value on disk then return a deep copy of def.
 *
 * Free the return val with g_free()
 *
 * @param edb the edb to read from
 * @param def  the default value to return if the edb file is not valid or
 *             if the key does not have a value in the edb file.
 * @param fmt  The printf() style format string.
 */
char* edb_lookup_str( E_DB_File* edb, const char* def, const char* fmt, ... )
{
    char* rc = 0;
    va_list args;
    gchar* key = NULL;

    g_return_if_fail (edb   != NULL);
    g_return_if_fail (fmt   != NULL);
    g_return_if_fail (def   != NULL);

    
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



/**
 * Read an integer from an edb file and put it into a hash table.
 * The real key that is used in the edb lookup is 'prefix'/'key'.
 * If the 'key' does not have a value on disk then insert 'def'.
 *
 * @param edb  the database to read from
 * @param hash the hash to insert into.
 * @param key  key for info in edb file.
 * @param prefix the prefix to look for the info in the edb.
 * @param def  the default value to return if the edb file is not valid or
 *             if the key does not have a value in the edb file.
 */
void edb_to_hash_int( E_DB_File* edb, GHashTable* hash,
                      const char* key, const char* prefix, gint def)
{
    gint n=def;

    g_return_if_fail (edb   != NULL);
    g_return_if_fail (hash  != NULL);
    g_return_if_fail (key   != NULL);
    g_return_if_fail (prefix!= NULL);

    n = edb_lookup_int( edb, def, "%s/%s", prefix, key);
    g_hash_table_insert(hash, g_strdup(key), g_strdup_printf("%d",n,0));
}
#endif

/**
 * Clean up a key/value from a hash table that is made using
 * url_args_to_hash().
 */
static void hash_str_str_clean_f(gpointer key, gpointer value, gpointer user_data)
{
    g_free(key);
    g_free(value);
}


/**
 * destroy a hash that was made with url_args_to_hash().
 */
void hash_str_str_clean(GHashTable *hash)
{
    g_return_if_fail (hash  != NULL);
    
/*     g_hash_table_foreach( hash, hash_str_str_clean_f, 0 ); */
    g_hash_table_destroy( hash );
}



/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/


