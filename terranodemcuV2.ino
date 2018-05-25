          ////// Import des librairies nécessaires \\\\\\
          
#include <ESP8266WiFi.h>                // Pour utiliser le wifi du NodeMCU                               
#include <WiFiClient.h>                 // Pour créer une connexion TCP                  
#include <ESP8266WebServer.h>           // Pour faire un serveur web
#include <ESP8266mDNS.h>                // Pour le DNS du serveur web  
#include <ESP8266HTTPUpdateServer.h>    // Pour la mettre à jour le programme par le réseau
#include <TimeLib.h>                    // Pour utiliser les dates et heures
#include <NtpClientLib.h>               // Pour synchroniser le temps avec un serveur de temps NTP
#include <Ephemeris.h>                  // Pour calculer l'heure du lever et coucher du soleil en fonction de sa position géographique
#include <LiquidCrystal_I2C.h>          // Pour utiliser l'écran LCD I2C 
#include <Wire.h>                       // Pour communiquer avec l'écran par l I2C
#include "DHT.h"                        // Pour lire les sondes DHT

          ////// Définition des constantes et variables \\\\\\    
          
//***** Partie à modifier avec vos paramètres *****                 

const char* ssid = "XXXXXX";                       // Le noms de votre réseau wifi
const char* password = "YYYYYY";        // La clé de votre réseau wifi
const char* serveur = "XXX.XXX.XXX.XXX";         // L'adresse IP de votre serveur web (NAS, Raspberry pi ou hébergeur)
const char* update_username = "XXXX";               // On défini ici le login pour la fonction update
const char* update_password = "YYYY";           // On défini ici le mot de passe pour la fonction update
char* ville = "Gardanne";                           // On donne le noms de sa position géographique
float lat = 43.4541022;                             // On donne sa latitude (https://www.coordonnees-gps.fr/)
float lon = 5.486389300000042;                      // On donne sa longitude
// On défini l'écran  (0x3f : adresse I2C de l’écran (pour la trouver utiliser scanI2C.ino (voir tuto)) et 16 colonnes, 2 lignes)
LiquidCrystal_I2C lcd(0x3f, 16, 2);  
char* serveurNTP = "fr.pool.ntp.org";               // Le serveur NTP que l'on va interrogé (vous pouvez garder celui-la ou pool.ntp.org)

//**************************************************

