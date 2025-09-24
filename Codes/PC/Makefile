all : Robot_wifi # creer l'executable final Robot_wifi

# creer Instructions.o (l'objet) a partir de Instructions.c (le code source)
Instructions.o : Instructions.c 
	gcc -c Instructions.c -o Instructions.o -Wall
#creer map.o (l'objet) a partir de map.c (le code source)
map.o : map.c 
	gcc -c map.c -o map.o -lm -Wall
# creer Robot_wifi_communication.o (l'objet) a partir de Robot_wifi_communication.c (le code source)
Robot_wifi_communication.o : Robot_wifi_communication.c 
	gcc -c Robot_wifi_communication.c -o Robot_wifi_communication.o -Wall

# creer l'executable Robot_wifi a partir de Robot_wifi_communication.o et Instructions.o
Robot_wifi : Robot_wifi_communication.o Instructions.o map.o
	gcc Robot_wifi_communication.o Instructions.o map.o -o Robot_wifi -Wall -lm -lws2_32

# supprimer les fichiers objets
clean :
	del /Q *.o 