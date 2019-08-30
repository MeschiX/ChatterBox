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
#include <pthread.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>

#include "message.h"
#include "errors.h"
#include "logIn.h"
#include "stats.h"
#include "connections.h"
#include "settings.h"
#include "users.h"
#include "delivery.h"
#include "groups.h"
#include "utils.h"

/**
 * @file communication.c
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Contiene la definizione di diverse funzioni di comunicazione utilizzate
 * dal server per soddisfare le richieste
 */

int sReadHeader(long fd, message_hdr_t *hdr)
{
    if(hdr == NULL || fd < 0) {errno = EINVAL; return -1;}

    int test;
    struct iovec iov;

    iov.iov_base = hdr;
    iov.iov_len = sizeof(message_hdr_t);

    test = readv(fd, &iov, 1);
    if(test == 0 || (test == -1 && errno == EPIPE))
        return CLIENT_CLOSED_CONNECTION;
    if(test == -1)
        return test;

    return test;
}

int sReadData(long fd, message_data_t *data)
{
    if(data == NULL || fd < 0) {errno = EINVAL; return -1;}

    int test;
    char* ph;
    unsigned int bytesRead;
    unsigned int bufRead = 0;
    struct iovec iov;

    iov.iov_base = &(data->hdr);
    iov.iov_len = sizeof(message_data_hdr_t);

    test = readv(fd, &iov, 1);
    if(test == 0 || (test == -1 && errno == EPIPE))
        return CLIENT_CLOSED_CONNECTION;
    if(test == -1)
        return test;

    bytesRead = test;

    // Lettura del buffer finché non è stato letto tutto
    if(data->hdr.len != 0)
    {
        data->buf = malloc(data->hdr.len);
        CHECK_NULL_FATAL_ERROR(data->buf, "Allocando spazio per contenere la parte dati di un messaggio in lettura");

        ph = data->buf;

        while (bufRead < data->hdr.len)
        {
            test = read(fd, ph, data->hdr.len - bufRead);
            if(test == 0 || (test == -1 && errno == EPIPE))
                return CLIENT_CLOSED_CONNECTION;
            if(test == -1)
                return test;

            bufRead += test;
            ph += test;
        }

        bytesRead += bufRead;
    }

    return bytesRead;
}

int sReadFile(long fd, message_data_t* data)
{
    if(data == NULL || fd < 0) {errno = EINVAL; return -1;}

    int test;
    char* ph;
    unsigned int bytesRead;
    unsigned int bufRead = 0;
    struct iovec iov;

    iov.iov_base = &(data->hdr);
    iov.iov_len = sizeof(message_data_hdr_t);

    test = readv(fd, &iov, 1);
    if(test == 0 || (test == -1 && errno == EPIPE))
        return CLIENT_CLOSED_CONNECTION;
    if(test == -1)
        return test;

    bytesRead = test;

    // Lettura del buffer finché non è stato letto tutto
    if(data->hdr.len != 0)
    {
        data->buf = malloc(data->hdr.len);
        CHECK_NULL_FATAL_ERROR(data->buf, "Allocando lo spazio necessario per leggere il file  dal socket");

        ph = data->buf;

        while (bufRead < data->hdr.len)
        {
            test = read(fd, ph, data->hdr.len - bufRead);
            if(test == 0 || (test == -1 && errno == EPIPE))
                return CLIENT_CLOSED_CONNECTION;
            if(test == -1)
                return test;

            bufRead += test;
            ph += test;
        }

        bytesRead += bufRead;
    }

    // Controllo sulla dimensione del file
    if((int)data->hdr.len/1000 > MaxFileSize)
        return FILE_MESSAGE_TOO_BIG;
    else
        return bytesRead;
}

int sReadMsg(long fd, message_t *msg)
{
    if(msg == NULL || fd < 0) {errno = EINVAL; return -1;}

    int test;
    unsigned int bytesRead;

    // Lettura intestazione
    test = sReadHeader(fd, &(msg->hdr));
    if(test <= 0){return test;}

    bytesRead = test;

    // Lettura parte dati
    test = sReadData(fd, &(msg->data));
    if(test <= 0){return test;}

    bytesRead += test;

    return bytesRead;
}

int sSendHeader(long fd, message_hdr_t* hdr)
{
    if(fd <= 0 || hdr == NULL){errno = EINVAL; return -1;}

    int test;
    struct iovec iov;

    iov.iov_base = hdr;
    iov.iov_len = sizeof(message_hdr_t);

    test = writev(fd, &iov, 1);
    if(test == 0 || (test == -1 && errno == EPIPE))
        return CLIENT_CLOSED_CONNECTION;

    return test;
}

int sSendData(long fd, message_data_t *data)
{
    if(data == NULL || fd < 0) {errno = EINVAL; return -1;}

    int test;
    char* ph;
    unsigned int written;
    unsigned int bufWritten = 0;
    struct iovec iov;

    iov.iov_base = &(data->hdr);
    iov.iov_len = sizeof(message_data_hdr_t);

    test = writev(fd, &(iov), 1);
    if(test == 0 || (test == -1 && errno == EPIPE))
        return CLIENT_CLOSED_CONNECTION;
    if(test == -1)
        return test;

    written = test;

    // Scrittura del buffer finché non è stato scritto tutto
    if(data->hdr.len != 0)
    {
        ph = data->buf;

        while (bufWritten < data->hdr.len)
        {
            test = write(fd, ph, data->hdr.len - bufWritten);
            if(test == 0 || (test == -1 && errno == EPIPE))
                return CLIENT_CLOSED_CONNECTION;
            if(test == -1)
                return test;

            bufWritten += test;
            ph += test;
        }

        written += bufWritten;
    }

    return written;
}

int sSendMsg(long fd, message_t* msg)
{
    if(msg == NULL || fd < 0) {errno = EINVAL; return -1;}

    int test;
    int written;

    // Scrittura intestazione
    test = sSendHeader(fd, &(msg->hdr));
    if(test <= 0){return test;}

    written = test;

    // Scrittura parte dati
    test = sSendData(fd, &(msg->data));
    if(test <= 0){return test;}

    written += test;

    return written;
}

