<?php

require "../connexion.php";
    
try {    
    $PDO = new PDO('mysql:host='.DB_HOST.';dbname='.DB_NAME, DB_USER, DB_PASS, array(PDO::MYSQL_ATTR_INIT_COMMAND => "SET NAMES utf8"));
    $PDO->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING); 

} catch(Exception $e) {
    echo "Impossible de se connecter à la base de données '".DB_NAME."' sur ".DB_HOST." avec le compte utilisateur '".DB_USER."'";
    echo "<br/>Erreur PDO : <i>".$e->getMessage()."</i>";
    die();
}
 
$sql = 'SELECT COUNT(*) AS nb FROM temphumi';
$result = $PDO->query($sql);
$columns = $result->fetch();    
$nb = $columns['nb'];
$result->closeCursor();

echo '<br/>';
echo 'il y a '.$nb.' entrées dans la base de donnée <br/>';	
echo '
Veux tu la sauvegarder ? 	
<form method="post" action="index.php">
<input type="hidden" name="reponse" value="oui">		
<input type="submit" value="Valider">
</form> 				
<p><img src="../img/serpent3.gif" alt="gif"/></p>	
Ou veux tu la purger ? 	
<form method="post" action="index.php">
<input type="hidden" name="reponse2" value="oui">		
<input type="submit" value="Valider">
</form>		
'; 
$reponse=$_POST['reponse'];
$reponse2=$_POST['reponse2'];    

if($reponse=="oui") {		
	require "backup.php";		
	}	
if($reponse2=="oui") {
	require "purgebdd.php";
	}

?>
