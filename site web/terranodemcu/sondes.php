 <?php 

///////////// Réception des datas du NodeMCU \\\\\\\\\\\\\\

//  Les valeurs des sondes
$TempC = $_GET["tempC"];
$HumiC = $_GET["humiC"];
$TempF = $_GET["tempF"];
$HumiF = $_GET["humiF"];

// Les heures et minutes du lever et coucher du soleil
$Hjour = $_GET["Hjour"];
$Mjour = $_GET["Mjour"];
$Hnuit = $_GET["Hnuit"];
$Mnuit = $_GET["Mnuit"];

// L' état des sondes
$sondepointchaud = $_GET["sondepointchaud"];
$sondepointfroid = $_GET["sondepointfroid"];

///////////// Le fichier CSV \\\\\\\\\\\\\\

// Les lignes du tableau
$lignes[] = array('humi', 'temp');
$lignes[] = array($HumiF, $TempF);
$lignes[] = array($HumiC, $TempC);

// Paramétrage de l'écriture du futur fichier CSV
$chemin = 'data.csv';
$delimiteur = ','; // Pour une tabulation, utiliser $delimiteur = "t";

// Création du fichier csv (le fichier est vide pour le moment)
// w+ : consulter http://php.net/manual/fr/function.fopen.php
$fichier_csv = fopen($chemin, 'w+');

// Boucle foreach sur chaque ligne du tableau
foreach($lignes as $ligne){
	// chaque ligne en cours de lecture est insérée dans le fichier
	// les valeurs présentes dans chaque ligne seront séparées par $delimiteur
	fputcsv($fichier_csv, $ligne, $delimiteur);
}

// fermeture du fichier csv
fclose($fichier_csv);	

///////////// La base de donnée \\\\\\\\\\\\\\

// On récupère les variables de connexion à la base de donnée
require "connexion.php";

// On essaye de se connecter à la base de donnée
try {    
	$PDO = new PDO('mysql:host='.DB_HOST.';dbname='.DB_NAME, DB_USER, DB_PASS, array(PDO::MYSQL_ATTR_INIT_COMMAND => "SET NAMES utf8"));
	$PDO->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING); 
	
// Si on n' arrive pas a se connecter à la base de donnée on affiche l' erreur
} catch(Exception $e) {
	echo "Impossible de se connecter à la base de données '".DB_NAME."' sur ".DB_HOST." avec le compte utilisateur '".DB_USER."'";
	echo "<br/>Erreur PDO : <i>".$e->getMessage()."</i>";
	die();
}
	
// On récupère la dernière entrée de la table 'etatsondes' et on place les champs dans une variable
$resultats=$PDO->query("SELECT * FROM etatsondes ORDER BY date DESC LIMIT 1");
$resultats->setFetchMode(PDO::FETCH_OBJ);
while( $resultat = $resultats->fetch() )
{
		$D = $resultat->date;
		$PC = $resultat->sondepointchaud;
		$PF = $resultat->sondepointfroid;					
}
$resultats->closeCursor();	
						
// Si la table 'etatsondes' est vide ou si une des variable diffère du champs associé, on entre les nouvelles valeurs dans la table 'etatsondes'
if ((!isset($D)) or ($sondepointchaud != $PC) or ($sondepointfroid != $PF)) {
	$resultats = $PDO->prepare("INSERT INTO etatsondes (sondepointchaud, sondepointfroid) values (:sondepointchaud, :sondepointfroid)");
	$resultats->bindParam(':sondepointchaud', $sondepointchaud);
	$resultats->bindParam(':sondepointfroid', $sondepointfroid);		
	$resultats->execute();					
	$resultats->closeCursor();
}	

// Si la lecture des sondes est OK on entre les datas des sondes dans la table 'temphumi'
if 	(($sondepointchaud == 1) and ($sondepointfroid == 1)) {			
	$resultats = $PDO->prepare("INSERT INTO temphumi (tempC, humiC, tempF, humiF) values (:tempC, :humiC, :tempF, :humiF)");
	$resultats->bindParam(':tempC', $TempC);
	$resultats->bindParam(':humiC', $HumiC);
	$resultats->bindParam(':tempF', $TempF);
	$resultats->bindParam(':humiF', $HumiF);
	$resultats->execute();					
	$resultats->closeCursor();			
}

// On récupère la dernière entrée de la table 'ephem' et on place les champs dans une variable
$resultats=$PDO->query("SELECT * FROM ephem ORDER BY date DESC LIMIT 1");
$resultats->setFetchMode(PDO::FETCH_OBJ);
while( $resultat = $resultats->fetch() )
{
		$D2 = $resultat->date;
		$Hj = $resultat->Hjour;
		$Mj = $resultat->Mjour;
		$Hn = $resultat->Hnuit;
		$Mn = $resultat->Mnuit;			
}
$resultats->closeCursor();

// Si la table 'ephem' est vide ou si une des variable diffère du champs associé, on entre les nouvelles valeurs dans la table 'ephem'
if ((!isset($D2)) or ($Hjour != $Hj) or ($Mjour != $Mj) or ($Hnuit != $Hn) or ($Mnuit != $Mn)) {
	$resultats = $PDO->prepare("INSERT INTO ephem (Hjour, Mjour, Hnuit, Mnuit) values (:Hjour, :Mjour, :Hnuit, :Mnuit)");
	$resultats->bindParam(':Hjour', $Hjour);
	$resultats->bindParam(':Mjour', $Mjour);
	$resultats->bindParam(':Hnuit', $Hnuit);
	$resultats->bindParam(':Mnuit', $Mnuit);
	$resultats->execute();
	$resultats->closeCursor();
}
					
?>
