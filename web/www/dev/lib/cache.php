<?php
date_default_timezone_set("UTC");

function cache_init ()
{
    $dir = option('cache_dir');
    $ref = option('cache_reference');

    if (!file_exists($ref))
        halt("Required reference file or directory for the cache system '$ref' does not exit");

    if ( !mkdir($dir))
        halt("Cannot create cache dir $dir.");

    $time =  date("U", filemtime($ref));
    touch($dir, $time);
}

# Removes all the cached files if there were changes in the $reference file.
function cache_reset ()
{
    $dir = option('cache_dir');
    $ref = option('cache_reference');

    if (!file_exists($ref))
        halt("Required reference file or directory for the cache system '$ref' does not exit");

    if (!file_exists($dir))
        cache_init();

    $current =  date("U", filemtime($ref));
    $cache   =  date("U", filemtime($dir));

    if ($current != $cache)
    {
        $files = file_list_dir($dir);
        foreach ($files as $file)
            if ( !unlink(file_path($dir, $file)) )
                halt("Cannot remove cache file '$dir/$file'.");

        if( !touch($dir, $current) )
            halt("Cannot touch the directory '$dir'.");
    }
}

# Serializes the data structure $value as file $filename or returns the
# unserialized data in $filename if $value is not set.
function cache ($filename, $value = null)
{
    static $cache = array ();
    $dir = option('cache_dir');

    if ( !file_exists($dir) )
        cache_init();

    $time = date("U", filemtime($dir));

    if ( is_null($value) ) {
        if ( empty($cache[$filename]) ) {
            if ( file_exists(file_path($dir, $filename)) ) {
                if ( !$code = file_get_contents(file_path($dir, $filename)) )
                    halt("Cannot read the cache folder '$dir'.");
                $value = unserialize($code);
            } else
                return false;
        }

    } else
        if ( !file_put_contents(file_path($dir, $filename), serialize($value)) )
            halt("Cannot write to the cache folder '$dir'.");

    # Mantain the original time to avoid resetting the cache directory
    touch($dir, $time);

    $cache[$filename] = $value;
    return $cache[$filename];
}

?>