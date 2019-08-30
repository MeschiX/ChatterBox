/*
 * membox Progetto del corso di LSO 2017/2018
 *
 * Dipartimento di Informatica Università di Pisa
 * Docenti: Prencipe, Torquati
 *
 */
/**
 * @file chatty.c
 * @brief File principale del server chatterbox
 */
#include "config.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <pthread.h>

#include "stats.h"
#include "errors.h"
#include "utils.h"
#include "users.h"
#include "income.h"
#include "fdSet.h"
#include "logIn.h"
#include "groups.h"
#include "listener.h"


/*--------------------------------VARIABILI GLOBALI-------------------------------*/
// Path utilizzato per la creazione del socket AF_UNIX
char UnixPath[UNIX_PATH_MAX];
// Directory dove memorizzare i files da inviare agli utenti
char DirName[UNIX_PATH_MAX];
// File nel quale verranno scritte le statistiche del server
char StatFileName[UNIX_PATH_MAX];
// Numero massimo di connessioni pendenti
int MaxConnections;
// Numero di thread nel pool
int ThreadsInPool;
// Dimensione massima di un messaggio testuale (numero di caratteri)
int MaxMsgSize;
// Dimensione massima di un file accettato dal server (kilobytes)
int MaxFileSize;
// Numero massimo di messaggi che il server 'ricorda' per ogni client
int MaxHistMsgs;
// Struttura statistiche di sistema
struct statistics  chattyStats = { PTHREAD_RWLOCK_INITIALIZER, 0,0,0,0,0,0,0 };
// Tabella degli utenti di sistema
usersTable_t usrTable;
// Tabella dei logIn di sistema
logsTable_t lgTable;
// Tabella dei gruppi di sistema
groupsTable_t grTable;
// Coda di servizio di sistema
incomeList_t service;
// Master set dei file descriptors
fdSet_t masterSet;
// Flag di output colorato
bool colorful = FALSE;
/**
 * @brief Flag di terminazione del server
 */
volatile bool serverStop = FALSE;
/**
 * @brief Flag di ricezione del segnale @c SIGUSR1
 */
volatile bool pendingSIGUSR1 = FALSE;
/*--------------------------------------------------------------------------------*/




/*------------------------------SIGNALS HANDLERS----------------------------------*/
static void handler(int signum)
{
    if(signum == SIGUSR1)
    {
        pendingSIGUSR1 = TRUE;
        // Re-impostazione dell'handler
        if(signal(SIGUSR1, &handler) == SIG_ERR)
            exit(EXIT_FAILURE);
    }
    else if(signum == SIGINT || signum == SIGTERM || signum == SIGQUIT)
        serverStop = TRUE;
}
/*--------------------------------------------------------------------------------*/




/*-----------------------------------USAGE----------------------------------------*/
static void usage(const char *progname)
{
    fprintf(stderr, "Il server va lanciato con il seguente comando:\n");
    fprintf(stderr, "  %s -f conffile\n", progname);
}
/*--------------------------------------------------------------------------------*/




