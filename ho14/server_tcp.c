#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>             //read e write
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "operazioni.h"
#include <ctype.h>
#include <semaphore.h>

#define MAX_NUM 10
#define PORT 8080

pthread_mutex_t lock;
sem_t sem;


void* thread_function(void* arg) {
    char buffer[4096];
    char *messaggio;
    int id_movimento, sock, n;
    float importo;
    char causale[256];
    
    sock = *(int *)arg; 
    
    printf("Client %d (fd) connesso\n", sock);
    
    while (1) {
    
        n = read(sock, buffer, 1024);
        if (n < 0) {
            perror("read failed");
            close(sock);
        } 
        
        if (n > 0) {
            buffer[n] = '\0';
            
            printf("RISPOSTA RICEVUTA DAL CLIENT %d: %s\n", sock, buffer);
            
            if (strcmp(buffer, "EXIT\n") == 0) {
                printf("client %d disconnesso\n", sock);
                close(sock);
                break;
            }
            if (strcmp(buffer, "ADD\n") == 0) {
                messaggio = "INSERISCI ID, IMPORTO e causale separati da spazio\n";
                send(sock, messaggio, strlen(messaggio), 0);
                n = read(sock, buffer, 1024);
                buffer[n] = '\0';
                sscanf(buffer, "%d %f %s", &id_movimento, &importo, causale);
                inserisci_movimento(id_movimento, importo, causale);
                messaggio = "OPERAZIONE ADD EFFETTUATA\n";
                printf("CLIENT: %d: %s\n", sock, messaggio);                
                send(sock, messaggio, strlen(messaggio), 0);
            }

            if (strcmp(buffer, "UPDATE\n") == 0) {
                messaggio = "INSERISCI ID DEL MOVIMENTO DA MODIFICARE, IMPORTO E CAUSALE (SEPARATI DA SPAZIO)\n";
                send(sock, messaggio, strlen(messaggio), 0);
                n = read(sock, buffer, 1024);
                buffer[n] = '\0';
                sscanf(buffer, "%d %f %s", &id_movimento, &importo, causale);
                if(modifica(id_movimento, importo, causale)) {
                    messaggio = "OPERAZIONE EFFETTUATA\0";
                    printf("CLIENT: %d: %s\n", sock, messaggio);  
                    send(sock, messaggio, strlen(messaggio), 0);
                } else {
                    messaggio = "MOVIMENTO NON TROVATO\0";
                    printf("CLIENT: %d: %s\n", sock, messaggio);  
                    send(sock, messaggio, strlen(messaggio), 0);                                        
                }
            }

            if (strcmp(buffer, "DELETE\n") == 0) {
                messaggio = "INSERISCI ID DEL MOVIMENTO DA ELIMINARE\n";
                send(sock, messaggio, strlen(messaggio), 0);
                n = read(sock, buffer, 1024);
                buffer[n] = '\0';
                sscanf(buffer, "%d", &id_movimento);
                if(elimina(id_movimento)) {
                    messaggio = "OPERAZIONE EFFETTUATA\n";
                    printf("CLIENT: %d: %s\n", sock, messaggio);  
                    send(sock, messaggio, strlen(messaggio), 0);    
                } else {
                    messaggio = "OPERAZIONE EFFETTUATA MA NESSUN MOVIMENTO TROVATO\n";
                    printf("CLIENT: %d: %s\n", sock, messaggio);  
                    send(sock, messaggio, strlen(messaggio), 0);
                }
            }
           
            if (strcmp(buffer, "LIST\n") == 0) {
                messaggio = stampa_movimenti(head);
                send(sock, messaggio, strlen(messaggio), 0);
                printf("OPERAZIONE LIST EFFETTUATA DAL CLIENT: %d\n", sock);  
                
            }

        }
        if ( n == 0) { 
            printf("client %d disconnesso", sock);
            close(sock);
        }
    }
    sem_post(&sem);
    close(sock);
    return NULL;
    
}


int main() {

    int sockfd, newsockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t clilen = sizeof(client_addr);
    pthread_t thread_id;
    
    pthread_mutex_init(&lock, NULL);
    sem_init(&sem, 0, MAX_NUM);

    //creare il socket
    /*
    Crea un socket TCP (SOCK STREAM) utilizzando il dominio IPv4 (AF INET). Se
    la creazione fallisce, viene stampato un messaggio di errore e il programma
    termina.
    */
    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("errore nella creazione del server");
        exit(EXIT_FAILURE);
    }

    /*
    Configura l’indirizzo del server:
    • sin family: Imposta il dominio di indirizzamento a IPv4.
    • sin addr.s addr: Imposta l’indirizzo IP a INADDR ANY, che significa che
        il server accetterà connessioni su qualsiasi interfaccia di rete.
    • sin port: Imposta la porta su cui il server ascolta, convertendola in formato di rete con htons.
    */


    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0 ){
        perror("bind failed");
        close(sockfd);        
        exit(EXIT_FAILURE);
    }
    
    /*
    Associa il socket all’indirizzo e alla porta specificati. Se il binding fallisce, viene
    stampato un messaggio di errore, il socket viene chiuso e il programma termina
    */

    if(listen(sockfd, 10) < 0) {
        perror("listen failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    /*
    Mette il socket in ascolto per connessioni in entrata, con una coda massima di
    10 connessioni pendenti. Se fallisce, viene stampato un messaggio di errore, il
    socket viene chiuso e il programma termina.
    */
    printf("Server in ascolto...\n");    

    while (1)
    {
        
    
        sem_wait(&sem);

        if((newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &clilen )) < 0) {
            perror("accept failed");
            sem_post(&sem);
            exit(EXIT_FAILURE);
        }

        if(pthread_create(&thread_id, NULL, thread_function, &newsockfd) != 0) {
            perror("errore nella creazione del thread");
            close(sockfd);
            sem_post(&sem);
            exit(EXIT_FAILURE);
        }


    }

    sem_destroy(&sem);
    pthread_mutex_destroy(&lock);
    close(sockfd);

}
