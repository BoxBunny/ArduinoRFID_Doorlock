<?php
session_start();
error_reporting(E_ALL);
ini_set("display_error", 1);

$sign = $_POST['key'];

if(isset($sign)) {
	if($sign=='1') {
		echo "DOOR OPEN";

	}
	else if($sign=='2') {
		$_SESSION['mode'] = 'mode on';
		echo "Tagging the NFC is " . $_SESSION['mode'] . "<br>";

	}
	else {
		echo "Denied";
	}
}
else
{
	echo "Denied";
}
?>
