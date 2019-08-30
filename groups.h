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

#ifndef __GROUPS_H__
#define __GROUPS_H__

#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "utils.h"

/**
 * @file groups.h
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Contiene la definizione della struttura @ref groupsTable e di tutti i tipi
 * dei suoi componenti, le dichiarazioni delle funzioni di creazione, gestione
 * e distruzione dei gruppi e della tabella dei gruppi.
 *
 * @details Le strutture definite in questo file sono:\n
 * - La struttura @ref member definisce un membro di un gruppo
 * - La struttura @ref group contiene la lista di tutti i suoi membri (@ref member)
 * - La struttura @ref groupNode incapsula la struttura @ref group in modo da renderla concatenabile
 * - La struttura @ref groupsList concatena i gruppi che hanno il solito indirizzo hash
 * - La struttura @ref groupsTable è una tabella hash che usa le strutture @ref groupsList come
 * liste di trabocco
 *
 * Per gestire la concorrenza sulle operazioni di tabella si utilizza un set di
 * @c pthread_rwlock_t.
 * Ognuna di queste consente l'accesso in mutua esclusione ad un numero fissato
 * (proporzionale alla dimensione dela tabella @ref USERS_MUTEX_BLOCK_SIZE) di
 * righe adiacenti della tabella.
 */


/*---------------------DEFINIZIONE MEMBER---------------------------------*/
/**
 * @struct member
 * @brief Struttura che definisce un nodo della lista dei membri del gruppo
 */
struct member
{
    char* nickName; /**< Nome dell'utente che è componente del gruppo */
    struct member* next; /**< Puntatore all'elemento successivo nella lista */
};

/**
 * @typedef member_t
 * @brief Definizione del tipo componente di un gruppo
 */
typedef struct member member_t;
/*-----------------------------------------------------------------------*/





/*----------------------DEFINIZIONE GROUP--------------------------------*/
/**
 * @struct group
 * @brief Struttura che definisce un gruppo
 */
struct group
{
    char* founder; /**< Username del fondatore del gruppo */
    char* groupName; /**< Nome del gruppo */
    member_t* members; /**< Puntatore al primo elemento della lista dei membri del gruppo */
};

/**
 * @typedef group_t
 * @brief Definizione del tipo gruppo
 */
typedef struct group group_t;
/*-----------------------------------------------------------------------*/



/*--------------------DEFINIZIONE GROUP NODE-----------------------------*/
/**
 * @struct groupNode
 * @brief Struttura che definisce un nodo di lista di gruppi
 */
struct groupNode
{
    group_t gr; /**< Puntatore al gruppo contenuto nel nodo*/
    struct groupNode* next; /**< Puntatore al prossimo elemento nella lista */
    struct groupNode* prev; /**< Puntatore all'elemento precedente nella lista */
};

/**
 * @typedef groupNode_t
 * @brief Definizione del tipo nodo di lista di gruppi
 */
typedef struct groupNode groupNode_t;
/*-----------------------------------------------------------------------*/



/*----------------------DEFINIZIONE GROUP LIST---------------------------*/
/**
 * @struct groupsList
 * @brief Struttura che definisce una lista di gruppi
 */
struct groupsList
{
    groupNode_t* head; /**< Puntatore al primo elemento della lista dei gruppi */
    unsigned int loadFactor; /**< Numero di elementi nella lista */
};

/**
 * @typedef groupsList_t
 * @brief Definizione del tipo lista di gruppi
 */
typedef struct groupsList groupsList_t;
/*-----------------------------------------------------------------------*/




/*-------------------DEFINIZIONE GROUP TABLE-----------------------------*/
/**
 * @struct groupsTable
 * @brief Struttura che definisce la tabella dei gruppi
 */
struct groupsTable
{
    pthread_rwlock_t* bloksLocks; /**< Set di locks ognuna relativa ad un blocco della tabella */
    groupsList_t* table; /**< Tabella di gruppi */
};

/**
 * @typedef groupsTable_t
 * @brief Definizione del tipo tabella di gruppi
 */
typedef struct groupsTable groupsTable_t;
/*-----------------------------------------------------------------------*/




/*---------------------TABELLA DEI GRUPPI GLOBALE------------------------*/
/**
 * @brief Variabile globale che si riferisce alla tabella dei gruppi di sistema
 */
extern groupsTable_t grTable;
/*-----------------------------------------------------------------------*/