int registerUser(char* nickName)
{
    // Controllo sulla validità dei parametri
    if(nickName == NULL){errno = EINVAL; return -1;}
    // Controllo sulla lunghezza del nome
    if (strlen(nickName) > MAX_NAME_LENGTH)
        return USERNAME_TOO_LONG;

    int test;
    int index;
    int mutexIndex;
    usersList_t* row;

    // Calcolo dell'indice di riga
    index = userHash((unsigned char*)nickName);
    row = &((usrTable.table)[index]);
    // Calcolo dell'indice della lock relativa alla porzione di tabella contenente la riga selezionata
    mutexIndex = index / USERS_MUTEX_BLOCK_SIZE;

    WRITE_LOCK((usrTable.blockLocks)[mutexIndex]);

    // Inserimento nella riga
    test = insertUserNode(row, nickName);
    CHECK_FATAL_ERROR(test, "Inserendo l'utente nella lista utenti");

    RW_UNLOCK((usrTable.blockLocks)[mutexIndex]);

    if(test == 0)
    {// Inserimento effettuato
        WRITE_LOCK(chattyStats.statsLock);
        // Aggiornamento statistiche
        chattyStats.nusers++;
        RW_UNLOCK(chattyStats.statsLock)
    }

    return test;
}

int unregisterUser(char* nickName, long client)
{
    if(nickName == NULL){errno = EINVAL; return -1;}

    int test;
    int grIndex;
    int grMutexIndex;
    int usrIndex;
    int usrMutexIndex;
    userNode_t* usrSave;
    groupNode_t* grSave;
    groupsList_t* grRow;
    usersList_t* usrRow;

    // Calcolo dell'indice di riga nella tabella utenti
    usrIndex = userHash((unsigned char*)nickName);
    usrRow = &((usrTable.table)[usrIndex]);
    // Calcolo dell'indice della lock relativa alla porzione di tabella contenente la riga selezionata
    usrMutexIndex = usrIndex / USERS_MUTEX_BLOCK_SIZE;
    usrSave = NULL;

    WRITE_LOCK((usrTable.blockLocks)[usrMutexIndex]);

    // Selezione dell'utente nella tabella
    test = getUserNode(usrRow, nickName, &usrSave);
    CHECK_FATAL_ERROR(test, "Cercando l'utente nella tabella utenti");
    if(usrSave == NULL)
    {
        RW_UNLOCK((usrTable.blockLocks)[usrMutexIndex]);
        return USER_NOT_REGISTERED;
    }
    // Rmizione da tutti i gruppi
    for(int i = 0; i < usrSave->user.groups.belonging; i++)
    {
        // Calcolo dell'indice di riga nella tabella dei gruppi
        grIndex = groupHash((unsigned char*)(usrSave->user.groups.set)[i]);
        grRow = &((grTable.table)[grIndex]);
        // Calcolo dell'indice della lock relativa alla porzione di tabella dei gruppi contenente la riga selezionata
        grMutexIndex = grIndex / GROUPS_MUTEX_BLOCK_SIZE;
        grSave = NULL;

        WRITE_LOCK((grTable.bloksLocks)[grMutexIndex]);
        // Selezione del grupo da cui rimuovere l'utente
        test = getGroupNode(grRow, (usrSave->user.groups.set)[i], &grSave);
        CHECK_FATAL_ERROR(test, "Cercando il gruppo nella tabella dei gruppi");
        if(grSave == NULL)
        {
            RW_UNLOCK((usrTable.blockLocks)[usrMutexIndex]);
            RW_UNLOCK((grTable.bloksLocks)[grMutexIndex]);
            errno = EPERM;
            return -1;
        }

        // Rimozione dell'utente dal gruppo
        test = removeMember(&(grSave->gr), nickName);
        CHECK_FATAL_ERROR(test, "Rimuovendo l'username dalla lista dei membri del gruppo");
        if(test < 0)
        {
            RW_UNLOCK((usrTable.blockLocks)[usrMutexIndex]);
            RW_UNLOCK((grTable.bloksLocks)[grMutexIndex]);
            errno = EPERM;
            return -1;
        }

        RW_UNLOCK((grTable.bloksLocks)[grMutexIndex]);
    }
    // LogOut di tutti i client connessi con questo utente
    for (int i = 0; i < usrSave->user.distribution.active; i++)
    {
        // Controllo che non venga disconnesso il client che ha fatto richiesta di deregistrazione
        if ((usrSave->user.distribution.addresses)[i] != client)
        {
            // Logout del client
            test = logOut((usrSave->user.distribution.addresses)[i]);
            CHECK_FATAL_ERROR(test, "Effettuando il logout dei diversi client durante la deregistrazione");
        }

        (usrSave->user.distribution.addresses)[i] = -1;
    }

    // Rimozione dell'utente dalla tabella utenti
    test = removeUserNode(usrRow, nickName);
    CHECK_FATAL_ERROR(test, "Rimuovendo l'utente dalla lista utenti");

    RW_UNLOCK((usrTable.blockLocks)[usrMutexIndex]);

    // Aggiornamento statistiche
    WRITE_LOCK(chattyStats.statsLock);
    chattyStats.nusers--;
    RW_UNLOCK(chattyStats.statsLock);

    return 0;
}

int checkRegistration(char* nickName)
{
    if(nickName == NULL){errno = EINVAL; return -1;}

   int test;
   int index;
   int mutexIndex;
   userNode_t* save;
   usersList_t* row;

   // Calcolo dell'indice di riga
   index = userHash((unsigned char*)nickName);
   row = &((usrTable.table)[index]);
   // Calcolo dell'indice della lock relativa alla porzione di tabella contenente la riga selezionata
   mutexIndex = index / USERS_MUTEX_BLOCK_SIZE;
   save = NULL;

   WRITE_LOCK((usrTable.blockLocks)[mutexIndex]);

   test = getUserNode(row, nickName, &save);
   CHECK_FATAL_ERROR(test, "Selezionando l'utente nella riga");

   RW_UNLOCK((usrTable.blockLocks)[mutexIndex]);

   if(save == NULL)
       return 0;
   else
       return 1;
}

