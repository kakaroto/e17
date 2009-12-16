<?php
/**
    Static CSS grid generator.

    Currently based on blueprintcss code. But we can easily change it to

    I plan to make it work differently soon, so we don't have to add so much
    boilerplate in the template and its more flexible to changes in the base
    CSS Grid framework.

    @todo Detect and display errors in either the template or the generator or
          simply display an OK status message (for designers without PHP).
    @todo Add compression for the resulting CSS code.

*/

ob_start();
include 'grid.css.php';
$css = ob_get_contents();
file_put_contents('grid.css', $css);
ob_end_clean();

function page_container ()
{
    echo 'width:950px; margin:0 auto; display: block; clear: both';
}

function column_container ( $child = false )
{
    echo 'width:950px; margin:0 auto; display: block';
}

function column ( $span, $last = false )
{
    echo 'float:left; overflow: hidden; ';

    if ( $span < 12 )
        $width = ($span * 70) + ( ($span - 1) * 10 );
    else
        $width = 950;

    echo "width: {$width}px; ";

    if ( $last or ( $span == 12 ) )
        echo 'margin-right: 0; ';
    else
        echo 'margin-right:10px; ';
}

function clearfix ()
{
    echo 'content:"\0020"; display:block; height:0; clear:both;visibility:hidden; overflow:hidden;';
}

?>