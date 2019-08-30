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
#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>

/**
 * @file utils.h
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Contiene la dichiarazione del tipo booleano, dichiarazione di
 * macro per l' acquisizione e il rilascio di lock e la dichiarazione di altre
 * macro e funzioni utili per la colorazione dell'output su terminale.
 *
 * @details La colorazione dell'output che viene implementata in queste funzioni
 * non è una funzionalità perfettamente portabile perciò possibile
 * attivarla e disattivarla tramite opzione @c -c da riga di comando al momento
 * del lancio del programmma.
 */


/*---------------------DEFINIZIONE COLORAZIONE OUTPUT-----------------------*/
/**
 * @brief Sequenza di reset del formato dell'output
 */
#define RESET   "\033[0m"

/**
 * @brief Sequenza che rende in *grassetto* i prossimi caratteri in output
 */
#define BOLD    "\033[1m"

/**
 * @brief Sequenza che rende in *rosso* i prossimi caratteri in output
 */
#define RED     "\033[31m"

/**
 * @brief Sequenza che rende in *verde* i prossimi caratteri in output
 */
#define GREEN   "\033[32m"

/**
 * @brief Sequenza che rende in *giallo* i prossimi caratteri in output
 */
#define YELLOW  "\033[33m"

/**
 * @brief Sequenza che rende in *blu* i prossimi caratteri in output
 */
#define BLUE    "\033[34m"

/**
 * @brief Sequenza che rende in *magenta* i prossimi caratteri in output
 */
#define MAGENTA "\033[35m"

/**
 * @brief Sequenza che rende in *ciano* i prossimi caratteri in output
 */
#define CYAN    "\033[36m"

/**
 * @brief Sequenza che rende in *bianco* i prossimi caratteri in output
 */
#define WHITE   "\033[37m"

/**
 * @brief Sequenza che rende in *grassetto rosso* i prossimi caratteri in output
 */
#define BOLDRED     "\033[1m\033[31m"

/**
 * @brief Sequenza che rende in *grassetto verde* i prossimi caratteri in output
 */
#define BOLDGREEN   "\033[1m\033[32m"

/**
 * @brief Sequenza che rende in *grassetto giallo* i prossimi caratteri in output
 */
#define BOLDYELLOW  "\033[1m\033[33m"

/**
 * @brief Sequenza che rende in *grassetto blu* i prossimi caratteri in output
 */
#define BOLDBLUE    "\033[1m\033[34m"

/**
 * @brief Sequenza che rende in *grassetto magenta* i prossimi caratteri in output
 */
#define BOLDMAGENTA "\033[1m\033[35m"

/**
 * @brief Sequenza che rende in *grassetto ciano* i prossimi caratteri in output
 */
#define BOLDCYAN    "\033[1m\033[36m"

/**
 * @brief Sequenza che rende in *grassetto bianco* i prossimi caratteri in output
 */
#define BOLDWHITE   "\033[1m\033[37m"
/*--------------------------------------------------------------------------*/



/*---------------------ACQUISIZIONE E RILASCIO LOCKS------------------------*/
/**
 * @brief Macro di abbreviazione dell'operazione di acquisizione di una
 * @c pthread_mutex_t
 */
#define LOCK(lock)                  \
    pthread_mutex_lock(&(lock));

/**
 * @brief Macro di abbreviazione dell'operazione di rilascio di una
 * @c pthread_mutex_t
 */
#define UNLOCK(lock)                \
    pthread_mutex_unlock(&(lock));

/**
 * @brief Macro di abbreviazione dell'operazione di acquisizione di una
 * @c pthread_rwlock_t in lettura
 */
#define READ_LOCK(lock)             \
    pthread_rwlock_rdlock(&(lock));

/**
 * @brief Macro di abbreviazione dell'operazione di acquisizione di una
 * @c pthread_rwlock_t in scrittura
 */
#define WRITE_LOCK(lock)            \
    pthread_rwlock_wrlock(&(lock));

/**
 * @brief Macro di abbreviazione dell'operazione di rilascio di una
 * @c pthread_rwlock_t
 */
#define RW_UNLOCK(lock)            \
    pthread_rwlock_unlock(&(lock));
/*--------------------------------------------------------------------------*/



