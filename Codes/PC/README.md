# Dossier `Codes/PC`

Ce dossier continet les différents fichiers permettant d'envoyer et de recevoir des données entre le PC et le robot. Voici la description des fichiers présents :

## Fichiers

- **map.c**  
  Permet de supprimer l'octet de qualité envoyé par le lidar et de transformer les mesures reçues du lidar (coordonnées polaires) en coordonnées cartésiennes pour tracer la carte en 2D.

- **Instructions.c**  
  Permet d'extraire les commandes envoyées au robot à partir des commandes saisies dans le terminal. Ces commandes sont ensuite utilisées pour tracer la trajectoire du robot sur Matlab.

- **Robot_communication.c**  
  Programme principal qui gère la communication entre le PC et le robot, notamment l'envoi et la réception des messages et des instructions via le Wi-Fi créé par la carte ESP8266 du robot, par le protocole TCP/IP.

---

## Compatibilité

Ce code est compatible avec **Windows**.  
> Si vous souhaitez l'utiliser sur **Linux**, il est nécessaire de modifier certaines bibliothèques spécifiques à Windows (par exemple : `winsock2.h`) ainsi que les parties du code associées à la gestion réseau et autres fonctionnalités dépendantes du système.

---

## Structure des instructions envoyées au robot

Les instructions envoyées vers le robot suivent un format précis :

- On commence par indiquer le **nombre d'instructions** (sur 2 octets).
- Chaque instruction inclut :
  - **distance** à parcourir (2 octets)
  - **angle** à tourner (3 octets)
- Les instructions sont envoyées de façon séquentielle : nombre d'instructions, puis les distances, puis les angles correspondants.

### Unité de distance

- Une unité de distance correspond à un tour complet de la roue du robot, soit **20 cm** dans la configuration actuelle.
- Pour modifier cette distance, il faut adapter dans le code (fichier `main.c`) le nombre d'impulsions d'encodeur par tour.  
  Exemple : 86 impulsions = 1 tour complet = 20 cm.  
  Si on met 43 impulsions, alors 1 unité = 10 cm (un demi-tour de roue).  
  À adapter selon le diamètre de votre roue.

### Unité d'angle

- L'angle est compris entre **0° et 360°**.  
  Pour tourner à **-90°**, il faut envoyer au robot la valeur **270** (car 360 - 90 = 270).

### Exemple de commande

Si on tape dans le terminal : `020103090270`

- `02` : deux instructions
- Première instruction :  
  - distance : `01` (une unité = 20 cm)
  - angle : `090` (90° à droite)
- Deuxième instruction :  
  - distance : `03` (trois unités = 60 cm)
  - angle : `270` (270° = -90° à gauche)

**Interprétation** :  
Le robot va :
1. Tourner 90° à droite puis avancer de 20 cm (Instruction 1)
2. Tourner 90° à gauche puis avancer de 60 cm (Instruction 2)

---

## Étapes de connexion

1. **Se déconnecter des autres réseaux Wi-Fi**  
   Il est impératif de se déconnecter de tout autre réseau avant de rejoindre le réseau de l’ESP8266, sous peine de ne pas établir la communication.

2. **Se connecter au réseau Wi-Fi de l’ESP8266**  
   - Nom du réseau : `ESP8266_AP`  
   - Mot de passe : `12345678`  
   - L'adresse IP de la carte est généralement `192.168.4.1`.  
   - Vérifiez la connexion ou l’adresse IP en tapant dans un terminal :  
     ```bash
     ping 192.168.4.1
     ```
   - La connexion TCP entre le PC et la carte ESP8266 est établie sur le port 23.

3. **Compiler et lancer le programme côté PC**  
   - Ouvrir PowerShell (Windows) ou un terminal (Linux).
   - Se placer dans le répertoire PC téléchargé depuis ce dépôt.
   - Compiler avec :
     ```bash
     make
     ```
   - Lancer l’exécutable :
     ```bash
     ./Robot_wifi
     ```
   - Si la connexion au serveur ESP8266 est réussie, le message "connecté au serveur" s'affiche, suivi de "Entrer un message pour envoyer des instructions au robot".

---

## Attention lors des reconnexions

- Si vous vous déconnectez puis reconnectez au réseau `ESP8266_AP`, le robot ne reçoit plus les instructions.
- Dans ce cas, il faut réinitialiser la carte ESP8266 (par exemple en la redémarrant), puis refaire l’ensemble de la procédure de connexion.

---

N'hésitez pas à consulter le code source pour plus de détails sur le fonctionnement de chaque module.
