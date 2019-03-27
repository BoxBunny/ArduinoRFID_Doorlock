<?php
	
	function checkTwo($x, $y) {
		if ($x == $y)
			return true;
		else
			return false;
	}
	
	error_reporting(E_ALL);
	ini_set("display_error", 1);

	$connect = mysqli_connect("192.168.0.22","root","wuad9505","doorlock");
 
	if(!$connect) die('Not connected : ' . mysqli_error()); 
	
	$tagged_nfc = $_GET['nfc'];
	
	echo "NFC nubmer : " . $tagged_nfc . '<br>';

	$sql = "SELECT nfc, name FROM reg_user";
 
	$result = mysqli_query($connect,$sql);

	while($row = mysqli_fetch_array($result))
	{
		//echo $row['nfc'] . "  " . $row['name'] . '<br>';

		if(checkTwo($tagged_nfc, $row['nfc'])) {
			echo 'DOOR OPEN' . '<br>';
			
			if(!isset($_COOKIE[$row['name']])) {
    				echo $row['name'] . " is entered.<br>";
				setcookie($row['name'], 'staying', time() + 43200);

				//데이터베이스에 출입기록 로그 입력
				$sql2 = "INSERT INTO day_access (name) 
					values ('" . $row['name'] . "')";
				$result2 = mysqli_query($connect, $sql2);
			}
			else {
    				echo $row['name'] . " has been lefted.<br>";
				setcookie($row['name'], '', time() - 43200);

				//데이터베이스에 출입기록 로그 입력
				$sql3 = "SELECT name, enter_log FROM day_access WHERE staying = 1 AND name = '" . $row['name'] . "'";
				$result3 = mysqli_query($connect, $sql3);
				
				while($row2 = mysqli_fetch_array($result3)) {
				
					$sql4 = "INSERT INTO entry_access (name, enter_log) VALUES ('" . $row2['name'] . "', '" . $row2['enter_log'] . "')";
					$result4 = mysqli_query($connect, $sql4);
					
					$sql5 = "UPDATE day_access SET staying = 0 WHERE enter_log = '" . $row2['enter_log'] . "'";
					$result5 = mysqli_query($connect, $sql5);
				}
			}
			break;
		}
	}

	mysqli_close($connect);
?>


