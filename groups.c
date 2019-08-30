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

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "errors.h"
#include "utils.h"
#include "groups.h"

/**
 * @file groups.c
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Contiene le definizioni delle funzioni di creazione, gestione
 * e distruzione delle strutture della tabella dei gruppi di sistema @ref grTable
 * (@ref groupsTable)
 * @see groups.h
 */


/*-------------------DEFINIZIONE FUNZIONI GROUP--------------------------*/
int initializeGroup(group_t* gr, char* grName, char* fnd)
{
    // Controllo sulla validità degli argomenti
    if(gr == NULL || grName == NULL){errno = EINVAL; return -1;}

    // Allocazione nome gruppo e fondatore
    gr->groupName = (char*)calloc(strlen(grName)+1, sizeof(char));
    CHECK_NULL_FATAL_ERROR(gr->groupName, "Allocando spazio per il nome del gruppo");
    gr->founder = (char*)calloc(strlen(fnd)+1, sizeof(char));
    CHECK_NULL_FATAL_ERROR(gr->groupName, "Allocando spazio per il nome del fondatore del gruppo");

    // Scrittura nome gruppo e fondatore
    strcpy(gr->groupName, grName);
    strcpy(gr->founder, fnd);
    gr->members = NULL;

    return 0;
}

int addMember(group_t* gr, char* nickName)
{
    // Controllo sulla validità degli argomenti
    if(gr == NULL || nickName == NULL){errno = EINVAL; return -1;}

    member_t* new;
    member_t* iter = gr->members;

    // Controllo se il nickname è già presente nella lista dei membri
    if(gr->members != NULL)
    {
        if(strcmp(iter->nickName, nickName) == 0)
            return USER_ALREADY_JOINT;

        while(iter->next != NULL)
        {
            if(strcmp(iter->nickName, nickName) == 0)
                return USER_ALREADY_JOINT;
            else
                iter = iter->next;
        }
    }

    // Aggiunta del membro
    new = (member_t*)calloc(1, sizeof(member_t));
    CHECK_NULL_FATAL_ERROR(new, "Allocando spazio per il nuovo membro da inserire nel gruppo");

    new->nickName = (char*)calloc(strlen(nickName)+1, sizeof(char));
    CHECK_NULL_FATAL_ERROR(new->nickName, "Allocando spazio per il nome del nuovo membro da inserire nel gruppo");

    strcpy(new->nickName, nickName);
    new->next = NULL;

    iter->next = new;

    return 0;
}


int removeMember(group_t* group, char* nickName)
{
    // Controllo sulla validità degli argomenti
    if(group == NULL || nickName == NULL){errno = EINVAL; return -1;}

    if(group->members == NULL)
        return USER_NOT_JOINT;
    else
    {
        member_t* iter1 = group->members;

        // Ricerca del membro da eliminare
        if(strcmp(iter1->nickName, nickName) == 0)
        {
            group->members = group->members->next;
            free(iter1->nickName);
            free(iter1);
        }
        else
        {
            member_t* iter2 = iter1->next;

            while(strcmp(iter2->nickName, nickName) != 0 && iter2 != NULL)
            {
                iter1 = iter2;
                iter2 = iter2->next;
            }

            if(iter2 == NULL)
            {
                return USER_NOT_JOINT;
            }
            else
            {
                iter1->next = iter2->next;
                free(iter2->nickName);
                free(iter2);
            }
        }
    }

    return 0;
}


int isMember(group_t* group, char* nickName)
{
    // Controllo sulla validità degli argomenti
    if(group == NULL || nickName == NULL){errno = EINVAL; return -1;}
    if(group->members == NULL){return 0;}

    bool stop = FALSE;
    member_t* iter = group->members;

    // Ricerca del membro
    while(iter != NULL && !stop)
    {
        if(strcmp(iter->nickName, nickName) == 0)
            stop = TRUE;
        else
            iter = iter->next;
    }

    if(stop)
        return 1;
    else
        return 0;
}


int destroyGroup(group_t* gr)
{
    // Controllo sulla validità degli argomenti
    if(gr == NULL){return 0;}

    member_t* iter;

    // Distruzione della lista dei membri
    while(gr->members != NULL)
    {
        iter = gr->members->next;
        free(gr->members->nickName);
        free(gr->members);
        gr->members = iter;
    }

    // Rimozione del nome
    if(gr->groupName != NULL)
        free(gr->groupName);
    // Rimozione del fondatore
    if(gr->founder != NULL)
        free(gr->founder);

    return 0;
}
/*-----------------------------------------------------------------------*/




/*----------------DEFINIZIONE FUNZIONI GROUP LIST------------------------*/
int initializeGroupList(groupsList_t* grList)
{
    // Controllo sulla validità degli argomenti
    if(grList == NULL){errno = EINVAL; return -1;}

    grList->loadFactor = 0;
    grList->head = NULL;

    return 0;
}