const char* host = "terranodemcu";                  // le noms qui remplace l'IP du NodeMCU pour la fonction update 
const char* update_path = "/update";                // le chemin pour le fichier d' update
// On défini le serveur web pour l' update
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
// défini le capteur DHT22
const char DHTTYPE = DHT22;   // DHT11, DHT21 ou DHT22
// Pins des sondes
const byte DHTPINPC = 14;     // D5
const byte DHTPINPF = 0;      // D3
// On défini les sondes
DHT dhtPC(DHTPINPC, DHTTYPE);
DHT dhtPF(DHTPINPF, DHTTYPE);
// les notes de musique pour la chanson de Starswar
const int c = 261;
const int d = 294;
const int e = 329;
const int f = 349;
const int g = 391;
const int gS = 415;
const int a = 440;
const int aS = 455;
const int b = 466;
const int cH = 523;
const int cSH = 554;
const int dH = 587;
const int dSH = 622;
const int eH = 659;
const int fH = 698;
const int fSH = 740;
const int gH = 784;
const int gSH = 830;
const int aH = 880;
// la timezone (fuseau horaire)
const byte timeZone = 1;        
// Pins des relais
const byte lum = 5;           // D1    
const byte chauff = 16;       // D0
// On défini le bouton
const byte  buttonPin = 12;   // D6
// Détecteur de sécheresse
const byte PinHumi=13;        // D7
byte secheresse;             
// Le buzzer
const byte buzzerPin = 15;    // D8
// Les LEDs
const byte ledPin1 = 1;       // D9
const byte ledPin2 = 3;       // D10
// On définie les variables où seront stockées les caractères perso             
const byte HEAD = 1; 
const byte TAIL = 2;             
const byte CLEAR = 3;
const byte SKULL = 4;
const byte BONES = 5;     
const byte ACCENT = 6;  
// Le dessin des caractères HEAD TAIL CLEAR (https://maxpromer.github.io/LCD-Character-Creator/)
byte snakeHead[8] = {0x11, 0xa, 0x1f, 0x15, 0xa, 0x11, 0xe};
byte snakeTail[8] = {0x4, 0xe, 0x1b, 0x1b, 0xe, 0xe, 0x4};
byte noSnake[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
byte tete[8] = {0x0, 0x0, 0x0, 0x1F, 0x15, 0x1F, 0x11, 0x1F};
byte os[8] = {0x11, 0x0E, 0x0E, 0x11, 0x0, 0x0, 0x0, 0x0};
byte eAigu[8] = {0x01, 0x02, 0x0, 0x0E, 0x11, 0x1F, 0x10, 0x0F};
// les variables de temps 
unsigned int Hnow, Hmatin, Hsoir, Hjour, Mjour, Hnuit, Mnuit;
// Compteur de la fonction "sec", "beep" et "terrarium"
unsigned int alarm, counter, compteursonde = 0; 
// Pour le changement d'heure
byte UTCOffset;
// On déclare les variables pour gérer la boucle de chaque fonction du loop avec millis()
unsigned long tempsgestiontemps, tempssec, tempsterra, tempsenvoi;
// Pour la reconnexion au wifi
boolean wifiFirstConnected = false;
// Pour le flag de la fonction qui est lancée par une interruption
boolean flag_bouton = false;
// pour la fonction NTP
boolean syncEventTriggered = false; // Vrai si un temps a été déclenché
NTPSyncEvent_t ntpEvent;            // Dernier événement déclenché

          ///// Les fonctions \\\\\

void sec() {  
  if((millis() - tempssec) > 2000) {
    secheresse = digitalRead(PinHumi);    // on lit la valeur du détecteur
    if (secheresse) {                     // si il est à 1 c'est qu'il n'y a plus d'eau    
      // pour éviter de lancer l'alarme sur une fausse détection   
      alarm++;                            // on incrémente le compteur  
      if (alarm > 2) {
        lcd.backlight();
        lcd.setCursor(1,0);
        lcd.write(SKULL);
        lcd.setCursor(4,0);
        lcd.print("Il n'y a");
        lcd.setCursor(14,0); 
        lcd.write(SKULL);   
        lcd.setCursor(1, 1);
        lcd.write(BONES);  
        lcd.setCursor(3,1);
        lcd.print("plus d'eau");
        lcd.setCursor(14,1); 
        lcd.write(BONES);        
        starwars();
        lcd.clear();
        lcd.noBacklight();       
        alarm = 0;
      }          
    } else {
      alarm = 0;
    }
  tempssec = millis();
  }
}

// La fonction bouton qui lance l'affichage LCD
void bouton() {                 // elle est lancée sur une interruption du buttonPin
  if (flag_bouton) {            // si le flag est vrai 
      affichage();              // on lance l'affichage
      flag_bouton = false;      // on rebascule le flag à faux
  }    
}

// intro animation LCD 
void intro() {         
  lcd.setCursor(0,1);
  lcd.write(HEAD);   
  delay(200);  
  for (int i=0; i<6; i++) {
   int j = i+1;
   lcd.setCursor(i,1);
   lcd.write(TAIL);
   lcd.setCursor(j,1);
   lcd.write(HEAD);
   delay(200);
  }          
  lcd.setCursor(0,1);
  lcd.write(CLEAR);
  lcd.setCursor(6,1);
  lcd.write(TAIL);
  lcd.setCursor(7,0);
  lcd.write(HEAD);
  delay(200);      
  lcd.setCursor(0,0);
  lcd.print("Terra");
  for (int i=1; i<7; i++) {
   int j = i+6;
   int k = j+1;
   lcd.setCursor(i,1);
   lcd.write(CLEAR);
   lcd.setCursor(j,0);
   lcd.write(TAIL);
   lcd.setCursor(k,0);
   lcd.write(HEAD);
   delay(200);
  }   
  lcd.setCursor(7,1);
  lcd.print("NodeMCU");
  for (int i=7; i<9; i++) {
   int j = i+6;
   int k = j+1;
   lcd.setCursor(i,0);
   lcd.write(CLEAR);
   lcd.setCursor(j,0);
   lcd.write(TAIL);
   lcd.setCursor(k,0);
   lcd.write(HEAD);
   delay(200);
  }      
  lcd.setCursor(8,0);
  lcd.write(CLEAR);
  lcd.setCursor(15,0);
  lcd.write(TAIL);
  delay(200);
  for (int i=9; i<16; i++) {
   lcd.setCursor(i,0);
   lcd.write(CLEAR);
   delay(200);
  }   
  lcd.clear();
}

//sortie animation LCD  
void outro () {          
  lcd.setCursor(2,0);
  lcd.print("Bye bye !!!");
  lcd.setCursor(1,1);
  lcd.print("Terra-NodeMCU");
  delay(1500); 
  lcd.clear(); 
  lcd.noBacklight();
 }

// La fonction de l'affichage des datas des sondes en temps réel sur l'écran LCD ********
void affichage() {
  // lire la sonde point chaud
  float hC = dhtPC.readHumidity();  
  float tC = dhtPC.readTemperature();
  // lire la sonde point froid
  float hF = dhtPF.readHumidity();  
  float tF = dhtPF.readTemperature();
  // on allume le retro-éclairage
  lcd.backlight();
  delay(100);    
  // on lance l'animation d'introduction
  intro();
  // on lance l'animation 
  lcd.setCursor(0,0);
  lcd.print("Nous sommes le :");
  lcd.setCursor(0,1);
  lcd.print(day());
  lcd.print("/");
  lcd.print(month());
  lcd.print("/");
  lcd.print(year());
  delay(1500); 
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Il est :");
  lcd.setCursor(0,1);
  lcd.print(hour());
  lcd.print("h");
  lcd.print(minute());
  lcd.print("min");    
  delay(1500); 
  lcd.clear();    
  lcd.setCursor(3,0);
  lcd.print("Point chaud :");
  lcd.setCursor(0,1);
  lcd.print("Temp = ");
  lcd.print(float (tC));
  lcd.print(" ");
  lcd.write(223);
  lcd.print("C");
  delay(1500);    
  lcd.setCursor(0,1);
  lcd.print("Humi =  ");
  lcd.print(float (hC));
  lcd.print(" %");
  delay(1500);
  lcd.clear();    
  lcd.setCursor(3,0);
  lcd.print("Point froid :");
  lcd.setCursor(0,1);
  lcd.print("Temp = ");
  lcd.print(float (tF));
  lcd.print(" ");
  lcd.write(223);    // caractère spécial du °
  lcd.print("C");
  delay(1500);        
  lcd.setCursor(0,1);
  lcd.print("Humi =  ");
  lcd.print(float (hF));
  lcd.print(" %");
  delay(1500);
  lcd.clear(); 
  delay(1000);
  // on lance l'animation de sortie
  outro();         
}


// la fonction qui calcul le lever et coucher du soleil en fonction de sa position et de la date
void printRiseAndSet(char *city, FLOAT latitude, FLOAT longitude, int UTCOffset, int day, int month, int year) {
  Ephemeris::setLocationOnEarth(latitude,longitude);           
  SolarSystemObject sun = Ephemeris::solarSystemObjectAtDateAndTime(Sun,day,month,year,0,0,0);   
  if( sun.riseAndSetState == RiseAndSetOk ) {
    int hours,minutes;
    FLOAT seconds;    
    Ephemeris::floatingHoursToHoursMinutesSeconds(Ephemeris::floatingHoursWithUTCOffset(sun.rise,UTCOffset), &hours, &minutes, &seconds);
    Hjour = hours;
    Mjour = minutes;
    Hmatin = hours * 100 + minutes;        
    Ephemeris::floatingHoursToHoursMinutesSeconds(Ephemeris::floatingHoursWithUTCOffset(sun.set,UTCOffset), &hours, &minutes, &seconds);
    Hnuit = hours;
    Mnuit = minutes;
    Hsoir = hours * 100 + minutes;    
  }  
}

// La fonction qui fait clignoter les LED en rythme et joue les notes de la chanson
void beep(int note, int duration) {   
  secheresse = digitalRead(PinHumi);  // On vérifie la présence d'eau 
  if (!secheresse) {                  // Dés qu'on en a remis 
    counter = 0;
    return;                           // On arrête la chanson
  }
  // Jouer la tonalité sur buzzerPin
  tone(buzzerPin, note, duration); 
  // Jouer différentes LED en fonction de la valeur de 'compteur'
  if(counter % 2 == 0) {           // si le reste du compteur / 2 est égale à 0   
    digitalWrite(ledPin1, HIGH);
    delay(duration);
    digitalWrite(ledPin1, LOW);    
  } else {                        // sinon on inverse l'état des leds
    digitalWrite(ledPin2, HIGH);
    delay(duration);
    digitalWrite(ledPin2, LOW);
    } 
  // Arrêt de la tonalité sur buzzerPin
  noTone(buzzerPin); 
  delay(50); 
  // Incrémentation du compteur
  counter++;
}

// Première section de la fonction song starwars 
void firstSection() {
  beep(a, 500);
  beep(a, 500);    
  beep(a, 500);
  beep(f, 350);
  beep(cH, 150);  
  beep(a, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 650); 
  delay(500); 
  beep(eH, 500);
  beep(eH, 500);
  beep(eH, 500);  
  beep(fH, 350);
  beep(cH, 150);
  beep(gS, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 650); 
  delay(500);
}

// Deuxième section de la fonction song starwars  
void secondSection() {
  beep(aH, 500);
  beep(a, 300);
  beep(a, 150);
  beep(aH, 500);
  beep(gSH, 325);
  beep(gH, 175);
  beep(fSH, 125);
  beep(fH, 125);    
  beep(fSH, 250); 
  delay(325); 
  beep(aS, 250);
  beep(dSH, 500);
  beep(dH, 325);  
  beep(cSH, 175);  
  beep(cH, 125);  
  beep(b, 125);  
  beep(cH, 250);   
  delay(350);
}

// La fonction song starwars
void starwars() {  
  // Jouer la première section
  firstSection(); 
  // Jouer la deuxième section
  secondSection(); 
  // Variante 1
  beep(f, 250);  
  beep(gS, 500);  
  beep(f, 350);  
  beep(a, 125);
  beep(cH, 500);
  beep(a, 375);  
  beep(cH, 125);
  beep(eH, 650); 
  delay(500); 
  // Rejouer la deuxième section
  secondSection(); 
  // Variante 2
  beep(f, 250);  
  beep(gS, 500);  
  beep(f, 375);  
  beep(cH, 125);
  beep(a, 500);  
  beep(f, 375);  
  beep(cH, 125);
  beep(a, 650);   
  delay(650);
  counter = 0;
}

// La fonction qui s'occupe du chauffage et de la lumière
void terrarium() {  
  byte target;
  // Si le temps actuel par rapport au temps de démarrage du timer est > 7s ( toutes les 7s )
  if((millis() - tempsterra) > 11000) {  // utilisation de nombre premier pour ne jamais lire les sondes en même temps que la fonction "envoidata"        
    // Pour faciliter les calcul (21h03 devient 2103) 
    Hnow = hour() * 100 + minute();        // Heure actuelle      
    if ( Hnow > Hmatin && Hnow < Hsoir ) { // Si c'est le jour      
      target = 28;                         // La consigne du chauffage est de 28 °C   
      digitalWrite(lum, LOW);              // On coupe le relais et le courant passe donc la lumière est allumée
    }  
    else {                                 // Sinon , donc c'est la nuit                                   
      target = 23;                         // La consigne du chauffage est de 23 °C
      digitalWrite(lum, HIGH);             // On active le relais, il coupe le courant, la lumière est éteinte
    }        
    // On lis la sondes point chaud (température uniquement c'est pour la régulation du chauffage)
    float tC = dhtPC.readTemperature();   
    // Si la lecture de la sonde échoue on incrémente le compteur sinon il vaut 0
    if (isnan(tC)) {
      compteursonde++; 
    } else {
      compteursonde = 0; 
    }
    // Si le compteur dépasse 2 (problème sur la sonde du chauffage) ou si la consigne de température est dépassée on coupe le chauffage
    if (compteursonde > 2 || target < tC) {
      digitalWrite(chauff, HIGH); 
    } else { 
      digitalWrite(chauff, LOW); 
    }    
    tempsterra = millis();          // On réinitialise le timer 
  }       
}

//  La fonction qui envoi les datas au serveur web
void envoidata() {
  // Si le temps actuel par rapport au temps de démarrage du timer est > 17s ( toutes les 17s )
  if((millis() - tempsenvoi) > 17000) {   // Utilisation de nombre premier pour ne jamais lire les sondes en même temps que la fonction terrarium                  
    // Lire la sonde point chaud 
    float hC = dhtPC.readHumidity();  
    float tC = dhtPC.readTemperature();    
    // Lire la sonde point froid
    float hF = dhtPF.readHumidity();  
    float tF = dhtPF.readTemperature();   
    // Création de la connexion TCP
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(serveur, httpPort)) {
      Serial.println("échec de la connexion");
      return;
    }
    // Envoi de toutes ces datas par l'URL et par GET au fichier sonde.php sur le serveur (dans le dossier terranodemcu)
    client.print(String("GET /terrarium/terranodemcu/sondes.php?tempC=") + String(float (tC)) + "&humiC=" + String(float(hC)) + "&tempF=" + String(float(tF)) + "&humiF=" + String(float(hF)) + 
                 "&Hjour=" + String(int(Hjour)) + "&Mjour=" + String(int(Mjour)) + "&Hnuit=" + String(int(Hnuit)) + "&Mnuit=" + String(int(Mnuit)) +
                 " HTTP/1.1\r\n" +  "Host: " + serveur + "\r\n" + "Connection: close\r\n\r\n");       
    while(client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }       
  tempsenvoi = millis();  // On réinitialise le timer  
  }      
}