/*--------------------------DICHIARAZIONE TIPI------------------------------*/
/**
 * @enum bool_e
 * @brief Enumerazione per rappresentare il tipo booleano
 */
enum bool_e
{
    FALSE, /**< Rappresenta 0*/
    TRUE /**< Rappresenta 1*/
};


/**
 * @typedef bool
 * @brief Definizione del tipo booleano attraverso @ref bool_e
 * @see bool_e
 */
typedef enum bool_e bool;


/**
 * @enum color
 * @brief Enumerazione per distinguere i colori dell'output
 * durante le varie chiamate di stampa.
 * @details Offre la possibilità di mantenere una sola funzione
 * di stampa che prende una di queste alternative come parametro
 */
enum color
{
    eNORMAL,       /**< Normale */
    eBOLD,         /**< Grassetto */
    eRED,          /**< Rosso */
    eGREEN, 	   /**< Verde */
    eYELLOW, 	   /**< Giallo */
    eBLUE, 	       /**< Blu */
    eMAGENTA, 	   /**< Magenta */
    eCYAN, 	       /**< Ciano */
    eWHITE, 	   /**< Bianco */
    eBOLDRED, 	   /**< Grassetto rosso */
    eBOLDGREEN,    /**< Grassetto verde */
    eBOLDBLUE, 	   /**< Grassetto blu */
    eBOLDCYAN, 	   /**< Grassetto ciano */
    eBOLDWHITE,	   /**< Grassetto bianco */
    eBOLDYELLOW,   /**< Grassetto giallo */
    eBOLDMAGENTA   /**< Grassetto magenta */
};


/**
 * @typedef color_t
 * @brief Definizione del tipo colore dell'output @ref color
 */
typedef enum color color_t;
/*----------------------------------------------------------------------*/




/*-----------------------FLAG DI COLORAZIONE----------------------------*/
/**
 * @var colorful
 * @brief Flag che determina l'abilitazione dell'output colorato
 * (disabilitato di default)
 * @details Impostabile da riga di comando al momento del
 * lancio del programma con l'opzione @c -c
 */
extern bool colorful;
/*----------------------------------------------------------------------*/



/*-----------------------FUNZIONI DI UTILITA'---------------------------*/
/**
 * @brief Copia in formato *array di bytes* un qualsiasi dato in un buffer (@c char*)
 * @param data Dato da copiare
 * @param size Dimensione del dato da copiare
 * @param buff Puntatore al buffer destinazione
 * @retval N: Il numero di bytes copiati nel caso che la copia sia
 * avvenuta con successo
 * @retval -1: Se si è verificato un errore
 *
 * | @c errno | Motivo                                  |
 * | :------: | :-------------------------------------: |
 * |@c EINVAL | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere il manuale di
 * malloc().
 */
int copyToBinary(void* data, size_t size, char** buff);


/**
 * @brief Funzione di servizio che cancella ricorsivamente tutti i file e sotto cartelle
 * all'interno della cartella identificata dal path passato come argomento,
 * infine elimina la cartella radice.
 * @param path Path della cartella da eliminare
 * @retval 0: L'eliminazione è avvenuta con successo
 * @retval -1: Se si è verificato un errore e viene settata
 * opportunamente la variabile @c errno
 *
 * | @c errno | Motivo                                  |
 * | :------: | :-------------------------------------: |
 * |@c EINVAL | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere il manuale di
 * opendir(), readdir(), malloc() stat().
 */
int removeDir(char* path);


/**
 * @brief Funzione di servizio che estrae il nome del file dal
 * path completo o relativo passato come argomento e lo salva
 * nel puntatore @p save passato come argomento.
 * @warning La funzione non effettua nessun controllo sull'esistenza
 * del file o sulla sua natura (directory, files speciali ecc...).
 * In realtà si limita a troncare la stringa passata come argomento
 * dall'ultimo carattere "/" in poi.
 * @param path Path del file da cui si intende estrarre il nome
 * @param save Puntatore dove verrà salvata la stringa corrispondente
 * al nome del file
 * @retval 0: Se l'estrazione è andata a buon fine
 * @retval -1: Se si è verificato un errore e viene settata
 * opportunamente la variabile @c errno
 *
 * | @c errno | Motivo                                  |
 * | :------: | :-------------------------------------: |
 * |@c EINVAL | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere il manuale di
 * calloc().
 */
