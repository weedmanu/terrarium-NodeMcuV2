<?php
  // Démarrage ou restauration de la session
  session_start();
  $_SESSION = array();
  // Destruction de la session
  session_destroy();
  // Destruction du tableau de session
  unset($_SESSION);
?>
<?php header ('location: ../index.php');?>
