<?php
session_start();

if(!isset($_SESSION['mode'])) {
    echo "NFC 입력모드가 아닙니다";
} 
else {
    if(!isset($_SESSION['nfc'])){
    echo "NFC가 아직 태그되지 않았습니다.";
   }
   else {
    echo "NFC: " . $_SESSION['nfc'] . "<br>";
    unset($_SESSION['nfc']);
    unset($_SESSION['mode']);
   }
}
?>
