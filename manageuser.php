<!DOCTYPE html>
<html>
<head>
	<title>Doorlock409 Manage User</title>
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
		<p>아두이노와 어플리케이션 동작<br>시뮬레이션</p><br>
		<form action="/doorlock409/dooropen.php" method="post">
		<fieldset>
			<legend>C#</legend>
			/doorlock409/dooropen.php<br>
			RFID 리더기 동작<br>
			<input type="radio" name="key" value="1" checked> Opendoor (1)<br>
			<input type="radio" name="key" value="2" checked> NFC 요청 (2)<br>
			<input type="radio" name="key" value="3" checked> NULL (3)
 			<br><br>
 			<input type="submit" value="RFID Control">
		</fieldset>
		</form>

		<form action="/doorlock409/nfc_add.php" method="post">
		<fieldset>
			<legend>아두이노</legend>
			/doorlock409/nfc_add.php<br>
			아두이노 NFC 번호 전송<br>
  			<input type="text" name="nfc" value=""><br><br>
 			<input type="submit" value="nfc_add">
		</fieldset>
		</form> 

		<form action="/doorlock409/send_nfc_app.php" method="get">
		<fieldset>
			<legend>C#</legend>
			/doorlock409/send_nfc_app.php<br>
			Send 'nfc' to C# :<br>
 			<input type="submit" value = "받아오기">
		</fieldset>
		</form> 

		<form action="/doorlock409/input_reg_user.php" method="post">
		<fieldset>
			<legend>회원가입 데이터베이스 삽입<br>/doorlock409/input_reg_user.php</legend>
			ID<br>
  			<input type="text" name="user_id" value="409-">
 			<br>Password<br>
  			<input type="password" name="user_passwd" value="1234">
 			<br>Name<br>
  			<input type="text" name="name" value="test-">
 			<br>NFC<br>
  			<input type="text" name="nfc" value="ABCDEF"><br>
 			<br>
 			<input type="submit" value="Submit">
		</fieldset>
		</form>
		<br>

		<form action="/doorlock409/fetch_reg_nfc.php" method="get">
		<fieldset>
			<legend>문열기 모드</legend>
			/doorlock409/fetch_reg_nfc.php<br>
			아두이노 NFC 문 열기<br>
  			<input type="text" name="nfc" value="12 123 12 123 "><br><br>
 			<input type="submit" value="NFC 입력">
		</fieldset>
		</form>
	</div>

</body>
</html>
