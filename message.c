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

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "config.h"
#include "utils.h"
#include "errors.h"
#include "message.h"

/**
 * @file errors.c
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Conntiene le definizioni delle funzioni di inizializzazione, gestione e distruzione
 * della struttura @ref message_t
 * @see message.h
 */

int initializeMessage(message_t* mex)
{
    if (mex == NULL) {errno = EINVAL; return -1;}

    mex->hdr.op = 0;
    memset((mex->hdr.sender), 0, MAX_NAME_LENGTH + 1);
    memset(mex->data.hdr.receiver, 0, MAX_NAME_LENGTH + 1);
    mex->data.hdr.len = 0;
    mex->data.buf = NULL;

    return 0;
}

int copyMessage(message_t* src, message_t** dest)
{
    if(src == NULL || dest == NULL){errno = EINVAL; return -1;}

    // Allocazione del nuovo messaggio
    (*dest) = (message_t*)calloc(1, sizeof(message_t));
    CHECK_NULL_FATAL_ERROR((*dest), "Allocando spazio per la copia del messaggio");

    // Copia dell'header
    (*dest)->hdr.op = src->hdr.op;
    strcpy((*dest)->hdr.sender, src->hdr.sender);

    // Copia della parte dati
    (*dest)->data.hdr.len = src->data.hdr.len;
    strcpy((*dest)->data.hdr.receiver, src->data.hdr.receiver);

    if(src->data.buf != NULL)
    {
		(*dest)->data.buf = (char*)calloc(strlen(src->data.buf)+1, sizeof(char));
		CHECK_NULL_FATAL_ERROR((*dest)->data.buf, "Allocando spazio per la parte dati del messaggio durante la copia");
		strcpy((*dest)->data.buf, src->data.buf);
    }

    return 0;
}

int destroyMessage(message_t* msg)
{
    if(msg == NULL){return 0;}

    if((msg)->data.buf)
        free((msg)->data.buf);

    return 0;
}
