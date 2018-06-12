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

// les notes de musique
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS  455
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GSH  830
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

          ////// Définition des constantes et variables \\\\\\    
          
//***** Partie à modifier avec vos paramètres *****                 

const char* ssid = "XXXX";                       	// Le noms de votre réseau wifi
const char* password = "YYYY";        				// La clé de votre réseau wifi
const char* serveur = "XXX.XXX.XXX.XXX";         	// L'adresse IP de votre serveur web (NAS, Raspberry pi ou hébergeur)
const char* update_username = "XXXX";               // On défini ici le login pour la fonction update
const char* update_password = "YYYY";           	// On défini ici le mot de passe pour la fonction update
char* ville = "Gardanne";                           // On donne le noms de sa position géographique
float lat = 43.4541022;                             // On donne sa latitude (https://www.coordonnees-gps.fr/)
float lon = 5.486389300000042;                      // On donne sa longitude
// On défini l'écran  (0x27 : adresse I2C de l’écran (pour la trouver utiliser scanI2C.ino (voir tuto)) et 16 colonnes, 2 lignes)
LiquidCrystal_I2C lcd(0x27, 16, 2);  
char* serveurNTP = "fr.pool.ntp.org";               // Le serveur NTP que l'on va interrogé
// pour la fonction événement  les prénoms, dates anniversaires et heure d'avertissement, et jour et heure d'avertissement du repas des serpents
const char* noms[] = {"manu", "sosso", "yohan", "clara"};  
int dateanniv[] = {2612, 1008, 2806, 2206};
int heureanniv[] = {730, 731, 800, 801, 1600, 1601, 1630, 1631, 1930, 1931, 2000, 2001};
int daterepas[] = {1, 9, 12, 18, 26};
int heurerepas[] = {830, 835, 2030, 2035};

//**************************************************

const char* host = "terranodemcu";                  // le noms qui remplace l'IP pour la fonction update 
const char* update_path = "/update";                // le chemin pour le fichier d' update
// On défini le serveur web pour l' update
ESP8266WebServer Webserveur(80);
ESP8266HTTPUpdateServer httpUpdater;
MDNSResponder mdns; // serveur mDNS
// défini le capteur DHT22
const char DHTTYPE = DHT22;   // DHT11, DHT21 ou DHT22
// Pins des sondes
const byte DHTPINPC = 14;     // D5
const byte DHTPINPF = 0;      // D3
// On défini les sondes
DHT dhtPC(DHTPINPC, DHTTYPE);
DHT dhtPF(DHTPINPF, DHTTYPE);
float tC,hC,tF,hF;
byte target;
// la timezone (fuseau horaire)
const byte timeZone = 1;        
// Pins des relais
const byte lum = 16;           // D0    
const byte chauff = 5;       // D1
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
const byte COEUR = 7;
const byte REPAS = 8;
// Le dessin des caractères HEAD TAIL CLEAR (https://maxpromer.github.io/LCD-Character-Creator/)
uint8_t snakeHead[8] = {0x11, 0xa, 0x1f, 0x15, 0xa, 0x11, 0xe};
uint8_t snakeTail[8] = {0x4, 0xe, 0x1b, 0x1b, 0xe, 0xe, 0x4};
uint8_t noSnake[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
uint8_t tete[8] = {0x0, 0x0, 0x0, 0x1F, 0x15, 0x1F, 0x11, 0x1F};
uint8_t os[8] = {0x11, 0x0E, 0x0E, 0x11, 0x0, 0x0, 0x0, 0x0};
uint8_t eAigu[8] = {0x01, 0x02, 0x0, 0x0E, 0x11, 0x1F, 0x10, 0x0F};
uint8_t heart[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};
uint8_t bell[8]  = {0x4, 0xe, 0xe, 0xe, 0x1f, 0x0, 0x4};
// les variables de temps 
unsigned int Hnow, Hmatin, Hsoir, Hjour, Mjour, Hnuit, Mnuit;
// Compteur de la fonction "sec", "beep" et "terrarium"
unsigned int alarm, compteur, compteursonde = 0; 
// Pour le changement d'heure
byte UTCOffset;
// On déclare les variables pour gérer la boucle de chaque fonction du loop avec millis()
unsigned long tempsgestiontemps, tempssec, tempsterra, tempsanniv, tempsrepas;
// Pour la reconnexion au wifi, pour la fonction NTP, Pour le flag de la fonction qui est lancée par une interruption et pour l'etat des leds.
boolean wifiFirstConnected, syncEventTriggered, flag_bouton, ledState = false;
NTPSyncEvent_t ntpEvent;            // Dernier événement déclenché

// Le setup qui prépare le programme
void setup(void) {
  // On déclare un nom pour les 3 fonctions de gestion du wifi
  static WiFiEventHandler e1, e2, e3;
  // les 3 fonctions qui gèrent la connexion et déconnexion du wifi
  e1 = WiFi.onStationModeGotIP (onSTAGotIP);
  e2 = WiFi.onStationModeDisconnected (onSTADisconnected);
  e3 = WiFi.onStationModeConnected (onSTAConnected);     
  Serial.begin(115200);    
  Serial.print("Connecting ");
  Serial.print(ssid);  
  WiFi.begin(ssid, password);
  Serial.println("");
  int h = 0;    
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");    
    if (h++ > 40) { // si trop long on abandonne
      Serial.println();
      Serial.println("Failed to connect");
      return;
    }    
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  if (mdns.begin(host, WiFi.localIP())) {
    Serial.println("MDNS démarré");
  }  
  httpUpdater.setup(&Webserveur, update_path, update_username, update_password);  
  Serial.printf("Ouvre http://%s.local dans ton navigateur pour me commander\n", host);     
  Webserveur.on("/", handle_root);
  Webserveur.on("/Star", handle_Star);
  Webserveur.on("/Happy", handle_Happy);
  Webserveur.on("/Harry", handle_Harry);
  Webserveur.on("/Game", handle_Game); 
  Webserveur.on("/Pira", handle_Pirate);
  Webserveur.on("/Mario", handle_Mario);
  Webserveur.on("/McGy", handle_McGyver);
  Webserveur.on("/India", handle_India);
  Webserveur.on("/SW2", handle_SW2);
  Webserveur.on("/BonB", handle_BonB);
  Webserveur.on("/Enter", handle_Enter);
  Webserveur.on("/twenty", handle_twenty);
  Webserveur.on("/looney", handle_looney);
  Webserveur.on("/Barbie", handle_Barbie);
  Webserveur.on("/Green", handle_Green);
  Webserveur.on("/Bond", handle_Bond);  
  Webserveur.begin(); // demarrage du serveur 
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
  httpUpdater.setup(&Webserveur, update_path, update_username, update_password);
  // On démarre le serveur
  Webserveur.begin();
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
  // les LED , après la déclaration des LED en sortie, on aura plus rien sur le port série
  // car elles sont branché sur le TX RX du NodeMCU
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);
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
  lcd.createChar(COEUR, heart);
  lcd.createChar(REPAS, bell);
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
  tempsgestiontemps, tempssec, tempsterra = millis(); 
}

// le loop qui lance les fonctions en boucle
void loop(void){
  Webserveur.handleClient();
  sec(); 
  gestiontemps();
  bouton();  
  terrarium();
  evenement();  
}

          ///// Les fonctions \\\\\

void sec() {  
  if((millis() - tempssec) > 2000) {
    secheresse = digitalRead(PinHumi);    // on lit la valeur du détecteur
    if (secheresse) {                     // si il est à 1 c'est qu'il n'y a plus d'eau    
      // pour éviter de lancer l'alarme sur une fausse détection   
      alarm++;                            // on incrémente le compteur  
      if (alarm > 2) {
        eau();                            // on lance la fonction eau
        alarm = 0;
      }          
    } else {
      alarm = 0;
    }
  tempssec = millis();
  }
}

