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

#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "config.h"
#include "errors.h"
#include "utils.h"
#include "membership.h"

/**
 * @file membership.c
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Contiene le definizioni delle funzioni di creazione, gestione
 * e distruzione delle strutture @ref membership
 * @see membership.h
 */


int initializeMembership(membership_t* membersh)
{
    if(membersh == NULL){errno = EINVAL; return -1;}

    // Inizializzazione del vettre di appartenenza
    for(int i=0; i<MAX_USER_SIMULTANEUS_GROUPS; i++)
    {
        memset((membersh->set)[i], 0, MAX_NAME_LENGTH);
    }
    // Inizializzazione del contatore
    membersh->belonging = 0;

    return 0;
}


int association(membership_t* membersh, char* grName)
{
    // Controllo sula validità degli argomenti
    if(membersh == NULL || grName == NULL){errno = EINVAL; return -1;}
    // Controllo che non si sia raggiunto il numero massimo di gruppi a cui un utente può appartenere
    if(membersh->belonging == MAX_USER_SIMULTANEUS_GROUPS)
        return USER_GROUPS_QUOTA_EXCEEDED;

    // Controllo che il nome del gruppo non sia già presente nel vettore di appartenenza
    for(int i = 0; i < membersh->belonging; i++)
    {
        if(strcmp((membersh->set)[i], grName) == 0)
            return USER_ALREADY_JOINT;
    }
    // Aggiunta del gruppo al vettore
    strcpy((membersh->set)[membersh->belonging], grName);
    membersh->belonging++;

    return 0;
}


int dissociation(membership_t* membersh, char* grName)
{
    // Controllo sulla validità degli argomenti
    if(membersh == NULL || grName == NULL){errno = EINVAL; return -1;}

    int i = 0;
    bool stop = FALSE;

    while(i < membersh->belonging && !stop)
    {
        // Controllo che il gruppo sia presente nel vettore
        if(strcmp((membersh->set)[i], grName) == 0)
        {
            if(i != membersh->belonging - 1)
                strcpy((membersh->set)[i], (membersh->set)[membersh->belonging - 1]);

            memset((membersh->set)[membersh->belonging - 1], 0, MAX_NAME_LENGTH);

            membersh->belonging--;

            stop = TRUE;
        }
        else
            i++;
    }

    if(!stop)
        // L'tente non appartene al gruppo di cui si è richiesta la dissociazione
        return USER_NOT_JOINT;
    else
        // Dissociazione riuscita
        return 0;
}
