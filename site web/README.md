Le fichier **sondes.php** se charge de :

* créer le fichier **data.csv** pour créer les jauges sur la page web.
* d'envoyer les datas dans les tables de la base de donnée pour faire les graphes

Dans le fichier **connexion.php** vous devez modifier :

* ligne 5 : login de la bdd
* ligne 6 : mot de passe de la bdd

Il vous faudra PHP, apache, mariaDB (mysql) et phpmyadmin.

le fichier **terranodemcu.sql** défini la BDD, pour cela ouvrez phpmyadmin, créez une base de donnée nommée **terranodemcu** et importez **terranodemcu.sql**.

j'utilise un NAS Synology pour héberger le site web, mais un Raspberry ou autre NAS fait l'affaire.

Pour dessiner les jauges j'utilise la librairie [Rgraph](https://www.rgraph.net/demos/gauge-bank.html)

Pour faire les graphes j'utilise la librairie [highcharts](https://www.highcharts.com/demo)

elle sont dans le dossier **lib**

Dans le fichier **backup.php** du dossier **admin** vous devez modifier :

* ligne 8   : login de la bdd
* ligne 9   : mot de passe de la bdd

Et dans le fichier **auth.php** du **dossier admin** vous devez modifier :		
		
* ligne 3 : remplacer XXXX par un login d'accès pour la page admin du site web
* ligne 4 : remplacer YYYY par un mot de passe pour la page admin du site web		
		
La partie admin étant protégé par un login et mot de passe

Vous devez également donner les droits en lecture et écriture au groupe **http** (pour un NAS) ou **www-data** (pour un Raspberry) du fichier **data.csv**  ainsi qu'au dossier **backup** qui se trouve dans le dossier **admin**.
