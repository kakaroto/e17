<?php
/**
    This cache system is simple but works well, I have benchmarked it and fared
    very well. The only thing we need is  to remove calls to cache_reset()
    and replace it with a simple daemon that cleanups the cache files
    following more advanced parameters.
*/

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

    $cache_dir = option('cache_dir');
    if ( !file_exists($cache_dir) ) cache_init();

    $time = date("U", filemtime($cache_dir));
    $path = file_path($cache_dir, $filename);

    if ( is_null($value) )
    {
        if ( empty($cache[$filename]) )
        {
            if ( file_exists($path) )
            {
                if ( !$code = file_get_contents($path) )
                    halt("Cannot read cache file '$path'.");
                else
                    return unserialize($code);
            }
            else
                return false;
        }
        else
            return $cache[$filename];
    }
    else
    {
        $code = serialize($value);
        if ( !file_put_contents($path, $code) )
            halt("Cannot write cache file '$path'.");
        else
        {
            # Mantain the original time to avoid resetting the cache directory
            touch($cache_dir, $time);

            $cache[$filename] = $value;
            return $cache[$filename];
        }
    }
}

?>