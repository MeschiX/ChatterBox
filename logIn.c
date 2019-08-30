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
#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#include "utils.h"
#include "errors.h"
#include "settings.h"
#include "logIn.h"

/**
 * @file logIn.c
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Contiene le definizioni delle funzioni di creazione e gestione
 * della struttura @ref logsTable
 * @see logIn.h
 */

/*------------DICHIARAZIONE FUNZIONI GESTIONE LOGINS------------*/
int initializeLogsTable()
{
    int test;

    // Allocazione tabella logIn
    lgTable.strip = (log_t*)malloc((MaxConnections + ThreadsInPool + 1) * sizeof(log_t));
    CHECK_NULL_FATAL_ERROR(lgTable.strip, "Allocando spazio per la tabella dei logIns");

    lgTable.currentLogs = 0;

    test = pthread_mutex_init(&(lgTable.logInOutLock), NULL);
    if (test != 0)
    {
        errno = test;
        CHECK_FATAL_ERROR(-1, "Inizializzando la lock dei logIn/logOut relativa alla tabella dei logIns");
    }

    // Inizializzazione campi tabella
    for(int i=0; i<MaxConnections + ThreadsInPool + 1; i++)
    {
        memset(&((lgTable.strip)[i].username), 0, MAX_NAME_LENGTH);

        (lgTable.strip)[i].active = FALSE;

        test = pthread_rwlock_init(&((lgTable.strip)[i].logLock), NULL);
        if (test != 0)
        {
            errno = test;
            CHECK_FATAL_ERROR(-1, "Inizializzando la lock relativa ad un indirizzo nella tabella dei logIn");
        }
    }

    return 0;
}


int logIn(char* nickname, int address)
{
    if (nickname == NULL || address <= 0){errno = EINVAL; return -1;}

    LOCK(lgTable.logInOutLock);

    // Controllo che non venga superato il numero massimo di logIn contemporanei al server
    if (lgTable.currentLogs >= MaxConnections)
    {
        UNLOCK(lgTable.logInOutLock);
        return LOGIN_QUOTA_EXCEEDED;
    }

    WRITE_LOCK((lgTable.strip)[address].logLock);

    // Controllo che lo slot per il file descriptor richiesto sia libero
    if((lgTable.strip)[address].active)
    {
        RW_UNLOCK((lgTable.strip)[address].logLock);
        UNLOCK(lgTable.logInOutLock);
        errno = EADDRINUSE;
        return -1;
    }
    else
    {
        // Inserimento del nome nello slot
        strcpy((lgTable.strip)[address].username, nickname);
        // Impostazione del flag su attivo
        (lgTable.strip)[address].active = TRUE;
    }

    // Incremento del contatore logIn
    lgTable.currentLogs++;

    RW_UNLOCK((lgTable.strip)[address].logLock);

    UNLOCK(lgTable.logInOutLock);

    return 0;
}


int logOut(int address)
{
    if(address <= 0){errno = EINVAL; return -1;}

    int test;

    LOCK(lgTable.logInOutLock);

    WRITE_LOCK((lgTable.strip)[address].logLock);

    // Controllo che lo slot per il file descriptor richiesto sia attivo
    if(!((lgTable.strip)[address].active))
    {
        RW_UNLOCK((lgTable.strip)[address].logLock);
        UNLOCK(lgTable.logInOutLock);
        return CLIENT_NOT_LOGGED;
    }
    else
    {
        // Rimozione del nome dallo slot
        memset(&((lgTable.strip)[address].username), 0, MAX_NAME_LENGTH);
        // Impostazione del flag su disattivo
        (lgTable.strip)[address].active = FALSE;
    }

    // Decremento del contatore
    lgTable.currentLogs--;

    RW_UNLOCK((lgTable.strip)[address].logLock);

    UNLOCK(lgTable.logInOutLock);

    // Chiusura del filedescriptor associato
    test = close(address);
    CHECK_FATAL_ERROR(test, "Chiudendo il file descriptor relativo all'indirizzo");

    return 0;
}


int destroyLogsTable()
{
    int test;

    // Distruzione della lock per i logIn/logOut
    test = pthread_mutex_destroy(&(lgTable.logInOutLock));
    if (test != 0)
    {
        errno = test;
        CHECK_FATAL_ERROR(-1, "Distruggendo la lock di logIn/logOut relativa alla tabella dei logIn");
    }

    // Distruzione dei campi della tabella
    for (int i = 0; i < MaxConnections + ThreadsInPool + 1; i++)
    {
        test= pthread_rwlock_destroy(&((lgTable.strip)[i].logLock));
        if (test != 0)
        {
            errno = test;
            CHECK_FATAL_ERROR(-1, "Distruggendo la lock relativa all'indirizzo nella tabella dei logIn");
        }

        if((lgTable.strip)[i].active)
        {
            test = close(i);
            CHECK_FATAL_ERROR(test, "Chiudendo il file descriptor relativo all'indirizzo durante la distruzione della tabella di logIn");
            memset(&((lgTable.strip)[i].username), 0, MAX_NAME_LENGTH + 1);
        }
    }

    // Distruzione della tabella
    free(lgTable.strip);

    return 0;
}
/*--------------------------------------------------------------*/
