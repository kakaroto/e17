
<?php

require_once "donate_settings.php";

$loginParams = mysql_connect("$dbhost", "$dbusername", "$dbpass");
mysql_select_db("$dbname",$loginParams);

$result = mysql_query("select SUM(donation) as donation, COUNT(name) as donors from donations group by null");
if(!$result){
	echo mysql_error();
}
$row = mysql_fetch_array($result);
$donation = $row['donation']+$offset;
$donation_formatted = number_format ($donation, 2);
$donors = $row['donors'];
?>
<center>
<p>
    <b>Current Status</b>
     <b>Total Donated: <? echo $cursym.$donation_formatted; ?><br> By <? echo $donors; ?> donors.</b>
</p>
<p style="margin-top:0; margin-bottom:0; position:relative;"><b>Most Recent Donors:</b><br>

<?

$result = mysql_query("select name, donation, date from donations ORDER BY date DESC");
if(!$result){
	echo mysql_error();
}

$i=0;
//$row = mysql_fetch_array($result);
 $num = 0;
while ($row = mysql_fetch_array($result)) {
    echo $row['name'] ."<br>";
    if($num++ > 15)
      break;
}	

/*
#$donation += $offset;
$donation_formatted = number_format ($donation, 2);
$goal_formatted = number_format($goal, 2);
$percent_raised = number_format((float)$donation/(float)$goal*100, 1);
*/
?>
</p></center>
