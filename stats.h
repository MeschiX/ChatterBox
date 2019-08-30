/*
 * chatterbox Progetto del corso di LSO 2017/2018
 *
 * Dipartimento di Informatica Università di Pisa
 * Docenti: Prencipe, Torquati
 *
 */
#if !defined(MEMBOX_STATS_)
#define MEMBOX_STATS_

#include "config.h"

#include <stdio.h>
#include <time.h>
#include <pthread.h>

/**
 * @file stats.h
 * @author Alessandro Meschi
 * @date 7 Maggio 2019
 * @brief Contiene la definizione della struttura statustiche di sistema
 * e della funzione che ne stampa i valori fu file
 */

/**
 * @struct statistics
 * @brief Struttura che definisce le statistiche di sistema
 */
struct statistics {
    pthread_rwlock_t statsLock;                 /**< Lock per le operazioni in mutua esclusione sulle statistiche */
    unsigned long nusers;                       /**< n. di utenti registrati */
    unsigned long nonline;                      /**< n. di utenti connessi */
    unsigned long ndelivered;                   /**< n. di messaggi testuali consegnati */
    unsigned long nnotdelivered;                /**< n. di messaggi testuali non ancora consegnati */
    unsigned long nfiledelivered;               /**< n. di file consegnati */
    unsigned long nfilenotdelivered;            /**< n. di file non ancora consegnati */
    unsigned long nerrors;                      /**< n. di messaggi di errore */
};


/**
 * @brief Variabile globale che si riferisce alle statistiche del server
 */
extern struct statistics chattyStats;


/**
 * @function printStats
 * @brief Stampa le statistiche nel file passato come argomento
 * @param fout Descrittore del file aperto in append.
 * @return 0 in caso di successo, -1 in caso di fallimento
 */
static inline int printStats(FILE *fout) {

    pthread_rwlock_rdlock(&(chattyStats.statsLock));

    if (fprintf(fout, "%ld - %ld %ld %ld %ld %ld %ld %ld\n",
		(unsigned long)time(NULL),
		chattyStats.nusers,
		chattyStats.nonline,
		chattyStats.ndelivered,
		chattyStats.nnotdelivered,
		chattyStats.nfiledelivered,
		chattyStats.nfilenotdelivered,
		chattyStats.nerrors
		) < 0) return -1;

    pthread_rwlock_unlock(&(chattyStats.statsLock));

    fflush(fout);

    return 0;
}

#endif /* MEMBOX_STATS_ */
