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

#ifndef __INCOME_H__
#define __INCOME_H__

#include <pthread.h>

/**
 * @file income.h
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Contiene la definizione del tipo @ref income_t e @ref incomeList_t,
 * l'interfaccia di creazione uso e distruzione della struttura @ref incomeList
 *
 * @details La struttura @ref incomeList consiste in una lista concatenata concorrente.
 * Questa serve al thread principale per memorizzare ogni file descriptor dal quale è
 * possibile effettuare una lettura.
 * Serve invece ai thread secondari (@ref listener) per estrarre file descriptors e quindi
 * cominciare la gestione di una nuova richiesta.
 * Quando il thread principale deve interrompere il dispatchinng delle richieste allora
 * inserirà il valore speciale @ref INCOME_LIST_TERM nella catena.
 */

/*----------------------DEFINIZIONE INCOME-----------------------------*/
/**
 * @struct income
 * @brief Struttura che definisce il nodo della coda di servizio
 */
struct income
{
    int fd; /**< File descriptor contenuto nel nodo*/
    struct income* next; /**< Puntatore al nodo richiesta successivo*/
};


/**
 * @typedef income_t
 * @brief Definizione del tipo nodo della coda di servizio
 */
typedef struct income income_t;
/*---------------------------------------------------------------------*/



/*-------------------DEFINIZIONE INCOME LIST---------------------------*/
/**
 * @struct incomeList
 * @brief Struttura che definisce la coda di servizio
 */
struct incomeList
{
    pthread_mutex_t incomeLock; /**< Lock che permette le operazioni in muta esclusione sulla lista di servizio*/
    pthread_cond_t incomeCond; /**< Variabile condizione sulla quale i threads worker si mettono in attesa di una nuova richiesta*/
    income_t* head; /**< Puntatore alla testa della catena*/
    int length; /**< Variabile che memorizza la lunghezza della catena*/
};


/**
 * @typedef incomeList_t
 * @brief Definizione del tipo coda di servizio
 */
typedef struct incomeList incomeList_t;
/*---------------------------------------------------------------------*/




/*--------------------CODA DI SERVIZIO DI SISTEMA----------------------*/
/**
 * @brief Variabile globale che si riferisce alla coda di servizio di sistema
 */
extern incomeList_t service;
/*---------------------------------------------------------------------*/





/*---------------DICHIARAZIONE FUNZIONI INCOME LIST--------------------*/
/**
 * @brief Inizializza la coda di servizio di sistema (@ref service)
 * @retval 0: Se l'inizializzazione è avvenuta con successo
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * @see Per i codici di errore vedere il manuale di
 * pthread_mutex_init(), pthread_cond_init().
 */
int initializeIncomeList();


/**
 * @brief Inserisce un nuovo nodo nella coda di servizio di sistema (@ref service)
 * segnalandone la presenza ad un thread in attesa su @ref incomeList.incomeCond
  * @param fd File desfriptor che rappresenta la nuova richiesta
 * (un descrittore di file dal quale è possibile leggere)
 * @retval 0: Se l'inserimento è avvenuto con successo
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno | Motivo                                  |
 * | :------: | :-------------------------------------: |
 * |@c EINVAL | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere il manuale di
 * malloc(), pthread_cond_signal()
 */
int pushIncome(int fd);


/**
 * @brief Estrae, se presente, una struttura @ref income dalla lista di servizio @ref service
 * altrimenti si mettte in attesa su @ref incomeList.incomeCond
 * @retval fd: Il file descriptor che rappresenta la richiesta estratta
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno | Motivo                                                                                          |
 * | :------: | :---------------------------------------------------------------------------------------------: |
 * |@c EPERM  | Un Trhead in attesa è stato avvisato della presenza di una richiesta che non risulta nella coda |
 *
 * @see Per gli altri codici di errore vedere il manuale di
 * pthread_cond_wait()
 */
int popIncome();


/**
 * @brief Inserisce un messaggio di terminazione (@ref INCOME_LIST_TERM) in fondo
 * alla coda di servizio @ref service segnalando a tutti i thread in attesa su
 * @ref incomeList.incomeCond la presenza di una nuova richiesta
 * @retval 0: Se l'inserimento è avvenuto correttamente
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * @see Per gli altri codici di errore vedere il manuale di
 * malloc(),  pthread_cond_broadcast().
 */
int breakIncomeList();



/**
 * @brief Libera la memoria occupata dalla coda di servizio di sistema (@ref service)
 * @retval 0: Se la cancellazione è andata a buon fine.
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 * @see Per i codici di errore vedere il manuale di
 * pthread_mutex_destroy(), pthread_cond_destroy()
 */
int destroyIncomeList();
/*---------------------------------------------------------------------*/

#endif
