#ifndef MAP_H
#define MAP_H
void Extraire_Qualite_Angle_Distance(uint8_t *data, int nbpoints, uint8_t *quality, float *angle, float *distance);
void Transforme_Cartesien(float *angle, float *distance, float *x, float *y, int nbpoints);
#endif