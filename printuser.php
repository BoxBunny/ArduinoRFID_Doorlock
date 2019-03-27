<!DOCTYPE html>
<html>
<head>
	<title>Doorlock409 User List</title>
	<link rel="stylesheet" href="css/style.css">
</head>

<body style="background=color: lightcyan">

	<h1>Doorlock409</h1>

	<div class="sidenav"><br><br><br>
		<a href="main.php">Home  </a>
		<a href="manageuser.php">Manage User  </a>
		<a href="printuser.php">Registered User</a>
	</div>

	<div class="content">

		<p>Registered LAB Student</p>

		<?php
			error_reporting(E_ALL);
			ini_set("display_error", 1);

			$connect = mysqli_connect("192.168.0.22","root","wuad9505","doorlock");
 
			if(!$connect) die('Not connected : ' . mysqli_error()); 

		?>

		<?php
			$sql = "SELECT user_id, name, nfc FROM reg_user";
 
			$result = mysqli_query($connect,$sql);

			echo '<table class="text=center">
				<tr>' .
				'<th>user_id</th><th>name</th><th>nfc</th>' .
				'</tr>';

			echo '<h2>LAB Assistant</h2>';

			while($row = mysqli_fetch_array($result))
			{

				echo '<tr><td>' . $row['user_id'] . '</td>' . 
					'<td>' . $row['name'] . '</td>' .
					'<td>' . $row['nfc'] . '</td>'.'<br>' ;
			}
		?>

		<?php
			$sql = "SELECT entry_id, name, enter_log FROM entry_access";
 
			$result = mysqli_query($connect,$sql);

			echo '<table class="text=center"><tr>' .
				'<th>_id</th><th>name</th><th>ent_date</th>' .
				'</tr>';

			echo '<h2>Entry LOG</h2>';

			while($row = mysqli_fetch_array($result))
			{

				echo '<tr><td>' . $row['entry_id'] . '</td>' . 
				'<td>' . $row['name'] . '</td>' .
				'<td>' . $row['enter_log'] . '</td>'.'<br>' ;
			}

			mysqli_free_result($result);
 
			mysqli_close($connect);
		?>
	</div>

</body>

</html>


