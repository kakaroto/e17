<?PHP

require_once "donate_settings.php";

$loginParams = mysql_connect("$dbhost", "$dbusername", "$dbpass");
mysql_select_db("$dbname",$loginParams);

$result = mysql_query("select name, donation from `donations` 
                        ORDER BY donation DESC");
						
if(!$result)
	echo mysql_error();

$i=0;
//$row = mysql_fetch_array($result);
while ($row = mysql_fetch_array($result)) {
    if($i++ < 20)
        $top5[] = $row['name'];
	$donation += $row['donation'];
//	$row = mysql_fetch_array($result);
}	

$donation += $offset;

$donation_formatted = number_format ($donation, 2);
$goal_formatted = number_format($goal, 2);
$percent_raised = number_format((float)$donation/(float)$goal*100, 1);

?>
    <h2>Current Status</h2>
	<div id="donation_meter_progress"
		style="width:500px; 
background:url('http://www0.get-e.org/donate/images/progress_background.png');
		text-align:center; border:1px solid #b3b3b3;
        height:25px;">
		<div id="donation_meter_bar" 
			style="height:25px; vertical-align:middle;
            background:url('http://donate.edevelop.org/images/progress<?php echo ( 
$percent_raised >= 100 ? '_complete' : '' );?>_foreground.png');background-repeat: repeat-x;
            width:<?php echo ( $percent_raised >= 100 ? '100' : $percent_raised ); ?>%;">
		</div>
	</div>
    <p style="width:500px; position:relative; top:-32px;
                    z-index:3; font-weight:bold; font-size:12px;
                    color:#000000; padding:0; text-align:center; clear:none;
					margin-bottom:0;">
			<?php echo $cursym.$donation_formatted . '/$' . $goal_formatted
                    . ' (' . $percent_raised . '%)'; ?>
    </p>
	<?php if(count($top5)) { ?>
    <p style="margin-top:0; margin-bottom:0; position:relative; top:-25px;"><b>Top Donators:</b> <?php echo implode(', ', $top5);?></p>
	<?php } ?>
