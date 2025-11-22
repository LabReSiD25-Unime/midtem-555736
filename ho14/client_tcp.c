#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024
#define sosizzaCunsata 20
int main() {
    int sockfd;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    
    // Crea il socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Configura l'indirizzo del server
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    if ((inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr)) <= 0) {
        perror("indirizzo non valido");
        exit(EXIT_FAILURE);
    }

    // Connessione al server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connessione fallita");
        exit(EXIT_FAILURE);
    }

    printf("Client connesso\n");
    memset(buffer, 0, BUFFER_SIZE);
    
    int n;
    while (1) {
        
        printf("\nDIGITA \nADD-per inserire un movimento \nDELETE-per eliminare un movimento\nUPDATE-per modificare un movimento\nLIST-per listare i documenti\nEXIT-per uscire\n");
        fgets(buffer, sizeof(buffer), stdin);
        
        for (int i = 0; i < strlen(buffer); i++) {
            buffer[i] = toupper(buffer[i]);
        }   


        if (strcmp(buffer, "EXIT\n") == 0) {
            send(sockfd, buffer, strlen(buffer), 0);   
            printf("Uscita...\n");
            break;
        }

        if(strcmp(buffer, "LIST\n") == 0)  {
            send(sockfd, buffer, strlen(buffer), 0);
            memset(buffer, 0, BUFFER_SIZE);
            n = read(sockfd, buffer, BUFFER_SIZE - 1);
            if (n > 0) {
                buffer[n] = '\0';
                printf("%s", buffer);
            }
            memset(buffer, 0, BUFFER_SIZE);
        } else {
            send(sockfd, buffer, strlen(buffer), 0);
            memset(buffer, 0, BUFFER_SIZE);
            n = read(sockfd, buffer, BUFFER_SIZE - 1);
            if (n > 0) {
                buffer[n] = '\0';
                printf("%s", buffer);
            }    
            // Input utente
            memset(buffer, 0, BUFFER_SIZE);
            fgets(buffer, sizeof(buffer), stdin);
            send(sockfd, buffer, strlen(buffer), 0);
            n = read(sockfd, buffer, BUFFER_SIZE - 1);
            if (n > 0) {
                buffer[n] = '\0';
                printf("%s", buffer);
            }
        
        }
        
    
    
        // Leggi eventuale risposta
        
    }

    close(sockfd);
    return 0;
}