int insertGroupNode(groupsList_t* grList, char* grName, char* fnd)
{
    // Controllo sulla validità degli argomenti
    if(grList == NULL || grName == NULL){errno = EINVAL; return -1;}

    int test;
    bool stop = FALSE;
    groupNode_t* new;
    groupNode_t* iter = grList->head;

    // Controllo se è già presente un gruppo con lo stesso nome
    if(iter != NULL)
    {
        while(iter->next != NULL && !stop)
        {
            if(strcmp(iter->gr.groupName, grName) == 0)
                stop = TRUE;
            else
                iter = iter->next;
        }

        if(!stop && strcmp(iter->gr.groupName, grName) == 0)
            stop = TRUE;

        if(stop)
            return GROUP_ALREADY_REGISTERED;
    }

    // Inserimento del grupo
    new = (groupNode_t*)calloc(1, sizeof(groupNode_t));
    CHECK_NULL_FATAL_ERROR(new, "Inserendo il nuovo gruppo in una lista di gruppi");

    test = initializeGroup(&(new->gr), grName, fnd);
    CHECK_FATAL_ERROR(test, "Inizializzando il nuovo gruppo");

    new->next = NULL;

    if(iter == NULL)
    {
        grList->head = new;
        new->prev = NULL;
    }
    else
    {
        iter->next = new;
        new->prev = iter;
    }

    grList->loadFactor++;

    return 0;
}


int getGroupNode(groupsList_t* grList, char* grName,  groupNode_t** save)
{
    // Controllo sulla validità degli argomenti
    if(grList == NULL || grName == NULL){errno = EINVAL; return -1;}

    groupNode_t* iter;
    bool stop = FALSE;

    iter = grList->head;
    // Ricerca del gruppo
    while(iter != NULL && !stop)
    {
        if(strcmp(iter->gr.groupName, grName) == 0)
            stop = TRUE;
        else
            iter = iter->next;
    }

    if(stop)
    {
        *save = (iter);
        return 1;
    }
    else
    {
        *save = NULL;
        return 0;
    }
}


int removeGroupNode(groupsList_t* grList, char* grName)
{
    // Controllo sulla validità degli argomenti
    if(grList == NULL || grName == NULL){errno = EINVAL; return -1;}

    bool stop;
    groupNode_t* iter1;
    groupNode_t* iter2;

    if(grList->head == NULL)
        return GROUP_NOT_REGISTERED;
    else
    {
        // Ricerca del gruppo da rimuovere
        if(strcmp(grList->head->gr.groupName, grName) == 0)
        {
            iter2 = grList->head;
            grList->head = iter2->next;
            destroyGroup(&(iter2->gr));
            free(iter2);
            grList->loadFactor--;
            return 0;
        }
        else
        {
            iter1 = grList->head;
            iter2 = grList->head->next;
            while(iter2 != NULL && !stop)
            {
                if(strcmp(iter2->gr.groupName, grName) == 0)
                    stop = TRUE;
                else
                {
                    iter1 = iter2;
                    iter2 = iter2->next;
                }
            }

            if(stop)
            {
                iter1->next = iter2->next;
                destroyGroup(&(iter2->gr));
                free(iter2);
                grList->loadFactor--;
                return 0;
            }
            else
                return GROUP_NOT_REGISTERED;
        }
    }

    return 0;
}

int destroyGroupList(groupsList_t* grList)
{
    // Controllo sulla validità degli argomenti
    if(grList == NULL){return 0;}

    groupNode_t* iter;

    if(grList->head != NULL)
    {
        // Rimozione di tutti i grupi dalla lista
        while(grList->head != NULL)
        {
            iter = grList->head;
            grList->head = iter->next;
            destroyGroup(&(iter->gr));
            free(iter);
        }
    }

    return 0;
}
/*-----------------------------------------------------------------------*/




/*----------------DEFINIZIONE FUNZIONI GROUP TABLE-----------------------*/
int groupHash(unsigned char* str)
{
    // Controllo sulla validità degli argomenti
    if(str == NULL){errno = EINVAL; return -1;}

    unsigned long hash = 5381;
    int c;

    while ((c = *(str++))!=0)
        hash = ((hash << 5) + hash) + c;

    return (int)(hash % GROUPS_TABLE_SIZE);
}


