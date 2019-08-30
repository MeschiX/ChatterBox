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

#ifndef __LOGINS_H__
#define __LOGINS_H__

#include "config.h"

#include <pthread.h>

#include "utils.h"
#include "settings.h"

/**
 * @file logIn.h
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Contiene la definizione del tipo @ref logsTable_t e le dichiarazioni
 * delle funzioni di creazione, gestione e distruzione della tabella dei logIns.
 *
 * @details La struttura @ref log rappresenta un'istanza di login da parte di un client
 * infatti essa raccoglie l'username di un utente registrato al sistema.
 * La struttura @ref logsTable invece raccoglie tutte le istanze di login
 * organizzandole in una tabella hash nella quale si accede attraverso il file descriptor
 * del socket di connessione con il quale un client è loggato nel sistema
 * (che funge da chiave considerando un hashing diretto)
 * collegato allo username dell'utente con il quale il client a fatto accesso
 * La sua dimensione è il doppio del valore contenuto nella variabile globale
 * @ref MaxConnections definita in @ref settings.h.
 * La dimensione viene impostata al doppio di @ref MaxConnections per avere
 * un certo margine di valori possibili per i files descriptors dato che l'assegnamento
 * di tali valori non è predicibile, soprattutto perché durante
 * l'esecuzione del programma vengo aperti e chiusi file in modo non deterministico.
 * Per gestire la concorrenza sulle operazioni di tabella si utilizza un set di
 * @c pthread_rwlock_t, una per ogni riga della tabella e un'ulteriore @c pthread_mutex_lock
 * (@ref logsTable.logInOutLock) per garantire che il login/logout avvengano uno per volta.
 */

/*--------------------------DEFINIZIONE LOG------------------------------*/
/**
 * @struct log
 * @brief Struttura che rappresenta il logIn di un client tramite username
 * al servizio offerto dal server
 */
struct log
{
    bool active; /**< Flag di attivazione */
    char username[MAX_NAME_LENGTH]; /**< Nickname dell'utente che usa questo questo login */
    pthread_rwlock_t logLock; /**< Lock che permette le operazioni in muta esclusione sull'istanza di login e sulle letture/scitture sul file descriptor collegato */
};

/**
 * @typedef log_t
 * @brief Definizione del tipo log
 */
typedef struct log log_t;
/*-----------------------------------------------------------------------*/



/*----------------------DEFINIZIONE LOGSTABLE----------------------------*/
/**
 * @struct logsTable
 * @brief Struttura che rappresenta l'elenco dei logIn al server
 */
struct logsTable
{
    pthread_mutex_t logInOutLock; /**< Lock che impodesce operazioni di logIn o logOut simultanee */
    int currentLogs; /**< Contatore dei logIn attuali */
    log_t* strip; /**< Vettore di strutture @ref log */
};

/**
 * @typedef logsTable_t
 * @brief Definizione del tipo elenco di logIns
 */
typedef struct logsTable logsTable_t;
/*-----------------------------------------------------------------------*/


/*------------------TABELLA DEI LOGINS GLOBALE---------------------------*/
/**
 * @brief Variabile globale che si riferisce alla tabella dei login di sistema
 */
extern logsTable_t lgTable;
/*-----------------------------------------------------------------------*/



/*-------------------DICHIARAZIONE FUNZIONI LOGS TABLE-------------------*/
/**
 * @brief Inizializza la tabella dei logins(@ref logsTable) di sistema (@ref lgTable)
 * @retval 0: Se l'inizializzazione è avvenuta correttamente
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 * @see Per i codici di errore vedere il manuale di
 * pthread_rwlock_init(), pthread_mutex_init().
 */
int initializeLogsTable();


/**
 * @brief Inserisce il nume di un utente (@ref user) nella tabella dei login
 * di sistema @ref lgTable occupando lo slot relativo al file descriptor
 * con il quale il client loggato con quell'utente è connesso.
 * @param nickname Nickname del utente da loggare al servizio
 * @param address File descriptor con cui il client che chiede il login
 * è connesso al server
 * @retval 0: Se l'inserimento è avvenuto con successo
 * @retval -5: Se il numero massimo di login nel sistema è stato raggiunto
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno       | Motivo                                   |
 * | :------------: | :--------------------------------------: |
 * |@c EINVAL       | Almeno uno degli argomenti non è valido  |
 * |@c EADDRINUSE   | L'indirizzo @p address risulta in uso    |
 *
 */
int logIn(char* nickname, int address);


/**
 * @brief Rimuove il nuckname di un utente dalla tabella dei login di sistema
 * @ref lgTable e rende di nuovo disponibile lo slot della tabella reativo
 * al file descriptor sul quale era precedentemente connesso l'utente.
 * @param address File descriptor con cui il client si è disconnesso dal server
 * @retval 0: Se la rimozione è avvenuta con successo
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                                             |
 * | :----------: | :----------------------------------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido                            |
 * |@c EPERM      | Il file descriptor @p address non è presente in @p logs->strip     |
 *
 * @see Per gli altri codici di errore vedere il manuale di
 * close().
 */
int logOut(int address);


/**
 * @brief Libera lo spazio in memoria occupato dalla una struttura @ref logsTable
 * pssata come argomento e chiude tutti i file descriptor che risultano attivi.
 * @retval 0: Se l'eliminazione è avvenuta con successo
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 * @see Per i codici di errore vedere il manuale di
 * pthread_mutex_destroy(), close().
 */
int destroyLogsTable();
/*-----------------------------------------------------------------------*/

#endif