/*-----------------DICHIARAZIONE FUNZIONI GROUP--------------------------*/
/**
 * @brief Inizializza una struttura @ref group
 * @param gr Puntatore alla struttura @ref group da inizializzare
 * @param fnd Username dell'utente che crea il gruppo
 * @param grName Nome del gruppo che verrà assegnato alla struttura
 * puntata da @p gr durante l'inizializzazione
 * @retval 0: Se l'inizializzazione è avvenuta con successo
 * @retval -1: Se si è verificato un altro errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                  |
 * | :----------: | :-------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido |
 */
int initializeGroup(group_t* gr, char* grName, char* fnd);


/**
 * @brief Inserisce il nome di un nuovo membro all'interno della lista dei membri
 * @ref group.members di @p gr
 * @param gr Puntatore alla struttura @ref group nella quale si intende effettuare
 * l'inserimento
 * @param nickName Nome del nuovo membro da aggiungere alla lista dei membri
 * @retval 0: Se l'inserimento è avvenuto con successo
 * @retval -10: Se l'utente risulta essere già nella lista dei membrei di @p gr
 * @retval -1: Se si è verificato un altro errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                  |
 * | :----------: | :-------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido |
 */
int addMember(group_t* gr, char* nickName);


/**
 * @brief Rimuove un membro specifico dalla lista dei membri
 * @ref group.members di @p gr
 * @param gr Puntatore alla struttura @ref group che contiene
 * la lista dei membri dalla quale si intende effettuare la rimozione.
 * @param nickName Nome dell'utente da rimuovere dalla lista dei membri
 * @retval 0: Se la rimozione è avvenuta con successo
 * @retval -11: Se l'utente non è presente nella lista dei membri di @p gr
 * @retval -1: Se si è verificato un altro errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                   |
 * | :----------: | :--------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido  |
 */
int removeMember(group_t* gr, char* nickName);


/**
 * @brief Controlla l'appartenenza di un utente ad un certo gruppo,
 * ossia la presenza dell'username relativo all'utente all'interno della
 * lista dei membri @ref group.members di @p gr
 * @param gr Puntatore alla struttura @ref group nella quale si vuole
 * effettuare il controllo
 * @param nickName Username dell'utente di cui si vuole controllare la presenta
 * @retval 1: Se l'utente risulta essere presente nella lista dei membri del gruppo @p gr
 * @retval 0: Se l'utente non risulta essere presente nella lista dei membri
 * del gruppo @p gr
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                   |
 * | :----------: | :--------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido  |
 */
int isMember(group_t* gr, char* nickName);


/**
 * @brief Libera la memoria occupata dalla struttura @ref group
 * passata come argomento.
 * @param gr Indirizzo del puntatore alla struttura @ref group
 * che si intende eliminare.
 * @retval 0: Questa funzione non può fallire.
 */
int destroyGroup(group_t* gr);
/*-----------------------------------------------------------------------*/



/*---------------DICHIARAZIONE FUNZIONI GROUP LIST-----------------------*/
/**
 * @brief Inizializza una struttura @ref groupsList
 * @param grList Puntatore alla struttura @ref groupsList da inizializzare
 * @retval 0: Se l'inizializzazione è avvenuta con successo
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                   |
 * | :----------: | :--------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido  |
 */
int initializeGroupList(groupsList_t* grList);


/**
 * @brief Inserisce un nuovo gruppo con nome @p grName nella lista
 * puntata da @p grL (@ref groupsList) controllando, durante la verifica,
 * che non sia presente nella lista un'altro gruppo con il solito nome.
 * @param grL Puntatore alla lista dove si intende effettuare l'inserimento
 * @param grName Nome del nuovo gruppo da inserire nella lista
 * @param fnd Username dell'utente che richiede la creazione del gruppo
 * @retval 0: Se l'inerimento è andato a buon fine
 * @retval -12: Se è già presente nella lista un gruppo con il solito nome
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                   |
 * | :----------: | :--------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido  |
 *
 * @see Per gli altri codici di errore vedere il manuale di
 * calloc().
 */
int insertGroupNode(groupsList_t* grL, char* grName, char* fnd);


/**
 * @brief Recerca all'interno della lista @p grL (@ref groupsList), il gruppo (@ref group)
 * che abbia il nome uguale alla stringa @p grName e salva il suo indirizzo
 * nel puntatore @p save passato per parametro
 * @param grL Lista dove effettuare la ricerca
 * @param grName Nome del gruppo da ricercare
 * @param save Puntatore che conterrà l'indirizzo del gruppo cercato nel
 * caso in cui la ricerca abbia successo
 * @retval 1: Se la ricerca ha avuto successo e salva il risultato in @p save
 * @retval 0: Se la ricerca non ha prodotto nessun risultato e @p save viene
 * settato a @c NULL
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                   |
 * | :----------: | :--------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido  |
 *
 * @see Per gli altri codici di errore vedere il manuale di
 */
