## Connexion entre le PC et la carte ESP8266

### Prérequis
- ESP8266 programmé avec le firmware fourni dans ce repo
- PC sous Windows ou Linux, avec `make` installé
  - **Astuce Windows :** pour utiliser les Makefiles, vous pouvez installer [Chocolatey](https://chocolatey.org/) puis `make` avec la commande :
    ```
    choco install make
    ```
### Étapes de connexion

1. **Se déconnecter des autres réseaux Wi-Fi**
   - Il est impératif de se déconnecter de tout autre réseau avant de rejoindre le réseau de l’ESP8266, sous peine de ne pas établir la communication.

2. **Se connecter au réseau Wi-Fi de l’ESP8266**
   - Nom du réseau : **ESP8266_AP**
   - Mot de passe : **12345678**
   - L'adresse IP de la carte est généralement **192.168.4.1**.
   - Vous pouvez vérifier la connexion ou l’adresse IP en tapant dans un terminal :
     ```
     ping 192.168.4.1
     ```
   - La connexion TCP entre le PC et la carte Esp8266 est établie sur le port 23
3. **Compiler et lancer le programme côté PC**
   - Ouvrir PowerShell (Windows) ou un terminal (Linux).
   - Se placer dans le répertoire `code_PC` téléchargé depuis ce dépôt.
   - Compiler avec :
     ```
     make
     ```
   - Lancer l’exécutable :
     ```
     ./Robot_wifi
     ```
   - Si la connexion au serveur ESP8266 est réussie, le message `connecté au serveur` s'affiche, suivi de `Entrer un message` pour envoyer des instructions au robot.

### Attention lors des reconnexions

- **Si vous vous déconnectez puis reconnectez au réseau ESP8266_AP, le robot ne reçoit plus les instructions.**
- Dans ce cas, il faut **rénitialiser la carte ESP8266** (par exemple en la redémarrant), puis refaire l’ensemble de la procédure de connexion.

---

## Notes techniques sur la transmission des données

- Les mesures LIDAR sont envoyées par la carte STM32 à l’ESP8266 sous forme de paquets de 50 octets, chaque paquet représentant 10 points de mesure.
- La transmission se fait à une fréquence d’un paquet toutes les 10 ms, avec une vitesse de 9600 bauds.
- Ce choix de taille de paquet (50 octets) est dicté par la limitation du buffer SoftwareSerial utilisé sur l’ESP8266, qui ne peut stocker que 64 octets. Après chaque réception et transmission vers le PC, le buffer est vidé pour permettre la réception du prochain paquet.
- Cette approche garantit que les données ne sont pas perdues et que la communication reste fluide entre la STM32, l’ESP8266 et le PC.
- Toute modification de la fréquence ou de la taille des paquets doit tenir compte de cette limitation du buffer.

---

### Dépannage
- Vérifiez que le PC est bien connecté au réseau ESP8266_AP.
- Vérifiez que l’ESP8266 est alimenté et que le serveur TCP est bien lancé.
- Désactivez le firewall ou autorisez le port 23 (TCP).
- Si la connexion échoue, vérifiez l’adresse IP et le port.

### Sécurité
> Ce réseau Wi-Fi est destiné à un usage expérimental. Le mot de passe est simple et la connexion n’est pas sécurisée.
