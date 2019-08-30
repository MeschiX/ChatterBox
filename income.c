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
#include <pthread.h>

#include "config.h"
#include "errors.h"
#include "utils.h"
#include "income.h"

/**
 * @file income.c
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Contiene la definizione delle funzioni di inizializzazione, gestione e
 * distruzione della struttura @ref incomeList
 */


/*---------------DEFINIZIONE FUNZIONI INCOME LIST----------------------*/
int initializeIncomeList()
{
    int test;

    test = pthread_mutex_init(&(service.incomeLock), NULL);
    if ( test == -1 )
    {
        errno = test;
        CHECK_FATAL_ERROR(-1, "Inizializzando la lock relativa alla lista di servizio");
    }
    test = pthread_cond_init(&(service.incomeCond), NULL);
    if ( test == -1 )
    {
        errno = test;
        CHECK_FATAL_ERROR(-1, "Inizializzando la variabile condizione relativa alla lista di servizio");
    }

    service.length = 0;
    service.head = NULL;

    return 0;
}


int pushIncome(int fd)
{
    if(fd <= 0){errno = EINVAL; return -1;}

    int test;
    income_t* iterator;
    income_t* new;

    new = (income_t*)malloc(sizeof(income_t));
    CHECK_NULL_FATAL_ERROR(new, "Allocando memoria per il nuovo nodo della coda di servizio")

    new->fd = fd;
    new->next = NULL;


    LOCK(service.incomeLock);

    if(service.head == NULL)
        service.head = new;
    else
    {
        iterator = service.head;

        while(iterator->next != NULL)
            iterator = iterator->next;

        iterator->next = new;
    }

    service.length++;

    UNLOCK(service.incomeLock);

    test = pthread_cond_signal(&(service.incomeCond));
    if(test != 0)
    {
        errno = test;
        CHECK_FATAL_ERROR(-1, "Segnalando la presenza di un nuovo nodo nella coda di servizio")
    }

    return 0;
}


int popIncome()
{
    int test;
    int retValue;
    income_t* tmp = NULL;


    LOCK(service.incomeLock);

    while(service.length == 0)
    {
        test = pthread_cond_wait(&(service.incomeCond), &(service.incomeLock));
        if(test != 0)
        {
            errno = test;
            CHECK_FATAL_ERROR(-1, "Aspettando un nuovo nodo nella coda di servizio");
        }
    }

    if(service.head == NULL){errno = EPERM; return -1;}

    retValue = service.head->fd;

    if(retValue != INCOME_LIST_TERM)
    {
        tmp = service.head;
        service.head = tmp->next;
        service.length--;
        free(tmp);
    }

    UNLOCK(service.incomeLock);

    return retValue;
}


int breakIncomeList()
{
    int test;
    income_t* iterator;
    income_t* new;

    new = (income_t*)malloc(sizeof(income_t));
    CHECK_NULL_FATAL_ERROR(new, "Allocando spazio per il nodo di terminazione della coda di servizio");

    new->fd = INCOME_LIST_TERM;
    new->next = NULL;


    LOCK(service.incomeLock);

    if(service.head == NULL)
        service.head = new;
    else
    {
        iterator = service.head;

        while(iterator->next != NULL)
            iterator = iterator->next;

        iterator->next = new;
    }

    service.length++;

    UNLOCK(service.incomeLock);

    test = pthread_cond_broadcast(&(service.incomeCond));
    if(test != 0)
    {
        errno = test;
        CHECK_FATAL_ERROR(-1, "Segnalando la presenza del nodo di terminazione nella coda di servizio");
    }

    return 0;
}


int destroyIncomeList()
{
    int test;
    income_t* save;
    income_t* iterator;

    iterator = service.head;

    while(iterator != NULL)
    {
        save = iterator->next;
        free(iterator);
        iterator = save;
    }

    test = pthread_mutex_destroy(&(service.incomeLock));
    if(test != 0)
    {
        errno = test;
        CHECK_FATAL_ERROR(-1, "Distruggendo la lock relativa alla coda di servizio");
    }

    test = pthread_cond_destroy(&(service.incomeCond));
    if(test != 0)
    {
        errno = test;
        CHECK_FATAL_ERROR(-1, "Distruggendo la variabile condizione relativa alla coda di servizio");
    }

    return 0;
}
/*---------------------------------------------------------------------*/
