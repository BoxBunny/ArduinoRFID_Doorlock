<?php
session_start();
$_SESSION['nfc'] = $_POST['nfc'];

if(!isset($_SESSION['mode'])) {
    echo "NFC 입력모드가 아닙니다";
} else {
    echo "NFC: " . $_SESSION['nfc'] . "<br>";
}
?>

