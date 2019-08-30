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

#ifndef __FD_SET_H__
#define __FD_SET_H__

#include <sys/select.h>
#include <pthread.h>

/**
 * @file fdSet.h
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Contiene la definizione della struttura @ref fdSet che rappresenta
 * un insieme di file descriptors utilizzati durante la system call select().
 * Contiene la dichiarazione delle funzioni di inizializzazione, gestione e
 * distruzione della struttura @ref fdSet
 * @details La struttura @ref fdSet rappresenta un'estensione del tipo @c fd_set
 * in modo tale da gestire correttamente la concorrenza sulle operazioni
 * di modifica dell'insieme.
 */


/*------------------------DEFINIZIONE FDSET----------------------------*/
/**
 * @struct fdSet
 * @brief Struttura che rappresenta un insieme di file descriptor esteso
 */
struct fdSet
{
    int maxFd; /**< Valore massimo fra i file descriptors appartenenti all'insieme */
    fd_set master; /**< Insieme di file descriptors */
    pthread_mutex_t setLock; /**< Lock che permette le operazioni in mutua esclusione sull'insieme */
};


/**
 * @typedef fdSet_t
 * @brief Definizione del tipo insieme esteso di file descriptors
 */
typedef struct fdSet fdSet_t;
/*---------------------------------------------------------------------*/




/*----------------SET DI FILE DESCRIPTORS DI SISTEMA-------------------*/
/**
 * @brief Variabile globale che rappresenta il set di file descriptor di
 * sistema da ascoltare durante la system call select().
 */
extern fdSet_t masterSet;
/*---------------------------------------------------------------------*/





/*--------------DICHIARAZIONE FUNZIONI GESTIONE FDSET------------------*/
/**
 * @brief Inizializza l'insieme di file descriptors di sistema (@ref masterSet)
 * @retval 0: Se l'inizializzazione è avvenuta correttamente.
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * @see Per i codici di errore vedere il manuale di
 * pthread_mutex_init().
 */
int initializeSet();


/**
 * @brief Inserisce uno specifico file descriptor (@p fd) all'interno del
 * set di file descriptors di sistema (@ref masterSet).
 * @param fd File descriptor che si intende inserire nel set
 * @retval 0: Se l'inserimento è andato a buon fine
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno | Motivo                                  |
 * | :------: | :-------------------------------------: |
 * |@c EINVAL | Almeno uno degli argomenti non è valido |
 *
 */
int INset(int fd);


/**
 * @brief Rimuove uno specifico file descriptor (@p fd) dal set di file
 * descriptors di sistema (@ref masterSet).
 * @param fd File descriptor che si intende rimuovere dal set
 * @retval 0: Se la rimozione è avvenuta con successo
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno | Motivo                                  |
 * | :------: | :-------------------------------------: |
 * |@c EINVAL | Almeno uno degli argomenti non è valido |
 *
 */
int OUTset(int fd);


/**
 * @brief Crea una copia dei campi @ref fdSet.master e @ref fdSet.maxFd  contenuti
 * in @ref masterSet salvandoli
 * rispettivamente nelle aree di memoria puntate dai puntatori @p saveSet, @p saveMaxFd
 * @param saveSet Puntatore all'area di memoria che conterrà la copia
 * di @ref fdSet.master
 * @param saveMaxFd Puntatore all'area di memoria che conterrà la
 * copia de @ref fdSet.maxFd
 * @retval 0: Questa funzione non può fallire
 */
int saveSet(fd_set* saveSet, int* saveMaxFd);


/**
 * @brief Libera l'area di memoria occupata dagli elementi dell'insieme
 * dei file descriptors di sistema (@ref masterSet).
 * @retval 0: Se la conacellazione è avvenuta con successo
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * @see Per i codici di errore vedere il manuale di
 * pthread_mutex_destroy().
 */
int destroySet();
/*---------------------------------------------------------------------*/

#endif