int logInUser(char* nickName, int address)
{
    if(nickName == NULL || address <= 0){errno = EINVAL; return -1;}

    int test;
    int index;
    int mutexIndex;
    userNode_t* save;
    usersList_t* row;

    // Calcolo dell'indice di riga
    index = userHash((unsigned char*)nickName);
    row = &((usrTable.table)[index]);
    // Calcolo dell'indice della lock relativa alla porzione di tabella contenente la riga selezionata
    mutexIndex = index / USERS_MUTEX_BLOCK_SIZE;
    save = NULL;

    WRITE_LOCK((usrTable.blockLocks)[mutexIndex]);

    // Selezione dell'utente da loggare
    test = getUserNode(row, nickName, &save);
    CHECK_FATAL_ERROR(test, "Cercando l'utente nella tabella utenti");
    if(save == NULL)
    {
        RW_UNLOCK((usrTable.blockLocks)[mutexIndex]);
        return USER_NOT_REGISTERED;
    }

    // Si controlla che non venga superato il numero massimo di logIs simultanei per utente
    if(save->user.distribution.active >= MAX_USER_SIMULTANEUS_CONNECTIONS)
    {
        RW_UNLOCK((usrTable.blockLocks)[mutexIndex]);
        return USER_LOGIN_QUOTA_EXCEEDED;
    }

    // LogIn
    test = logIn(nickName, address);
    CHECK_FATAL_ERROR(test, "Aggiungendo un login nella tabella dei login");
    if(test != 0)
    {
        RW_UNLOCK((usrTable.blockLocks)[mutexIndex]);
        return test;
    }

    // Viene aggiunto il file descriptor al vettore di indirizzi dell'utente
    test = addAddress(&(save->user.distribution), address);
    CHECK_FATAL_ERROR(test, "Aggiungendo l'indirizzo nella lista degli indirizzi relativa all'utente");
    if(test != 0)
    {
        RW_UNLOCK((usrTable.blockLocks)[mutexIndex]);
        return test;
    }

    RW_UNLOCK((usrTable.blockLocks)[mutexIndex]);

    // Aggiornamento delle statistiche
    WRITE_LOCK(chattyStats.statsLock);
    chattyStats.nonline++;
    RW_UNLOCK(chattyStats.statsLock);

    return 0;
}

int logOutUser(int address)
{
    if(address <= 0){errno = EINVAL; return -1;}

    int test;
    int index;
    int mutexIndex;
    char buf[MAX_NAME_LENGTH];
    userNode_t* save;
    usersList_t* row;

    READ_LOCK((lgTable.strip)[address].logLock);

    // Ci si assicura che il file descriptor passato come parametro risulti attivo
    // nella tabella dei logIn
    if(!((lgTable.strip)[address].active))
    {
        RW_UNLOCK((lgTable.strip)[address].logLock);
        return CLIENT_NOT_LOGGED;
    }
    // Viene copiato il nome dell'utente connesso su questo file descriptor
    strcpy(buf, ((lgTable.strip)[address]).username);

    RW_UNLOCK((lgTable.strip)[address].logLock);

    // Calcolo dell'indice di riga
    index = userHash((unsigned char*)buf);
    row = &((usrTable.table)[index]);
    // Calcolo dell'indice della lock relativa alla porzione di tabella contenente la riga selezionata
    mutexIndex = index / USERS_MUTEX_BLOCK_SIZE;
    save = NULL;

    WRITE_LOCK((usrTable.blockLocks)[mutexIndex]);

    // Selezione dell'utente
    test = getUserNode(row, (lgTable.strip)[address].username, &save);
    CHECK_FATAL_ERROR(test, "Cercando l'utente nella tabella utenti");
    // LogOut dell'indirizzo
    test = logOut(address);
    CHECK_FATAL_ERROR(test, "Rimuovendo un login dalla tabella dei login");

    // Aggiornamento delle statistiche
    WRITE_LOCK(chattyStats.statsLock);
    chattyStats.nonline--;
    RW_UNLOCK(chattyStats.statsLock);

    // Se l'utente ha effettuato una deregistrazione allora non risulterà nella tabella utenti
    // quindi la funzione deve terminare qui
    if(save == NULL)
    {
        RW_UNLOCK((usrTable.blockLocks)[mutexIndex]);
        return USER_NOT_REGISTERED;
    }

    // Altrimenti si rimuove il file descriptor dal vettore indirizzi dell'utente
    test = removeAddress(&(save->user.distribution), address);
    CHECK_FATAL_ERROR(test, "Rimuovendo l'indirizzo dalla lista degli indirizzi relativa all'utente");

    RW_UNLOCK((usrTable.blockLocks)[mutexIndex]);

    return 0;
}

int createGroup(char* grName, char* fnd)
{
    if(grName == NULL || fnd == NULL){errno = EINVAL; return -1;}

    int test;
    int index;
    int mutexIndex;
    userNode_t* save;
    usersList_t* row;

    // Calcolo dell'indice di riga
    index = userHash((unsigned char*)fnd);
    row = &((usrTable.table)[index]);
    // Calcolo dell'indice della lock relativa alla porzione di tabella contenente la riga selezionata
    mutexIndex = index / USERS_MUTEX_BLOCK_SIZE;
    save = NULL;

    WRITE_LOCK((usrTable.blockLocks)[mutexIndex]);

    // Selezione dell'utente fondatore
    test = getUserNode(row, fnd, &save);
    CHECK_FATAL_ERROR(test, "Cercando l'utente nella tabella utenti");
    if(save == NULL)
    {
        RW_UNLOCK((usrTable.blockLocks)[mutexIndex]);
        return USER_NOT_REGISTERED;
    }

    RW_UNLOCK((usrTable.blockLocks)[mutexIndex]);

    // Inserimento del nuovo gruppo
    test = insertGroup(grName, fnd);
    CHECK_FATAL_ERROR(test, "inserendo il gruppo nella tabella dei gruppi");

    return test;
}

