#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <winsock2.h>
#include <math.h>
#include "Instruction.h"
#include "map.h"

#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif

#define SERVER_IP "192.168.4.1"  // Remplacez par l'adresse IP de votre serveur
#define SERVER_PORT 23

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server_addr;
    int i, packet_count;
    char message[1024];
    int message_len;
    int bytes_received; // Nombre d'octets reçus
    int total_received; // Compteur pour suivre les octets reçus dans un paquet
    uint8_t data[5000];  // Buffer pour stocker les 1000 points (100 paquets de 50 octets(10 points))
    FILE *map_local = fopen("map_local.txt", "w"); // Fichier pour stocker la carte locale
    FILE *map_globale = fopen("map_globale.txt","w");//stocker la map globale de la zone à explorer
    uint8_t Quality[1000] = {0}; //pour stocker la qualité des 1000 points
    float Angle[1000] = {0}; //pour stocker l'angle des 1000 points
    float Distance[1000] = {0}; //pour stocker la distance des 1000 points
    float Xp[1000] = {0}; //pour stocker les coordonnes cartésiennes des 1000 points
    float Yp[1000] = {0}; //pour stocker les coordonnes cartésiennes des 1000 points
    
    if (map_globale == NULL) {
        printf("Erreur : impossible d'ouvrir le fichier map_globale.txt.\n");
        return 1;
    }

    // Initialiser Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Échec de l'initialisation de Winsock. Code erreur : %d\n", WSAGetLastError());
        return 1;
    }

    // Créer un socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Échec de la création du socket : %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Configuration de l'adresse du serveur
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    if (server_addr.sin_addr.s_addr == INADDR_NONE) {
        printf("Adresse invalide ou non supportée. Code erreur : %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Connexion au serveur
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("La connexion a echoue. Code erreur : %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printf("Connecte au serveur.\n");

    while (1) {
        // Saisie du message à envoyer
        printf("Entrez un message : ");
        if (fgets(message, sizeof(message), stdin) == NULL) {
            break;
        }

        message_len = strlen(message);
        if (send(sock, message, message_len, 0) < 0) {
            printf("Erreur lors de l'envoi. Code erreur : %d\n", WSAGetLastError());
            break;
        }
        
        // Réception des 100 paquets de 50 octets
        for (packet_count = 0; packet_count < 100; packet_count++) {
            total_received = 0; // Réinitialiser le compteur pour chaque paquet

            while (total_received < 50) {
                bytes_received = recv(sock, (char *)data + (packet_count * 50) + total_received, 50 - total_received, 0);
                if (bytes_received > 0) {
                    total_received += bytes_received;
                } else if (bytes_received == 0) {
                    printf("Connexion fermee par le serveur.\n");
                    break;
                } else {
                    printf("Erreur de réception. Code erreur : %d\n", WSAGetLastError());
                    break;
                }
            }

            if (total_received == 50) {
                printf("Paquet N°%d reçu.\n", packet_count + 1);
            } else {
                printf("Erreur : réception incomplète pour le paquet N°%d (%d octets reçus).\n", packet_count + 1, total_received);
                break;
            }
        }
        
        // Vérifier si les 100 paquets ont été reçus
        if (packet_count == 100) {
            printf("Tous les points ont ete reçus.\n");
            // Appel de la fonction pour extraire les données
            Extraire_Qualite_Angle_Distance(data, 1000, Quality, Angle, Distance);
            //transformer les coordonnées en coordonnées cartésiennes
            Transforme_Cartesien(Angle, Distance, Xp, Yp, 1000);
            //reouvrir le ficher 
            map_local = fopen("map_local.txt", "w");
            if (map_local == NULL) {
                printf("Erreur : impossible de réouvrir le fichier map_local.txt.\n");
                
            }
            //stocker les données dans les fichiers map_local.txt et map_globale.txt en format cartésien
            for (i = 0; i < 1000; i++) {
                fprintf(map_local, "%.2f %.2f\n",Xp[i], Yp[i]);
                fprintf(map_globale, "%.2f %.2f\n",Xp[i], Yp[i]);
            }
            // S'assurer que les données sont écrites immédiatement dans le fichier
            fflush(map_local);
            fflush(map_globale);
            //fermer le ficher
            fclose(map_local);
            printf("donnes stockees\n");
            
        } 
        else {
            printf("Erreur : tous les paquets n'ont pas ete recus.\n");
        }
    }
    // Fermeture des fichiers
    fclose(map_globale);

    // Fermeture du socket et nettoyage de Winsock
    closesocket(sock);
    WSACleanup();

    return 0;
}