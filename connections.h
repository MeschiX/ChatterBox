/*
 * chatterbox Progetto del corso di LSO 2017/2018
 *
 * Dipartimento di Informatica Università di Pisa
 * Docenti: Prencipe, Torquati
 *
 */
#ifndef CONNECTIONS_H_
#define CONNECTIONS_H_

/**
 * @brief Numero massimo di tentativi per creare una connessione
 */
#define MAX_RETRIES     10

/**
 * @brief Numero massimo di secondi di attesa
 */
#define MAX_SLEEPING     3

#include "message.h"

/**
 * @file  connections.h
 * @brief Contiene le funzioni che implementano il protocollo
 *        tra i clients ed il server (parte client)
 */

/*--------------------------CLIENT SIDE----------------------------------*/
/**
 * @brief Apre una connessione AF_UNIX verso il server
 *
 * @param path Path del socket AF_UNIX
 * @param ntimes numero massimo di tentativi di retry
 * @param secs tempo di attesa tra due retry consecutive
 *
 * @return il descrittore associato alla connessione in caso di successo
 *         -1 in caso di errore
 */
int openConnection(char* path, unsigned int ntimes, unsigned int secs);

/**
 * @brief Legge l'header del messaggio
 *
 * @param fd     descrittore della connessione
 * @param hdr    puntatore all'header del messaggio da ricevere
 *
 * @return <=0 se c'e' stato un errore
 *         (se <0 errno deve essere settato, se == 0 connessione chiusa)
 */
int readHeader(long fd, message_hdr_t *hdr);

/**
 * @brief Legge il body del messaggio
 *
 * @param fd     descrittore della connessione
 * @param data   puntatore al body del messaggio
 *
 * @return <=0 se c'e' stato un errore
 *         (se <0 errno deve essere settato, se == 0 connessione chiusa)
 */
int readData(long fd, message_data_t *data);

/**
 * @brief Legge l'intero messaggio
 *
 * @param fd     descrittore della connessione
 * @param msg   puntatore al messaggio
 *
 * @return <=0 se c'e' stato un errore
 *         (se <0 errno deve essere settato, se == 0 connessione chiusa)
 */
int readMsg(long fd, message_t *msg);

/**
 * @brief Invia l'intestazione del messaggio
 * @param fd Descrittore della connessione
 * @param hdr Puntatore all'header del messaggio
 * @return <= 0 se c'e stato un errore
 */
int sendHeader(long fd, message_hdr_t* hdr);

/**
 * @brief Invia il body del messaggio al server
 *
 * @param fd     descrittore della connessione
 * @param msg    puntatore al messaggio da inviare
 *
 * @return <=0 se c'e' stato un errore
 */
int sendData(long fd, message_data_t *msg);

/**
 * @brief Invia un messaggio di richiesta al server
 *
 * @param fd     descrittore della connessione
 * @param msg    puntatore al messaggio da inviare
 *
 * @return <=0 se c'e' stato un errore
 */
int sendRequest(long fd, message_t *msg);
/*-----------------------------------------------------------------------*/


#endif /* CONNECTIONS_H_ */
