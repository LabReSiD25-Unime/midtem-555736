#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "operazioni.h"

extern pthread_mutex_t lock;
movimento *head = NULL;

//Funzione per inserire un nuovo nodo (movimento) all'inizio della lista collegata
void inserisci_movimento(int id, float importo, char* causale) {
    
    pthread_mutex_lock(&lock);
    //Dichiara un puntatore a struttura movimento
    movimento *m;
        
    //Alloca dinamicamente memoria per un nuovo nodo moviment
    m = (movimento*) malloc(sizeof(movimento));
    
    m->id = id;
    m->importo = importo;
    m->causale = strdup(causale);
    
    //Collega il nuovo nodo alla testa attuale della lista
    m->next = head;
    //Aggiorna la testa della lista per puntare al nuovo nodo
    head = m;
    pthread_mutex_unlock(&lock);
    
}

movimento *ricerca_movimento(int id) {
    movimento* m = head;
    while((m != NULL) && (m->id != id)) {
        m = m->next;
    }
    return m; 
}

int modifica(int id, float nuovo_importo, char* nuova_causale) {
    pthread_mutex_lock(&lock);
    movimento *m = head;
    int flag;
    m = ricerca_movimento(id);
    if(m == NULL) {
        printf("elemento non trovato");
        flag = 0;
    } else {
        m->importo = nuovo_importo;
        m->causale = strdup(nuova_causale);
        flag = 1;
    }
    pthread_mutex_unlock(&lock);
    return flag;
}

int elimina(int id) {
    pthread_mutex_lock(&lock);
    movimento *curr = head;
    movimento *prev = NULL;

    while((curr != NULL) && (curr->id != id)) {
        prev = curr;
        curr = curr->next;
    }

    if (curr == NULL) {
        //Nodo con id non trovato, restituisce la lista invariata
        printf("NESSUN NODO ELIMINATO, ID NON TROVATO\n");
        pthread_mutex_unlock(&lock);
        return 0;
    }

    if (prev == NULL) {
        //Il nodo da eliminare è in testa
        head = curr->next;
    } else {
        // Collega il nodo precedente al successivo, saltando quello da eliminare
        prev->next = curr->next;
    }
    
    // Libera prima la stringa causale se è stata allocata dinamicamente
    free(curr->causale);

    // Poi libera il nodo
    free(curr);
    pthread_mutex_unlock(&lock);
    return 1;
}

char* stampa_movimenti(movimento *head) {
    pthread_mutex_lock(&lock);
    movimento *m = head;
    char messaggio[1024]; 
    char *buffer = malloc(4096);
    int c = 0;
    
    if (m == NULL) {
        strcpy(buffer, "NESSUN MOVIMENTO\n");
    }
    
    while(m != NULL) {
        sprintf(messaggio, "ID: %d\tIMPORTO: %.2f€\tCAUSALE: %s\n", m->id, m->importo, m->causale);
        strcat(buffer, messaggio); 
        m = m->next;
        c++; 
    }
    pthread_mutex_unlock(&lock);
    return buffer;

}
