# TER-Robot_explorateur - STM32

Ce dossier contient le code source de la carte STM32.  
## Fichiers présents dans `Src/`

- **Robot_Lidar.c**  
  Permet de communiquer avec le LIDAR : initialisation, envoi des commandes pour démarrer/arrêter un scan, récupération et traitement des données de distance.

- **Robot_MPU6050.c**  
  Gère la communication avec le capteur inertiel MPU6050 (accéléromètre & gyroscope). Permet de lire l’orientation et l’accélération du robot.

- **Robot_communication.c**  
  Initialise les périphériques USART du STM32 pour communiquer avec le LIDAR et l’ESP8266.  
  Ce fichier contient :
    - Des fonctions pour initialiser les périphériques USART 
    - Des fonctions pour envoyer et recevoir des messages via USART.
    - Des fonctions de communication avec la carte ESP8266 (WiFi).
    - La logique pour décoder les instructions envoyées depuis le PC via l’ESP8266 (interprétation des commandes reçues).
    - La gestion du protocole d’échange entre le robot et les modules connectés.

- **Robot_control_direction.c**  
  Contrôle la direction du robot. Traite les ordres de déplacement (gauche, droite, tout droit, marche arrière) et ajuste la mécanique en conséquence.

- **Robot_encodeurs.c**  
  Gère les encodeurs des roues/moteurs. Permet de mesurer la vitesse et la distance parcourue, utile pour le suivi de position ou le contrôle de vitesse.

- **Robot_voyants.c**  
  Contrôle les voyants lumineux (LEDs) sur le robot. Utilisé pour indiquer l’état du robot (réceptions de nouvelles instructions, erreur …)
- **Timers.c**  
  Configure et utilise les timers internes de la STM32 pour la gestion du temps, des temporisations et des interruptions périodiques nécessaires au fonctionnement du robot.

- **i2c.c**  
  pour initialiser le périphérique I2C de STM ( ce code est générer directement par STM32CubeIDE)
---

Chaque fichier source (.c) est accompagné d’un fichier d’en-tête (.h) dans le dossier `Inc/` correspondant, qui déclare les fonctions et variables globales du module.

N’hésite pas à consulter les commentaires dans les fichiers pour plus de détails sur chaque fonction !
