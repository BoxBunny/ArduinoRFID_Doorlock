<?php
error_reporting(E_ALL);
ini_set("display_error", 1);

$connect = mysqli_connect("192.168.0.22","root","wuad9505","doorlock");
 
if(!$connect) die('Not connected : ' . mysqli_error()); 

$name = $_POST['name'];
$user_id = $_POST['user_id'];
$user_passwd = $_POST['user_passwd'];
$nfc = $_POST['nfc'];

$sql1 = "INSERT INTO reg_user (reg_id, name, user_id, user_passwd, nfc, reg_date) 
VALUES (default, '". $name ."', '". $user_id ."', password(".$user_passwd."), '". $nfc ." ', default);
";

$result1 = mysqli_query($connect, $sql1);

if ($result1) 
{
	echo $connect->affected_rows." 데이터가 정상적으로 추가되었습니다.";
}
else
{
	echo "ERROR: 자료가 추가되지 않았습니다.";
}


mysqli_close($connect);
?>