/*-----------------------------------MAIN-----------------------------------------*/
int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    /*--------------------------------DICHIARAZIONI-------------------------------*/
    /*----FILE DI CONFIGURAZIONE-----*/
    // Stringa che conterrà il path assoluto o relativo del
    // file di configurazione
    char* configFile;

    /*----FILE DESCRIPTORS-----*/
    // Socket usato per la connessione
    int ConnectionSocket;
    // Variabile per il salvataggio del file descriptor del socket
    //per la comunicazione con i client
    int ClientSocket;
    // File descriptor del file delle statistiche
    int fdStat;

    /*----VARIABILI PER MULTIPLEXING-----*/
    // Set master aggiornato continuamente dai vari threads
    fdSet_t* masterSet;
    // Set usato come copia del master set passato alla funzione select()
    fd_set backUpSet;
    // Variabile usata per copiare il valore del campo maxFd del master set
    int saveMaxFd;

    /*----STRUTTURE GESTIONE SEGNALI-----*/
    struct sigaction act;
    struct sigaction actPIPE;

    /*-----VARIABILE TEST------*/
    // Variabile usata per testare il valore di ritorno delle funzioni
    int test = 0;

    /*----STRUTTURA ADDRESS-----*/
    // Struttura per memorizzare l'indirizzo e la famiglia del socket di connessione
    struct sockaddr_un SocketAddress;

    /*----DICHIARAZIONE DEL POOL DI THREADS-----*/
    // Pool di threads workers
    pthread_t* threadsPool;

    /*----ARGOMENTI DEI THREADS-----*/
    int* wargs;

    /*----INTERVALLO SELECET-----*/
    struct timeval waiting;
    /*-----------------------------FINE DICHIARAZIONI------------------------------*/


     /*-----PARSING LINEA DI COMANDO-----*/
    // Parsing degli argomenti della linea di comando
    while ((test = getopt(argc, argv, "cf:")) != -1)
    {
        switch (test){
        case 'f':
            configFile = optarg;
            break;
        case 'c':
            colorful = TRUE;
            break;
        default:
            usage(argv[0]);
            exit(EXIT_FAILURE);
            break;
        }
    }

    /*-------------------------INIZIALIZZAZIONE DEL SERVER-------------------------*/
    print("INIZIALIZZAZIONE DEL SERVER\n\n", eBOLDWHITE);
    print("Modalità colorata:\t\t\t", eBOLDMAGENTA);
    if(colorful)
        print("attiva\n", eBOLDGREEN);
    else
        print("disattiva\n", eNORMAL);
    print("Il file di configurazione è:\t\t", eBOLDMAGENTA);
    print("\"%s\"\n", eBOLDGREEN, configFile);

    /*-----IMPOSTAZIONE VALORI SETTING-----*/
    print("Analisi del file di configurazione...\t", eBOLDMAGENTA);
    test = setUpConfiguration(configFile);
    CHECK_FATAL_ERROR(test, "Analizzando il file di configurazione");
    print("OK\n", eBOLDGREEN);

    /*-----STAMPA DEI VALORI DI SETTING-----*/
    print("I valori di setting sono:\n", eBOLDMAGENTA);
    printSettingValues();

    /*-----INIZIALIZZAZIONE CODA DI SERVIZIO-----*/
    print("Inizializzazione coda di servizio... ", eBOLDMAGENTA);
    test = initializeIncomeList();
    CHECK_FATAL_ERROR(test, "Inizializzando la coda di servizio");
    print("OK\n\n", eBOLDGREEN);

    /*-----INIZIALIZZAZIONE TABELLA UTENTI-----*/
    print("Inizializzazione della tabella utenti... ", eBOLDMAGENTA);
    test = initializeUsersTable();
    CHECK_FATAL_ERROR(test, "Inizializzando la tabella utenti di sistema");
    print("OK\n\n", eBOLDGREEN);

    /*-----INIZIALIZZAZIONE TABELLA LOGIN-----*/
    print("Inizializzazione della tabella logIn... ", eBOLDMAGENTA);
    test = initializeLogsTable();
    CHECK_FATAL_ERROR(test, "Inizializzando la tabella dei logIn");
    print("OK\n\n", eBOLDGREEN);

    /*-----INIZIALIZZAZIONE TABELLA GRUPPI-----*/
    print("Inizializzazione tabella dei gruppi... ", eBOLDMAGENTA);
    test = initializeGroupsTable();
    CHECK_FATAL_ERROR(test, "Inizializzando la tebella di gruppi");
    print("OK\n\n", eBOLDGREEN);

    /*-----SETUP DIRECTORY /tmp/-----*/
    // Rimozione di cartelle e file residui, se presenti, di vecchie esecuzioni
    print("Rimozione di eventuali vecchi files... ", eBOLDMAGENTA);
    test = unlink(StatFileName);
    if(test == -1 && errno != ENOENT){CHECK_FATAL_ERROR(test, "Eliminando il vecchio file delle statistiche")}
    test  = unlink(UnixPath);
    if(test == -1 && errno != ENOENT){CHECK_FATAL_ERROR(test, "Eliminando il vecchio socket di connessione")}
    test = removeDir(DirName);
    if(test == -1 && errno != ENOENT){CHECK_FATAL_ERROR(test, "Eliminando la vecchia directory dei file scambiati")}
    print("OK\n\n", eBOLDGREEN);
    // Creazione del file per le statistiche
    print("Creazione del nuovo file delle statistiche... ", eBOLDMAGENTA);
    fdStat = creat(StatFileName, 0777);
    CHECK_FATAL_ERROR(fdStat, "Creando il file per le statistiche");
    close(fdStat);
    print("OK\n\n", eBOLDGREEN);
    // Creazione della cartella per il salvataggio dei file scambiati: 'DirName'
    print("Creazione della cartella per lo scambio dei files... ", eBOLDMAGENTA);
    test = mkdir(DirName, 0777);
    CHECK_FATAL_ERROR(test, "Creando la cartella per loscambio dei files");
    print("OK\n\n", eBOLDGREEN);

    /*-----INIZIALIZZAZIONE INTERVALLO SELECT-----*/
    waiting.tv_sec = 0;
    waiting.tv_usec = 100;

    /*-----GESTIONE SEGNALI-----*/
    print("Impostando la gestione dei segnali... ", eBOLDMAGENTA);
    memset(&act, 0, sizeof(struct sigaction));
    memset(&actPIPE, 0, sizeof(struct sigaction));
    act.sa_handler = handler;
    actPIPE.sa_handler = SIG_IGN;
    act.sa_flags = 0;
    actPIPE.sa_flags = 0;
    test = sigaction(SIGINT, &act, NULL);
    CHECK_FATAL_ERROR(test, "Impostando l'handler per SIGINT");
    test = sigaction(SIGTERM, &act, NULL);
    CHECK_FATAL_ERROR(test, "Impostando l'handler per SIGTERM");
    test = sigaction(SIGQUIT, &act, NULL);
    CHECK_FATAL_ERROR(test, "Impostando l'handler per SIGQUIT");
    test = sigaction(SIGUSR1, &act, NULL);
    CHECK_FATAL_ERROR(test, "Impostando l'handler per SIGUSR1");
    test = sigaction(SIGPIPE, &actPIPE, NULL);
    CHECK_FATAL_ERROR(test, "Impostando l'handler per SIGPIPE");
    print("OK\n\n", eBOLDGREEN);

    /*-----INIZIALIZZAZIONE MULTIPLEXING-----*/
    // Creazione e inizializzazione del master set
    print("Inizializzazione del master set dei file descriptors... ", eBOLDMAGENTA);
    test = initializeSet();
    CHECK_FATAL_ERROR(test, "Inizializzando il master set dei file descriptors");
    print("OK\n\n", eBOLDGREEN);

    /*-----INIZIALIZZAZIONE PARAMETRI THREADS-----*/
    print("Inizializzazione dei parametri dei threads... ", eBOLDMAGENTA);
    wargs = (int*)malloc(ThreadsInPool * sizeof(int));
    CHECK_NULL_FATAL_ERROR(wargs, "Allocando spazio per i parametri dei threads");
    for (int i = 0; i < ThreadsInPool; i++)
    {
        wargs[i] = i+1;
    }
    print("OK\n\n", eBOLDGREEN);

    /*-----INIZIALIZZAZIONE THREADS POOL-----*/
    print("Inizializzazione del threads pool... ", eBOLDMAGENTA);
    threadsPool = (pthread_t*)malloc((ThreadsInPool+1)*sizeof(pthread_t));
    CHECK_NULL_FATAL_ERROR(threadsPool, "Allocando spazio per il threads pool");
    print("OK\n\n", eBOLDGREEN);

    /*-----CREAZIONE THREADS-----*/
    print("Creazione dei threads...\n", eBOLDMAGENTA);
    // Creazione worker treads
    for(int i=0; i<ThreadsInPool; i++)
    {
        // Effettiva creazione del thread
        test = pthread_create(&(threadsPool[i]), NULL, listener, (void*)&(wargs[i]));
        CHECK_FATAL_ERROR(test, "Creando i threads");
        print("\tThread %d creato\n", eGREEN, wargs[i]);
    }
    print("\tOK\n\n", eBOLDGREEN);

    /*-----INIZIALIZZAZIONE STURTTURA SOCKET ADDRESS-----*/
    print("Inizializzazione della struttura socket address... ", eBOLDMAGENTA);
    // Inizializzazione della struttura per l'inidrizzo e la famiglia della connessione
    memset(&SocketAddress, 0, sizeof(struct sockaddr_un));
    SocketAddress.sun_family = AF_UNIX;
    strncpy(SocketAddress.sun_path, UnixPath, sizeof(SocketAddress.sun_path)-1);
    print("OK\n\n", eBOLDGREEN);

    /*-----CREAZIONE DEL SOCKET PER LA CONNESSIONE-----*/
    // Creazione del socket
    print("Creazione del socket di connessione... ", eBOLDMAGENTA);
    ConnectionSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    CHECK_FATAL_ERROR(test, "Creando il socket di connessione");
    // Associazione del socket all'indirizzo contenuto nella relativa struttura
    test = bind(ConnectionSocket, (struct sockaddr *)&SocketAddress, sizeof(struct sockaddr_un));
    CHECK_FATAL_ERROR(test, "Collegando il socket con lastruttura socket address");
    print("\tOK\n\n", eBOLDGREEN);

    /*-----IMPOSTAZIONE DEL MASSIOMO DI CONNESSIONI PENDENTI-----*/
    print("Impostazione del numero massimo di connessioni pendenti sul socket di connessione... ", eBOLDMAGENTA);
    test = listen(ConnectionSocket, SOMAXCONN);
    CHECK_FATAL_ERROR(test, "Impostando il numero massimo di connessioni pendenti");
    print("OK\n\n", eBOLDGREEN);

    /*-----REGISTRAZIONE DEL SOCKET DI CONNESSIONE-----*/
    print("Registrazione del socket di connessione al master set... ", eBOLDMAGENTA);
    test = INset(ConnectionSocket);
    CHECK_FATAL_ERROR(test, "Registrando il socket di connessione al master set");
    print("OK\n\n", eBOLDGREEN);
    /*-------------------------INIZIALIZZAZIONE TERMINATA-------------------------*/
    print("INIZIALIZZAZIONE COMPLETATA\n\n", eBOLDGREEN);


    /*------------------------CICLO DI ASCOLTO DEL SERVER-------------------------*/
    print("INIZIO ROUTINE\n\n", eBOLDWHITE);
    while(!serverStop)
    {
        // Controllo del flag di ricezione del segnale SIGUSR1
        if (pendingSIGUSR1)
        {
            FILE* fout;

            // C'è un segnale pendente
            print("RICEVUTO SEGNALE SIGUSR1\n", eBOLDYELLOW);
            print("Generazione delle statistiche... ", eBOLDMAGENTA);

            fout = fopen(StatFileName, "a");
            CHECK_NULL_FATAL_ERROR(fout, "Aprendo il file delle statistiche");

            // Stampa delle statistiche
            test = printStats(fout);
            CHECK_FATAL_ERROR(test, "Scrivendo le statistiche nel file");
            print("OK\n\n", eBOLDGREEN);

            test = fclose(fout);
            CHECK_FATAL_ERROR(test, "Chiudendo il file delle statistiche");

            // Reset del flag
            pendingSIGUSR1 = FALSE;
        }

        test = saveSet(&backUpSet, &saveMaxFd);
        CHECK_FATAL_ERROR(test, "Effettuando la copia del master set");

        // Attesa di un file descriptor pronto per la lettura
        int test = select(saveMaxFd+1, &backUpSet, NULL, NULL, &waiting);
        if((test == -1 && errno == EINTR) || test == 0)
            continue;
        if (test == -1 && errno != EINTR)
        {
            CHECK_FATAL_ERROR(test, "Durante la system call select");
        }

        // Si esaminano tutti i file descriptors registrati per rilevare nuove connessioni
        // o nuovi messaggi in entrata
        for(int sock=3; sock <= saveMaxFd; sock++)
        {
            if (FD_ISSET(sock, &backUpSet))
            {
                if(sock == ConnectionSocket)
                {// Nova connessione
                    print("Nuova connessione... ", eBOLDMAGENTA, ClientSocket);
                    // Accettazione nuova connessione
                    ClientSocket = accept(ConnectionSocket, NULL, 0);
                    CHECK_FATAL_ERROR(ClientSocket, "Accettando la connessione da parte del client");
                    print("ACCETTATA ", eBOLDGREEN);
                    print("→ ", eNORMAL);
                    print("%d\n", eBOLDBLUE, ClientSocket);

                    // Inserimento nel master set del nuovo file descriptor
                    test = INset(ClientSocket);
                    CHECK_FATAL_ERROR(test, "Inserendo il file descriptor appena connesso nel master set");
                }
                else
                {// Richiesta pendente
                    // Rimozione del file descriptor dall'insieme di monitoraggio di select
                    test = OUTset(sock);
                    CHECK_FATAL_ERROR(test, "Rimuovendo il file descriptor dal master set");
                    // Inserimento del file descriptor nella catena richieste
                    test = pushIncome(sock);
                    CHECK_FATAL_ERROR(test, "Inserendo il file descriptor nella coda di servizio");
                }
             }
        }
    }
    print("RICEVUTO SEGNALE SIGINT/SIGTERM/SIGQUIT\n", eBOLDYELLOW);
    /*---------------------FINE CICLO DI ASCOLTO DEL SERVER----------------------*/
    print("ROUTINE TERMINATA\n\n", eBOLDGREEN);

    /*------------------------------CHIUSURA SERVER------------------------------*/
    print("CHIUSURA DEL SERVER\n\n", eBOLDWHITE);

    // Inserimento del messaggio di terminazione nella catena richieste
    print("Terminando la coda di servizio... ", eBOLDMAGENTA);
    test = breakIncomeList();
    CHECK_FATAL_ERROR(test, "Terminando la coda di servizio");
    print("OK\n\n", eBOLDGREEN);

    // Attesa della terminazione di tutti i threads
    print("Attesta della terminazione dei threads...\n", eBOLDMAGENTA);
    for(int i=0; i<ThreadsInPool; i++)
    {
        test = pthread_join(threadsPool[i], NULL);
        CHECK_FATAL_ERROR(test, "Attendendo la terminazione del thread");
        print("\tThread %d terminato\n", eGREEN, i+1);
    }
    free(threadsPool);
    free(wargs);
    print("\tOK\n\n", eBOLDGREEN);

    // Cancellazione della catena di richieste
    print("Cancellazione della coda di servizio... ", eBOLDMAGENTA);
    test = destroyIncomeList();
    print("OK\n\n", eBOLDGREEN);

    // Cancellazione del master set
    print("Cancellazione del master set... ", eBOLDMAGENTA);
    test = destroySet(&masterSet);
    CHECK_FATAL_ERROR(test, "Eliminando il master set");
    print("OK\n\n", eBOLDGREEN);

    // Cancellazione della struttura statistiche
    print("Cancellazione delle statistche... ", eBOLDMAGENTA);
    test = pthread_rwlock_destroy(&(chattyStats.statsLock));
    if(test != 0)
    {
        errno = test;
        CHECK_FATAL_ERROR(-1, "Distruggendo la lock relativa alla struttura statistche di sistema");
    }
    print("OK\n\n", eBOLDGREEN);

    // Cancellazione della tabella utenti
    print("Cancellazione della tabella utenti... ", eBOLDMAGENTA);
    test = destroyUsersTable();
    CHECK_FATAL_ERROR(test, "Eliminando la tabella degli utenti");
    print("OK\n\n", eBOLDGREEN);


    // Cancellazione della tabella logIns
    print("Cancellazione della tabella dei logIn... ", eBOLDMAGENTA);
    test = destroyLogsTable();
    CHECK_FATAL_ERROR(test, "Eliminando la tabella dei logIn");
    print("OK\n\n", eBOLDGREEN);

    // Cancellazione della tabella dei gruppi
    print("Cancellazione della tabella dei gruppi... ", eBOLDMAGENTA);
    test = destroyGroupsTable();
    CHECK_FATAL_ERROR(test, "Eliminando la tabella dei gruppi");
    print("OK\n\n", eBOLDGREEN);


    // Cancellazione dei file e le directory create a runtime
    print("Cancellazione dei files e cartelle create a runtime... ", eBOLDMAGENTA);
    test = unlink(StatFileName);
    if(test == -1 && errno != ENOENT){CHECK_FATAL_ERROR(test, "Eliminando il file delle statistiche");}
    test  = unlink(UnixPath);
    if(test == -1 && errno != ENOENT){CHECK_FATAL_ERROR(test, "Eliminando il socket di connessione");}
    test = removeDir(DirName);
    if(test == -1 && errno != ENOENT){CHECK_FATAL_ERROR(test, "Eliminando la cartella per lo scambio dei files");}
    print("OK\n\n", eBOLDGREEN);

    print("SERVER CHIUSO\n\n", eBOLDGREEN);
    /*----------------------------FINE CHIUSURA SERVER---------------------------*/

    return 0;
}
/*--------------------------------------------------------------------------------*/
