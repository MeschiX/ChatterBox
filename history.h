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

#ifndef __HISTORY_H__
#define __HISTORY_H__


#include "utils.h"
#include "message.h"

/**
 * @file history.h
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Contiene la definizione del tipo @ref history_t e le dichiarazioni
 * delle funzioni per la creazione, gestione e distruzione dello storico messaggi
 *
 * @details Una struttura history verrà associata ad un utente (@ref user) e
 * consiste in un vettore di messaggi limitato dalla variabile globale @ref MaxHistMsgs.
 * Questo deve contenere gli ultimi @ref MaxHistMsgs messaggi destinati all'utente a cui è
 * associata la struttura stessa.
 */



/*---------------------------HISOTRY--------------------------*/
/**
 * @struct old
 * @brief Struttura che definisce un elemento del vettore delo storico dei messaggi
 */
struct old
{
    message_t* mex; /**< Puntatore al messaggio */
    bool delivered; /**< Flag di ricezione da parte del destinatario*/
};


/**
 * @typedef old_t
 * @brief Definizione del tipo messaggio in storico
 */
typedef struct old old_t;


/**
 * @struct history
 * @brief Struttura che definisce lo storico dei messaggi
 */
struct history
{
    short last; /**<  Indice dell'ultimo messaggio inserito*/
    short loadFactor; /**< Numero dei messaggi attualmente presenti nello storico*/
    old_t* cronology; /**< Vettore di messaggi in storico*/
};


/**
 * @typedef history_t
 * @brief Definizione del tipo storico dei messaggi
 */
typedef struct history history_t;
/*------------------------------------------------------------*/




/*------------------FUNZIONI GESTIONE HISTORY-------------------*/
/**
 * @brief Inizializza una struttura @ref history vuota
 * @param hist Puntatore alla struttura @ref history da inizializzare
 * @retval 0: Se l'inizializzazione è avvenuta correttamente
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                  |
 * | :----------: | :-------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere il manuale di
 * malloc().
 */
int initializeHistory(history_t* hist);


/**
 * @brief Inserisce un nuovo messaggio in coda allo storico di messaggi
 * @ref history.cronology di @p hist
 * @details Se all'interno dello storico di @p hist sono presenti @ref MaxHistMsgs messaggi
 * allora verrà cancellato il messaggio che risiede nello storico da più tempo
 * per lasciare un posto libero nello storico per il nuovo messaggio @p income.
 * @param hist Puntatore alla struttutra @ref history che contiene lo
 * storico dei messaggi @ref history.cronology dove si intande effettuare l'inserimento.
 * @param delivered Flag che indica se il messaggio è stato effettivamente inviato
 * allo @ref user corrispondente a @p hist.
 * @param income Nuovo messaggio che si intende inserire nello storico.
 * @retval 0: Se l'inserimento è avvenuto con successo
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                  |
 * | :----------: | :-------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere il manuale di
 * malloc().
 */
int pushMessage(history_t* hist, message_t* income, bool delivered);


/**
 * @brief Libera la memoria occupata dalla struttura @ref history passata come argomento
 * @param hist Indirizzo del puntatore alla struttura @ref history
 * che si intende eliminare
 * @retval 0: Questa funzione non può fallire
 */
int destroyHistory(history_t* hist);
/*--------------------------------------------------------------*/

#endif
