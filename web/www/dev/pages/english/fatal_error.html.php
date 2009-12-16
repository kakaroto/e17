<h2>A non-recoverable error has been triggered</h2>
<div id="message">
    <p class="error">
        (<?php      echo    $errno ?>)
        <code><?php echo    $errstr ?></code>

        <strong> in </strong>

        <tt><?php   echo    $errfile ?></tt>

        <strong> at </strong>

        <?php       echo    $errline ?>
    </p>
</div>

<div id="backtrace">
    <h4>Backtrace</h2>
    <pre>
        <?php print_r( debug_backtrace() ) ?>
    </pre>
</div>