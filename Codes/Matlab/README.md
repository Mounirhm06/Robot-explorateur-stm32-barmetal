# Cartographie Robot Explorateur - Explications et Utilisation du Script MATLAB

Ce document explique le fonctionnement et l'utilisation du script MATLAB `cartographie.m` destiné à visualiser en temps réel la trajectoire et la cartographie locale générées par le robot explorateur.

---

## Fonctionnement général

À **chaque nouvelle instruction envoyée au robot** (par exemple, via le terminal), **deux fichiers sont générés** dans le répertoire de travail :

1. **`position_robot.txt`**  
   - Ce fichier transforme les instructions tapées sur le terminal en coordonnées (X, Y).
   - Il permet de reconstituer et de tracer la *trajectoire* du robot sur le plan.

2. **`map_local.txt`**  
   - Ce fichier contient les points envoyés par le robot qui représentent le milieu cartographié.
   - Il s’agit d’un *nuage de points* en coordonnées cartésiennes, description de l’environnement exploré.

---

## Visualisation en temps réel

Le script MATLAB (`cartographie.m`) :
- **lit en permanence** les deux fichiers (`position_robot.txt` et `map_local.txt`),
- **modifie l’affichage graphique à chaque nouvelle mise à jour** de l’un ou l’autre fichier.

**Important :**
- Le script doit être **lancé avant d’envoyer la première commande au robot**.  
  Il doit ensuite rester en fonctionnement pour permettre la visualisation continue.
- Le script doit être placé dans le **même répertoire** que les fichiers `position_robot.txt` et `map_local.txt`.  
  *Si ce n’est pas le cas, il faudra alors indiquer le chemin complet (path) vers ces deux fichiers dans le code.*

---

## Représentation graphique

- **Le robot** : carré rouge (`rectangle`)
- **La carte locale** : nuage de points bleus (`plot`)
- **La trajectoire du robot** : points bleus reliés par des pointillés verts (`plot`)

---

## Utilisation

1. **Placez le script `cartographie.m` dans le même dossier que `position_robot.txt` et `map_local.txt`, ou modifiez les chemins dans le script.**
2. **Lancez le script MATLAB :**
   ```matlab
   cartographie
   ```
3. **Envoyez vos instructions au robot depuis le terminal.**
4. **Observez en temps réel la trajectoire et la cartographie locale qui s’affichent et se mettent à jour automatiquement.**
5. **Ne fermez pas le script tant que l’exploration n’est pas terminée.**

