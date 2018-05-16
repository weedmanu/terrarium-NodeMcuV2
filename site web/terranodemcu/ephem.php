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

$resultats=$PDO->query("SELECT * FROM ephem ORDER BY date DESC LIMIT 1");
$resultats->setFetchMode(PDO::FETCH_OBJ);
while( $resultat = $resultats->fetch() )
{
		$D = $resultat->date;
		$Hj = $resultat->Hjour;
		$Mj = $resultat->Mjour;
		$Hn = $resultat->Hnuit;
		$Mn = $resultat->Mnuit;			
}
$resultats->closeCursor();	

?>

<p style="color:#90EE90">La lumière s'allumera à <span style="color:yellow"><?php echo " $Hj H et $Mj";?>min </span> et s'éteindra à <span style="color:#EF9090"><?php echo " $Hn H et $Mn";?>min </span></p>




