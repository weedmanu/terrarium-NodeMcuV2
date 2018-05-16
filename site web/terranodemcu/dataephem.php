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
	
$reponse = $PDO->query("SELECT DATE(date) as date FROM ephem");	
$rows = array();
$rows['name'] = 'date';
while ($donnees = $reponse->fetch())
{	
	$rows['data'][] = $donnees['date'];		
}
$reponse->closeCursor();

$reponse = $PDO->query("SELECT ((Hnuit*60 + Mnuit) - (Hjour*60 + Mjour))/60 as jour FROM ephem");	
$rows1 = array();
$rows1['name'] = 'jour';
while ($donnees = $reponse->fetch())
{	
	$rows1['data'][] = $donnees['jour'];		
}	
$reponse->closeCursor();

$reponse = $PDO->query("SELECT (24*60 - ((Hnuit*60 + Mnuit) - (Hjour*60 + Mjour)))/60 as nuit FROM ephem");	
$rows2 = array();
$rows2['name'] = 'nuit';
while ($donnees = $reponse->fetch())
{	
	$rows2['data'][] = $donnees['nuit'];		
}
$reponse->closeCursor();

$result = array();
array_push($result,$rows);
array_push($result,$rows1);
array_push($result,$rows2);

print json_encode($result, JSON_NUMERIC_CHECK);

?>



