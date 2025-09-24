/* Développer par HAMMAMI Mounir */
/************************** version 16/04/21.03/**********************************/
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

// Création d'un port série logiciel sur les broches D5 et D6
SoftwareSerial mySerial(D5, D6); // D5 = RX, D6 = TX

// Informations du réseau Wi-Fi
const char* ssid = "ESP8266_AP";
const char* password = "12345678";

// Création d'un serveur TCP sur le port 23
WiFiServer server(23);
WiFiClient client;

uint8_t DataFromSTM[50] = {0}; // Buffer pour stocker les données reçues par bloc de 50 octets 
uint8_t nBytes_recu = 0; //  compter les octets 
uint8_t Npaquet = 0; // compter les paquets de 50 octets, 1000 points en total, 5 octets/point => 100 paquets de 10 points 

void setup() {
  // Initialisation du port série matériel pour le débogage
  Serial.begin(9600);
  delay(100);
  
  // Initialisation du port SoftwareSerial à 9600 bauds pour la communication avec la STM32
  mySerial.begin(9600);
  
  // Création du point d'accès Wi-Fi
  WiFi.softAP(ssid, password);
  Serial.println("Point d'accès Wi-Fi créé !");
  Serial.print("Adresse IP : ");
  Serial.println(WiFi.softAPIP());
  
  // Démarrer le serveur TCP
  server.begin();
}

void loop() {
  // Vérifier la connexion du PC
  if (!client || !client.connected()) {
    client = server.available();
    if (client) {
      delay(100);  // Petit délai pour laisser le temps à la connexion de se stabiliser
      // Vider le buffer SoftwareSerial 
      while (mySerial.available()) {
        mySerial.read();  
      }
      Serial.println("Client connecté, buffer SoftwareSerial vidé.");
    } 
  }

  // Vérifier si le PC est toujours connecté
  if (client && !client.connected()) {
    Serial.println("Client déconnecté.");
    client.stop(); // Réinitialiser le client
  }

  // Transmission des données reçues du PC vers la STM32
  if (client && client.connected() && client.available()) {
    String clientData = client.readStringUntil('\n'); // Lire les données envoyées du PC
    if (clientData.length() > 0) {
      mySerial.println(clientData); // Envoyer les données vers le STM32
      Serial.println("Données envoyées au STM32 : " + clientData);
    }
  }
  
  // Transmission des données reçues du STM32 vers le PC
  if (mySerial.available()) {
    nBytes_recu = 0; // Réinitialiser le compteur pour chaque réception complète
    Npaquet++;//incrementer le nombre de paquets recu
    while (nBytes_recu < 50) {
      if (mySerial.available()) {
        DataFromSTM[nBytes_recu] = mySerial.read(); // Lire l'octet
        nBytes_recu++; // Incrémenter le compteur
      }
    }
    nBytes_recu = 0;//renitialiser le compteur
    
    // Envoyer les 50 octets au PC
    if (client && client.connected()) {
      client.write(DataFromSTM, 50); 
      Serial.printf("Paquet N %d envoye\n", Npaquet);
    }

    // vérifier si tous les paquets sont reçus 
    if(Npaquet  == 100){
      Npaquet = 0;//renitialiser le nombre recu 
    }
    // vider le buffer de mySerial 
    while (mySerial.available()) {
        mySerial.read();  
      }
      //Serial.println("buffer pret pour le prochain paquet");
  }
}