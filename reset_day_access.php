

<?php
	error_reporting(E_ALL);
	ini_set("display_error", 1);

	$connect = mysqli_connect("192.168.0.22","root","wuad9505","doorlock");
 
	if(!$connect) die('Not connected : ' . mysqli_error()); 

	$sql1 = "DELETE FROM day_access";
 
	$result1 = mysqli_query($connect,$sql1);

	if ($result1) 
{
	echo $connect->affected_rows." data deleted successfully.";
}

	mysqli_close($connect);
?>