int joinUser(char* grName, char* nickName)
{
    if(grName == NULL || nickName == NULL){errno = EINVAL; return -1;}

    int test;
    int grIndex;
    int grMutexIndex;
    int usrIndex;
    int usrMutexIndex;
    userNode_t* usrSave;
    groupNode_t* grSave;
    groupsList_t* grRow;
    usersList_t* usrRow;

    // Calcolo dell'indice di riga nella tabella utenti
    usrIndex = userHash((unsigned char*)nickName);
    usrRow = &((usrTable.table)[usrIndex]);
    // Calcolo dell'indice della lock relativa alla porzione di tabella contenente la riga selezionata
    usrMutexIndex = usrIndex / USERS_MUTEX_BLOCK_SIZE;
    usrSave = NULL;

    WRITE_LOCK((usrTable.blockLocks)[usrMutexIndex]);

    // Selezione dell'utente da associare al gruppo
    test = getUserNode(usrRow, nickName, &usrSave);
    CHECK_FATAL_ERROR(test, "Cercando l'utente nella tabella utenti");
    if(usrSave == NULL)
    {
        RW_UNLOCK((usrTable.blockLocks)[usrMutexIndex]);
        return USER_NOT_REGISTERED;
    }

    // Calcolo dell'indice di riga nella tabella dei gruppi
    grIndex = groupHash((unsigned char*)grName);
    grRow = &((grTable.table)[grIndex]);
    // Calcolo dell'indice della lock relativa alla porzione di tabella dei gruppi contenente la riga selezionata
    grMutexIndex = grIndex / GROUPS_MUTEX_BLOCK_SIZE;
    grSave = NULL;

    WRITE_LOCK((grTable.bloksLocks)[grMutexIndex]);

    // Selezione del gruppo al quale si intende effettuare l'associazione
    test = getGroupNode(grRow, grName, &grSave);
    CHECK_FATAL_ERROR(test, "Cercando il gruppo nella tabella dei gruppi");
    if(grSave == NULL)
    {
        RW_UNLOCK((usrTable.blockLocks)[usrMutexIndex]);
        RW_UNLOCK((grTable.bloksLocks)[grMutexIndex]);
        return test;
    }

    // Aggiunta del gruppo al vettore di appartenenza dell'utente
    test = association(&(usrSave->user.groups), grName);
    CHECK_FATAL_ERROR(test, "Aggiungendo il nome del un gruppo alla lista di appartenenza dell'utente");
    if(test != 0)
    {
        RW_UNLOCK((usrTable.blockLocks)[usrMutexIndex]);
        RW_UNLOCK((grTable.bloksLocks)[grMutexIndex]);
        return test;
    }

    // Aggiunta dell'utente alla lista dei membri del gruppo
    test = addMember(&(grSave->gr), nickName);
    CHECK_FATAL_ERROR(test, "Inserendo l'username nella lista dei membri del gruppo");
    if(test < 0)
    {
        RW_UNLOCK((usrTable.blockLocks)[usrMutexIndex]);
        RW_UNLOCK((grTable.bloksLocks)[grMutexIndex]);
        errno = EPERM;
        return -1;
    }

    RW_UNLOCK((usrTable.blockLocks)[usrMutexIndex]);
    RW_UNLOCK((grTable.bloksLocks)[grMutexIndex]);

    return 0;
}

int disjoinUser(char* grName, char* nickName)
{
    if(grName == NULL || nickName == NULL){errno = EINVAL; return -1;}

    int test;
    int grIndex;
    int grMutexIndex;
    int usrIndex;
    int usrMutexIndex;
    userNode_t* usrSave;
    groupNode_t* grSave;
    groupsList_t* grRow;
    usersList_t* usrRow;

    // Calcolo dell'indice di riga nella tabella utenti
    usrIndex = userHash((unsigned char*)nickName);
    usrRow = &((usrTable.table)[usrIndex]);
    // Calcolo dell'indice della lock relativa alla porzione di tabella contenente la riga selezionata
    usrMutexIndex = usrIndex / USERS_MUTEX_BLOCK_SIZE;
    usrSave = NULL;

    WRITE_LOCK((usrTable.blockLocks)[usrMutexIndex]);

    // Selezione dell'utente da dissociare dal gruppo
    test = getUserNode(usrRow, nickName, &usrSave);
    CHECK_FATAL_ERROR(test, "Cercando l'utente nella tabella utenti");
    if(usrSave == NULL)
    {
        RW_UNLOCK((usrTable.blockLocks)[usrMutexIndex]);
        return USER_NOT_REGISTERED;
    }

    // Calcolo dell'indice di riga nella tabella dei gruppi
    grIndex = groupHash((unsigned char*)grName);
    grRow = &((grTable.table)[grIndex]);
    // Calcolo dell'indice della lock relativa alla porzione di tabella dei gruppi contenente la riga selezionata
    grMutexIndex = grIndex / GROUPS_MUTEX_BLOCK_SIZE;
    grSave = NULL;

    WRITE_LOCK((grTable.bloksLocks)[grMutexIndex]);

    // Selezione del gruppo dal quale si intende dissociare l'utente
    test = getGroupNode(grRow, grName, &grSave);
    CHECK_FATAL_ERROR(test, "Cercando il gruppo nella tabella dei gruppi");
    if(grSave == NULL)
    {
        RW_UNLOCK((usrTable.blockLocks)[usrMutexIndex]);
        RW_UNLOCK((grTable.bloksLocks)[grMutexIndex]);
        return test;
    }

    // Rimozione del gruppo dal vettore di appartenenza dell'utente
    test = dissociation(&(usrSave->user.groups), grName);
    CHECK_FATAL_ERROR(test, "Rimuovendo il nome del un gruppo dalla lista di appartenenza dell'utente");
    if(test != 0)
    {
        RW_UNLOCK((usrTable.blockLocks)[usrMutexIndex]);
        RW_UNLOCK((grTable.bloksLocks)[grMutexIndex]);
        return test;
    }

    // Rimozione dell'utente dalla lista dei membri del gruppo
    test = removeMember(&(grSave->gr), nickName);
    CHECK_FATAL_ERROR(test, "Rimuovendo l'username dalla lista dei membri del gruppo");
    if(test < 0)
    {
        RW_UNLOCK((usrTable.blockLocks)[usrMutexIndex]);
        RW_UNLOCK((grTable.bloksLocks)[grMutexIndex]);
        errno = EPERM;
        return -1;
    }

    RW_UNLOCK((usrTable.blockLocks)[usrMutexIndex]);
    RW_UNLOCK((grTable.bloksLocks)[grMutexIndex]);

    return 0;
}

