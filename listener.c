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
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

#include "utils.h"
#include "errors.h"
#include "message.h"
#include "connections.h"
#include "groups.h"
#include "settings.h"
#include "income.h"
#include "communication.h"
#include "fdSet.h"
#include "listener.h"

/**
 * @file  listener.c
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Contiene la definizione della funzione di routine eseguita da ogni
 * thread worker generato dal server.
 */

void* listener(void* args)
{
    // Identificatore del thread
    int ID;
    // Variabile di test per il valore di ritorno delle funzioni
    int test;
    // File descriptor del client da servire
    int client;

    ID = *((int*)args);

    while(TRUE)
    {
        // Messaggio in entrata
        message_t INmex;
        // Messaggio in uscita
        message_t OUTmex;

        // Inizializzazione messaggi entrata/uscita
        initializeMessage(&INmex);
        initializeMessage(&OUTmex);

        // Estrazione della richiesta dalla catena
        client = popIncome();
        CHECK_FATAL_ERROR(client, "Estraendo il file descriptor dalla coda di servizio");

        // Controllo se è stato estratto il messaggio di terminazione
        if(client == INCOME_LIST_TERM)
        {// Estratto messaggio di terminazione
            threadPrint(ID, "Messaggio di terminazione ricevuto\n", eBOLDCYAN);
            break;
        }
        else
        {// Estratto file descriptor di un client pronto a scrivere
            threadPrint(ID, "Estratto file descriptor di un client da servire: %d\n", eNORMAL, client);
        }

        // Lettura del messaggio dal client
        threadPrint(ID, "Lettura del messaggio di richiesta dal client connesso su %d... ", eNORMAL, client);
        test = sReadMsg(client, &INmex);
        //CHECK_FATAL_ERROR(test, "Leggendo il messaggio di richiesta dal client");
        print("LETTO\n", eBOLDGREEN);
        if(test <= 0)
        {// Il client ha chiuso la connessione
            // Disconnessione del client
            threadPrint(ID, "Disconnessione del client connesso su %d... ", eNORMAL, client);
            test = logOutUser(client);
            CHECK_FATAL_ERROR(test, "Esegunedo l'operazione di disconnessione del client");
            if(test == CLIENT_NOT_LOGGED)
            {// Il client non aveva esiguito il login
                test = close(client);
                CHECK_FATAL_ERROR(test, "Esegunedo l'operazione di disconnessione del client");
            }
            print("DISCONNESSO\n", eBOLDGREEN);

            continue;
        }

        /*------------------------ALTERNATIVE DI MESSAGGI-------------------------*/
        switch(INmex.hdr.op){
        case REGISTER_OP:
        // Messaggio di richiesta di registrazione
        {
            // Registrazione nella tabella degli utenti
            threadPrint(ID, "Registrando l'utente \"%s\" connesso su %d... ", eNORMAL, INmex.hdr.sender, client);
            test = registerUser(INmex.hdr.sender);
            CHECK_FATAL_ERROR(test, "Eseguendo l'oerazione di registrazione di un utente");
            if(test != 0)
            {// Registrazione fallita
                print("REGISTRAZIONE FALLITA\n", eBOLDRED);
                printError(ID, test, "Registrando l'utente \"%s\" sul socket %d: ", INmex.hdr.sender, client);
                // Invio del messaggio di errore
                test = sendError(client, OP_NICK_ALREADY);
                CHECK_FATAL_ERROR(test, "Inviando il messaggio di errore");
                if(test < 0)
                    printError(ID, test, "Inviando il messaggio di errore su %d: ", client);
                break;
            }
            print("REGISTRATO\n", eBOLDGREEN);
        }
        case CONNECT_OP:
        // Messaggio di richiesta di logIn
        {
            // Operazione di login
            threadPrint(ID, "Effettuando il login dell'utente \"%s\" connesso su %d... ", eNORMAL, INmex.hdr.sender, client);
            test = logInUser(INmex.hdr.sender, client);
            CHECK_FATAL_ERROR(test, "Eseguendo l'operazione di login di un utente");
            if(test != 0)
            {// Login fallito
                print("LOGIN FALLITO\n", eBOLDRED);
                printError(ID, test, "Effettuando il login di \"%s\" connesso su %d: ", INmex.hdr.sender, client);
                // Invio del messaggio di errore
                if(test == USER_NOT_REGISTERED)
                    test = sendError(client, OP_NICK_UNKNOWN);
                else
                    test = sendError(client, OP_FAIL);
                CHECK_FATAL_ERROR(test, "Inviando il messaggio di errore");
                if(test < 0)
                    printError(ID, test, "Inviando il messaggio di errore su %d: ", client);
                break;
            }
            print("LOGIN RIUSCITO\n", eBOLDGREEN);
        }
        case USRLIST_OP:
        // Messaggio di richiesta della lista degli utenti online
        {
            // Puntatore alla stringa contenente la lista degli utenti online
            char* logList = NULL;
            // Generazione della lista degli itenti loggati
            threadPrint(ID, "Generando la lista degli utenti online...", eNORMAL);
            test = getLoggedUsersList(&logList);
            CHECK_FATAL_ERROR(test, "Generando la lista degli utenti online");
            print("GENERATA\n", eBOLDGREEN);
            // Invio del messaggio contenente la lista
            threadPrint(ID, "Inviando la lista degli utenti online a \"%s\" connesso su %d... ",
                        eNORMAL, INmex.hdr.sender, client);
            test = sendOk(client, test, logList);
            CHECK_FATAL_ERROR(test, "Inviando la lista degli utenti online");
            if (test < 0)
                printError(ID, test, "Inviando la lista degli utenti online su %d: ", client);
            else
                print("INVIATA\n", eBOLDGREEN);
            // Cancellazione della lista
            if(logList != NULL)
                free(logList);
            break;
        }
        case POSTTXTALL_OP:
        // Messaggio di richiesta di invio messaggio di testo a tutti gli utenti registrati
        case POSTTXT_OP:
        // Messaggio di richiesta di invio messaggio di testo ad uno specifico utente
        {
            // Verifica sulla registrazione del mittente
            if(!checkRegistration(INmex.hdr.sender))
            {
                printError(ID, -14, "Ricevuta una richiesta di invio messaggio da un utente non registrato \"%s\": ",
                           INmex.hdr.sender);
                // Invio del messaggio di errore
                test = sendError(client, OP_NICK_UNKNOWN);
                if(test < 0)
                    printError(ID, test, "Inviando il messaggio di errore su %d: ", client);
                break;
            }
            if(strcmp(INmex.data.hdr.receiver, "") == 0)
                threadPrint(ID, "Inviando il messaggio da \"%s\" a tutti gli utenti registrati... ",
                            eNORMAL, INmex.hdr.sender, INmex.data.hdr.receiver);
            else
                threadPrint(ID, "Inviando il messaggio da \"%s\" a \"%s\"... ",
                            eNORMAL, INmex.hdr.sender, INmex.data.hdr.receiver);
            test = ship(&INmex);
            CHECK_FATAL_ERROR(test, "Inviando il messaggio al destinatario");
            if(test < 0)
            {// Invio fallito
                print("INVIO FALLITO\n", eBOLDRED);
                printError(ID, test, "Inviando un messaggio da \"%s\" (connesso su %d) a \"%s\": ",
                           INmex.hdr.sender, client, INmex.data.hdr.receiver);
                // Invio del messaggio di errore
                if(test == RECEIVER_UNKNOWN || test == USER_NOT_JOINT)
                    test = sendError(client, OP_NICK_UNKNOWN);
                else if(test == TEXT_MESSAGE_TOO_LONG)
                    test = sendError(client, OP_MSG_TOOLONG);
                else
                    test = sendError(client, OP_FAIL);
                CHECK_FATAL_ERROR(test, "Inviando il messaggio di errore");
                if(test < 0)
                    printError(ID, test, "Inviando il messaggio di errore su %d: ", client);
                break;
            }
            print("INVIATO\n", eBOLDGREEN);
            // Invio del messaggio di risposta positiva al mittente
            test = sendOk(client, 0, NULL);
            CHECK_FATAL_ERROR(test, "Inviando il messaggio di invio messaggio riuscito");
            if(test < 0)
                printError(ID, test, "Inviando il messaggio di invio riuscito al mittente \"%s\" connesso su %d: ",
                           INmex.hdr.sender, client);
            break;
        }
        case GETPREVMSGS_OP:
        // Messaggio di richiesta dello storico dei messaggi
        {
            // Invio dello storico
            threadPrint(ID, "Inviando lo storico dei messaggi a \"%s\" connesso su %d... ", eNORMAL, INmex.hdr.sender, client);
            test = sendUserHistory(INmex.hdr.sender, client);
            CHECK_FATAL_ERROR(test, "Inviando lo storico dei messaggi all'utente");
            if(test != 0)
            {
                print("INVIO FALLITO\n", eBOLDRED);
                if(test != CLIENT_CLOSED_CONNECTION)
                {// Invio del messaggio di errore
                    test = sendError(client, OP_FAIL);
                    CHECK_FATAL_ERROR(test, "Inviando il messaggio di errore");
                    if(test < 0)
                        printError(ID, test, "Inviando il messaggio di errore su %d: ", client);
                }
                break;
            }
            print("INVIATO\n", eBOLDGREEN);
            break;
        }
        case UNREGISTER_OP:
        // Messaggio di richiesta di deregistrazione di un utente
        {
            threadPrint(ID, "Deregistrando l'utente \"%s\" connesso su %d... ", eNORMAL, INmex.hdr.sender, client);
            // Rimozione dell'utente dalla tabella utenti
            test = unregisterUser(INmex.hdr.sender, client);
            CHECK_FATAL_ERROR(test, "Effettuando l'operazione di deregistrazione");
            if(test != 0)
            {
                print("DEREGISTRAZIONE FALLITA\n", eBOLDRED);
                printError(ID, test, "Disconnettendo tutti i clients logati con l'utente \"%s\", tranne quello connesso su %d: ",
                           INmex.hdr.sender, client);
                test = sendError(client, OP_FAIL);
                CHECK_FATAL_ERROR(test, "Inviando il messaggio di errore");
                if(test < 0)
                   printError(ID, test, "Inviando il messaggio di errore su %d: ", client);
                break;
            }
            print("DEREGISTRATO\n", eBOLDGREEN);
            // Invio del messaggio di avvenuta deregistrazione
            test = sendOk(client, 0, NULL);
            CHECK_FATAL_ERROR(test, "Inviando il messaggio di avvenuta deregistrazione");
            if(test != 0)
                printError(ID, test, "Inviando il messaggio di avvenuta deregistrazione su %d: ", client);
            break;
        }
        case POSTFILE_OP:
        // Messaggio di richiesta di invio di un file
        {
            int fd;
            char* fileName = NULL;
            char* filePath;
            message_data_t fileMex;
            // Verifica sulla registrazione del mittente
            if(!checkRegistration(INmex.hdr.sender))
            {
                printError(ID, -14, "Ricevuta una richiesta di invio messaggio da un utente non registrato \"%s\": ",
                           INmex.hdr.sender);
                // Invio del messaggio di errore
                test = sendError(client, OP_NICK_UNKNOWN);
                if(test < 0)
                    printError(ID, test, "Inviando il messaggio di errore su %d: ", client);
                break;
            }
            // Estrzione del nome del file
            test = getNameFromPath(INmex.data.buf, &fileName);
            CHECK_FATAL_ERROR(test, "Estraendo il nome del file");
            // Composizione del nuovo path del file
            filePath = calloc(strlen(DirName)+1+strlen(fileName)+1, sizeof(char));
            CHECK_NULL_FATAL_ERROR(filePath, "Allocando spazio per il nuovo path del file");
            strcpy(filePath, DirName);
            strcat(filePath, "/");
            strcat(filePath, fileName);
            // Lettura del file
            threadPrint(ID, "Leggendo il file \"%s\" da \"%s\" connesso su %d...", eNORMAL, fileName, INmex.hdr.sender, client);
            test = sReadFile(client, &fileMex);
            CHECK_FATAL_ERROR(test, "Leggendo il file dal socket");
            if(test <= 0)
            {
                print("LETTURA FALLITA\n", eBOLDRED);
                printError(ID, test, "Leggendo il file \"%s\" da \"%s\": ", fileName, INmex.hdr.sender);
                // Invio del messaggio di errore
                if(test == FILE_MESSAGE_TOO_BIG)
                {
                    test = sendError(client, OP_MSG_TOOLONG);
                    CHECK_FATAL_ERROR(test, "Inviando il messaggio di errore");
                    if(test < 0)
                        printError(ID, test, "Inviando il messaggio di errore su %d: ", client);
                }
                free(fileMex.buf);
                free(filePath);
                free(fileName);
                break;
            }
            print("FILE RICEVUTO\n", eBOLDGREEN);
            // Controllo sull'esistenza del file
            if(access(filePath, F_OK) != 0)
            {// Il file non esiste nella cartella dei files del sistema
                char* placeHolder;
                unsigned int written = 0;
                threadPrint(ID, "Salvataggio il file \"%s\" in \"%s\"... ", eNORMAL, fileName, DirName);
                // Creazione del file
                fd = open(filePath, O_CREAT | O_RDWR, 0666);
                CHECK_FATAL_ERROR(test, "Creando il file per il salvataggio");
                // Scrittura del file
                if(fileMex.hdr.len != 0)
                {
                    placeHolder = fileMex.buf;
                    while(written < fileMex.hdr.len)
                    {
                        test = write(fd, placeHolder, fileMex.hdr.len - written);
                        CHECK_FATAL_ERROR(test, "Scrivendo il file ricevuto");
                        written += test;
                        placeHolder += test;
                    }
                }
                test = close(fd);
                CHECK_FATAL_ERROR(test, "Chiudendo il file appena scritto");
                print("SALVATO\n", eBOLDGREEN);
            }
            else
                threadPrint(ID, "Il file \"%s\" è gia presente in \"%s\"\n", eNORMAL, fileName, DirName);
            // Preparazione del messaggio
            OUTmex.hdr.op = POSTFILE_OP;
            strcpy(OUTmex.hdr.sender, INmex.hdr.sender);
            strcpy(OUTmex.data.hdr.receiver, INmex.data.hdr.receiver);
            OUTmex.data.hdr.len = strlen(fileName)+1;
            OUTmex.data.buf = strdup(fileName);
            // Invio del messaggio di tipo file al destinatario
            threadPrint(ID, "Invio del messaggio di tipo file a \"%s\" da \"%s\" connesso su %d...",
                        eNORMAL, OUTmex.data.hdr.receiver, OUTmex.hdr.sender, client);
            test = ship(&OUTmex);
            CHECK_FATAL_ERROR(test, "Inviando il messaggio di tipo file al destinatario");
            if(test < 0)
            {// Invio fallito
                print("INVIO FALLITO\n", eBOLDRED);
                printError(ID, test, "Inviando un messaggio di tipo file a \"%s\" da \"%s\" connesso su %d: ",
                           OUTmex.data.hdr.receiver, OUTmex.hdr.sender, client);
                // Invio del messaggio di errore
                if(test == RECEIVER_UNKNOWN)
                    test = sendError(client, OP_NICK_UNKNOWN);
                else
                    test = sendError(client, OP_FAIL);
                CHECK_FATAL_ERROR(test, "Inviando il messaggio di errore");
                if(test < 0)
                    printError(ID, test, "Inviando il messaggio di errore su %d: ", client);
                free(fileMex.buf);
                free(filePath);
                free(fileName);
                break;
            }
            print("INVIO RIUSCITO\n", eBOLDGREEN);
            // Invio del messaggio di risposta positiva al mittente
            test = sendOk(client, 0, NULL);
            CHECK_FATAL_ERROR(test, "Inviando il messaggio di invio messaggio riuscito");
            if(test < 0)
                printError(ID, test, "Inviando il messaggio di invio riuscito al mittente \"%s\" connesso su %d: ",
                           INmex.hdr.sender, client);
            // Cancelazione della memoria occupata dai buffer
            free(fileMex.buf);
            free(filePath);
            free(fileName);
            break;
        }
        case GETFILE_OP:
        // Messaggio di richiesta di scaricamento di un file
        {
            int fd;
            char* map;
            char* filePath;
            struct stat st;
            // Verifica sulla registrazione del mittente
            if(!checkRegistration(INmex.hdr.sender))
            {
                printError(ID, -14, "Ricevuta una richiesta di invio messaggio da un utente non registrato \"%s\": ",
                           INmex.hdr.sender);
                // Invio del messaggio di errore
                test = sendError(client, OP_NICK_UNKNOWN);
                if(test < 0)
                    printError(ID, test, "Inviando il messaggio di errore su %d: ", client);
                break;
            }
            // Composizione del path del file
            filePath = calloc(strlen(DirName)+1+strlen(INmex.data.buf)+1, sizeof(char));
            CHECK_NULL_FATAL_ERROR(filePath, "Allocando spazio per il nuovo path del file");
            strcpy(filePath, DirName);
            strcat(filePath, "/");
            strcat(filePath, INmex.data.buf);
            // Controllo dell'esistenza del file
            threadPrint(ID, "Controllando l'esistenza di \"%s\" in \"%s\"... ", eNORMAL, INmex.data.buf, DirName);
            if(access(filePath, F_OK) != 0)
            {// Il file non esiste
                print("FILE NON TROVATO\n", eBOLDRED);
                // Invio del messaggio di errore
                test = sendError(client, OP_NO_SUCH_FILE);
                CHECK_FATAL_ERROR(test, "Inviando il messaggio di errore");
                if(test != 0)
                    printError(ID, test, "Inviando il messaggio di errore su %d: ", client);
                free(filePath);
                break;
            }
            print("FILE TROVATO\n", eBOLDGREEN);
            // Estrazione delle informazioni sul file
            test = stat(filePath, &st);
            CHECK_FATAL_ERROR(test, "Estraendo le informazioni sul file richiesto");
            // Apertura del file
            fd = open(filePath, O_RDONLY);
            CHECK_FATAL_ERROR(fd, "Aprendo il file richiesto");
            // Mappatura del file in memoria
            map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
            if(map == MAP_FAILED)
                CHECK_FATAL_ERROR(-1, "Mappando il file richiesto in memoria");
            // Chiusura del file
            test = close(fd);
            CHECK_FATAL_ERROR(test, "Chiudendo il file appena mappato in memoria");
            // Invio del messaggio di risposta contenente il file
            threadPrint(ID, "Inviando il messaggio contenente il file \"%s\" a \"%s\" connesso su %d... ",
                        eNORMAL, INmex.data.buf, INmex.hdr.sender, client);
            test = sendOk(client, st.st_size, map);
            CHECK_FATAL_ERROR(test , "Inviando il messaggio di risposta contenente il file richiesto");
            if(test == CLIENT_CLOSED_CONNECTION)
            {// Invio fallito per chiusura della connessione
                print("INVIO FALLITO\n", eBOLDRED);
                printError(ID, test, "Inviando il messaggio di risposta contenente il file \"%s\" su %d: ",
                           INmex.data.buf, client);
                free(filePath);
                break;
            }
            print("FILE INVIATO\n", eBOLDGREEN);
            free(filePath);
            break;
        }
        case CREATEGROUP_OP:
        // Messaggio di richiesta di creazion di un gruppo
        {
            // Creazione del gruppo
            threadPrint(ID, "Creando il gruppo \"%s\" su richiesta di \"%s\" connesso su %d... ",
                        eNORMAL, INmex.data.hdr.receiver, INmex.hdr.sender, client);
            test = createGroup(INmex.data.hdr.receiver, INmex.hdr.sender);
            CHECK_FATAL_ERROR(test, "Creando il gruppo di utenti");
            if(test != 0)
            {// Creazione fallita
                print("CREAZIONE FALLITA\n", eBOLDRED);
                printError(ID, test, "Creando il gruppo \"%s\" su richiesta di \"%s\" conneso su %d: ",
                           INmex.data.hdr.receiver, INmex.hdr.sender, client);
                // Invio del messaggio di erore
                if(test == GROUP_ALREADY_REGISTERED)
                    test = sendError(client, OP_NICK_ALREADY);
                else
                    test = sendError(client, OP_FAIL);
                CHECK_FATAL_ERROR(test, "Inviando il messaggio di errore");
                if(test < 0)
                    printError(ID, test, "Inviando il messaggio di errore su %d: ", client);
                break;
            }
            print("CREAZIONE RIUSCITA\n", eBOLDGREEN);
            // Aggiunta al gruppo dell'utente che ha fatto richiesta della creazione
            threadPrint(ID, "Aggiungendo \"%s\" connesso su %d al gruppo \"%s\" appena creato... ",
                        eNORMAL, INmex.hdr.sender, client, INmex.data.hdr.receiver);
            test = joinUser(INmex.data.hdr.receiver, INmex.hdr.sender);
            CHECK_FATAL_ERROR(test, "Aggiungendo l'utente al gruppo appena creato");
            if(test != 0)
            {
                print("ASSOCIAZIONE FALLITA\n", eBOLDRED);
                printError(ID, test, "Aggiungendo \"%s\" connesso su %d al gruppo \"%s\" appena creato",
                           INmex.hdr.sender, client, INmex.data.hdr.receiver);
                // Invio del messaggio di erore
                test = sendError(client, OP_FAIL);
                CHECK_FATAL_ERROR(test, "Inviando il messaggio di errore");
                if(test < 0)
                    printError(ID, test, "Inviando il messaggio di errore su %d: ", client);
                break;
            }
            print("ASSOCIAZIONE RIUSCITA\n", eBOLDGREEN);
            // Invio del messaggio di OK
            test = sendOk(client, 0, NULL);
            CHECK_FATAL_ERROR(test, "Inviando il messaggio di avvenuta creazione del gruppo");
            if(test < 0)
                printError(ID, test, "Inviando il messaggio di avvenuta creazione del gruppo a \"%s\" connesso su %d: ",
                           INmex.hdr.sender, client);
            break;
        }
        case ADDGROUP_OP:
        // Messaggio di richiesta di adesione ad un gruppo
        {
            // Adesione al gruppo richiesto
            threadPrint(ID, "Adesione di \"%s\" connesso su %d al gruppo \"%s\"... ",
                        eNORMAL, INmex.hdr.sender, client, INmex.data.hdr.receiver);
            test = joinUser(INmex.data.hdr.receiver, INmex.hdr.sender);
            CHECK_FATAL_ERROR(test, "Aggiungendo l'utente al gruppo");
            if(test < 0)
            {
                print("ASSOCIAZIONE FALLITA\n", eBOLDRED);
                // Invio del messaggio di errore
                if(test == GROUP_NOT_REGISTERED || test == USER_NOT_REGISTERED)
                    test = sendError(client, OP_NICK_UNKNOWN);
                else
                    test = sendError(client, OP_FAIL);
                CHECK_FATAL_ERROR(test, "Inviando il messaggio di errore");
                if(test < 0)
                    printError(ID, test, "Inviando il messaggio di errore su %d: ", client);
                break;
            }
            print("ASSOCIAZIONE RIUSCITA\n", eBOLDGREEN);
            // Invio del messaggio di OK
            test = sendOk(client, 0, NULL);
            CHECK_FATAL_ERROR(test, "Inviando il messaggio di avvenuta associazione al gruppo");
            if(test < 0)
                printError(ID, test, "Inviando il messaggio di avvenuta associazione al gruppo di \"%s\" connesso su %d: ",
                           INmex.hdr.sender, client);
            break;
        }
        case DELGROUP_OP:
        // Messaggio di richiesta di dissociazione da un gruppo
        {
            // Dissociazione dal gruppo
            threadPrint(ID, "Dissociazione di \"%s\" connesso su %d dal gruppo \"%s\"... ",
                        eNORMAL, INmex.hdr.sender, client, INmex.data.hdr.receiver);
            test = disjoinUser(INmex.data.hdr.receiver, INmex.hdr.sender);
            CHECK_FATAL_ERROR(test, "Dissociando l'utente dal gruppo");
            if(test < 0)
            {
                print("DISSOCIAZIONE FALLITA\n", eBOLDRED);
                // Invio del messaggio di errore
                test = sendError(client, OP_NICK_UNKNOWN);
                CHECK_FATAL_ERROR(test, "Inviando il messaggio di errore");
                if(test < 0)
                    printError(ID, test, "Inviando il messaggio di errore su %d: ", client);
                break;
            }
            print("DISSOCIAZIONE RIUSCITA\n", eBOLDGREEN);
            // Invio del messaggio di OK
            test = sendOk(client, 0, NULL);
            CHECK_FATAL_ERROR(test, "Inviando il messaggio di avvenuta dissociazione");
            if(test < 0)
                printError(ID, test, "Inviando il messaggio di avvenuta dissociazione dal gruppo a \"%s\" connesso su %d: ",
                           INmex.hdr.sender, client);
            break;
        }
        case REMOVEGROUP_OP:
        // Messaggio di richiesta di cncellazione di un gruppo
        {
            // Cancellazione del gruppo
            threadPrint(ID, "Cancellazione del gruppo \"%s\" dal sistema...",
                        eNORMAL, INmex.data.hdr.receiver);
            test = deleteGroup(INmex.data.hdr.receiver, INmex.hdr.sender);
            CHECK_FATAL_ERROR(test, "Cancellando il gruppo");
            if(test < 0)
            {
                print("CANCELLAZIONE FALLITA\n", eBOLDRED);
                // Invio del messaggio di errore
                test = sendError(client, OP_NICK_UNKNOWN);
                CHECK_FATAL_ERROR(test, "Inviando il messaggio di errore");
                if(test < 0)
                    printError(ID, test, "Inviando il messaggio di errore su %d: ",
                               client);
                break;
            }
            print("CANCELLAZIONE RIUSCITA\n", eBOLDGREEN);
            // Invio del messaggio di OK
            test = sendOk(client, 0, NULL);
            CHECK_FATAL_ERROR(test, "Inviando il messaggio di avvenuta cancellazione");
            if(test < 0)
                printError(ID, test, "Inviando il messaggio di avvenuta cancellazzione gruppo a \"%s\" connesso su %d: ",
                           INmex.hdr.sender, client);
            break;
        }
        default:
            threadPrint(ID, "Ricevuto un messaggio non valido: %s\n", eRED, strerror(EOPNOTSUPP));
            break;
        }

        if(INmex.data.buf != NULL)
            free(INmex.data.buf);
        if(OUTmex.data.buf != NULL)
            free(OUTmex.data.buf);

        //Reinserimento del file descriptor nel master set di sistema
        test = INset(client);
        CHECK_FATAL_ERROR(test, "Reinserendo il file descriptor nel master set");

        printf("\n\n");
        /*------------------------------------------------------------------------*/
    }

    return NULL;

}
