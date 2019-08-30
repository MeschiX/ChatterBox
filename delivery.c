/*
 * Progetto del corso di LSO 2017/2018
 *
 * Dipartimento di Informatica Università di Pisa
 * Docenti: Prencipe, Torquati
 *
 * Autore: Alessandro Meschi
 * Matricola: 525658
 * Email: alessandro.meschi@icloud.com
 *
 * Questo programma è, in ogni sua parte, opera originale dell'autore.
 */

#include <stdlib.h>
#include <errno.h>

#include "config.h"
#include "utils.h"
#include "errors.h"
#include "delivery.h"


/**
 * @file delivery.c
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Contiene le definizioni delle funzioni di creazione e gestione
 * della struttura @see delivery
 * @see delivery.h
 */


/*--------------FUNZIONI DI GESTIONE DELIVERY------------------*/
int initializeDelivery(delivery_t* dvy)
{
    if(dvy == NULL){errno = EINVAL; return -1;}
    // Inizializzazione del contatore
    dvy->active = 0;

    // Inizializzazione del vettore
    for(int i = 0; i < MAX_USER_SIMULTANEUS_CONNECTIONS; i++)
    {
        (dvy->addresses)[i] = -1;
    }

    return 0;
}

int addAddress(delivery_t* dvy, int address)
{
    // Controllo sulla validità dei parametri
    if(dvy == NULL || address <= 0){errno = EINVAL; return -1;}
    // Controllo se il numero massimo di connessioni simultanee è già stato raggiunto
    if(dvy->active == MAX_USER_SIMULTANEUS_CONNECTIONS){return USER_LOGIN_QUOTA_EXCEEDED;}

    int i = 0;
    bool stop = FALSE;

    while(i < MAX_USER_SIMULTANEUS_CONNECTIONS && !stop)
    {
        if((dvy->addresses)[i] == -1)
        {
            (dvy->addresses)[i] = address;
            stop = TRUE;
        }
        else
        {
            if((dvy->addresses)[i] == address)
            {
                errno = EPERM;
                return -1;
            }

            i++;
        }

    }

    dvy->active++;

    return 0;
}


int removeAddress(delivery_t* dvy, int address)
{
    // Controllo sulla validità dei parametri
    if(dvy == NULL || address <= 0){errno = EINVAL; return -1;}
    if(dvy->active <= 0){errno = EPERM; return -1;}

    int i = 0;
    bool stop = FALSE;

    while(i < MAX_USER_SIMULTANEUS_CONNECTIONS && !stop)
    {
        if((dvy->addresses)[i] == address)
        {
            (dvy->addresses)[i] = (dvy->addresses)[dvy->active - 1];
            (dvy->addresses)[dvy->active - 1] = -1;
            stop = TRUE;
        }
        else
            i++;
    }

    if(!stop)
    {// L'inidrizzo non è stato trovato nel vettore
        errno = EPERM;
        return -1;
    }
    else
        dvy->active--;

    return 0;
}
/*--------------------------------------------------------------*/
