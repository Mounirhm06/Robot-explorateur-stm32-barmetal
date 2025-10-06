# 🤖 Robot explorateur autonome

Ce projet présente la conception et le développement d’un **robot explorateur** capable de **cartographier une zone inconnue** en temps réel à l’aide de capteurs embarqués et d’une communication Wi-Fi avec un PC.

---

## 🎯 Objectif du projet

Le robot a pour but d’explorer un environnement inconnu, de collecter les données nécessaires à la génération d’une carte, et à terme, d’évoluer de manière **autonome** en évitant les obstacles.
Le développement actuel couvre toutes les étapes sauf la navigation autonome complète.

### Fonctionnalités implémentées à ce stade

* Commande manuelle du robot depuis le **terminal du PC** via Wi-Fi (envoi direct d’instructions).
* Exécution des instructions par le robot : déplacement, rotation, collecte de données.
* Transmission des mesures Lidar et capteurs vers le PC.
* Génération de la carte en temps réel sur PC/Matlab.

### Fonctionnalité en cours de développement

* **Navigation autonome** : dans la version finale, le PC calculera automatiquement les trajectoires et enverra au robot les séquences d’instructions correspondantes.
  (Le système de communication et de cartographie est déjà prêt pour l’intégrer.)

---

## ⚙️ Architecture générale

Le robot repose sur un **châssis Cherokey 4WD** motorisé, piloté par une **STM32L476RG**, et équipé de capteurs pour percevoir l’environnement.

* **Lidar Rplidar A1M8-R5** → acquisition des données spatiales (nuage de points).
* **Encodeurs PEC12R** → mesure de la distance parcourue.
* **MPU6050** → mesure de l’orientation et de la rotation.
* **ESP8266** → communication Wi-Fi entre le robot et le PC (protocole TCP/IP).
* **STM32L476RG** → gestion bas niveau (bare metal, accès direct aux registres).
* **PC** → envoi d’instructions manuelles, traitement des données et cartographie.

---

## 🧩 Composants utilisés

| **Composant**        | **Fonction**                                          |
| -------------------- | ----------------------------------------------------- |
| Châssis Cherokey 4WD | Déplacement dans les différentes directions           |
| Rplidar A1M8-R5      | Récupérer les données spatiales de la zone à explorer |
| MPU6050              | Mesurer l’angle de rotation du robot                  |
| ESP8266              | Connexion Wi-Fi entre le robot et le PC               |
| Encodeur PEC12R      | Renvoyer la position des roues                        |
| STM32L476RG          | Piloter les capteurs et actionneurs du robot          |
| PC                   | Traiter les données et tracer la carte en temps réel  |

---

## 🔌 Câblage des principaux composants

### Châssis Cherokey

| **Pin carte** | **Pin châssis** | **Moteurs** | **Fonction** |
| ------------- | --------------- | ----------- | ------------ |
| PA0           | D4              | M1M3        | AV/AR        |
| PA1           | D5              | M1M3        | Vitesse      |
| PA4           | D7              | M2M4        | AV/AR        |
| PA5           | D6              | M2M4        | Vitesse      |

### Lidar

| **Pin carte** | **Pin Lidar** |
| ------------- | ------------- |
| PA9           | RX Lidar      |
| PA10          | TX Lidar      |

### ESP8266

| **Pin carte** | **Pin ESP** |
| ------------- | ----------- |
| PC4           | D5          |
| PC5           | D6          |

### Encodeurs

| **Pin carte** | **Pin encodeurs** |
| ------------- | ----------------- |
| PC0           | Encodeur sur M1   |
| PC1           | Encodeur sur M2   |

### MPU6050

| **Pin carte** | **Pin MPU** |
| ------------- | ----------- |
| PB8           | SCL         |
| PB9           | SDA         |

---

## 📁 Structure du dépôt

* **ESP8266/** — Code du module Wi-Fi assurant la communication TCP/IP entre le robot et le PC.
* **STM32/** — Code bare metal (accès registres) pour le contrôle des moteurs et la gestion des capteurs.
* **PC/** — Programmes côté PC pour envoyer des instructions au robot et traiter les données reçues.
* **Matlab/** — Scripts pour tracer la trajectoire et la carte du robot en temps réel.

---

## 📡 Communication et fonctionnement

* La connexion entre le PC et le robot s’effectue en **Wi-Fi** via l’ESP8266.
* Le protocole **TCP/IP** est utilisé pour assurer la fiabilité des échanges.
* Le PC envoie des **instructions manuelles** (via le terminal) telles que distance et angle.
* Le robot exécute ces instructions, collecte les mesures du Lidar et renvoie les données pour la **cartographie en temps réel**.

---

## 🧠 Perspectives

La prochaine étape du projet est la **navigation autonome**, où le PC calculera dynamiquement les trajets optimaux en fonction de la carte construite et des obstacles détectés.
Le code actuel est déjà structuré pour intégrer cette fonctionnalité sans modification majeure.

---

## 🎥 Démonstration

Une présentation complète du projet, incluant les explications, la mise en œuvre et une démonstration du robot, est disponible sur YouTube :
👉 [https://www.youtube.com/watch?v=Ssc_gZg0svA](https://www.youtube.com/watch?v=Ssc_gZg0svA)

---

## 🧾 Synthèse

* Contrôle moteur et capteurs sur **STM32 (bare metal)**
* Acquisition de données par **Lidar, MPU6050 et encodeurs**
* Communication **Wi-Fi (ESP8266)** avec le PC
* Traitement et cartographie **en temps réel sur PC/Matlab**
* Navigation autonome en développement

Ce dépôt regroupe l’ensemble du code, des schémas et des explications nécessaires pour comprendre et reproduire le robot explorateur.
