#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include <pthread.h>

#define PORT 1053
#define BUFFER_SIZE 256
#define USER_LIMIT 2

struct arguments {
    int sock;
    bool terminate;
    bool reuse;
};

struct arguments args[USER_LIMIT];

void *thread_action(void *arg);

int main(int argc, char **argv){

	//Spremenjlivka za preverjane izhodnega statusa funkcij
	int iResult;
    

	/*
	Ustvarimo nov vtiè, ki bo poslušal
	in sprejemal nove kliente preko TCP/IP protokola
	*/
	int listener=socket(AF_INET, SOCK_STREAM, 0);
	if (listener == -1) {
		printf("Error creating socket\n");
		return 1;
	}

	//Nastavimo vrata in mrežni naslov vtièa
	sockaddr_in  listenerConf;
	listenerConf.sin_port=htons(PORT);
	listenerConf.sin_family=AF_INET;
	listenerConf.sin_addr.s_addr=INADDR_ANY;

	//Vtiè povežemo z ustreznimi vrati
	iResult = bind( listener, (sockaddr *)&listenerConf, sizeof(listenerConf));
	if (iResult == -1) {
		printf("Bind failed\n");
		close(listener);
		return 1;
    }

	//Zaènemo poslušati
	if ( listen( listener, 5 ) == -1 ) {
		printf( "Listen failed\n");
		close(listener);
		return 1;
	}

	//Definiramo nov vtiè in medpomnilik
	int clientSock;
    
    pthread_t threads[USER_LIMIT];
    
    for (int i = 0; i < USER_LIMIT; i++) {
        args[i].reuse = true;
    }
	
	/*
	V zanki sprejemamo nove povezave
	in jih strežemo (najveè eno naenkrat)
	*/
	while (1)
	{
        clientSock = accept(listener,NULL,NULL);
        if (clientSock == -1) {
            printf("Accept failed\n");
            close(listener);
            return 1;
        }
        for (int i = 0; i < USER_LIMIT; i++) {
            if(args[i].terminate){
                if(args[i].terminate){
                    close(args[i].sock);
                }
                args[i].sock = clientSock;
                args[i].terminate = false;
                pthread_create(&threads[i], NULL, *thread_action, &args[i]);
                break;
            }
            if(i == USER_LIMIT - 1){
                close(clientSock);
            }
        }
	}

	close(listener);

	return 0;
}


void *thread_action(void *arg){
    struct arguments *parg = (struct arguments*) arg;
    char buff[BUFFER_SIZE];
    int iResult;
    
    //Postrezi povezanemu klientu
    do{
        //Sprejmi podatke
        iResult = recv(parg->sock, buff, BUFFER_SIZE, 0);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);
            
            //Vrni prejete podatke pošiljatelju
            iResult = send(parg->sock, buff, iResult, 0 );
            if (iResult == -1) {
                printf("send failed!\n");
                parg->terminate = false;
                close(parg->sock);
                break;
            }
            printf("Bytes sent: %d\n", iResult);
        } else if (iResult == 0){
            parg->terminate = true;
            printf("Connection closing...\n");
        } else {
            printf("recv failed!\n");
            close(parg->sock);
            break;
        }
    } while (iResult > 0);
    
    return NULL;
}

