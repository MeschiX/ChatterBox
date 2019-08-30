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

#ifndef __DELIVERY_H__
#define __DELIVERY_H__

#include "config.h"


/**
 * @file delivery.h
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Contiene la definizione del tipo @ref delivery_t e le dichiarazioni
 * delle funzioni di creazione, gestione e distruzione del vettore di
 * indirizzi.
 *
 * @details Una struttura @ref delivery verrà associata ad un utente (@ref user) e
 * consiste in un vettore di interi corrispondenti ai file descriptor sui quali
 * i clients sono connessi al server e che hanno effettuato il login (@ref logInUser)
 * al servizio usando il nome del suddetto utente.
 * Il numero massimo di client simultanei per un utente è definito dalla
 * macro @ref MAX_USER_SIMULTANEUS_CONNECTIONS in @ref config.h
 */

/*--------------------------DEFINIZIONE DELIVERY---------------------------*/
/**
 * @struct delivery
 * @brief Struttura che definisce il vettore degli indirizzi
 */
struct delivery
{
    long addresses[MAX_USER_SIMULTANEUS_CONNECTIONS]; /**< Vettore degli indirizzi */
    short active; /**< Contatore dei clients loggati con l'utente a cui la struttura fa riferimento */
};

/**
 * @typedef delivery_t
 * @brief Definizione del tipo lista di indirizzi
 */
typedef struct delivery delivery_t;
/*--------------------------------------------------------------*/



/*--------------FUNZIONI DI GESTIONE DELIVERY------------------*/
/**
 * @brief Inizializza una nuova struttura @ref delivery vuota
 * @param dvy Puntatore alla struttura @ref delivery da inizializzare
 * @retval 0: Se l'inizializzazione è avvenuta con successo
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                  |
 * | :----------: | :-------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido |
 *
 */
int initializeDelivery(delivery_t* dvy);


/**
 * @brief Inserisce un nuovo indirizzo in coda al vettore di indirizzi
 * @ref delivery.addresses di @p delivery
 * @param dvy Puntatore alla struttura @ref delivery che contiene
 * il vettore indirizzi nel quale si intende effettuare l'inserimento.
 * @param addr Nuovo file descriptor da aggiungere nel vettore indirizzi
 * @retval 0: Se l'inserimento è avvenuto con successo
 * @retval -6: Se il numero massimo di logins simultanei per l'utente è
 * già stato raggiunto (@ref MAX_USER_SIMULTANEUS_CONNECTIONS)
 * @retval -1: Se si è verificato un altro errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno  | Motivo                                                              |
 * | :-------: | :-----------------------------------------------------------------: |
 * |@c EINVAL  | Almeno uno degli argomenti non è valido                             |
 * |@c EPERM   | Il file descriptor @p address è già presente nel vettore indirizzi	 |
 *
 * @see Per gli altri codici di errore vedere il manuale di
 * malloc().
 */
int addAddress(delivery_t* dvy, int addr);


/**
 * @brief Rimuove un indirizzo specifico dal vettore di indirizzi
 * @ref delivery.addresses di @p delivery
 * @param dvy Puntatore alla struttura @ref delivery che contiene
 * il vettore indirizzi dal quale si intende effettuare la rimozione.
 * @param addr File descriptor da rimuovere dal vettore indirizzi
 * @retval 0: Se la rimozione è avvenuta con successo
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                                          |
 * | :----------: | :-------------------------------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido                         |
 * |@c EPERM      | Il file descriptor @p addr non è presente nel vettore indirizzi |
 */
int removeAddress(delivery_t* dvy, int addr);
/*--------------------------------------------------------------*/

#endif
