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

#ifndef __ERRORS_H__
#define __ERRORS_H__

/**
 * @file errors.h
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Contiene le definizioni dei codici di erorre usati all'interno
 * del programma, le definizioni dei messaggi di errore relativi ai codici
 * e le dichiarazioni di alcune macro e funzioni di controllo del valore di ritorno
 * delle funzioni e stampa formattata del messaggio di errore.
 */



/*----------------------------CODICI DI ERRORE------------------------------*/
/**
 * @brief Codice di errore restituito nel caso in cui venga richiesta la registrazione
 * di un utente con un nome già presente nel sistema
 */
#define USER_ALREADY_REGISTERED    -2

/**
 * @brief Codice di errore resttituito nel caso in cui arrivi un a richiesta
 * di registrazione di utente che riporta uno username troppo lungo (@ref MAX_NAME_LENGTH)
 */
#define USERNAME_TOO_LONG    -3

/**
 * @brief Codice di errore restituito nel caso in cui arrivi una richiesta di registrazione
 * e la tabella degli utenti (@ref usersTable_t) risulta essere piena (@ref USERS_TABLE_SIZE)
 */
#define USERS_QUOTA_EXCEEDED    -4

/**
 * @brief Codice di errore restituito nel caso in cui si sia raggiunto il numero massimo di
 * logins contemporanei (@ref MaxConnections)
 */
#define LOGIN_QUOTA_EXCEEDED    -5

/**
 * @brief Codice di errore restituito nel caso in cui arrivi una richiesta di invio di un messaggio
 * e il destinatario non risulta essere registrato al sistema
 */
#define RECEIVER_UNKNOWN    -6

/**
 * @brief Codice di errore restituito nel caso in cui arrivi un richiesta di login
 * ad un utente che ha raggiunto il massimo numero di login simultanei (@ref MAX_USER_SIMULTANEUS_CONNECTIONS)
 */
#define USER_LOGIN_QUOTA_EXCEEDED -7

/**
 * @brief Codice di errore restituito nel caso in cui si sia raggiunto il numero massimo
 * di che possono risiedere nel sistema
 */
#define GROUPS_QUOTA_EXCEEDED -8

/**
 * @brief Codice fdi errore restituito nel caso in cui arrivi una richiesta di associazione
 * ad un gruppo da parte di un utente che ha gia raggiunto il numero massimo di
 * associazioni simultanee (@ref MAX_USER_SIMULTANEUS_GROUPS)
 */
#define USER_GROUPS_QUOTA_EXCEEDED -9

/**
 * @brief Codice di errore restituito nel caso in cui arrivi una richiesta di adesione
 * ad un gruppo da parte di un utente che già ne fa parte.
 */
#define USER_ALREADY_JOINT -10

/**
 * @brief Codice di errore restituito nel caso in cui arrivi una richiesta di invio
 * di un messaggio ad un gruppo da parte di un utente che non ne fa parte o di cancellazione
 * da un gruppo da parte di un utente che non ne fa parte
 */
#define USER_NOT_JOINT -11

/**
 * @brief Codice di errore restituito nel caso in cui arrivi una richiesta di
 * creazione di un gruppo che è già presente nel sistema
 */
#define GROUP_ALREADY_REGISTERED -12

/**
 * @brief Codice di errore restituito nel caso in cui arrivi da un utente,
 * una richiesta di adesione ad un gruppo che non è registrato nel sistema
 */
#define GROUP_NOT_REGISTERED -13

/**
 * @brief Codice di errore restituito nel caso in cui arrivi una richiesta di
 * deregistrazione da parte di un utente che non risulta registrato nel
 * sistema.
 */
#define USER_NOT_REGISTERED -14

/**
 * @brief Codice di errore restituito nel caso in cui arrivi una richiesta di
 * registrazione con uno username uguale al nome di un gruppo che é già registrato nel sistema
 */
#define IS_GROUPNAME -15

/**
 * @brief Codice di errore restituito nel caso in cui arrivi una richiesta di creazione
 * di un gruppo con un nome che corrisponde allo username di un utente già registrato
 * nel sistema
 */
#define IS_USERNAME -16