void eau() {
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
  Hnow = hour() * 100 + minute();        // Heure actuelle      
  if ( Hnow > 700 && Hnow < 2200 ) {     // Si c'est le jour      
    Starwars();                          // On lance l'alarme
  } else {
    delay(1500);        
  }
  lcd.clear();
  lcd.noBacklight();
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

// La fonction qui s'occupe du chauffage et de la lumière
void terrarium() {  
  byte target;
  // Si le temps actuel par rapport au temps de démarrage du timer est > 7s ( toutes les 7s )
  if((millis() - tempsterra) > 11000) {  
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
    // On lis les sondes
    hC = dhtPC.readHumidity();  
    tC = dhtPC.readTemperature();    
    // Lire la sonde point froid
    hF = dhtPF.readHumidity();  
    tF = dhtPF.readTemperature();         
    if (tC > target + 0.25 || isnan(tC)) {
      digitalWrite(chauff, HIGH); 
    } 
    if (tC < target - 0.25) { 
      digitalWrite(chauff, LOW); 
    }    
    if ( isnan(tC) || isnan(tF) ) {
      return;
    }
    // Création de la connexion TCP
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(serveur, httpPort)) {
      Serial.println("échec de la connexion");
      return;
    }
    // Envoi de toutes ces datas par l'URL et par GET au fichier sonde.php sur le serveur (dans le dossier terranodemcu)
    client.print(String("GET /terranodemcu/sondes.php?tempC=") + String(float (tC)) + "&humiC=" + String(float(hC)) + "&tempF=" + String(float(tF)) + "&humiF=" + String(float(hF)) + 
                 "&Hjour=" + String(int(Hjour)) + "&Mjour=" + String(int(Mjour)) + "&Hnuit=" + String(int(Hnuit)) + "&Mnuit=" + String(int(Mnuit)) +
                 " HTTP/1.1\r\n" +  "Host: " + serveur + "\r\n" + "Connection: close\r\n\r\n");       
    while(client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
    tempsterra = millis();          // On réinitialise le timer 
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
      printRiseAndSet(ville, lat, lon, UTCOffset, day(),month(),year());
  } else {        
    if (ntpEvent == noResponse) {           
      NTP.getTimeDateString (NTP.getLastNTPSync ()); 
    }                 
    else if (ntpEvent == invalidAddress) {       
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
    Serial.print (NTP.getTimeDateString ()); Serial.print (" ");
    Serial.print (NTP.isSummerTime () ? "Summer Time. " : "Winter Time. ");
    Serial.print ("WiFi is ");
    Serial.print (WiFi.isConnected () ? "connected" : "not connected"); Serial.print (". ");
    Serial.print ("Uptime: ");
    Serial.print (NTP.getUptimeString ()); Serial.print (" since ");
    Serial.println (NTP.getTimeDateString (NTP.getFirstSync ()).c_str ());
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

void evenement()
{
  int Hnow = hour() * 100 + minute();
  int DMnow = day() * 100 + month();  
  int Dnow = day();    
  for (int i = 0; i < (sizeof(dateanniv)/sizeof(int)); i++) {  
     if (DMnow == dateanniv[i]) {      
       for (int j = 0; j < (sizeof(heureanniv)/sizeof(int)); j++) {             
           if (Hnow == heureanniv[j]) {                 
              if((millis() - tempsanniv) > 10000) {   
                lcd.backlight();
                lcd.setCursor(1,0);
                lcd.print("happy birthday");
                lcd.setCursor(0,1);
                lcd.write(COEUR);
                lcd.write(COEUR);
                lcd.write(COEUR);
                lcd.setCursor(5,1);
                lcd.print(noms[i]); 
                lcd.setCursor(13,1); 
                lcd.write(COEUR);
                lcd.write(COEUR);
                lcd.write(COEUR);                          
                HappyBirthday();                
                lcd.clear();
                lcd.noBacklight();                                              
                tempsanniv = millis();         
              }              
           }                     
        }              
     }
  }  
  for (int i = 0; i < (sizeof(daterepas)/sizeof(int)); i++) {             
     if (daterepas[i] == Dnow) {                     
       for (int j = 0; j < (sizeof(heurerepas)/sizeof(int)); j++) {                       
           if (Hnow == heurerepas[j]) {                   
              if((millis() - tempsrepas) > 20000) {                 
                lcd.backlight();
                lcd.setCursor(1,0);
                lcd.print("c'est l'heure");
                lcd.setCursor(0,1);
                lcd.write(REPAS);
                lcd.write(REPAS);                
                lcd.setCursor(4,1);
                lcd.print("du repas"); 
                lcd.setCursor(14,1); 
                lcd.write(REPAS);
                lcd.write(REPAS);                                          
                GameOfThrones();                
                lcd.clear();
                lcd.noBacklight();               
                              tempsrepas = millis();         
              }              
           }                     
        }  
     }      
  }
}

void handle_root() {
  char page[] = "<body style='background:#7F7F7F;'>"               
  "<center><h2 style='color:#EC7878;'>La fonction Update</h2></center><br/>"
  "<center><p><a href=\"update\"><button style='background:#EC7878;border:2px solid black;font-size: 20px;padding: 20px 20px;border-radius: 25px;'>update du programme</button></a></p></center><br/>"  
  "<center><h2>Commande des chansons</h2></center><br/>" 
  "<center><div>"
  "<p><a href=\"Star\"><button>Starwars</button></a> &nbsp; &nbsp; <a href=\"Happy\"><button>Happy Birthday</button></a> &nbsp; &nbsp;"  
  "<a href=\"Pira\"><button>Pirate des caraibes</button></a> &nbsp; &nbsp; <a href=\"Mario\"><button>Mario</button></a> &nbsp; &nbsp;"
  "<a href=\"McGy\"><button>McGyver</button></a> &nbsp; &nbsp; <a href=\"India\"><button>Indiana Jones</button></a> &nbsp; &nbsp;"
  "<a href=\"SW2\"><button>Starwars2</button></a></p>"  
  "<p><a href=\"BonB\"><button>Bon Brute Truand</button></a> &nbsp; &nbsp; <a href=\"Enter\"><button>Entertainement</button></a> &nbsp; &nbsp;"
  "<a href=\"looney\"><button>looney toons</button></a> &nbsp; &nbsp; <a href=\"Barbie\"><button>Barbie Girl</button></a> &nbsp; &nbsp;"
  "<a href=\"Green\"><button>Greensleeves</button></a> &nbsp; &nbsp; <a href=\"Bond\"><button>James Bond</button></a> &nbsp; &nbsp;"
  "<a href=\"twenty\"><button>twentyCentFox</button></a></p>"
  "</center></div>"  
  "</body>";                
  Webserveur.send(200, "text/html", page); // repond avec la page web codee en HTML
}

void handle_Star() {     
  char page[] = "<body style='background:#7F7F7F;'>"
                "<div style='margin-top:10%;'>"
                "<center><h1>Starwars</h1></center><br/>"
                "<center><p><a href=\"/\"><button>Home</button></a></p></center>"
                "</div></body>";
  Webserveur.send(200, "text/html", page); // repond avec la page web codee en HTML
  Starwars();  
}

void handle_Happy() {    
  char page[] = "<body style='background:#7F7F7F;'>"
                "<div style='margin-top:10%;'>"
                "<center><h1>Happy Birthday</h1></center><br/>"
                "<center><p><a href=\"/\"><button>Home</button></a></p></center>"
                "</div></body>";
  Webserveur.send(200, "text/html", page); // repond avec la page web codee en HTML
  HappyBirthday();
}

void handle_Game() {    
  char page[] = "<body style='background:#7F7F7F;'>"
                "<div style='margin-top:10%;'>"
                "<center><h1>Game Of Thrones</h1></center><br/>"
                "<center><p><a href=\"/\"><button>Home</button></a></p></center>"
                "</div></body>";
  Webserveur.send(200, "text/html", page); // repond avec la page web codee en HTML
  GameOfThrones();
}

void handle_Harry() {    
  char page[] = "<body style='background:#7F7F7F;'>"
                "<div style='margin-top:10%;'>"
                "<center><h1>Harry Potter</h1></center><br/>"
                "<center><p><a href=\"/\"><button>Home</button></a></p></center>"
                "</div></body>";
  Webserveur.send(200, "text/html", page); // repond avec la page web codee en HTML
  HarryPotter();
}

void handle_Pirate() {    
  char page[] = "<body style='background:#7F7F7F;'>"
                "<div style='margin-top:10%;'>"
                "<center><h1>Pirate des caraïbes</h1></center><br/>"
                "<center><p><a href=\"/\"><button>Home</button></a></p></center>"
                "</div></body>";
  Webserveur.send(200, "text/html", page); // repond avec la page web codee en HTML
  Pirate();
}

void handle_Mario() {    
  char page[] = "<body style='background:#7F7F7F;'>"
                "<div style='margin-top:10%;'>"
                "<center><h1>Mario</h1></center><br/>"
                "<center><p><a href=\"/\"><button>Home</button></a></p></center>"
                "</div></body>";
  Webserveur.send(200, "text/html", page); // repond avec la page web codee en HTML
  mario();
}

void handle_McGyver() {    
  char page[] = "<body style='background:#7F7F7F;'>"
                "<div style='margin-top:10%;'>"
                "<center><h1>McGyver</h1></center><br/>"
                "<center><p><a href=\"/\"><button>Home</button></a></p></center>"
                "</div></body>";
  Webserveur.send(200, "text/html", page); // repond avec la page web codee en HTML
  McGyver();
}

void handle_India() {    
  char page[] = "<body style='background:#7F7F7F;'>"
                "<div style='margin-top:10%;'>"
                "<center><h1>Indiana Jones</h1></center><br/>"
                "<center><p><a href=\"/\"><button>Home</button></a></p></center>"
                "</div></body>";
  Webserveur.send(200, "text/html", page); // repond avec la page web codee en HTML
  IndianaJones();
}

void handle_SW2() {    
  char page[] = "<body style='background:#7F7F7F;'>"
                "<div style='margin-top:10%;'>"
                "<center><h1>StarWars2</h1></center><br/>"
                "<center><p><a href=\"/\"><button>Home</button></a></p></center>"
                "</div></body>";
  Webserveur.send(200, "text/html", page); // repond avec la page web codee en HTML
  StarWars2();
}

void handle_BonB() {    
  char page[] = "<body style='background:#7F7F7F;'>"
                "<div style='margin-top:10%;'>"
                "<center><h1>Bon Brute Truand</h1></center><br/>"
                "<center><p><a href=\"/\"><button>Home</button></a></p></center>"
                "</div></body>";
  Webserveur.send(200, "text/html", page); // repond avec la page web codee en HTML
  BonBruteTruand();
}

void handle_Enter() {    
  char page[] = "<body style='background:#7F7F7F;'>"
                "<div style='margin-top:10%;'>"
                "<center><h1>Entertainement</h1></center><br/>"
                "<center><p><a href=\"/\"><button>Home</button></a></p></center>"
                "</div></body>";
  Webserveur.send(200, "text/html", page); // repond avec la page web codee en HTML
  Entertainement();
}

void handle_twenty() {    
  char page[] = "<body style='background:#7F7F7F;'>"
                "<div style='margin-top:10%;'>"
                "<center><h1>twentyCentFox</h1></center><br/>"
                "<center><p><a href=\"/\"><button>Home</button></a></p></center>"
                "</div></body>";
  Webserveur.send(200, "text/html", page); // repond avec la page web codee en HTML
  twentyCentFox();
}

void handle_looney() {    
  char page[] = "<body style='background:#7F7F7F;'>"
                "<div style='margin-top:10%;'>"
                "<center><h1>looney toons</h1></center><br/>"
                "<center><p><a href=\"/\"><button>Home</button></a></p></center>"
                "</div></body>";
  Webserveur.send(200, "text/html", page); // repond avec la page web codee en HTML
  looney();
}

void handle_Barbie() {    
  char page[] = "<body style='background:#7F7F7F;'>"
                "<div style='margin-top:10%;'>"
                "<center><h1>Barbie Girl</h1></center><br/>"
                "<center><p><a href=\"/\"><button>Home</button></a></p></center>"
                "</div></body>";
  Webserveur.send(200, "text/html", page); // repond avec la page web codee en HTML
  BarbieGirl();
}

void handle_Green() {    
  char page[] = "<body style='background:#7F7F7F;'>"
                "<div style='margin-top:10%;'>"
                "<center><h1>Greensleaves</h1></center><br/>"
                "<center><p><a href=\"/\"><button>Home</button></a></p></center>"
                "</div></body>";
  Webserveur.send(200, "text/html", page); // repond avec la page web codee en HTML
  Greensleaves();
}

void handle_Bond() {    
  char page[] = "<body style='background:#7F7F7F;'>"
                "<div style='margin-top:10%;'>"
                "<center><h1>James Bond</h1></center><br/>"
                "<center><p><a href=\"/\"><button>Home</button></a></p></center>"
                "</div></body>";
  Webserveur.send(200, "text/html", page); // repond avec la page web codee en HTML
  Bond();
}


////////////////////////// La fonction beep ////////////////////////

void beep(int note, int duree)
{
  // Play buzzerPin
  tone(buzzerPin, note, duree);
 
  // choix des leds à allumer en fonction de la valeur de 'compteur' et de la durée de la note
  if(compteur % 2 == 0)
  {    
    digitalWrite(ledPin1, !ledState);    
    delay(duree);
    digitalWrite(ledPin1, ledState);    
  } 
  else
  {    
    digitalWrite(ledPin2, !ledState);      
    delay(duree);
    digitalWrite(ledPin2, ledState);   
  } 
  // Stop buzzerPin
  noTone(buzzerPin); 
  delay(50); 
  // On increment le compteur
  compteur++;
}

////////////////////////// Les chansons ////////////////////////

//******* Starwars coté obscure**************** 
void firstSection()
{
  beep(NOTE_A4, 500);
  beep(NOTE_A4, 500);    
  beep(NOTE_A4, 500);
  beep(NOTE_F4, 350);
  beep(NOTE_C5, 150);  
  beep(NOTE_A4, 500);
  beep(NOTE_F4, 350);
  beep(NOTE_C5, 150);
  beep(NOTE_A4, 650); 
  delay(500); 
  beep(NOTE_E5, 500);
  beep(NOTE_E5, 500);
  beep(NOTE_E5, 500);  
  beep(NOTE_F5, 350);
  beep(NOTE_C5, 150);
  beep(NOTE_GS4, 500);
  beep(NOTE_F4, 350);
  beep(NOTE_C5, 150);
  beep(NOTE_A4, 650); 
  delay(500);
}
 
void secondSection()
{
  beep(NOTE_A5, 500);
  beep(NOTE_A4, 300);
  beep(NOTE_A4, 150);
  beep(NOTE_A5, 500);
  beep(NOTE_GSH, 325);
  beep(NOTE_G5, 175);
  beep(NOTE_FS5, 125);
  beep(NOTE_F5, 125);    
  beep(NOTE_FS5, 250); 
  delay(325); 
  beep(NOTE_AS, 250);
  beep(NOTE_DS5, 500);
  beep(NOTE_D5, 325);  
  beep(NOTE_CS5, 175);  
  beep(NOTE_C5, 125);  
  beep(NOTE_AS4, 125);  
  beep(NOTE_C5, 250);   
  delay(350);
}

void Starwars() 
{    
  firstSection();  
  secondSection(); 
  //Variant 1
  beep(NOTE_F4, 250);  
  beep(NOTE_GS4, 500);  
  beep(NOTE_F4, 350);  
  beep(NOTE_A4, 125);
  beep(NOTE_C5, 500);
  beep(NOTE_A4, 375);  
  beep(NOTE_C5, 125);
  beep(NOTE_E5, 650); 
  delay(500);  
  secondSection(); 
  //Variant 2
  beep(NOTE_F4, 250);  
  beep(NOTE_GS4, 500);  
  beep(NOTE_F4, 375);  
  beep(NOTE_C5, 125);
  beep(NOTE_A4, 500);  
  beep(NOTE_F4, 375);  
  beep(NOTE_C5, 125);
  beep(NOTE_A4, 650); 
  delay(650);
  compteur = 0;
}

//******* GameOfThrones ****************
void GameOfThrones()
  {
    for(int i=0; i<3; i++)
    {
    beep(NOTE_G4, 500);     
    beep(NOTE_C4, 500);    
    beep(NOTE_DS4, 250);    
    beep(NOTE_F4, 250);    
    }
    for(int i=0; i<3; i++)
    {
    beep(NOTE_G4, 500);    
    beep(NOTE_C4, 500);    
    beep(NOTE_E4, 250);    
    beep(NOTE_F4, 250);    
    }
        beep(NOTE_G4, 500);
        beep(NOTE_C4, 500);        
        beep(NOTE_DS4, 250);
        beep(NOTE_F4, 250);
        beep(NOTE_D4, 500);        
    for(int i=0; i<2; i++)
    {
    beep(NOTE_G3, 500);
    beep(NOTE_AS3, 250);
    beep(NOTE_C4, 250);
    beep(NOTE_D4, 500);    
    }//
        beep(NOTE_G3, 500);
        beep(NOTE_AS3, 250);
        beep(NOTE_C4, 250);
        beep(NOTE_D4, 1000);
        beep(NOTE_F4, 1000);
        beep(NOTE_AS3, 1000);
        beep(NOTE_DS4, 250);
        beep(NOTE_D4, 250);
        beep(NOTE_F4, 1000);
        beep(NOTE_AS3, 1000);
        beep(NOTE_DS4, 250);
        beep(NOTE_D4, 250);
        beep(NOTE_C4, 500);
    for(int i=0; i<2; i++)
    {
    beep(NOTE_GS3, 250);
    beep(NOTE_AS3, 250);
    beep(NOTE_C4, 500);
    beep(NOTE_F3, 500);    
    }
          beep(NOTE_G4, 1000);
          beep(NOTE_C4, 1000);
          beep(NOTE_DS4, 250);
          beep(NOTE_F4, 250);
          beep(NOTE_G4, 1000);
          beep(NOTE_C4, 1000);
          beep(NOTE_DS4, 250);
          beep(NOTE_F4, 250);
          beep(NOTE_D4, 500);          
    for(int i=0; i<3; i++)
    {
    beep(NOTE_G3, 500);
    beep(NOTE_AS3, 250);
    beep(NOTE_C4, 250);
    beep(NOTE_D4, 500);
    }
  compteur = 0;
}


//******* Happy Birthday ****************
void HappyBirthday()
{
  beep(NOTE_G3, 200);
  beep(NOTE_G3, 200);
  beep(NOTE_A3, 500);
  beep(NOTE_G3, 500);
  beep(NOTE_C4, 500);
  beep(NOTE_B3, 1000);
  beep(NOTE_G3, 200);
  beep(NOTE_G3, 200);
  beep(NOTE_A3, 500);
  beep(NOTE_G3, 500);
  beep(NOTE_D4, 500);
  beep(NOTE_C4, 1000);
  beep(NOTE_G3, 200);
  beep(NOTE_G3, 200);
  beep(NOTE_G4, 500);
  beep(NOTE_E4, 500);
  beep(NOTE_C4, 500);
  beep(NOTE_B3, 500);
  beep(NOTE_A3, 750);
  beep(NOTE_F4, 200);
  beep(NOTE_F4, 200);
  beep(NOTE_E4, 500);
  beep(NOTE_C4, 500);
  beep(NOTE_D4, 500);
  beep(NOTE_C4, 1000); 
  compteur = 0;
}


//******* Harry Potter ****************
void HarryPotter()
{
  beep(NOTE_B4, 333);  
  beep(NOTE_E5, 500);   
  beep(NOTE_G5, 166);    
  beep(NOTE_FS5, 333);    
  beep(NOTE_E5, 666);
  beep(NOTE_B5, 333);   
  beep(NOTE_A5, 1000);    
  beep(NOTE_FS5, 1000);    
  beep(NOTE_E5, 500);   
  beep(NOTE_G5, 166);    
  beep(NOTE_FS5, 333);    
  beep(NOTE_DS5, 666);    
  beep(NOTE_F5, 333);    
  beep(NOTE_B4, 1666);    
  beep(NOTE_B4, 333);    
  beep(NOTE_E5, 500);    
  beep(NOTE_G5, 166);    
  beep(NOTE_FS5, 333);    
  beep(NOTE_E5, 666);    
  beep(NOTE_B5, 333);    
  beep(NOTE_D6, 666);    
  beep(NOTE_CS6, 333);    
  beep(NOTE_C6, 666);    
  beep(NOTE_GS5, 333);    
  beep(NOTE_C6, 500);    
  beep(NOTE_B5, 166);    
  beep(NOTE_AS5, 333); 
  beep(NOTE_AS4, 666);    
  beep(NOTE_G5, 333);    
  beep(NOTE_E5, 1666);    
  beep(NOTE_G5, 333);    
  beep(NOTE_B5, 666);    
  beep(NOTE_G5, 333);    
  beep(NOTE_B5, 666);    
  beep(NOTE_G5, 333);    
  beep(NOTE_C6, 666);    
  beep(NOTE_B5, 333);    
  beep(NOTE_AS5, 666);    
  beep(NOTE_FS5, 333);    
  beep(NOTE_G5, 500);    
  beep(NOTE_B5, 166);    
  beep(NOTE_AS5, 333);    
  beep(NOTE_AS4, 666);    
  beep(NOTE_B4, 333);    
  beep(NOTE_B5, 1666);    
  beep(NOTE_G5, 333);    
  beep(NOTE_B5, 666);    
  beep(NOTE_G5, 333);    
  beep(NOTE_B5, 666);    
  beep(NOTE_G5, 333);    
  beep(NOTE_D6, 666);    
  beep(NOTE_CS6, 333);    
  beep(NOTE_C6, 666);    
  beep(NOTE_GS5, 333);    
  beep(NOTE_C6, 500);    
  beep(NOTE_B5, 166);    
  beep(NOTE_AS5, 333);    
  beep(NOTE_AS4, 666);    
  beep(NOTE_G5, 333);    
  beep(NOTE_E5, 1666);    
  compteur = 0;
}

//******* Pirate des caraïbes ****************
void Pirate()
{  
  beep(NOTE_E4, 125);
  beep(NOTE_G4, 125);
  beep(NOTE_A4, 250);
  beep(NOTE_A4, 125);
  delay(50); 
  beep(NOTE_A4, 125);
  beep(NOTE_B4, 125); 
  beep(NOTE_C5, 250); 
  beep(NOTE_C5, 125);
  delay(50);
  beep(NOTE_C5, 125);
  beep(NOTE_D5, 125);
  beep(NOTE_B4, 250);
  beep(NOTE_B4, 125);
  delay(50);
  beep(NOTE_A4, 125);
  beep(NOTE_G4, 125);
  beep(NOTE_A4, 375);
  delay(50);  
  beep(NOTE_E4, 125);
  beep(NOTE_G4, 125);
  beep(NOTE_A4, 250);
  beep(NOTE_A4, 125);
  delay(50);
  beep(NOTE_A4, 125);
  beep(NOTE_B4, 125);
  beep(NOTE_C5, 250);
  beep(NOTE_C5, 125);
  delay(50);
  beep(NOTE_C5,125); 
  beep(NOTE_D5, 125);
  beep(NOTE_B4, 250);
  beep(NOTE_B4, 125);
  delay(50);
  beep(NOTE_A4, 125); 
  beep(NOTE_G4, 125);
  beep(NOTE_A4, 375);
  delay(50);    
  beep(NOTE_E4, 125);
  beep(NOTE_G4, 125);
  beep(NOTE_A4, 250);
  beep(NOTE_A4, 125);
  delay(50);
  beep(NOTE_A4, 125);
  beep(NOTE_C5, 125);
  beep(NOTE_D5, 250);
  beep(NOTE_D5, 125);
  delay(50);
  beep(NOTE_D5, 125); 
  beep(NOTE_E5, 125);
  beep(NOTE_F5,250);
  beep(NOTE_F5, 125);
  delay(50);
  beep(NOTE_E5, 125); 
  beep(NOTE_D5, 125);
  beep(NOTE_E5, 125);
  beep(NOTE_A4, 250);
  delay(50);  
  beep(NOTE_A4,125); 
  beep(NOTE_B4, 125);
  beep(NOTE_C5, 250);
  beep(NOTE_C5, 125);
  delay(50);
  beep(NOTE_D5, 250);
  beep(NOTE_E5, 125);
  beep(NOTE_A4, 250);
  delay(50);
  beep(NOTE_A4, 125);
  beep(NOTE_C5, 125);
  beep(NOTE_B4, 250);
  beep(NOTE_B4, 125);
  delay(50);
  beep(NOTE_C5, 125);
  beep(NOTE_A4, 125);
  beep(NOTE_B4, 375);
  delay(200);
  beep(NOTE_A4, 250); 
  beep(NOTE_A4, 125);     
  beep(NOTE_E4, 125);
  beep(NOTE_G4, 125);
  beep(NOTE_A4, 250);
  beep(NOTE_A4, 125);
  delay(50); 
  beep(NOTE_A4, 125);
  beep(NOTE_B4, 125); 
  beep(NOTE_C5, 250); 
  beep(NOTE_C5, 125);
  delay(50);
  beep(NOTE_C5, 125);
  beep(NOTE_D5, 125);
  beep(NOTE_B4, 250);
  beep(NOTE_B4, 125);
  delay(50);
  beep(NOTE_A4, 125);
  beep(NOTE_G4, 125);
  beep(NOTE_A4, 375);
  delay(50);  
  beep(NOTE_E4, 125);
  beep(NOTE_G4, 125);
  beep(NOTE_A4, 250);
  beep(NOTE_A4, 125);
  delay(50);
  beep(NOTE_A4, 125);
  beep(NOTE_B4, 125);
  beep(NOTE_C5, 250);
  beep(NOTE_C5, 125);
  delay(50);
  beep(NOTE_C5,125); 
  beep(NOTE_D5, 125);
  beep(NOTE_B4, 250);
  beep(NOTE_B4, 125);
  delay(50);
  beep(NOTE_A4, 125); 
  beep(NOTE_G4, 125);
  beep(NOTE_A4, 375);
  delay(50);    
  beep(NOTE_E4, 125);
  beep(NOTE_G4, 125);
  beep(NOTE_A4, 250);
  beep(NOTE_A4, 125);
  delay(50);
  beep(NOTE_A4, 125);
  beep(NOTE_C5, 125);
  beep(NOTE_D5, 250);
  beep(NOTE_D5, 125);
  delay(50);
  beep(NOTE_D5, 125); 
  beep(NOTE_E5, 125);
  beep(NOTE_F5, 250);
  beep(NOTE_F5, 125);
  delay(50);
  beep(NOTE_E5, 125); 
  beep(NOTE_D5, 125);
  beep(NOTE_E5, 125);
  beep(NOTE_A4, 250);
  delay(50);  
  beep(NOTE_A4,125); 
  beep(NOTE_B4, 125);
  beep(NOTE_C5, 250);
  beep(NOTE_C5, 125);
  delay(50);
  beep(NOTE_D5, 250);
  beep(NOTE_E5, 125);
  beep(NOTE_A4, 250);
  delay(50);
  beep(NOTE_A4, 125);
  beep(NOTE_C5, 125);
  beep(NOTE_B4, 250);
  beep(NOTE_B4, 125);
  delay(50);
  beep(NOTE_C5, 125);
  beep(NOTE_A4, 125);
  beep(NOTE_B4, 375);
  delay(200);
  beep(NOTE_E5, 250);
  delay(400);
  beep(NOTE_F5, 250);
  delay(400); 
  beep(NOTE_E5, 125);
  beep(NOTE_E5, 125);
  delay(50);
  beep(NOTE_G5, 125);
  delay(50);
  beep(NOTE_E5, 125);  
  beep(NOTE_D5, 125);
  delay(400);    
  beep(NOTE_D5, 250);
  delay(400);
  beep(NOTE_C5, 250);
  delay(400);  
  beep(NOTE_B4, 125);  
  beep(NOTE_C5, 125);
  delay(50);
  beep(NOTE_B4, 125);
  delay(50);
  beep(NOTE_A4,  500);    
  beep(NOTE_E5, 250);  
  delay(400);
  beep(NOTE_F5, 250);
  delay(400);    
  beep(NOTE_E5, 125);
  beep(NOTE_E5, 125);
  delay(50);
  beep(NOTE_G5, 125);
  delay(50);
  beep(NOTE_E5, 125);
  beep(NOTE_D5, 125);
  delay(400);  
  beep(NOTE_D5, 250);
  delay(400);
  beep(NOTE_C5, 250);
  delay(400);  
  beep(NOTE_B4, 125);
  beep(NOTE_C5, 125);
  delay(50);
  beep(NOTE_B4, 125);
  delay(50);
  beep(NOTE_A4, 500);
  compteur = 0;
}


//******* Mario ****************
void mario()
{
  beep(NOTE_E7, 120);
  beep(NOTE_E7, 120);
  delay(120);  
  beep(NOTE_E7, 120);
  delay(120);  
  beep(NOTE_C7, 120);
  beep(NOTE_E7, 120);
  delay(120);  
  beep(NOTE_G7, 120);
  delay(240);    
  beep(NOTE_G6, 120);
  delay(360); 
  beep(NOTE_C7, 120);
  delay(240); 
  beep(NOTE_G6, 120);
  delay(240);  
  beep(NOTE_E6, 120);
  delay(240);    
  beep(NOTE_A6, 120);
  delay(120);  
  beep(NOTE_B6, 120);
  delay(120);  
  beep(NOTE_AS6, 120);
  beep(NOTE_A6, 120);
  delay(120); 
  beep(NOTE_G6, 90);
  beep(NOTE_E7, 90);
  beep(NOTE_G7, 90);
  beep(NOTE_A7, 120);
  delay(120);  
  beep(NOTE_F7, 120);
  beep(NOTE_G7, 120);
  delay(120);  
  beep(NOTE_E7, 120);
  delay(120);  
  beep(NOTE_C7, 120);
  beep(NOTE_D7, 120);
  beep(NOTE_B6, 120);
  delay(240);  
  beep(NOTE_C7, 120);
  delay(240);  
  beep(NOTE_G6, 120);
  delay(240);    
  beep(NOTE_E6, 120);
  delay(240);     
  beep(NOTE_A6, 120);
  delay(120);  
  beep(NOTE_B6, 120);
  delay(120);  
  beep(NOTE_AS6, 120);
  beep(NOTE_A6, 120);
  delay(120); 
  beep(NOTE_G6, 90);
  beep(NOTE_E7, 90);
  beep(NOTE_G7, 90);
  beep(NOTE_A7, 120);
  delay(120);  
  beep(NOTE_F7, 120);
  beep(NOTE_G7, 120);
  delay(120);  
  beep(NOTE_E7, 120);
  delay(120);  
  beep(NOTE_C7, 120);
  beep(NOTE_D7, 120);
  beep(NOTE_B6, 120);
  delay(240);  
  beep(NOTE_C4, 120);
  beep(NOTE_C5, 120);
  beep(NOTE_A3, 120);
  beep(NOTE_A4, 120);
  beep(NOTE_AS3, 120);
  beep(NOTE_AS4, 120);
  delay(90);    
  beep(NOTE_C4, 120);
  beep(NOTE_C5, 120);
  beep(NOTE_A3, 120);
  beep(NOTE_A4, 120);
  beep(NOTE_AS3, 120);
  beep(NOTE_AS4, 120);
  delay(90);    
  beep(NOTE_F3, 120);
  beep(NOTE_F4, 120);
  beep(NOTE_D3, 120);
  beep(NOTE_D4, 120);
  beep(NOTE_DS3, 120);
  beep(NOTE_DS4, 120);
  delay(90);   
  beep(NOTE_F3, 120);
  beep(NOTE_F4, 120);
  beep(NOTE_D3, 120);
  beep(NOTE_D4, 120);
  beep(NOTE_DS3, 120);
  beep(NOTE_DS4, 120);
  delay(120);    
  beep(NOTE_DS4, 180);
  beep(NOTE_CS4, 180);
  beep(NOTE_D4, 180);
  beep(NOTE_CS4, 60);
  beep(NOTE_DS4, 60);
  beep(NOTE_DS4, 60);
  beep(NOTE_GS3, 60);
  beep(NOTE_G3, 60);
  beep(NOTE_CS4, 60);
  beep(NOTE_C4, 180);
  beep(NOTE_FS4, 180);
  beep(NOTE_F4, 180);
  beep(NOTE_E3, 180);
  beep(NOTE_AS4, 180);
  beep(NOTE_A4, 180);
  beep(NOTE_GS4, 100);
  beep(NOTE_DS4, 100);
  beep(NOTE_B3, 100);
  beep(NOTE_AS3, 100);
  beep(NOTE_A3, 100);
  beep(NOTE_GS3, 100);
  delay(90);  
  compteur = 0;
}

//******* McGyver ****************
void McGyver()
{
  beep(NOTE_B4, 200);
  beep(NOTE_E5, 200);
  beep(NOTE_A5, 200);
  beep(NOTE_B5, 200);
  beep(NOTE_A5, 400);
  beep(NOTE_E5, 200);
  beep(NOTE_B4, 200);
  delay(200);
  beep(NOTE_E5, 200);
  beep(NOTE_A5, 200);
  beep(NOTE_B5, 200);
  beep(NOTE_A5, 200);
  beep(NOTE_E5, 200);
  beep(NOTE_B4, 400);
  delay(200);
  beep(NOTE_E5, 200);
  beep(NOTE_A5, 200);
  beep(NOTE_B5, 200);
  beep(NOTE_A5, 400);
  beep(NOTE_E5, 200);
  beep(NOTE_B4, 200);
  delay(200);
  beep(NOTE_A5, 200);
  beep(NOTE_D6, 200);
  beep(NOTE_C6, 200);
  beep(NOTE_D6, 200);
  beep(NOTE_C6, 200);
  beep(NOTE_B5, 200);
  beep(NOTE_A5, 200);
  beep(NOTE_B4, 200);
  beep(NOTE_E5, 200);
  beep(NOTE_A5, 200);
  beep(NOTE_B5, 200);
  beep(NOTE_A5, 400);
  beep(NOTE_E5, 200);
  beep(NOTE_B4, 200);
  delay(200);
  beep(NOTE_E5, 200);
  beep(NOTE_A5, 200);
  beep(NOTE_B5, 200);
  beep(NOTE_A5, 200);
  beep(NOTE_E5, 200);
  beep(NOTE_B4, 400);
  delay(200);
  beep(NOTE_E5, 200);
  beep(NOTE_A5, 200);
  beep(NOTE_B5, 200);
  beep(NOTE_A5, 400);
  beep(NOTE_E5, 200);
  beep(NOTE_B4, 200);
  delay(200);
  beep(NOTE_A5, 200);
  beep(NOTE_D6, 200);
  beep(NOTE_C6, 200);
  beep(NOTE_D6, 200);
  beep(NOTE_C6, 200);
  beep(NOTE_B5, 200);
  beep(NOTE_A5, 200);
  beep(NOTE_B5, 400);
  delay(200);
  beep(NOTE_B5, 800);
  delay(200);
  beep(NOTE_B5, 400);
  delay(200);
  beep(NOTE_A5, 400);
  beep(NOTE_D6, 600);
  beep(NOTE_B5, 400);
  delay(200);
  beep(NOTE_B5, 800);
  delay(200);
  beep(NOTE_B5, 200);
  delay(200);
  beep(NOTE_A5, 800);
  delay(400);
  beep(NOTE_C6, 200);
  beep(NOTE_C6, 200);
  beep(NOTE_C6, 200);
  beep(NOTE_C6, 200);
  beep(NOTE_C6, 200);
  beep(NOTE_C6, 200);
  beep(NOTE_B5, 800);
  delay(100);
  beep(NOTE_FS5, 200);
  beep(NOTE_A5, 200);
  delay(200);
  beep(NOTE_G5, 800);
  delay(200);
  beep(NOTE_C6, 200);
  beep(NOTE_C6, 200);
  delay(200);
  beep(NOTE_B5, 400);
  beep(NOTE_A5, 200);
  beep(NOTE_B5, 200);
  beep(NOTE_A5, 200);
  beep(NOTE_G5, 200);
  delay(200);
  beep(NOTE_E6, 400);
  beep(NOTE_A5, 800);
  delay(100);
  beep(NOTE_C6, 200);
  beep(NOTE_C6, 200);
  delay(200);
  beep(NOTE_B5, 800);
  delay(200);
  beep(NOTE_FS5, 200);
  beep(NOTE_A5, 200);
  delay(200);
  beep(NOTE_G5, 800);
  delay(200);
  beep(NOTE_C6, 200);
  beep(NOTE_C6, 200);
  delay(200);
  beep(NOTE_B5, 400);
  beep(NOTE_A5, 200);
  beep(NOTE_B5, 200);
  beep(NOTE_A5, 200);
  beep(NOTE_G5, 200);
  delay(200);
  beep(NOTE_E6, 400);
  beep(NOTE_A5, 800);
  beep(NOTE_B5, 800);
  delay(50);
  beep(NOTE_C6, 200);
  beep(NOTE_B5, 200);
  beep(NOTE_A5, 200);
  beep(NOTE_C6, 400);
  beep(NOTE_B5, 200);
  beep(NOTE_A5, 200);
  beep(NOTE_D6, 200);
  beep(NOTE_C6, 200);
  beep(NOTE_B5, 200);
  beep(NOTE_D6, 400);
  beep(NOTE_C6, 200);
  beep(NOTE_B5, 200);
  beep(NOTE_E6, 400);
  beep(NOTE_D6, 200);
  beep(NOTE_E6, 200);
  beep(NOTE_FS6, 400);
  beep(NOTE_B5, 400);
  beep(NOTE_G6, 400);
  delay(200);
  beep(NOTE_FS6, 400);
  beep(NOTE_F6, 400);
  beep(NOTE_B5, 400);
  beep(NOTE_G6, 200);
  beep(NOTE_E6, 200);
  beep(NOTE_B5, 200);
  beep(NOTE_FS6, 200);
  beep(NOTE_D6, 200);
  beep(NOTE_A5, 200);
  beep(NOTE_E6, 200);
  beep(NOTE_C6, 200);
  beep(NOTE_G5, 200);
  beep(NOTE_D6, 200);
  beep(NOTE_B5, 200);
  beep(NOTE_G5, 200);
  beep(NOTE_C6, 200);
  beep(NOTE_E5, 200);
  beep(NOTE_B5, 200);
  beep(NOTE_D5, 200);
  beep(NOTE_C6, 200);
  beep(NOTE_B5, 200);
  beep(NOTE_A5, 200);
  beep(NOTE_G5, 200);
  beep(NOTE_AS5, 400);
  beep(NOTE_A5, 400);
  beep(NOTE_G6, 200);
  beep(NOTE_G5, 200);
  beep(NOTE_D6, 200);
  beep(NOTE_G5, 200);
  beep(NOTE_DS6, 200);
  beep(NOTE_DS5, 200);
  beep(NOTE_AS5, 200);
  beep(NOTE_A5, 200);
  beep(NOTE_G5, 200);
  beep(NOTE_G4, 200);
  beep(NOTE_D5, 200);
  beep(NOTE_G4, 200);
  beep(NOTE_DS5, 200);
  beep(NOTE_G4, 200);
  beep(NOTE_AS4, 200);
  beep(NOTE_A4, 200);
  beep(NOTE_G4, 200);
  beep(NOTE_G4, 200);
  beep(NOTE_G4, 200);
  beep(NOTE_G4, 200);
  beep(NOTE_G4, 200);
  beep(NOTE_G4, 200);
  beep(NOTE_G4, 200);
  delay(200);
  compteur = 0;
}


//******* IndianaJones ****************
void IndianaJones()
{
  beep(NOTE_E5, 240);
  delay(120);
  beep(NOTE_F5, 120);
  beep(NOTE_G5, 120);
  delay(120);
  beep(NOTE_C6, 960);
  delay(180);
  beep(NOTE_D5, 240);
  delay(120);
  beep(NOTE_E5, 120);
  beep(NOTE_F5, 960);
  delay(360);
  beep(NOTE_G5, 240);
  delay(120);
  beep(NOTE_A5, 120);
  beep(NOTE_B5, 120);
  delay(120);
  beep(NOTE_F6, 960);
  delay(240);
  beep(NOTE_A5, 240);
  delay(120);
  beep(NOTE_B5, 120);
  beep(NOTE_C6, 480);
  beep(NOTE_D6, 480);
  beep(NOTE_E6, 480);
  beep(NOTE_E5, 240);
  delay(120);
  beep(NOTE_F5, 120);
  beep(NOTE_G5, 120);
  delay(120);
  beep(NOTE_C6, 960);
  delay(240);
  beep(NOTE_D6, 240);
  delay(120);
  beep(NOTE_E6, 120);
  beep(NOTE_F6, 1440);
  beep(NOTE_G5, 240);
  delay(120);
  beep(NOTE_G5, 120);
  beep(NOTE_E6, 360);
  delay(120);
  beep(NOTE_D6, 240);
  delay(120);
  beep(NOTE_G5, 120);
  beep(NOTE_E6, 360);
  delay(120);
  beep(NOTE_D6, 240);
  delay(120);
  beep(NOTE_G5, 120);
  beep(NOTE_F6, 360);
  delay(120);
  beep(NOTE_E6, 240);
  delay(120);
  beep(NOTE_D6, 120);
  beep(NOTE_C6, 480);
  compteur = 0;
}


//******* StarWars coté force ****************
void StarWars2()
{
  beep(NOTE_F5, 166);
  beep(NOTE_F5, 166);
  beep(NOTE_F5, 166);
  beep(NOTE_AS5, 999);
  beep(NOTE_F6, 999);
  beep(NOTE_DS6, 166);
  beep(NOTE_D6, 166);
  beep(NOTE_C6, 166);
  beep(NOTE_AS6, 999);
  beep(NOTE_F6, 499);
  beep(NOTE_DS6, 166);
  beep(NOTE_D6, 166);
  beep(NOTE_C6, 166);
  beep(NOTE_AS6, 999);
  beep(NOTE_F6, 499);
  beep(NOTE_DS6, 166);
  beep(NOTE_D6, 166);
  beep(NOTE_DS6, 166);
  beep(NOTE_C6, 666);
  delay(333);
  beep(NOTE_F5, 166);
  beep(NOTE_F5, 166);
  beep(NOTE_F5, 166);
  beep(NOTE_AS5, 999);
  beep(NOTE_F6, 999);
  beep(NOTE_DS6, 166);
  beep(NOTE_D6, 166);
  beep(NOTE_C6, 166);
  beep(NOTE_AS6, 999);
  beep(NOTE_F6, 499);
  beep(NOTE_DS6, 166);
  beep(NOTE_D6, 166);
  beep(NOTE_C6, 166);
  beep(NOTE_AS6, 999);
  beep(NOTE_F6, 499);
  beep(NOTE_DS6, 166);
  beep(NOTE_D6, 166);
  beep(NOTE_DS6, 166);
  beep(NOTE_C6, 666);  
  compteur = 0;
}


//******* BonBruteTruand ****************
void BonBruteTruand()
{
  beep(NOTE_AS5, 133);
  beep(NOTE_DS6, 133);
  beep(NOTE_AS5, 133);
  beep(NOTE_DS6, 133);
  beep(NOTE_AS5, 802);
  beep(NOTE_FS5, 400);
  beep(NOTE_GS5, 400);
  beep(NOTE_DS5, 1071);
  beep(NOTE_AS5, 133);
  beep(NOTE_DS6, 133);
  beep(NOTE_AS5, 133);
  beep(NOTE_DS6, 133);
  beep(NOTE_AS5, 802);
  beep(NOTE_FS5, 400);
  beep(NOTE_GS5, 400);
  beep(NOTE_CS6, 1071);
  beep(NOTE_AS5, 133);
  beep(NOTE_DS6, 133);
  beep(NOTE_AS5, 133);
  beep(NOTE_DS6, 133);
  beep(NOTE_AS5, 802);
  beep(NOTE_FS5, 400);
  beep(NOTE_F5, 199);
  beep(NOTE_DS5, 199);
  beep(NOTE_CS5, 1071);
  beep(NOTE_AS5, 133);
  beep(NOTE_DS6, 133);
  beep(NOTE_AS5, 133);
  beep(NOTE_DS6, 133);
  beep(NOTE_AS5, 802);
  beep(NOTE_GS5, 400);
  beep(NOTE_DS5, 1071);  
  compteur = 0;
}


//******* Entertainement ****************
void Entertainement()
{
  beep(NOTE_D5, 214);
  beep(NOTE_DS5, 214);
  beep(NOTE_E5, 214);
  beep(NOTE_C6, 428);
  beep(NOTE_E5, 214);
  beep(NOTE_C6, 428);
  beep(NOTE_E5, 214);
  beep(NOTE_C6, 1284);
  beep(NOTE_C6, 214);
  beep(NOTE_D6, 214);
  beep(NOTE_DS6, 214);
  beep(NOTE_E6, 214);
  beep(NOTE_C6, 214);
  beep(NOTE_D6, 214);
  beep(NOTE_E6, 428);
  beep(NOTE_B5, 214);
  beep(NOTE_D6, 428);
  beep(NOTE_C6, 856);
  delay(428);
  beep(NOTE_D5, 214);
  beep(NOTE_DS5, 214);
  beep(NOTE_E5, 214);
  beep(NOTE_C6, 428);
  beep(NOTE_E5, 214);
  beep(NOTE_C6, 428);
  beep(NOTE_E5, 214);
  beep(NOTE_C6, 1284);
  delay(214);
  beep(NOTE_A5, 214);
  beep(NOTE_G5, 214);
  beep(NOTE_FS5, 214);
  beep(NOTE_A5, 214);
  beep(NOTE_C6, 214);
  beep(NOTE_E6, 428);
  beep(NOTE_D6, 214);
  beep(NOTE_C6, 214);
  beep(NOTE_A5, 214);
  beep(NOTE_D6, 856);  
  compteur = 0;
}


//******* 20thCenFox ****************
void twentyCentFox()
{
  beep(NOTE_B5, 107);
  delay(214);
  beep(NOTE_B5, 107);
  beep(NOTE_B5, 107);
  beep(NOTE_B5, 856);
  delay(107);
  beep(NOTE_C6, 107);
  delay(53);
  beep(NOTE_B5, 107);
  delay(53);
  beep(NOTE_C6, 107);
  delay(53);
  beep(NOTE_B5, 107);
  delay(53);
  beep(NOTE_C6, 107);
  delay(53);
  beep(NOTE_B5, 107);
  delay(214);
  beep(NOTE_B5, 107);
  beep(NOTE_B5, 107);
  beep(NOTE_B5, 107);
  delay(53);
  beep(NOTE_B5, 107);
  delay(53);
  beep(NOTE_B5, 107);
  delay(53);
  beep(NOTE_B5, 107);
  delay(53);
  beep(NOTE_B5, 107);
  delay(53);
  beep(NOTE_B5, 107);
  delay(53);
  beep(NOTE_B5, 107);
  delay(53);
  beep(NOTE_GS5, 107);
  delay(53);
  beep(NOTE_A5, 107);
  delay(53);
  beep(NOTE_B5, 107);
  delay(214);
  beep(NOTE_B5, 107);
  beep(NOTE_B5, 107);
  beep(NOTE_B5, 856);
  delay(428);
  beep(NOTE_E5, 214);
  beep(NOTE_GS5, 214);
  beep(NOTE_B5, 214);
  beep(NOTE_CS6, 1712);
  beep(NOTE_FS5, 214);
  beep(NOTE_A5, 214);
  beep(NOTE_CS6, 214);
  beep(NOTE_E6, 1712);
  beep(NOTE_A5, 214);
  beep(NOTE_CS6, 214);
  beep(NOTE_E6, 214);
  beep(NOTE_E6, 1712);
  beep(NOTE_B5, 214);
  beep(NOTE_GS5, 214);
  beep(NOTE_A5, 214);
  beep(NOTE_B5, 856);  
  compteur = 0;
}


//******* looney ****************
void looney()
{
  beep(NOTE_C6, 428);
  beep(NOTE_F6, 214);
  beep(NOTE_E6, 214);
  beep(NOTE_D6, 214);
  beep(NOTE_C6, 214);
  beep(NOTE_A5, 642);
  beep(NOTE_C6, 214);
  beep(NOTE_F6, 214);
  beep(NOTE_E6, 214);
  beep(NOTE_D6, 214);
  beep(NOTE_DS6, 214);
  beep(NOTE_E6, 642);
  beep(NOTE_E6, 214);
  beep(NOTE_E6, 214);
  beep(NOTE_C6, 214);
  beep(NOTE_D6, 214);
  beep(NOTE_C6, 214);
  beep(NOTE_E6, 214);
  beep(NOTE_C6, 214);
  beep(NOTE_D6, 214);
  beep(NOTE_A5, 214);
  beep(NOTE_C6, 214);
  beep(NOTE_G5, 214);
  beep(NOTE_AS5, 214);
  beep(NOTE_A5, 214);
  beep(NOTE_F5, 214);  
  compteur = 0;
}


//******* BarbieGirl ****************
void BarbieGirl()
{
  beep(NOTE_GS5, 240);
  beep(NOTE_E5, 240);
  beep(NOTE_GS5, 240);
  beep(NOTE_CS6, 240);
  beep(NOTE_A5, 480);
  delay(480);
  beep(NOTE_FS5, 240);
  beep(NOTE_DS5, 240);
  beep(NOTE_FS5, 240);
  beep(NOTE_B5, 240);
  beep(NOTE_GS5, 480);
  beep(NOTE_FS5, 240);
  beep(NOTE_E5, 240);
  delay(480);
  beep(NOTE_E5, 240);
  beep(NOTE_CS5, 240);
  beep(NOTE_FS5, 480);
  beep(NOTE_CS5, 480);
  delay(480);
  beep(NOTE_FS5, 240);
  beep(NOTE_E5, 240);
  beep(NOTE_GS5, 480);
  beep(NOTE_FS5, 480);
  compteur = 0;
}


//******* Greensleaves ****************
void Greensleaves()
{
  beep(NOTE_G5, 428);
  beep(NOTE_AS5, 856);
  beep(NOTE_C6, 428);
  beep(NOTE_D6, 642);
  beep(NOTE_DS6, 214);
  beep(NOTE_D6, 428);
  beep(NOTE_C6, 856);
  beep(NOTE_A5, 428);
  beep(NOTE_F5, 642);
  beep(NOTE_G5, 214);
  beep(NOTE_A5, 428);
  beep(NOTE_AS5, 856);
  beep(NOTE_G5, 428);
  beep(NOTE_G5, 642);
  beep(NOTE_F5, 214);
  beep(NOTE_G5, 428);
  beep(NOTE_A5, 856);
  beep(NOTE_F5, 428);
  beep(NOTE_D5, 856);
  beep(NOTE_G5, 428);
  beep(NOTE_AS5, 856);
  beep(NOTE_C6, 428);
  beep(NOTE_D6, 642);
  beep(NOTE_E6, 214);
  beep(NOTE_D6, 428);
  beep(NOTE_C6, 856);
  beep(NOTE_A5, 428);
  beep(NOTE_F5, 642);
  beep(NOTE_G5, 214);
  beep(NOTE_A5, 428);
  beep(NOTE_AS5, 642);
  beep(NOTE_A5, 214);
  beep(NOTE_G5, 428);
  beep(NOTE_FS5, 642);
  beep(NOTE_E5, 214);
  beep(NOTE_FS5, 428);
  beep(NOTE_G5, 856);
  compteur = 0;
}


//******* James Bond *******************

void Bond()
{
  beep(NOTE_CS6, 187);
  beep(NOTE_DS6, 93);
  beep(NOTE_DS6, 93);
  beep(NOTE_DS6, 187);
  beep(NOTE_DS6, 375);
  beep(NOTE_CS6, 187);
  beep(NOTE_CS6, 187);
  beep(NOTE_CS6, 187);
  beep(NOTE_CS6, 187);
  beep(NOTE_E6, 93);
  beep(NOTE_E6, 93);
  beep(NOTE_E6, 187);
  beep(NOTE_E6, 375);
  beep(NOTE_DS6, 187);
  beep(NOTE_DS6, 187);
  beep(NOTE_DS6, 187);
  beep(NOTE_CS6, 187);
  beep(NOTE_DS6, 93);
  beep(NOTE_DS6, 93);
  beep(NOTE_DS6, 187);
  beep(NOTE_DS6, 375);
  beep(NOTE_CS6, 187);
  beep(NOTE_CS6, 187);
  beep(NOTE_CS6, 187);
  beep(NOTE_CS6, 187);
  beep(NOTE_E6, 93);
  beep(NOTE_E6, 93);
  beep(NOTE_E6, 187);
  beep(NOTE_E6, 375);
  beep(NOTE_DS6, 187);
  beep(NOTE_D6, 187);
  beep(NOTE_CS6, 187);
  beep(NOTE_CS7, 187);
  beep(NOTE_C7, 1125);
  beep(NOTE_GS6, 187);
  beep(NOTE_FS6, 187);
  beep(NOTE_GS6, 1125);
  compteur = 0;
}