int deleteGroup(char* grName, char* applicant)
{
    if(grName == NULL || applicant == NULL){errno = EINVAL; return -1;}

    int test;
    int grIndex;
    int grMutexIndex;
    int usrIndex;
    int usrMutexIndex;
    groupNode_t* grSave;
    member_t* grIter;
    groupsList_t* grRow;
    userNode_t* usrSave;
    usersList_t* usrRow;


    // Calcolo dell'indice di riga nella tabella utenti
    usrIndex = userHash((unsigned char*)applicant);
    usrRow = &((usrTable.table)[usrIndex]);
    // Calcolo dell'indice della lock relativa alla porzione di tabella contenente la riga selezionata
    usrMutexIndex = usrIndex / USERS_MUTEX_BLOCK_SIZE;
    usrSave = NULL;

    WRITE_LOCK((usrTable.blockLocks)[usrMutexIndex]);

    // Selezione dell'utente che ha richiesto la cancellazione del gruppo
    test = getUserNode(usrRow, applicant, &usrSave);
    CHECK_FATAL_ERROR(test, "Selezionando l'utente per dissociarlo dal gruppo");
    if(test < 0)
    {
        RW_UNLOCK((usrTable.blockLocks)[usrMutexIndex]);
        return USER_NOT_REGISTERED;
    }
    RW_UNLOCK((usrTable.blockLocks)[usrMutexIndex]);


    // Calcolo dell'indice di riga nella tabella dei gruppi
    grIndex = groupHash((unsigned char*)grName);
    grRow = &((grTable.table)[grIndex]);
    // Calcolo dell'indice della lock relativa alla porzione di tabella dei gruppi contenente la riga selezionata
    grMutexIndex = grIndex / GROUPS_MUTEX_BLOCK_SIZE;
    grSave = NULL;

    WRITE_LOCK((grTable.bloksLocks)[grMutexIndex]);

    // Selezione del gruppo che si intende cancellare
    test = getGroupNode(grRow, grName, &grSave);
    CHECK_FATAL_ERROR(test, "Selezionando il gruppo da eliminare");
    if(grSave == NULL)
    {
        RW_UNLOCK((grTable.bloksLocks)[grMutexIndex]);
        return test;
    }

    // Si verifica che l'utente che ha richiesto la cancellazione sia il
    // fondatore del gruppo che si intende cancellare
    if(strcmp(grSave->gr.founder, applicant) != 0)
    {
        RW_UNLOCK((grTable.bloksLocks)[grMutexIndex]);
        return USER_NOT_FOUNDER;
    }

    // Vengono dissociati dal gruppo tutti i sui membri
    // Quindi viene eseguito un ciclo sulla lista dei membri del gruppo
    while(grSave->gr.members != NULL)
    {
        grIter = grSave->gr.members;
        // Calcolo dell'indice di riga nella tabella utenti
        usrIndex = userHash((unsigned char*)applicant);
        usrRow = &((usrTable.table)[usrIndex]);
        // Calcolo dell'indice della lock relativa alla porzione di tabella contenente la riga selezionata
        usrMutexIndex = usrIndex / USERS_MUTEX_BLOCK_SIZE;
        usrSave = NULL;

        WRITE_LOCK((usrTable.blockLocks)[usrMutexIndex]);

        // Selezione dell'utente da dissociare
        test = getUserNode(usrRow, grIter->nickName, &usrSave);
        CHECK_FATAL_ERROR(test, "Selezionando l'utente per dissociarlo dal gruppo");
        if(test < 0)
        {
            RW_UNLOCK((grTable.bloksLocks)[grMutexIndex]);
            RW_UNLOCK((usrTable.blockLocks)[usrMutexIndex]);
            errno = EPERM;
            return -1;
        }

        // Rimozione del gruppo dal vettore di appartenenza dell'utente
        test = dissociation(&(usrSave->user.groups), grName);
        CHECK_FATAL_ERROR(test, "Rimuovendo il nome del gruppo dalla lista di appartenenza dell'utente");
        if(test < 0)
        {
            RW_UNLOCK((grTable.bloksLocks)[grMutexIndex]);
            RW_UNLOCK((usrTable.blockLocks)[usrMutexIndex]);
            errno = EPERM;
            return -1;
        }

        RW_UNLOCK((usrTable.blockLocks)[usrMutexIndex]);

        // Incremento dell'iteratore
        grSave->gr.members = grIter->next;

        // Si libera spazio
        free(grIter->nickName);
        free(grIter);
    }

    // Si elimina il gruppo dalla lista dei gruppi della tabella grTable
    if(grSave->prev != NULL)
        grSave->prev->next = grSave->next;
    if(grSave->next != NULL)
        grSave->next->prev = grSave->prev;

    destroyGroup(&(grSave->gr));
    free(grSave);

    RW_UNLOCK((grTable.bloksLocks)[grMutexIndex]);

    return 0;
}