int initializeGroupsTable()
{
    int test;

    // Allocazione tabella
    grTable.table = (groupsList_t*)malloc(GROUPS_TABLE_SIZE*sizeof(groupsList_t));
    CHECK_NULL_FATAL_ERROR(grTable.table, "Allocando spazio per la tabella gei gruppi");
    // Allocazione vettore locks
    grTable.bloksLocks = (pthread_rwlock_t*)malloc((GROUPS_TABLE_SIZE/GROUPS_MUTEX_BLOCK_SIZE)*sizeof(pthread_rwlock_t));
    CHECK_NULL_FATAL_ERROR(grTable.bloksLocks, "Allocando spazio per il set di locks relativo alla tabella dei gruppi");

    // Inizializzazione dele liste
    for(int i=0; i<GROUPS_TABLE_SIZE; i++)
    {
        test = initializeGroupList(&((grTable.table)[i]));
        CHECK_FATAL_ERROR(test, "Inizializzando la lista dei gruppi");
        (grTable.table)[i].loadFactor = 0;
    }
    // Inizializzazione vettore locks
    for(int i=0; i<GROUPS_TABLE_SIZE/GROUPS_MUTEX_BLOCK_SIZE; i++)
    {
        test = pthread_rwlock_init(&((grTable.bloksLocks)[i]), NULL);
        if (test != 0)
        {
            errno = test;
            CHECK_FATAL_ERROR(test, "Inizializzando una lock del set di locks relativo alla tabella dei gruppi");
        }
    }

    return 0;
}


int insertGroup(char* grName, char* fnd)
{
    // Controllo sulla validità degli argomenti
    if(grName == NULL || fnd == NULL){errno = EINVAL; return -1;}

    int test;
    int index;
    int mutexIndex;
    groupsList_t* row;

    // Calcolo dell'indice di riga
    index = groupHash((unsigned char*)grName);
    row = &((grTable.table)[index]);
    // Calcolo dell'indice della lock relativa alla porzione di tabella contenente l'indice di riga selezionato
    mutexIndex = index / GROUPS_MUTEX_BLOCK_SIZE;

    WRITE_LOCK((grTable.bloksLocks)[mutexIndex]);

    // Inserimento del gruppo
    test = insertGroupNode(row, grName, fnd);
    CHECK_FATAL_ERROR(test, "Inserendo il nuovo gruppo nella lista dei gruppi");

    RW_UNLOCK((grTable.bloksLocks)[mutexIndex]);

    return test;
}


int disjoin(char* grName, char* memb)
{
    // Controllo sulla validità degli argomenti
    if(grName == NULL || memb == NULL){errno = EINVAL; return -1;}

    int test;
    int index;
    int mutexIndex;
    groupNode_t* save = NULL;
    groupsList_t* row;

    // Calcolo dell'indice di riga
    index = groupHash((unsigned char*)grName);
    row = &((grTable.table)[index]);
    // Calcolo dell'indice della lock relativa alla porzione di tabella contenente l'indice di riga selezionato
    mutexIndex = index / GROUPS_MUTEX_BLOCK_SIZE;

    WRITE_LOCK((grTable.bloksLocks)[mutexIndex]);

    // Ricerca del gruppo
    test = getGroupNode(row, grName, &save);
    CHECK_FATAL_ERROR(test, "Selezionando il gruppo da cui dissociare l'utente");
    if(test != 1){RW_UNLOCK((grTable.bloksLocks)[mutexIndex]); return test;}
    // Rimozione del membro dal gruppo
    test = removeMember(&(save->gr), memb);
    CHECK_FATAL_ERROR(test, "Rimuovendo l'username dalla lista dei membri del gruppo");

    RW_UNLOCK((grTable.bloksLocks)[mutexIndex]);

    return 0;
}


int removeGroup(char* grName)
{
    // Controllo sulla validità degli argomenti
    if(grName == NULL){errno = EINVAL; return -1;}

    int test;
    int index;
    int mutexIndex;
    groupsList_t* row;

    // Calcolo dell'indice di riga
    index = groupHash((unsigned char*)grName);
    row = &((grTable.table)[index]);
    // Calcolo dell'indice della lock relativa alla porzione di tabella contenente l'indice di riga selezionato
    mutexIndex = index / GROUPS_MUTEX_BLOCK_SIZE;

    WRITE_LOCK((grTable.bloksLocks)[mutexIndex]);

    // Rimozione del gruppo
    test = removeGroupNode(row, grName);
    CHECK_FATAL_ERROR(test, "Rimuovendo il gruppo dalla lista dei gruppi");

    RW_UNLOCK((grTable.bloksLocks)[mutexIndex]);

    return test;
}

int destroyGroupsTable()
{
    int test;
    groupsList_t* row;

    // Distruzione delle liste ("righe")
    for(int i = 0; i < GROUPS_TABLE_SIZE; i++)
    {
        row = &(grTable.table[i]);
        destroyGroupList(row);
    }

    // Distruzione del vettore delle locks
    for (int i = 0; i < GROUPS_TABLE_SIZE/GROUPS_MUTEX_BLOCK_SIZE ; ++i)
    {
        test = pthread_rwlock_destroy(&((grTable.bloksLocks)[i]));
        if (test != 0)
        {
            errno = test;
            CHECK_FATAL_ERROR(-1, "Distruggendo una lock del set di locks relativo alla tabella dei gruppi");
        }
    }

    free(grTable.bloksLocks);
    free(grTable.table);

    return 0;
}
/*-----------------------------------------------------------------------*/