// La fonction qui traite le résultat du la synchronisation 
void processSyncEvent (NTPSyncEvent_t ntpEvent) {  
  if (!ntpEvent) {        
      NTP.getTimeDateString (NTP.getLastNTPSync ()); 
      // si c'est l'heure d'été on modifie l'offset de la timezone de la fonction "ephemeris"
      if (NTP.isSummerTime ()) { 
        UTCOffset = 2; 
      } else { 
        UTCOffset = 1; 
      }   
      // On lance la fonction qui calcul le lever et coucher du soleil en fonction de sa position et de la date
      printRiseAndSet(ville, lat, lon, UTCOffset, day(),month(),year());
  } else {        
    if (ntpEvent == noResponse) {         // si on a pas de reponse du serveur NTP on consèrve la dernière synchronisation
		NTP.getTimeDateString (NTP.getLastNTPSync ());
    }                 
    else if (ntpEvent == invalidAddress) {   // si l'adresse du serveur NTP devenait erroné (improbable) on consèrve la dernière synchronisation
		NTP.getTimeDateString (NTP.getLastNTPSync ());
    }
  } 
}  

// La fonction qui s'occupe de gérer le temps
void gestiontemps() {   
  if ((millis () - tempsgestiontemps) > 7000) {     
    // S'il y a un déconnexion reconnexion du wifi on relance le cycle du NTP       
    if (wifiFirstConnected) {
          wifiFirstConnected = false;
          NTP.begin (serveurNTP, timeZone, true);
          NTP.setInterval (63);          
    }
    // Si on a une synchronisation NTP  on affiche les infos
    if (syncEventTriggered) {
          processSyncEvent (ntpEvent);
          syncEventTriggered = false; 
    }    
    NTP.getTimeDateString (NTP.getLastNTPSync ());
    tempsgestiontemps =  millis (); 
  }    
}

          ///// Les 3 fonctions de gestion du wifi \\\\\

