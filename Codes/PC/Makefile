all : Robot_wifi 
Instructions.o : Instructions.c 
	gcc -c Instructions.c -o Instructions.o -Wall
map.o : map.c 
	gcc -c map.c -o map.o -lm -Wall
Robot_wifi_communication.o : Robot_wifi_communication.c 
	gcc -c Robot_wifi_communication.c -o Robot_wifi_communication.o -Wall
Robot_wifi : Robot_wifi_communication.o Instructions.o map.o
	gcc Robot_wifi_communication.o Instructions.o map.o -o Robot_wifi -Wall -lm -lws2_32

clean :
	del /Q *.o 
