#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "map.h"

// Fonction pour séparer les octets de qualité, angle et distance
void Extraire_Qualite_Angle_Distance(uint8_t *data, int nbpoints, uint8_t *quality, float *angle, float *distance)
 {
    uint16_t angle_q6, distance_q2; // Variables temporaires pour stocker les valeurs intermédiaires
    for (int i = 0; i < nbpoints; i++) {
        int index = i * 5; // Chaque point occupe 5 octets
        quality[i] = data[index]; // Octet de qualité

        //transformer l'angle et la distance en 16 bits
        angle_q6 = ((data[index + 1] >> 1) & 0x7F) | (data[index + 2] << 7);
        distance_q2 = data[index + 3] | (data[index + 4] << 8);

        //transformer les valeurs en mesures réelles
        angle[i] = angle_q6/64.0; // Octets 2 et 3 pour l'angle, angle en °
        distance[i] = distance_q2/4.0; // Octets 4 et 5 pour la distance, distance en mm
    }
}

//Fonction pour transformer les coordonnées en coordonnées cartésiennes
void Transforme_Cartesien(float *angle, float *distance, float *x, float *y, int nbpoints)
{
    for (int i = 0; i < nbpoints; i++) {
        x[i] = (distance[i] * sin(angle[i] * M_PI / 180.0))/10; //xi en cm
        y[i] = (distance[i] * cos(angle[i] * M_PI / 180.0))/10; //yi en cm
    }
}