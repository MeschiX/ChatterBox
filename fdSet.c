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
#include <sys/select.h>
#include <pthread.h>
#include <errno.h>

#include "fdSet.h"
#include "utils.h"

/**
 * @file fdSet.h
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Contiene le definizioni delle funzioni di inizializzazione,
 * gestione e distruzione della struttura @ref fdSet
 * @see fdSet.h
 */

/*-----------------DEFINIZIONE FUNZIONI GESTIONE FDSET-----------------*/
int initializeSet()
{
    int test;

    masterSet.maxFd = 0;

    test = pthread_mutex_init(&(masterSet.setLock), NULL);
    if (test != 0) {errno=test; return -1;}

    FD_ZERO(&(masterSet.master));

    return 0;
}

int INset(int fd)
{
    if (fd <= 0) {errno = EINVAL; return -1;}

    LOCK(masterSet.setLock)

    FD_SET(fd, &(masterSet.master));

    if(fd > masterSet.maxFd)
        masterSet.maxFd = fd;

    UNLOCK(masterSet.setLock)

    return 0;
}


int OUTset(int fd)
{
    if (fd <= 0) {errno = EINVAL; return -1;}

    LOCK(masterSet.setLock)

    FD_CLR(fd, &(masterSet.master));

    if(fd == masterSet.maxFd)
    {
        for(int i=(masterSet.maxFd-1); i>=0; i--)
        {
            if (FD_ISSET(i, &(masterSet.master)))
            {
                masterSet.maxFd = i;
                break;
            }
        }
    }

    UNLOCK(masterSet.setLock)

    return 0;
}


int saveSet(fd_set* backSet, int* saveMaxFd)
{
    LOCK(masterSet.setLock)

    *backSet = masterSet.master;
    *saveMaxFd = masterSet.maxFd;

    UNLOCK(masterSet.setLock);

    return 0;
}

int destroySet()
{
    int test;

    test = pthread_mutex_destroy(&(masterSet.setLock));
    if(test != 0){errno = test; return -1;}

    return 0;
}
/*---------------------------------------------------------------------*/
