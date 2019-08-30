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

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>

#include "utils.h"
#include "settings.h"
#include "errors.h"
#include "stats.h"
#include "delivery.h"
#include "membership.h"
#include "users.h"

/**
 * @file users.c
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Contiene le definizioni delle funzioni di creazione, gestione
 * e distruzione delle strutture della tabella utenti di sistema @ref usrTable
 * (@ref usersTable)
 * @see users.h
 */


/*--------------------DEFINIZIONE FUNZIONI UTENTE------------------------*/
int initializeUser(user_t* usr, char* nickname)
{
    if(usr == NULL || nickname == NULL){errno = EINVAL; return -1;}

    int test;

    // Impostazione del nickname dell'utente
    usr->nickName = (char*)calloc(strlen(nickname)+1, sizeof(char));
    CHECK_NULL_FATAL_ERROR(usr->nickName, "Allocando spazio per l'username dell'utente");
    strcpy(usr->nickName, nickname);

    // Creazione dell'elenco degli indirizzi relativo all'utente
    test = initializeDelivery(&(usr->distribution));
    CHECK_FATAL_ERROR(test, "Inizializzando il vettore degli indirizzi dell'utente");

    // Creazione dell'elenco dei gruppi a cui l'utente appartiene
    test = initializeMembership(&(usr->groups));
    CHECK_FATAL_ERROR(test, "Inizializzando il vettore di appartenenza dell'utente");

    // Creazione dello storico dei messaggi relativo all'utente
    test = initializeHistory(&(usr->userIncomeHistory));
    CHECK_FATAL_ERROR(test, "Inizializzando lo storico dei messaggi dell'utente");

    return 0;
}


int isUser(user_t* usr, char* nickname)
{
    if(usr == NULL || nickname == NULL){return 0;}

    if(strcmp(usr->nickName, nickname)==0)
        return 1;
    else
        return 0;
}


int destroyUser(user_t* usr)
{
    if(usr == NULL){return 0;}

    if(usr->nickName != NULL)
        free(usr->nickName);

    destroyHistory(&(usr->userIncomeHistory));

    return 0;
}
/*-----------------------------------------------------------------------*/




/*-----------------------DEFINIZIONE FUNZIONI LISTA----------------------*/
int initializeUsersList(usersList_t* usrList)
{
    if(usrList == NULL){errno = EINVAL; return -1;}

    usrList->loadFactor = 0;
    usrList->head = NULL;

    return 0;
}


int insertUserNode(usersList_t* list, char* nickname)
{
    if(list == NULL || nickname == NULL){errno = EINVAL; return -1;}

    int test;
    bool stop = FALSE;
    userNode_t* newNode;
    userNode_t* iter = list->head;

    if(iter != NULL)
    {
        while (iter->next != NULL && !stop)
        {
            // Controllo che il nicknsme non sia già presente nella lista
            if(isUser(&(iter->user), nickname))
                stop = TRUE;
            else
                iter = iter->next;
        }

        if(!stop && isUser(&(iter->user), nickname))
            stop = TRUE;

        if(stop)
            // Nickname già presente
            return USER_ALREADY_REGISTERED;
    }

    newNode = (userNode_t*)malloc(sizeof(userNode_t));
    CHECK_NULL_FATAL_ERROR(newNode, "Allocando spazio per il nuovo nodo della lista utenti");

    test = initializeUser(&(newNode->user), nickname);
    CHECK_FATAL_ERROR(test, "Inizializzando la struttura user contenuta nel nuovo nodo della lista utenti");

    newNode->next = NULL;

    // Aggiunta del nuovo utente
    if(iter == NULL)
    {
        list->head = newNode;
        newNode->prev = NULL;
    }
    else
    {
        iter->next = newNode;
        newNode->prev = iter;
    }

    // Incremento del contatore
    list->loadFactor++;

    return 0;
}


int getUserNode(usersList_t* list, char* nickname, userNode_t** save)
{
    // Controllo sulla validità degli argomenti
    if(list == NULL || nickname == NULL){errno = EINVAL; return -1;}

    bool stop = FALSE;
    userNode_t* iter = list->head;

    // Ricerca del nickname all'interno della lista
    while(iter != NULL && !stop)
    {
        if(isUser(&(iter->user), nickname))
            stop = TRUE;
        else
            iter = iter->next;
    }

    if(stop)
    {// Utente trovato
        *save = iter;
        return 1;
    }
    else
    {// Utente non trovato
        *save = NULL;
        return 0;
    }
}