int getGroupNode(groupsList_t* grL, char* grName, groupNode_t** save);


/**
 * @brief Rimuove, se presente, il gruppo (@ref group) identificato dal
 * nome @p grName dalla lista @p grL (@ref groupsList).
 * @param grL Lista dove effettuare la rimozione
 * @param grName Nome del gruppo da rimuovere
 * @retval 0: Se la rimozione è avvenuta con successo
 * @retval -13: Se il gruppo non è presente nella lista @p grL
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                   |
 * | :----------: | :--------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido  |
 *
 * @see Per gli altri codici di errore vedere il manuale di
  */
int removeGroupNode(groupsList_t* grL, char* grName);


/**
 * @brief Libera la memoria occupata della struttura @ref groupsList puntata
 * da @p grL e da tutti i sui componenti
 * @param grL Puntatore alla lista dei gruppi (@ref groupsList) che si intende
 * cancellare
 * @retval 0: Questa funzione non può fallire
 */
int destroyGroupList(groupsList_t* grL);
/*-----------------------------------------------------------------------*/




/*---------------DICHIARAZIONE FUNZIONI GROUP TABLE----------------------*/
/**
 * @brief Funzione hash che indicizza gli elementi della tabbella dei gruppi @ref groupsTable
 * @param str Stringa da indicizzare
 * @retval N: Indirizzo corrispondente alla stringa nella tabella
 * @retval -1: Se si è verificato un altro errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                  |
 * | :----------: | :-------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido |
 *
 */
int groupHash(unsigned char* str);


/**
 * @brief Inizializza la tabella dei gruppi (@ref groupsTable) di sistema (@ref grTable)
 * @retval 0: Se l'inizializzazione è avvenuta correttamente
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 * @see Per i codici di errore vedere il manuale di
 * pthread_rwlock_init().
 */
int initializeGroupsTable();


/**
 * @brief Inserisce un nuovo gruppo (@ref group) all'interno della tabella dei gruppi
 * di sistema (@ref grTable)
 * @param grName Nome del nuovo gruppo da aggiungere alla tabella dei gruppi
 * @param fnd Username dell'utente che ha richiesto la creazione del gruppo
 * @retval 0: Se l'inserimento è avvenuto con successo
 * @retval -12: Se è già presente nella tabella un gruppo con nome uguale a @p grName
 * @retval -1: Se si è verificato un altro errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                  |
 * | :----------: | :-------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere il manuale di
 * insertGroupNode().
 */
int insertGroup(char* grName, char* fnd);


/**
 * @brief Rimuove l'username di un utente (@p newMember) dalla lista
 * dei membri di uno specifico gruppo (@ref group) all'interno della
 * tabella dei gruppi di sistema (@ref groupsTable).
 * @param grName Nome del gruppo dove si intende effettuare la rimovione
 * @param memb Nome dell'utente che si intende rimuovere dalla lista dei membri
 * del gruppo identificato da @p grName.
 * @retval 0: Se la rimozione è avvenuta con successo
 * @retval -13: Se il gruppo non è presente nella tabella dei gruppi
 * di sistema (@ref grTable)
 * @retval -1: Se si è verificato un altro errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                  |
 * | :----------: | :-------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere il manuale di
 * calloc(), removeMember().
 */
int disjoin(char* grName, char* memb);


/**
 * @brief Rimuove, se esiste il gruppo @ref group identificato da @p grName
 * dalla tabella dei gruppi di sistema (@ref grTable)
 * @param grName Nome del gruppo che si intende rimuovere
 * @retval 0: Se la rimozione è avvenuta con successo
 * @retval -13: Se il gruppo non è presente nella tabella dei gruppi
 * di sistema (@ref grTable)
 * @retval -1: Se si è verificato un altro errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                  |
 * | :----------: | :-------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere il manuale di
 */
int removeGroup(char* grName);

/**
 * @brief Libera la memoria occupata dalla dabella dei gruppi di sistema
 * @retval 0: Nel caso in cui l'eliminazione è avvenuta con successo
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 * @see Per i codici di errore vedere il manuale di
 * pthread_rwlock_destroy().
 */
int destroyGroupsTable();
/*-----------------------------------------------------------------------*/


#endif
