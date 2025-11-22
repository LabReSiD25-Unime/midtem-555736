#ifndef OPERAZIONI_H
#define OPERAZIONI_H

typedef struct movimento {
    int id;
    float importo;
    char* causale;
    struct movimento *next;
} movimento;

extern movimento *head;

void inserisci_movimento(int id, float importo, char* causale);
movimento* ricerca_movimento(int id);
int modifica(int id, float nuovo_importo, char* nuova_causale);
int elimina(int id);
char* stampa_movimenti(movimento* head);

#endif