// Fonction qui se lance lorsqu' on est connecté au wifi
void onSTAConnected (WiFiEventStationModeConnected ipInfo) {
  Serial.printf ("Connecté à %s\r\n", ipInfo.ssid.c_str ());
}

// Démarre le NTP uniquement après la connexion du réseau
void onSTAGotIP (WiFiEventStationModeGotIP ipInfo) {
  Serial.printf ("Récupération de l'adresse IP: %s\r\n", ipInfo.ip.toString ().c_str ());
  Serial.printf ("Connecté: %s\r\n", WiFi.status () == WL_CONNECTED ? "oui" : "non");    
  wifiFirstConnected = true; // on passe le flag à vrai (1er connexion)
}

// Gérer la déconnexion du réseau
void onSTADisconnected (WiFiEventStationModeDisconnected event_info) {
  Serial.printf ("Déconnecté du SSID: %s\n", event_info.ssid.c_str ());
  Serial.printf ("Raison: %d\n", event_info.reason);    
  NTP.stop(); // La synchronisation NTP est désactivée pour éviter les erreurs de synchronisation
}

// La fonction qui est lancée sur une interruption 
void interruptbouton() {
  flag_bouton = true;
}

// Le setup qui prépare le programme
void setup(void) {
  // On déclare un nom pour les 3 fonctions de gestion du wifi
  static WiFiEventHandler e1, e2, e3;
  // les 3 fonctions qui gèrent la connexion et déconnexion du wifi
  e1 = WiFi.onStationModeGotIP (onSTAGotIP);
  e2 = WiFi.onStationModeDisconnected (onSTADisconnected);
  e3 = WiFi.onStationModeConnected (onSTAConnected);     
  Serial.begin(115200);  
  Serial.println("initialisation...");        
  WiFi.mode(WIFI_AP_STA);
  Serial.print("Connexion à ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("L'adresse IP est  ");
  Serial.println(WiFi.localIP());
  wifiFirstConnected = false;
  // on pépare la fonction NTP
  NTP.onNTPSyncEvent ([](NTPSyncEvent_t event) {
        ntpEvent = event;
        syncEventTriggered = true;
  });   
  delay(100);        
  // On démarre la synchronisation du temps au pool NTP
  NTP.begin (serveurNTP, timeZone, true);  
  // On donne l'intervalle de synchronisation
  NTP.setInterval (63); 
  delay(100);  
  // On démarre le service DNS qui permet de remplacer l'IP du NodeMCU par un noms de host
  MDNS.begin(host);
  // On défini les paramètres du serveur web de la fonction update
  httpUpdater.setup(&httpServer, update_path, update_username, update_password);
  // On démarre le serveur
  httpServer.begin();
  MDNS.addService("http", "tcp", 80);
  // On affiche dans le port série l'adresse de connection pour la fonction update
  Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);  
  // On déclare le buzzer en sortie   
  pinMode(buzzerPin, OUTPUT);   
  // On déclare les pins des relais en sortie
  pinMode(lum, OUTPUT);
  pinMode(chauff, OUTPUT);
  // On initialise les DHT 
  dhtPC.begin();
  dhtPF.begin();
  // On déclare le GPIO du bouton et du détecteur de sécheresse en entrée avec la résistance de PULLUP: 
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(PinHumi, INPUT_PULLUP);
  // On attache une interruption sur buttonPin vers la fonctions "interruptbouton" en cas de mise à 0 du GPIO (état 1 par défaut)  
  attachInterrupt(digitalPinToInterrupt(buttonPin), interruptbouton, FALLING);
  // On déclare les GPIO pour l' I2C (D1 et D2 par défaut)
   Wire.begin(2,4); // D4 et D2 (car plus approprié pour notre programme)
  // On initialise le LCD
  lcd.begin();  
  // On éteint le retro-éclairage et efface l'écran
  lcd.noBacklight();
  lcd.clear();
  // Création des caractères perso
  lcd.createChar(HEAD, snakeHead);
  lcd.createChar(TAIL, snakeTail);
  lcd.createChar(CLEAR, noSnake);
  lcd.createChar(SKULL, tete);
  lcd.createChar(BONES, os);  
  lcd.createChar(ACCENT, eAigu);
  delay(100);    
  // En fonction de l'heure d'été ou hiver on modifie l'offset de la timezone de la fonction "ephemeris"
  // On l'affiche également sur le LCD 
  lcd.backlight();
  if (NTP.isSummerTime ()) {
    UTCOffset = 2 ;
    lcd.home();
    lcd.print("Heure d '");    
    lcd.write(ACCENT);
    lcd.print("t");
    lcd.write(ACCENT);
  } else { 
    UTCOffset = 1; 
    lcd.home();
    lcd.print("Heure d'hiver");       
  }  
  delay(2000);
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("Fin de l' ");
  lcd.setCursor(1,1);
  lcd.print("initialisation");
  delay(2000); 
  lcd.clear(); 
  lcd.noBacklight();  
  Serial.println("fin du setup");  
  // les LED , après la déclaration des LED en sortie, on aura plus rien sur le port série
  // car elles sont branché sur le TX RX du NodeMCU
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);     
  tempsgestiontemps, tempssec, tempsterra, tempsenvoi = millis(); 
}

// le loop qui lance les fonctions en boucle
void loop(void){
  httpServer.handleClient(); 
  gestiontemps();
  bouton();
  sec();
  terrarium();
  envoidata();
}
