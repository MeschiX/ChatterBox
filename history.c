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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "errors.h"
#include "ops.h"
#include "message.h"
#include "history.h"
#include "settings.h"

/**
 * @file history.c
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Contiene le definizioni delle funzioni di creazione e gestione
 * della struttura @ref history
 * @see history.h
 */


int initializeHistory(history_t* hist)
{
    if(hist == NULL){errno = EINVAL; return -1;}

    hist->cronology = (old_t*)malloc(MaxHistMsgs*sizeof(old_t));
    CHECK_NULL_FATAL_ERROR(hist->cronology, "Allocando spazio per il vettore dei messaggi nello storico");

    hist->loadFactor = 0;
    hist->last = 0;

    for (int i=0; i < MaxHistMsgs; i++)
    {
        (hist->cronology)[i].delivered = FALSE;
        (hist->cronology)[i].mex = NULL;
    }

    return 0;
}

int pushMessage(history_t* hist, message_t* income, bool delivered)
{
    if(hist == NULL || income == NULL){errno = EINVAL; return -1;}

    if(hist->loadFactor < MaxHistMsgs)
    {
        (hist->cronology)[hist->loadFactor].mex = income;
        (hist->cronology)[hist->loadFactor].delivered = delivered;
        hist->loadFactor++;
    }
    else
    {
        old_t* save = &((hist->cronology)[hist->last]);
        destroyMessage(save->mex);
        free(save->mex);

        save->mex = income;
        save->delivered = delivered;

        hist->last++;
        hist->last = hist->last % MaxHistMsgs;
    }

    return 0;
}

int destroyHistory(history_t* hist)
{
    if(hist == NULL){return 0;}

    for (int i = 0; i < MaxHistMsgs; i++)
    {
        destroyMessage((hist->cronology)[i].mex);
        free((hist->cronology)[i].mex);
    }

    free(hist->cronology);

    return 0;
}
