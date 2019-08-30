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

#ifndef __USER_H__
#define __USER_H__

#include "config.h"

#include <pthread.h>

#include "message.h"
#include "history.h"
#include "delivery.h"
#include "membership.h"
#include "stats.h"

/**
 * @file  users.h
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Contiene le definizioni dei tipi @ref user_t, @ref usersList_t e
 * @ref usersTable_t, le dichiarazioni delle relative funzioni di inizializzazione, gestione
 * e distruzione e la dichiarazione della tabella degli utenti di sistema
 *
 * @details Le strutture definite in questo file sono:\n
 * - La struttura @ref user contiene tutte le informazioni relative ad un utente
 * - La struttura @ref userNode incapsula la struttura @ref user in modo da poterla concatenare
 * - La struttura @ref usersList concatena gli utenti che hannno lo stesso indirizzo hash
 * - La struttura @ref usersTable è una tabella hash che usa le @ref usersList
 * come liste di trabocco.
 *
 * Per gestire la concorrenza sulle operazioni di tabella si utilizza un set di
 * @c pthread_rwlock_t.
 * Ognuna di queste consente l'accesso in mutua esclusione ad un numero fissato
 * (proporzionale alla dimensione dela tabella @ref USERS_MUTEX_BLOCK_SIZE) di
 * righe adiacenti della tabella.
 */



/*---------------------------------UTENTE-------------------------------*/
/**
 * @struct user
 * @brief Struttura che definisce uno specifico utente.
 */
struct user
{
    char* nickName; /**< Nickname dell'utente*/
    delivery_t distribution; /**< Vettore degli indirizzi associata all'utente */
    membership_t groups; /**< Lista dei gruppi a cui l'utente appartiene*/
    history_t userIncomeHistory; /**< Storico dei messaggi ricevuti dall'utente*/
};

/**
 * @typedef user_t
 * @brief Definizione del tipo utente.
 */
typedef struct user user_t;
/*----------------------------------------------------------------------*/




/*------------------------------LISTA UTENTI----------------------------*/
/**
 * @struct userNode
 * @brief Struttura che definisce un nodo della lista utenti.
 */
struct userNode
{
    user_t user; /**< Utente (@ref user_t) contenuto nel nodo */
    struct userNode* next; /**< Puntatore all prossimo utente nella lista */
    struct userNode* prev; /**< Puntatore all'utente precedente nella lista */
};

/**
 * @typedef userNode_t
 * @brief Definizione del tipo nodo di una lista utenti.
 */
typedef struct userNode userNode_t;

/**
 * @struct usersList
 * @brief Struttura che definisce una lista concatenata di @ref userNode_t
 */
struct usersList
{
    userNode_t* head; /**< Puntatore alla testa della lista*/
    int loadFactor; /**< Contatore di utenti che risiedono nella coda*/
};

/**
 * @typedef usersList_t
 * @brief Definizione del tipo lista di utenti.
 */
typedef struct usersList usersList_t;
/*----------------------------------------------------------------------*/




/*------------------------------TABELLA UTENTI--------------------------*/
/**
 * @struct usersTable
 * @brief Struttra che definisce una tabella utenti
 */
struct usersTable
{
    usersList_t* table; /**< Vettore di liste di utenti (tabella effettiva)*/
    pthread_rwlock_t* blockLocks; /**< Set di locks per i blocchi della tabella*/
};

/**
 * @typedef usersTable_t
 * @brief Definizione del tipo tabella di utenti
 */
typedef struct usersTable usersTable_t;
/*-----------------------------------------------------------------------*/




/*--------------------TABELLA DEGLI UTENTI GLOBALE-----------------------*/
/**
 * @brief Variabile locale che si riferisce alla tabella dei gruppi di sistema
 */
extern usersTable_t usrTable;
/*-----------------------------------------------------------------------*/




/*-----------------DICHIARAZIONE FUNZIONI UTENTE-------------------------*/
/**
 * @brief Inizializza una nuova struttura @ref user
 * e impostando il nome dell'utente (@ref user.nickName) con
 * la stringa @p nickName passata come argomento.
 * @param usr Puntatore alla struttura @ref user da inizializzare
 * @param nickName Nickname del nuovo utente
 * @retval 0: Se l'inizializzazione è avvenuta con successo
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno | Motivo                                  |
 * | :------: | :-------------------------------------: |
 * |@c EINVAL | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere il manuale di
 * malloc(), calloc(), initializeDelivery(), initializeMembership(), initializeHistory().
 */
int initializeUser(user_t* usr, char* nickName);


/**
 * @brief Funzione  che effettua il confronto fra il nickname di
 * @p user (@ref user.nickName) e la stringa @p nickname
 * @param usr Puntatore alla struttura @ref user di cui si vuole confrontare
 * il nickname con la sringa passata per argomento
 * @param nickname Stringa che si vuole confrontare con il nickname
 * @retval 1: Se il nickname di @p user corrisponde con la stringa @p nickname
 * @retval 0: Se il nickname di @p user non combacia con la stringa @p nickname
 * oppure almeno uno dei due parametri è uguale a @c NULL
 */
int isUser(user_t* usr, char* nickname);


