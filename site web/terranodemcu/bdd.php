<?php

require "connexion.php";
    
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
$result->closeCursor();

$nb = $columns['nb'];
$ef = $nb - 29730; 

if  ( $nb > 30000 )
	{
		$req = $PDO->exec("DELETE from temphumi ORDER BY date ASC LIMIT $ef");
		$req->closeCursor();			
	}
 
echo 'il y a '.$nb.' entrées dans la base de donnée';

?>