int removeUserNode(usersList_t* list, char* nickname)
{
    // Controllo sulla validità degli argomenti
    if(list == NULL || nickname == NULL){errno = EINVAL; return -1;}
    // Controllo che la lista non sia vuota
    if(list->head == NULL){return USER_NOT_REGISTERED;}

    userNode_t* save1;
    userNode_t* save2;
    bool stop = FALSE;

    // Controllo se l'utente da rimuovere è il primo della lista
    if(isUser(&(list->head->user), nickname))
    {
        save1 = list->head;
        list->head = list->head->next;
        destroyUser(&(save1->user));
        free(save1);
        list->loadFactor--;
        return 0;
    }

    save1 = list->head->next;
    save2 = list->head;

    // Controllo la parte rimanente della coda
    while(save1 != NULL && !stop)
    {
        if(isUser(&(save1->user), nickname))
            stop = TRUE;
        else
        {
            save2 = save1;
            save1 = save1->next;
        }
    }

    if(!stop)
        // Utente non presente nella lista
        return USER_NOT_REGISTERED;
    else
    {// Utente trovato
        save2->next = save1->next;
        destroyUser(&(save1->user));
        free(save1);
        list->loadFactor--;
        return 0;
    }
}


int destroyUsersList(usersList_t* list)
{
    // Controllo sulla validità degli argomenti
    if(list == NULL){return 0;}

    userNode_t* tmp;

    while(list->head!=NULL)
    {
        tmp = list->head->next;

        destroyUser(&(list->head->user));
        free(list->head);

        list->head = tmp;
    }

    return 0;
}
/*-----------------------------------------------------------------------*/




/*-----------------------DEFINIZIONE FUNZIONI TABELLA--------------------*/
int userHash(unsigned char* str)
{
    if(str == NULL){errno = EINVAL; return -1;}

    unsigned long hash = 5381;
    int c;

    while ((c = *(str++))!=0)
        hash = ((hash << 5) + hash) + c;

    return (int)(hash % USERS_TABLE_SIZE);
}


int initializeUsersTable()
{
    int test;

    // Allocazione tabella
    usrTable.table = (usersList_t*)malloc(USERS_TABLE_SIZE*sizeof(usersList_t));
    CHECK_NULL_FATAL_ERROR(usrTable.table, "Allocando la tabella utenti");

    // Allocazione vettore di locks
    usrTable.blockLocks = (pthread_rwlock_t*)malloc((USERS_TABLE_SIZE / USERS_MUTEX_BLOCK_SIZE) * sizeof(pthread_rwlock_t));
    CHECK_NULL_FATAL_ERROR(usrTable.blockLocks, "Allocando il set di locks della tabella utenti");

    // Inizializzazione del vettore di locks
    for (int i = 0; i < USERS_TABLE_SIZE / USERS_MUTEX_BLOCK_SIZE; i++)
    {
        test = pthread_rwlock_init(&((usrTable.blockLocks)[i]), NULL);
        if(test < 0)
        {
            errno = test;
            CHECK_FATAL_ERROR(-1, "Inizializzando il set di locks della tabella utenti");
        }
    }

    // Inizializzazione delle "righe"
    for(int i=0; i<USERS_TABLE_SIZE; i++)
    {
        test = initializeUsersList(&((usrTable.table)[i]));
        CHECK_FATAL_ERROR(test, "Inizializzando la lista utenti");
    }

    return 0;
}


int destroyUsersTable()
{
    int test;

    // Distruzione del vettore delle locks
    for (int i = 0; i < USERS_TABLE_SIZE / USERS_MUTEX_BLOCK_SIZE; i++)
    {
        test = pthread_rwlock_destroy(&((usrTable.blockLocks)[i]));
        if(test == -1){return -1;}
    }

    // Distruzione della tabella
    for(int i=0; i<USERS_TABLE_SIZE; i++)
    {
        destroyUsersList(&((usrTable.table)[i]));
        (usrTable.table)[i].head = NULL;
        (usrTable.table)[i].loadFactor = 0;
    }

    // Liberazione dei puntatori
    free(usrTable.table);
    usrTable.table = NULL;
    free(usrTable.blockLocks);
    usrTable.blockLocks = NULL;

    return 0;
}
/*-----------------------------------------------------------------------*/