/**
 * @brief Libera la memoria occupata dalla struttura @ref user passata come argomento
 * e da tutti i suoi componenti
 * @param usr Indirizzo del puntatore alla struttura @ref user che si intende cancellare
 * @retval 0: Questa funzione non può fallire.
 * @see Vengono chiamate la funzione destroyHistory().
 */
int destroyUser(user_t* usr);
/*-----------------------------------------------------------------------*/




/*---------------------DICHIARAZIONI FUNZIONI LISTA----------------------*/
/**
 * @brief Inizializza una struttura @ref usersList
 * @param usrList: Puntatore alla lista utenti (@ref usersList) da eliminare
 * @retval 0: Se l'inizializzazione è andata a buon fine
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno | Motivo                                  |
 * | :------: | :-------------------------------------: |
 * |@c EINVAL | Almeno uno degli argomenti non è valido |
 */
int initializeUsersList(usersList_t* usrList);


/**
 * @brief Inserisce un nuovo @ref user in coda alla @ref usersList puntata da @p list
 * controllando che un altro utente con lo stesso nickname non sia già presente nella lista
 * @param list Puntatore alla @ref usersList dove si intende fare l'inserimento.
 * @param nickname Nickname del nuovo @ref user che deve essere aggiunto.
 * @retval 0: Se l'inserimento è avvenuto con successo
 * @retval -2: Se è già presente nella lista un utente con il solito nickname
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno   | Motivo                                   |
 * | :--------: | :--------------------------------------: |
 * |@c EINVAL   | Almeno uno degli argomenti non è valido  |
 *
 * @see Per gli altri codici di errore vedere il manuale di
 * malloc().
 */
int insertUserNode(usersList_t* list, char* nickname);


/**
 * @brief Ricerca, all'interno di @p list (@ref usersList), una struttura @ref userNode
 * che contenga l'utente con il nickname (@ref user.nickName) uguale a quello passato come argomento
 * (@p nickname).
 * @param list Puntatore alla @ref usersList dove si intende effettuare la ricerca.
 * @param nickname Nickname del'@ref user da ricercare
 * @param save Variabile che eventualmente conterrà il risultato della ricerca
 * @retval 1: Se la ricerca ha prodotto risultato e l'indirizzo della @ref user cercata
 * viene salvato in @p save.
 * @retval 0: Se la ricerca non ha prodotto nessun risultato e save viene settato a @c NULL
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno   | Motivo                                  |
 * | :--------: | :-------------------------------------: |
 * |@c EINVAL   | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere il manuale di
 */
int getUserNode(usersList_t* list, char* nickname, userNode_t** save);


/**
 * @brief Rimuove, se presente, una struttura @ref user dalla @ref usersList
 * puntata da @p list, che contenga un @ref user.nickName uguale a @p nickname
 * passato come parametro.
 * @param list Puntatoer alla @ref usersList dalla quale si intende effettuare la rimozione
 * @param nickname Nickname dello @ref user da rimuovere dalla lista
 * @retval 0: Se la rimozione è avvenuta con successo
 * @retval -14: Se l'utente richiesto non risulta nella lista
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno | Motivo                    				|
 * | :------: | :-------------------------------------: |
 * |@c EINVAL | Almeno uno degli argomenti non è valido |
 */
int removeUserNode(usersList_t* list, char* nickname);


/**
 * @brief Libera la memoria occupata dalla struttura @ref usersList passata come argomento
 * e da tutti i sui componenti
 * @param list Puntatore alla struttura @ref usersList che si intende
 * cancellare
 * @retval 0: Questa funzione non può fallire.
 * @see Viene chiamata la funzione destroyUser().
 */
int destroyUsersList(usersList_t* list);
/*-----------------------------------------------------------------------*/




/*---------------------DICHIARAZIONI FUNZIONI TABELLA--------------------*/
/**
 * @brief Funzione di generazione indirizzi hash da stringa
 * (nickname di un utente) a intero (indice in tabella)
 * @details Funzione hash presa dalla versione djb2 di Dan Bernstein
 * @param str Stringa di cui si intende generare l'indirizzo hash
 * @retval index: L'indirizzo hash generato in caso di successo
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno   | Motivo                                  |
 * | :--------: | :-------------------------------------: |
 * |@c EINVAL   | Almeno uno degli argomenti non è valido |
 */
int userHash(unsigned char* str);


/**
 * @brief Inizializza la tabella degli utenti di sistama (@ref usrTable)
 * @retval 0: Se l'inizializzazione è avvenuta correttamente
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 * @see Per i codici di errore vedere il manuale di
 * malloc(), pthread_rwlock_init().
 */
int initializeUsersTable();

/**
 * @brief Libera la memoria occupata dalla tablle degli utenti di sistema (@ref usrTable)
 * e da tutti i sui componenti.
 * @retval 0: Se la cancellazione è avvenuta con successo.
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 * @see Per i altri codici di errore vedere il manuale di
 * pthread_rwlock_destroy().\n
 * Vengono chiamate le funzioni destroyUsersList().
 */
int destroyUsersTable();
/*-----------------------------------------------------------*/

#endif