int sendAtUser(message_t* msg, user_t* reciever)
{
    if (msg == NULL || reciever == NULL) {errno = EINVAL; return -1;}

    int test;
    int sock;
    int sent = 0;

    // Si cicla sulla struttura delivery dell'utente
    for (int i = 0; i < reciever->distribution.active; i++)
    {
        sock = (reciever->distribution.addresses)[i];

        WRITE_LOCK((lgTable.strip)[sock].logLock);

        // Si verifica la consistenza fra le due struttura
        if(!((lgTable.strip)[sock].active) ||
           strcmp(reciever->nickName, (lgTable.strip)[sock].username) != 0)
        {
            errno = EPERM;
            return -1;
        }

        // Si scrive il messaggio sul socket
        test = sSendMsg(sock, msg);
        CHECK_FATAL_ERROR(test, "Scrivendo il messaggio sul socket associato al destinatario");
        if(test < 0)
        {
            RW_UNLOCK((lgTable.strip)[sock].logLock);
            continue;
        }

        RW_UNLOCK((lgTable.strip)[sock].logLock);

        // Si incrementa il contatore di invii
        sent++;
    }

    // Aggiornamento dello storico dei messaggi dell'utente
    if(sent > 0)
        test = pushMessage(&(reciever->userIncomeHistory), msg, TRUE);
    else
        test = pushMessage(&(reciever->userIncomeHistory), msg, FALSE);
    CHECK_FATAL_ERROR(test, "Inserendo il messaggio nella history del destinatario");

    return sent;
}

int sendOk(int client, int bufLen, char* buf)
{
    if(client <= 0 || (bufLen > 0 && buf == NULL)){errno = EINVAL; return -1;}

    int test;
    message_hdr_t header;

    // Impostazione dell'intestazione
    setHeader(&header, OP_OK, "Chatty");

    WRITE_LOCK((lgTable.strip)[client].logLock);

    // Si verifica che l'indirizzo sia effettivamente attivo
    if(!((lgTable.strip)[client].active))
    {
        RW_UNLOCK((lgTable.strip)[client].logLock);
        errno = EPERM;
        return -1;
    }

    // Invio dell'intestazione
    test = sSendHeader(client, &header);
    CHECK_FATAL_ERROR(test, "Scrivendo l'header del messaggio sul socket associato al destinatario");
    if(test < 0)
    {
        RW_UNLOCK((lgTable.strip)[client].logLock);
        return test;
    }

    // Si verifica la presenza di una parte dati
    if(bufLen > 0)
    {
        message_data_t data;

        // Impostazione della parte dati
        setData(&data, "", buf, bufLen);

        // Invio della parte dati
        test = sSendData(client, &data);
        CHECK_FATAL_ERROR(test, "Scrivendo la parte dati del messaggio sul socket associato al destinatario");
        if(test < 0)
        {
            RW_UNLOCK((lgTable.strip)[client].logLock);
            return test;
        }
    }

    RW_UNLOCK((lgTable.strip)[client].logLock);

    return 0;
}

int sendError(int client, op_t errCode)
{
    // Si verifica la correttezza dei parametri e che il codice di errore
    // sia conforme a quanto indicato in ops.h
    if(client <= 0 ||
       (errCode != OP_NICK_ALREADY && errCode != OP_NICK_UNKNOWN &&
        errCode != OP_MSG_TOOLONG && errCode != OP_NO_SUCH_FILE &&
        errCode != OP_FAIL))
    {errno = EINVAL; return -1;}

    int test;
    message_hdr_t header;

    setHeader(&header, errCode, "Chatty");

    WRITE_LOCK((lgTable.strip)[client].logLock);

    // Si verifica se l'indirizzo è attivo
    if(!((lgTable.strip)[client].active))
    {// Indirizzo non attivo (quando un client non è riuscito a loggarsi)
        RW_UNLOCK((lgTable.strip)[client].logLock);
        test = sSendHeader(client, &header);
    }
    else
    {// Indirizzo attivo
        test = sSendHeader(client, &header);
        RW_UNLOCK((lgTable.strip)[client].logLock);
    }
    CHECK_FATAL_ERROR(test, "Scrivendo l'header del messaggio sul socket associato al destinatario");

    // Aggiornamento della statistica degli errori
    WRITE_LOCK(chattyStats.statsLock);
    chattyStats.nerrors++;
    RW_UNLOCK(chattyStats.statsLock);

    if(test < 0)
        return test;

    return 0;
}

