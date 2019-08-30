/*
 * chatterbox Progetto del corso di LSO 2017/2018
 *
 * Dipartimento di Informatica Università di Pisa
 * Docenti: Prencipe, Torquati
 *
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <assert.h>
#include <string.h>

#include "config.h"
#include "ops.h"

/**
 * @file  message.h
 * @brief Contiene il formato del messaggio
 */


/**
 *  @struct header
 *  @brief header del messaggio
 */
typedef struct {
    op_t     op;  /**< Tipo di operazione richiesta al server */
    char sender[MAX_NAME_LENGTH+1]; /**< Nickname del mittente */
} message_hdr_t;

/**
 *  @struct header
 *  @brief header della parte dati
 */
typedef struct {
    char receiver[MAX_NAME_LENGTH+1]; /**< Nickname del ricevente */
    unsigned int   len; /**< Lunghezza del buffer dati */
} message_data_hdr_t;

/**
 *  @struct data
 *  @brief body del messaggio
 */
typedef struct {
    message_data_hdr_t  hdr; /**< Header della parte dati */
    char               *buf; /**< Buffer dati */
} message_data_t;

/**
 *  @struct messaggio
 *  @brief tipo del messaggio
 */
typedef struct {
    message_hdr_t  hdr; /**< Header */
    message_data_t data; /**< Dati */
} message_t;



/* ------ funzioni di utilità ------- */
/**
 * @brief Inizializza ogni campo di una struttura @ref message_t
 * @retval 0: Se l'inizializzazione è avvenuta con successo
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno   | Motivo                                  |
 * | :--------: | :-------------------------------------: |
 * |@c EINVAL   | Almeno uno degli argomenti non è valido |
 */
int initializeMessage(message_t* mex);


/**
 * @brief Alloca in memoria una copia esatta del messaggio @p scr e
 * assegna a @p dest l'indirizzo della copia
 * @param src Puntatore al messaggio che si intende copiare
 * @param dest Indirizzo del puntatore al messaggio che conterrà la copia
 * @retval 0: Se la copia è avvenuta con successo
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno   | Motivo                                  |
 * | :--------: | :-------------------------------------: |
 * |@c EINVAL   | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere il manuale di
 * calloc().
 */
int copyMessage(message_t* src, message_t** dest);


/**
 * @brief Libera lo spazio in memoria occupato dai campi della  struttura @ref message_t
 * passata per argomento.
 * @param msg Puntatore alla struttura @ref message_t da eliminare.
 * @retval 0: Questa funzione non può fallire
 */
int destroyMessage(message_t* msg);


/**
 * @function setheader
 * @brief Scrive L'header del messaggio
 * @param hdr Puntatore all'header
 * @param op Tipo di operazione da eseguire
 * @param sender Mittente del messaggio
 */
static inline void setHeader(message_hdr_t *hdr, op_t op, char *sender) {
#if defined(MAKE_VALGRIND_HAPPY)
    memset((char*)hdr, 0, sizeof(message_hdr_t));
#endif
    hdr->op  = op;
    strncpy(hdr->sender, sender, strlen(sender)+1);
}


/**
 * @function setData
 * @brief Scrive la parte dati del messaggio
 * @param msg Puntatore al body del messaggio
 * @param rcv Nickname o groupname del destinatario
 * @param buf Puntatore al buffer
 * @param len Lunghezza del buffer
 */
static inline void setData(message_data_t *data, char *rcv, const char *buf, unsigned int len) {
#if defined(MAKE_VALGRIND_HAPPY)
    memset((char*)&(data->hdr), 0, sizeof(message_data_hdr_t));
#endif

    strncpy(data->hdr.receiver, rcv, strlen(rcv)+1);
    data->hdr.len  = len;
    data->buf      = (char *)buf;
}

#endif /* MESSAGE_H_ */
