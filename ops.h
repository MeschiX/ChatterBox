/*
 * chatterbox Progetto del corso di LSO 2017/2018
 *
 * Dipartimento di Informatica Università di Pisa
 * Docenti: Prencipe, Torquati
 *
 */

#if !defined(OPS_H_)
#define OPS_H_

/**
 * @file  ops.h
 * @brief Contiene i codici delle operazioni di richiesta e di risposta
 */


/**
 * @typedef op_t
 * @brief Definizione del tipo operazione
 */
typedef enum {

    /* ------------------------------------------ */
    /*      operazioni che il server deve gestire */
    /* ------------------------------------------ */

    REGISTER_OP      = 0,   /**< Richiesta di registrazione di un ninckname */
    CONNECT_OP       = 1,   /**< Richiesta di connessione di un client */
    POSTTXT_OP       = 2,   /**< Richiesta di invio di un messaggio testuale ad un nickname o groupname */
    POSTTXTALL_OP    = 3,   /**< Richiesta di invio di un messaggio testuale a tutti gli utenti */
    POSTFILE_OP      = 4,   /**< Richiesta di invio di un file ad un nickname o groupname */
    GETFILE_OP       = 5,   /**< Richiesta di recupero di un file */
    GETPREVMSGS_OP   = 6,   /**< Richiesta di recupero della history dei messaggi */
    USRLIST_OP       = 7,   /**< Richiesta di avere la lista di tutti gli utenti attualmente connessi */
    UNREGISTER_OP    = 8,   /**< Richiesta di deregistrazione di un nickname o groupname */
    DISCONNECT_OP    = 9,   /**< Richiesta di disconnessione */
    CREATEGROUP_OP   = 10,  /**< Richiesta di creazione di un gruppo */
    ADDGROUP_OP      = 11,  /**< Richiesta di aggiunta ad un gruppo */
    DELGROUP_OP      = 12,  /**< Richiesta di rimozione da un gruppo */
    REMOVEGROUP_OP   = 13,  /**< Richiesta di cancellazione di un gruppo */


    /* ------------------------------------------ */
    /*    messaggi inviati dal server             */
    /* ------------------------------------------ */

    OP_OK           = 20,  /**< Operazione eseguita con successo */
    TXT_MESSAGE     = 21,  /**< Notifica di messaggio testuale */
    FILE_MESSAGE    = 22,  /**< Notifica di messaggio "file disponibile" */

    OP_FAIL         = 25,  /**< Generico messaggio di fallimento */
    OP_NICK_ALREADY = 26,  /**< Nickname o groupname gia' registrato */
    OP_NICK_UNKNOWN = 27,  /**< Nickname non riconosciuto */
    OP_MSG_TOOLONG  = 28,  /**< Messaggio con size troppo lunga */
    OP_NO_SUCH_FILE = 29,  /**< Il file richiesto non esiste */


    OP_END          = 100  /**< limite superiore agli id usati per le operazioni */

} op_t;


#endif /* OPS_H_ */