int ship(message_t* outcome)
{
    // Controllo sui parametri
    if(outcome == NULL){errno = EINVAL; return -1;}
    // Controllo sulla lunghezza del messaggio
    if((outcome->hdr.op == POSTTXT_OP || outcome->hdr.op == POSTTXTALL_OP)
       && (int)outcome->data.hdr.len > MaxMsgSize)
        return TEXT_MESSAGE_TOO_LONG;

    int test;
    int sent = 0;
    int tmpDelivered = 0;
    int tmpFilesDeliverd = 0;
    int tmpNotDeliverd = 0;
    int tmpFilesNotDeliverd = 0;
    usersList_t* usrRow = NULL;
    userNode_t* usrSelect = NULL;
    message_t* cpyMex = NULL;

    // Alternative di messaggio da inviare
    switch (outcome->hdr.op) {
    case POSTFILE_OP:
    case POSTTXT_OP:
    {// Invio di un messaggio di tipo file o testuale ad un unico destinatario o gruppo

        int usrIndex;
        int usrMutexIndex;
        int grIndex;
        int grMutexIndex;
        groupsList_t* grRow = NULL;
        groupNode_t* grSelect = NULL;

        // Impostazione del messaggio
        if(outcome->hdr.op == POSTTXT_OP)
            outcome->hdr.op = TXT_MESSAGE;
        else if (outcome->hdr.op == POSTFILE_OP)
            outcome->hdr.op = FILE_MESSAGE;

        // Controllo sull'esistenza di un gruppo utenti con quel nome
        // Calcolo dell'indice di riga nella tabella dei gruppi
        grIndex = groupHash((unsigned char*)outcome->data.hdr.receiver);
        grRow = &((grTable.table)[grIndex]);
        // Calcolo dell'indice della lock relativa alla porzione di tabella dei gruppi contenente la riga selezionata
        grMutexIndex = grIndex / GROUPS_MUTEX_BLOCK_SIZE;
        grSelect = NULL;

        // Ricerca del gruppo
        WRITE_LOCK((grTable.bloksLocks)[grMutexIndex]);
        test = getGroupNode(grRow, outcome->data.hdr.receiver, &grSelect);
        if(grSelect != NULL)
        {// Invio del messaggio al gruppo
            member_t* iter = grSelect->gr.members;

            // Si verifica che il mittente sia membro del gruppo destinatario
            if(!isMember(&(grSelect->gr), outcome->hdr.sender))
            {
                RW_UNLOCK((grTable.bloksLocks)[grMutexIndex]);
                return USER_NOT_JOINT;
            }

            while(iter != NULL)
            {
                // Calcolo dell'indice di riga nella tabella utenti
                usrIndex = userHash((unsigned char*)iter->nickName);
                usrRow = &((usrTable.table)[usrIndex]);
                // Calcolo dell'indice della lock relativa alla porzione di tabella contenente la riga selezionata
                usrMutexIndex = usrIndex / USERS_MUTEX_BLOCK_SIZE;
                usrSelect = NULL;

                WRITE_LOCK((usrTable.blockLocks)[usrMutexIndex]);

                // Selezione del membro all'interno della tabella degli utenti
                test = getUserNode(usrRow, iter->nickName, &usrSelect);
                CHECK_FATAL_ERROR(test, "Cercando il membro del gruppo  nella tabella utenti");
                if (test < 0 || usrSelect == NULL)
                {
                    RW_UNLOCK((usrTable.blockLocks)[usrMutexIndex]);
                    RW_UNLOCK((grTable.bloksLocks)[grMutexIndex]);
                    errno = EPERM;
                    return -1;
                }

                // Copia del messaggio
                test = copyMessage(outcome, &cpyMex);
                CHECK_FATAL_ERROR(test, "Copiando il messagio da inviare");

                // Invio del messaggio
                test = sendAtUser(cpyMex, &(usrSelect->user));
                CHECK_FATAL_ERROR(test, "Inviando il messaggio al membro del gruppo");

                RW_UNLOCK((usrTable.blockLocks)[usrMutexIndex]);

                // Aggiornamento delle statistiche temporanee
                if (test == 0)
                {
                    if(outcome->hdr.op == TXT_MESSAGE)
                        tmpNotDeliverd++;
                    if(outcome->hdr.op == FILE_MESSAGE)
                        tmpFilesNotDeliverd++;
                } else
                {
                    if(outcome->hdr.op == TXT_MESSAGE)
                        tmpDelivered++;
                    if(outcome->hdr.op == FILE_MESSAGE)
                        tmpFilesDeliverd++;
                }

                // Incremento del contatore di invii e dell'iteratore
                sent += test;
                iter = iter->next;
            }
            RW_UNLOCK((grTable.bloksLocks)[grMutexIndex]);
        }
        else
        {// Ivio del messaggio a singolo utente
            RW_UNLOCK((grTable.bloksLocks)[grMutexIndex]);

            // Calcolo dell'indice di riga nella tabella utenti
            usrIndex = userHash((unsigned char*)outcome->data.hdr.receiver);
            usrRow = &((usrTable.table)[usrIndex]);
            // Calcolo dell'indice della lock relativa alla porzione di tabella contenente la riga selezionata
            usrMutexIndex = usrIndex / USERS_MUTEX_BLOCK_SIZE;
            usrSelect = NULL;

            WRITE_LOCK((usrTable.blockLocks)[usrMutexIndex]);

            // Selezione dell'utente nella tabella
            test = getUserNode(usrRow, outcome->data.hdr.receiver, &usrSelect);
            CHECK_FATAL_ERROR(test, "Cercando l'utente nella tabella utenti");
            if (test < 0 || usrSelect == NULL)
            {
                RW_UNLOCK((usrTable.blockLocks)[usrMutexIndex]);
                RW_UNLOCK((grTable.bloksLocks)[grMutexIndex]);
                errno = EPERM;
                return -1;
            }

            // Copia del messaggio
            test = copyMessage(outcome, &cpyMex);
            CHECK_FATAL_ERROR(test, "Copiando il messagio da inviare");

            // Invio del messaggio
            test = sendAtUser(cpyMex, &(usrSelect->user));
            CHECK_FATAL_ERROR(test, "Inviando il messaggio all'utente");

            RW_UNLOCK((usrTable.blockLocks)[usrMutexIndex]);

            // Aggiornamento delle statistiche temporanee
            if (test == 0)
            {
                if(outcome->hdr.op == TXT_MESSAGE)
                    tmpNotDeliverd++;
                if(outcome->hdr.op == FILE_MESSAGE)
                    tmpFilesNotDeliverd++;
            }
            else
            {
                if(outcome->hdr.op == TXT_MESSAGE)
                    tmpDelivered++;
                if(outcome->hdr.op == FILE_MESSAGE)
                    tmpFilesDeliverd++;
            }

            cpyMex = NULL;
            // Incremento del contatore degli invii
            sent += test;
        }

        break;
    }
    case POSTTXTALL_OP:
    {// Invio a tutti gli utenti registrati
        outcome->hdr.op = TXT_MESSAGE;

        // Ciclo fra i blocchi
        for(int i = 0; i < USERS_TABLE_SIZE / USERS_MUTEX_BLOCK_SIZE; i++)
        {
            WRITE_LOCK((usrTable.blockLocks)[i]);

            // Ciclo all'interno del blocco
            for(int j = 0; j < USERS_MUTEX_BLOCK_SIZE; j++)
            {
                // Selezione della riga
                usrRow = &((usrTable.table)[i * USERS_MUTEX_BLOCK_SIZE + j]);
                usrSelect = usrRow->head;

                // Ciclo all'interno della riga
                while(usrSelect != NULL)
                {
                    // Il messaggio non viene inviato al mittente
                    if(strcmp(usrSelect->user.nickName, outcome->hdr.sender) != 0)
                    {
                        // Copia del messaggio
                        test = copyMessage(outcome, &cpyMex);
                        CHECK_FATAL_ERROR(test, "Copiando il messagio da inviare");

                        // Invio del messaggio
                        test = sendAtUser(cpyMex, &(usrSelect->user));
                        CHECK_FATAL_ERROR(test, "Inviando il messaggio all'utente");

                        // Aggiornamento delle statistiche temporanee
                        if (test == 0)
                        {
                            if(outcome->hdr.op == TXT_MESSAGE)
                                tmpNotDeliverd++;
                            if(outcome->hdr.op == FILE_MESSAGE)
                                tmpFilesNotDeliverd++;
                        }
                        else
                        {
                            if(outcome->hdr.op == TXT_MESSAGE)
                                tmpDelivered++;
                            if(outcome->hdr.op == FILE_MESSAGE)
                                tmpFilesDeliverd++;
                        }

                        // Incremento del contatore invii
                        sent += test;
                    }

                    // Incremento iteratore riga
                    usrSelect= usrSelect->next;
                }
            }

            RW_UNLOCK((usrTable.blockLocks)[i]);
        }

        break;
    }
    default:
        errno = ENOMSG;
        return -1;
    }

    // Aggiornamento delle statistiche di sistema con quelle temporanee
    WRITE_LOCK(chattyStats.statsLock);
    chattyStats.ndelivered += tmpDelivered;
    chattyStats.nfiledelivered += tmpFilesDeliverd;
    chattyStats.nnotdelivered += tmpNotDeliverd;
    chattyStats.nfilenotdelivered += tmpFilesNotDeliverd;
    RW_UNLOCK(chattyStats.statsLock);

    return sent;
 }

