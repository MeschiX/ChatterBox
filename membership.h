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

#ifndef __MEMBERSHIP_H__
#define __MEMBERSHIP_H__

#include <stdlib.h>
#include <errno.h>

#include "config.h"

/**
 * @file membership.h
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Contiene la definizione del tipo @ref membership_t e le dichiarazioni
 * delle funzioni di creazine e gestione del vettore di appartenenza ai
 * gruppi
 *
 * @details Una struttura @ref membership sarà associata ad ogni utente (@ref user)
 * registrato al sistema e consiste in un vettore di stringe corrispondenti ai nomi
 * dei gruppi (@ref group) che sono stati creati e risiedono nel sistema (@ref group).
 * Il numero massimo di gruppi a cui un utente può appartenere simultaneamente è
 * definito dalla macro @ref MAX_USER_SIMULTANEUS_GROUPS in @ref config.h
 */


/*--------------------------DEFINIZIONE MEMBERSHIP------------------------*/
/**
 * @struct membership
 * @brief Struttura che definisce il vettore di appartenenza ai gruppi
 */
struct membership
{
    char set[MAX_USER_SIMULTANEUS_GROUPS][MAX_NAME_LENGTH]; /**< Vettore di appartenenza ai gruppi*/
    short belonging; /**< Contatore degli elemti presenti nel vettore */
};


/**
 * @typedef membership_t
 * @brief Definizione del tipo vettore di appartenenza ai gruppi
 */
typedef struct membership membership_t;
/*-----------------------------------------------------------------------*/




/*------------------DEFINIZIONE FUNZIONI DELIVERY------------------------*/
/**
 * @brief Inizializza una nuova struttura @ref membership vuota
 * @param membersh Puntatore alla struttura @ref membership da inizializzare
 * @retval 0: Se l'inizializzazione è avvenuta correttamente
 * @retval -1: Se si è erificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                  |
 * | :----------: | :-------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido |
 *
 */
int initializeMembership(membership_t* membersh);


/**
 * @brief Inserisce il nome di un gruppo all'interno del vettore di appartenenza
 * @ref membership.set di @p membersh
 * @param membersh Puntatore alla struttura @ref membership che contiene il vettore
 * indirizzi nel quale si intende effettuare l'inserimento.
 * @param grName Nome del gruppo da aggiungere al vettore indirizzi
 * @retval 0: Se l'inserimento è avvenuto correttamente
 * @retval -9: Se il numero massimo di associazioni a gruppi per l'utente
 * è stato raggiunto (@ref MAX_USER_SIMULTANEUS_GROUPS)
 * @retval -1: Se si è verificato un altro errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno | Motivo                                                                   |
 * | :------: | :----------------------------------------------------------------------: |
 * |@c EINVAL | Almeno uno degli argomenti non è valido                                  |
 * |@c EPERM  | Il nome @p grName è gia presente nel vettore di appartenenza @p membersh |
 */
int association(membership_t* membersh, char* grName);


/**
 * @brief Rimuove il nome di un certo gruppo dal vettore di appartenenza
 * @ref membership.set di @ref membership
 * @param membersh Puntatore alla struttura @ref membership che contiene il vettore
 * indirizzi nel quale si intende effettuare la rimozione.
 * @param grName Nome del gruppo da rimuovere dal vettore indirizzi.
 * @retval 0: Se la rimozione è avvenuta con successo
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                                      |
 * | :----------: | :---------------------------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido                     |
 * |@c EPERM      | Il groupName @p grName non è presente nel vettore indirizzi |
 */
int dissociation(membership_t* membersh, char* grName);
/*-----------------------------------------------------------------------*/

#endif
