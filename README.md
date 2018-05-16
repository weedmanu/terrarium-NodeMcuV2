# terrarium-NodeMcu-V2

## Modification du programme :

amélioration du code et modification de la gestion de la lumière :

* Récupère le temps sur le réseaux et prend en charge le changement d'heure avec la librairie :
	* [NtpClient](https://github.com/gmag11/NtpClient/archive/master.zip)
* Calcule l'heure du lever et coucher du soleil en fonction de votre position géographique avec la librairie :
	* [ephemeris](https://github.com/MarScaper/ephemeris.git)
	
Dans le programme **terranodemcu_V2.ino** voici les lignes à modifier avec vos paramètre :

* ligne 19 : votre SSID (nom de votre réseau wifi)
* ligne 20 : le mot de passe du wifi
* ligne 21 : ip du serveur web (NAS ou Raspberry)
* ligne 22 : on défini un login pour la fonction update
* ligne 23 : votre mot de passe pour la fonction update
* ligne 24 : le nom de votre ville ou village
* ligne 25 et 26: la latitude et la longitude (https://www.coordonneesgps.net/coordonnees-gps/)
* ligne 28 : adresse I2C de votre écran


le programme **terranodemcuV2.ino** envoie les datas sur le fichier **sondes.php** du site web.

Ce fichier **sondes.php** se charge de :

* créer le fichier **data.csv** pour créer les jauges sur la page web.
* d'envoyer les datas dans les tables de la base de donnée pour faire les graphes
