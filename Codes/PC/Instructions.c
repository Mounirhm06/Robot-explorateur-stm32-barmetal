#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Instruction.h"

//Fonction qui permet de localiser le robot dans la carte globale 
// ces fonctions permetent d'extraire les commandes du robot à partir des commandes saisies dans le terminal
// La commande est sous la forme "nb_commandes distance1 distance2 ...distanceN angle1 angle2 ...angleN"

void Robot_Commandes(char *commandes, int *nb_commandes, int *distance, int *angle)
{
    int i, j = 0;
    *nb_commandes = (commandes[0] - '0') * 10 + (commandes[1] - '0'); // Stocker dans les 2 premiers caractères
    for (i = 2; i < 2 * (*nb_commandes) + 1; i += 2) {
        distance[j] = (commandes[i] - '0') * 10 + (commandes[i + 1] - '0'); // Stocker la distance
        j++;
    }
    j = 0;
    for (i = 2 * (*nb_commandes) + 2; i < 5 * (*nb_commandes) + 2; i += 3) {
        angle[j] = (commandes[i] - '0') * 100 + (commandes[i + 1] - '0') * 10 + (commandes[i + 2] - '0'); // Stocker l'angle
        j++;
    }
}

// Fonction pour calculer les coordonnées et orientations du robot
void Position_Robot(float *x_robot, float *y_robot, int nb_commandes, int *distance, int *angle)
{
    int i,j;
    float orientation = 0 ; // Orientation initiale du robot
    int num_points = 20;//20 points intermidiaires entre chaque commande
    int nb_pass = 0; // Nombre de pas pour chaque commande
    float X = 0, Y = 0;
    float dx = 0, dy = 0; // Déplacements en cm
    int index = 0; // Indice pour le tableau de coordonnées
    for (i = 0; i < nb_commandes; i++) {
        if (angle[i] > 180) {
            angle[i] = angle[i] - 360; // Angle entre -180 et 180
        }

        // Mettre à jour l'orientation
        orientation = orientation + angle[i];

        // Calculer les nouvelles coordonnées
        X = 20*distance[i]  * sin((orientation)  * M_PI / 180.0); // en cm
        Y = 20*distance[i]  * cos((orientation)  * M_PI / 180.0); // en cm

        // on divise chaque distance de 20cm  petits pas de 1cm
       // nb_pass = num_points * distance[i];
        nb_pass = num_points ;
        dx = X / (nb_pass+0.0); // Déplacement en x
        dy = Y / (nb_pass+0.0); // Déplacement en y

        //calculer les points intermidiaires entre chaque commande
        for (j = 0; j < num_points; j++) {
            index = i * num_points + j; // Calculer l'indice global
            
            // Vérifier les limites du tableau
            if (index >= 400) {
                printf("Erreur : dépassement de la taille du tableau.\n");
                break;
            }

            x_robot[index+1] = x_robot[index] + dx ; // en cm
            y_robot[index+1] = y_robot[index] + dy ; // en cm
        }
    }
}