/**
 * @brief Codice di errore restituito nel caso in cui ci sia un tentativo di scrittura da parte
 * del server su un socket chiuso dal lato client
 */
#define CLIENT_CLOSED_CONNECTION -17

/**
 * @brief Codice di errore restituito nel caso in cui arrivi una richiesta di invio di un messaggio
 * testuale la cui lunghezza è superiore a @ref MaxMsgSize
 */
#define TEXT_MESSAGE_TOO_LONG -18

/**
 * @brief Codice di errore restituito nel caso in cui arrivi una richiesta di invio di un messaggio
 * di tipo file la cui dimensione è superiore a @ref MaxFileSize
 */
#define FILE_MESSAGE_TOO_BIG -19

/**
 * @brief Messaggio di errore restituito nel caso in cui si cerci di effettuare un'operazione di
 * logout su un indirizzo che non risulta nella tabella dei logIns
 */
#define CLIENT_NOT_LOGGED -20

/**
 * @brief Messaggio di errore restituito nel caso in cui arrivi una richiesta di cancellazione di
 * un gruppo da parte di un utente che non è il fondatore
 */
#define USER_NOT_FOUNDER -21
/*--------------------------------------------------------------------------*/




/*--------------------------MESSAGGI DI ERRORE------------------------------*/
/**
 * @brief Messaggio di errore relativo a @ref USER_ALREADY_REGISTERED
 */
#define STR_USER_ALREADY_REGISTERED "Lo stesso username è già registrato nel sistema\n"

/**
 * @brief Messaggio di errore relativo a @ref USERNAME_TOO_LONG
 */
#define STR_USERNAME_TOO_LONG "L'username è troppo lungo\n"

/**
 * @brief Messaggio di errore relativo a @ref USERS_QUOTA_EXCEEDED
 */
#define STR_USERS_QUOTA_EXCEEDED "La soglia massima di utenti registrati è stata superata\n"

/**
 * @brief Messaggio di errore relativo a @ref LOGIN_QUOTA_EXCEEDED
 */
#define STR_LOGIN_QUOTA_EXCEEDED "La soglia massima di login effettuati è stata superata\n"

/**
 * @brief Messaggio di errore relativo a @ref RECEIVER_UNKNOWN
 */
#define STR_RECEIVER_UNKNOWN "Destinatario sconosciuto\n"

/**
 * @brief Messaggio di errore relativo a @ref USER_LOGIN_QUOTA_EXCEEDED
 */
#define STR_USER_LOGIN_QUOTA_EXCEEDED "La soglia massima di login effettuati per il singolo utente è stata raggiunta\n"

/**
 * @brief Messaggio di errore relativo a @ref GROUPS_QUOTA_EXCEEDED
 */
#define STR_GROUPS_QUOTA_EXCEEDED "La soglia massima di gruppi registrati nel sitema è stata raggiunta\n"

/**
 * @brief Messaggio di errore relativo a @ref USER_GROUPS_QUOTA_EXCEEDED
 */
#define STR_USER_GROUPS_QUOTA_EXCEEDED  "La soglia massima di associazioni a gruppi effettuate per il singolo utente è stata raggiunta\n"

/**
 * @brief Messaggio di errore relativo a @ref USER_ALREADY_JOINT
 */
#define STR_USER_ALREADY_JOINT "L'utente fa già parte del gruppo\n"

/**
 * @brief Messaggio di errre relativo a @ref USER_NOT_JOINT
 */
#define STR_USER_NOT_JOINT "L'utente non fa parte del gruppo\n"

/**
 * @brief Messaggio di errore relativo a @ref GROUP_ALREADY_REGISTERED
 */
#define STR_GROUP_ALREADY_REGISTERED "Nel sistema è già presente un gruppo con questo nome\n"

/**
 * @brief Messaggio di errore relativo a @ref GROUP_NOT_REGISTERED
 */
#define STR_GROUP_NOT_REGISTERED "Il gruppo non è registrato nel sistema\n"

/**
 * @brief Messaggio di errore relativo a @ref USER_NOT_REGISTERED
 */
#define STR_USER_NOT_REGISTERED "L'utente non è registrato nel sistema\n"

/**
 * @brief Messaggio di errore relativo a @ref IS_GROUPNAME
 */