int getLoggedUsersList(char** save)
{
    int len = 0;
    char* buf = NULL;;

    // Ciclo all'interno della tabella dei logIn
    for(int i=0; i < MaxConnections + ThreadsInPool + 1; i++)
    {
        READ_LOCK((lgTable.strip)[i].logLock);

        // Controllo se il file descriptor è attivo
        if((lgTable.strip)[i].active)
        {
            // Si ingrandisce il buffer
            buf = (char*)realloc(buf,  len + MAX_NAME_LENGTH + 1);
            if(buf == NULL){RW_UNLOCK((lgTable.strip)[i].logLock); return -1;}

            // Si concatena opportunamente il nome dell'utente nel buffer
            strncpy(buf + len , (lgTable.strip)[i].username, MAX_NAME_LENGTH+1);
            len = len + MAX_NAME_LENGTH + 1;
        }

        RW_UNLOCK((lgTable.strip)[i].logLock);
    }

    *save = buf;

    return len;
}

int sendUserHistory(char* nickName, int address)
{
    // Controllo sulla validità degli argomenti
    if(nickName == NULL || address <= 0){errno = EINVAL; return -1;}

    int test;
    int index;
    int nItems;
    int mutexIndex;
    int tmpDelivered = 0;
    int tmpFilesDeliverd = 0;
    int tmpNotDeliverd = 0;
    int tmpFilesNotDeliverd = 0;
    char* buf;
    userNode_t* save;
    usersList_t* row;
    old_t* iterator;


    // Calcolo dell'indice di riga
    index = userHash((unsigned char*)nickName);
    row = &((usrTable.table)[index]);
    // Calcolo dell'indice della lock relativa alla porzione di tabella contenente la riga selezionata
    mutexIndex = index / USERS_MUTEX_BLOCK_SIZE;

    // Allocazione del buffer
    buf = (char*)calloc(1, sizeof(size_t));
    if(buf == NULL){return -1;}

    save = NULL;

    WRITE_LOCK((usrTable.blockLocks)[mutexIndex]);

    // Selezione dell'utente che ha richiesto lo storico dei messaggi
    test = getUserNode(row, nickName, &save);
    CHECK_FATAL_ERROR(test, "Cercando l'utente nella tabella utenti: ");
    if(save == NULL)
    {// Utente sconosciuto
        RW_UNLOCK((usrTable.blockLocks)[mutexIndex]);
        return USER_NOT_REGISTERED;
    }

    // Salvataggio del numero (in formato binario) di messaggi presenti nello storico
    nItems = save->user.userIncomeHistory.loadFactor;
    copyToBinary((void*)(&nItems), sizeof(size_t), &buf);

    // Invio del numero di messaggi presenti nello storico all'utento che lo ha richiesto
    test = sendOk(address, sizeof(size_t), buf);
    CHECK_FATAL_ERROR(test, "Inviando il messaggio di OK contenente il numero di messaggi presenti nella history dell'utente");
    if(test == CLIENT_CLOSED_CONNECTION)
    {
        free(buf);
        return test;
    }

    WRITE_LOCK((lgTable.strip)[address].logLock);

    // Sacansione dello storico
    for(int i=0; i<nItems; i++)
    {
        iterator = &((save->user.userIncomeHistory.cronology)[i]);

        test = sSendMsg(address, iterator->mex);
        CHECK_FATAL_ERROR(test, "Scrivendo il messaggio sul socket associato al destinatario");
        if(test < 0)
        {
            RW_UNLOCK((lgTable.strip)[address].logLock);
            break;
        }

        // Aggiornamento delle statistiche temporanee
        if(iterator->delivered == FALSE)
        {
            if(iterator->mex->hdr.op == TXT_MESSAGE)
            {
                tmpNotDeliverd--;
                tmpDelivered++;
            }
            else if (iterator->mex->hdr.op == FILE_MESSAGE)
            {
                tmpFilesNotDeliverd--;
                tmpFilesDeliverd++;
            }

            iterator->delivered = TRUE;
        }
    }

    RW_UNLOCK((lgTable.strip)[address].logLock);
    RW_UNLOCK((usrTable.blockLocks)[mutexIndex]);

    // Aggiornamento delle statistiche
    WRITE_LOCK(chattyStats.statsLock);
    chattyStats.ndelivered += tmpDelivered;
    chattyStats.nfiledelivered += tmpFilesDeliverd;
    chattyStats.nnotdelivered += tmpNotDeliverd;
    chattyStats.nfilenotdelivered += tmpFilesNotDeliverd;
    RW_UNLOCK(chattyStats.statsLock);

    free(buf);

    return 0;
}
/*--------------------------------------------------------------*/