int getNameFromPath(char* path, char** save);
/*----------------------------------------------------------------------*/




/*-----------------FUNZIONI GENERAZIONE DI OUTPUT COLORATO--------------*/
/**
 * @brief Funzione di servizio che permette la stampa di caratteri colorati e/o
 * in grassetto su terminale.
 * @details La stampa con stile viene effettuata solo se la variabile eseterna
 * @ref colorful è settata a TRUE, altrimenti la stampa avviene con lo stile
 * predefinito. La funzione prende un numero variabile di argomenti a seconda
 * dei dati che si intende stampare (come nella @c printf).
 * @param format Formato dela stringa da stampare (come nella @c printf)
 * @param c Colorazione e stile (@ref color) che si intende applicare alla
 * stringa @p format
 * @retval N: Il numero di bytes stampati in caso di successo,
 * @retval -1: Se si è verificato un errore
 *
 * | @c errno | Motivo                                  |
 * | :------: | :-------------------------------------: |
 * |@c EINVAL | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere il manuale di
 * va_start(), va_end(), calloc(), vprintf().
 */
int print(char* format, color_t c, ...);

/**
 * @brief Implementa la solita funzione di print() solo che, invece
 * di un numero variabile di argomenti, prende una @p va_list
 * @details La stampa con stile viene effettuata solo se la variabile eseterna
 * @ref colorful è settata a TRUE, altrimenti la stampa avviene con lo stile
 * predefinito. La funzione prende un numero variabile di argomenti a seconda
 * dei dati che si intende stampare (come nella @c printf).
 * @param format Formato dela stringa da stampare (come nella @c printf)
 * @param c Colorazione e stile (@ref color) che si intende applicare alla
 * stringa @p format
 * @param argList Lista degli argomenti di tipo @p va_list
 * @retval N: Il numero di bytes stampati in caso di successo,
 * @retval -1: Se si è verificato un errore
 *
 * | @c errno | Motivo                                  |
 * | :------: | :-------------------------------------: |
 * |@c EINVAL | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere il manuale di
 * calloc(), vprintf().
 */
int vprint(char* format, color_t c, va_list argList);


/**
 * @brief Come la funzione print() questa funzione permette la stampa di output
 * colorato su terminale anteponendo però un identificatore di thread con
 * una formattazione specifica.
 * @details La stampa con stile viene effettuata solo se la variabile eseterna
 * @ref colorful è settata a TRUE, altrimenti la stampa avviene con lo stile
 * predefinito. La funzione prende un numero variabile di argomenti a seconda
 * dei dati che si intende stampare (come nella @c printf).
 * @param ID Identificatore del thread
 * @param format Formato dela stringa da stampare (come nella @c printf)
 * @param c Colorazione e stile (@ref color) che si intende applicare alla
 * stringa @p format
 * @retval N: Il numero di bytes stampati in caso di successo,
 * @retval -1: Se si è verificato un errore
 *
 * | @c errno | Motivo                                  |
 * | :------: | :-------------------------------------: |
 * |@c EINVAL | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere il manuale di
 * va_start(), va_end(), calloc(), vprintf().
 */
int threadPrint(int ID, char* format, color_t c, ...);


/**
 * @brief Implementa la solita funzione di threadPrint() solo che, invece
 * di un numero variabile di argomenti, prende una @p va_list
 * @details La stampa con stile viene effettuata solo se la variabile eseterna
 * @ref colorful è settata a TRUE, altrimenti la stampa avviene con lo stile
 * predefinito. La funzione prende un numero variabile di argomenti a seconda
 * dei dati che si intende stampare (come nella @c printf).
 * @param ID Identificatore del thread
 * @param format Formato dela stringa da stampare (come nella @c printf)
 * @param c Colorazione e stile (@ref color) che si intende applicare alla
 * stringa @p format
 * @param argList Lista degli argomenti di tipo @p va_list
 * @retval N: Il numero di bytes stampati in caso di successo,
 * @retval -1: Se si è verificato un errore
 *
 * | @c errno | Motivo                                  |
 * | :------: | :-------------------------------------: |
 * |@c EINVAL | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere il manuale di
 * calloc(), vprintf().
 */
int vthreadPrint(int ID, char* format, color_t c, va_list argList);
/*----------------------------------------------------------------------*/


#endif
