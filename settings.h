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


#ifndef ___SETTINGS_H__
#define ___SETTINGS_H__

#include "config.h"

/**
 * @file settings.h
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Vengono dichiarate alcune variabili per il salvataggio
 * dei valori di impostazione del server e alcune funzioni
 * per effettuare il settaggio e la stampa di tali variabili.
 *
 * @details La funzioni dichiarate in questo header file fanno
 * riferimento ai file di configurazione del programma i quali sono
 * composti da righe di commento che iniziano per '\#' e da rige
 * di dichiarazione che possono iniziare con alcuni spazi e/o
 * tabulazioni seguite dal nome del campo di impostazione seguito
 * dal carattere '@c = ' seuito dal valore da associare a tale impostazione.\n
 * Per esempio:\n
 * @verbatim ####### Qesta è una riga di commento ####### @endverbatim
 * @verbatim RigaDiImpostazione = Valore  @endverbatim
 */

/* -------------------- VARIABILI DI CONFIGURAZIONE -------------------------- */
/**
 * @var UnixPath
 * @brief Path utilizzato per la creazione del socket AF_UNIX
 */
extern char UnixPath[UNIX_PATH_MAX];

/**
 * @var DirName
 * @brief Directory dove memorizzare i files da inviare agli utenti
 */
extern char DirName[UNIX_PATH_MAX];

/**
 * @var StatFileName
 * @brief File nel quale verranno scritte le statistiche del server
 */
extern char StatFileName[UNIX_PATH_MAX];

/**
 * @var MaxConnections
 * @brief Numero massimo di connessioni pendenti (vedi @ref log)
 */
extern int MaxConnections;

/**
 * @var ThreadsInPool
 * @brief Numero di thread nel pool
 */
extern int ThreadsInPool;

/**
 * @var MaxMsgSize
 * @brief Dimensione massima di un messaggio testuale (numero di caratteri)
 */
extern int MaxMsgSize;

/**
 * @var MaxFileSize
 * @brief Dimensione massima di un file accettato dal server (kilobytes)
 */
extern int MaxFileSize;

/**
 * @var MaxHistMsgs
 * @brief Numero massimo di messaggi che il server 'ricorda' per ogni client
 * (vedi @ref history)
 */
extern int MaxHistMsgs;
/*---------------------------------------------------------------------------*/




/*------------------------FUNZIONI DI CONFIGUARAZIOE--------------------------*/
/**
 * @brief Funzione che legge il file di configurazione e imposta
 * i valori letti nelle variabili corrispondenti
 * @param configurationFileName Path completo o relativo del file di configurazione
 * @retval N: Il numero di valori di setting impostati
 * @retval -1: Se si è verificato un errore
 *
 * | @c errno | Motivo                                  |
 * | :------: | :-------------------------------------: |
 * |@c EINVAL | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere il manuale di
 * memset(), calloc(), fgets(), fopen().
 */
int setUpConfiguration(char* configurationFileName);


/**
 * @brief Funzione che stampa su standard output tutte le variabili di
 * configurazione del server con i rispettivi valori
 * @retval N: Il numero di valori di setting impostati
 */
int printSettingValues();
/*---------------------------------------------------------------------------*/

#endif