#define STR_IS_GROUPNAME "Questo nome è già stato assegnato ad un gruppo\n"

/**
 * @brief Messaggio di errore relativo a @ref IS_USERNAME
 */
#define STR_IS_USERNAME "Questo nome è già stato assegnato ad un utente\n"

/**
 * @brief Messaggio di errore relativo a @ref CLIENT_CLOSED_CONNECTION
 */
#define STR_CLIENT_CLOSED_CONNECTION "Il client ha chiuso la connessione\n"

/**
 * @brief Messaggio di errore relativo a @ref TEXT_MESSAGE_TOO_LONG
 */
#define STR_TEXT_MESSAGE_TOO_LONG "Il messaggio testuale ha una lunghezza superiorre a quella consentita"

/**
 * @brief Messaggio di errore relativo a @ref FILE_MESSAGE_TOO_BIG
 */
#define STR_FILE_MESSAGE_TOO_BIG "Il file ricevuto ha una dimensione superiore a quella consentita"

/**
 * @brief MEssaggio di errore relativo a @ref CLIENT_NOT_LOGGED
 */
#define STR_CLIENT_NOT_LOGGED "Il client che si è disconnesso non è loggato nel sistema"

/**
 * @brief Messaggio di errore relativo a @ref USER_NOT_FOUNDER
 */
#define STR_USER_NOT_FOUNDER "L'utente che ha chiesto la cancellazione del gruppo non ne è il fondatore"
/*--------------------------------------------------------------------------*/




/*----------------DEFINIZIONE TEST DI RITORNO FUNZIONE----------------------*/
/**
 * @brief Macro che controlla che il valore di ritorno di una funzione
 * sia diverso da @c -1 altrimenti causa l'uscita dal programma con codice @c EXIT_FAILURE
 * stampando un messaggio di errore
 * @warning L'esito negativo del controllo causa l'uscita dal programma,
 * da usare quindi solo nel caso in cui l'errore controllato si debba
 * considerare fatale
 */
#define CHECK_FATAL_ERROR(test, errText)                                                           \
    if((test) == -1)                                                                               \
    {                                                                                              \
        print("\nFile: %s; Line %d; %s: %s\n", eRED, __FILE__, __LINE__, errText, strerror(errno));\
        exit(EXIT_FAILURE);                                                                        \
    }


/**
 * @brief Macro che controlla che il valore di ritorno di una funzione
 * sia diverso da @c NULL altrimenti causa l'uscita dal programma con codice @c EXIT_FAILURE
 * stampando un messaggio di errore
 * @warning L'esito negativo del controllo causa l'uscita dal programma,
 * da usare quindi solo nel caso in cui l'errore controllato si debba
 * considerare fatale
 */
#define CHECK_NULL_FATAL_ERROR(test, errText)                                                      \
    if((test) == NULL)                                                                             \
    {                                                                                              \
        print("\nFile: %s; Line %d; %s: %s\n", eRED, __FILE__, __LINE__, errText, strerror(errno));\
        exit(EXIT_FAILURE);                                                                        \
    }
/*--------------------------------------------------------------------------*/




/*----------------------------STAMPA ERRORI---------------------------------*/
/**
 * @brief Funzione di servizio che stampa i messaggi di errore con una
 * formattazione predefinita
 * @details Come la funzione @ref print questa funzione permette la stampa
 * formattata su terminale.
 * A differenza di @ref print questa funzione è dedicata solo alla stampa di
 * messaggi di errore, quindi ha uno stile predefinito e non richiede la scelta del
 * colore (@ref eRED).
 * @param ID Intero identificatore che esegue la stampa
 * @param errCode Codice dell'errore di cui si vuole stampare il messaggio
 * @param errText Formato del testo da anteporre al messaggio di errore
 * (come nella printf()).
 * @retval N: Il numero di bytes stampati in caso di successo,
 * @retval -1: Se si è verificato un errore
 *
 * | @c errno | Motivo                                  |
 * | :------: | :-------------------------------------: |
 * |@c EINVAL | Almeno uno degli argomenti non è valido |
 */
int printError(int ID, int errCode, char* errText, ...);
/*--------------------------------------------------------------------------*/

#endif
