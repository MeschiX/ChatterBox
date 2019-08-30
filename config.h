/*
 * membox Progetto del corso di LSO 2017/2018
 *
 * Dipartimento di Informatica Università di Pisa
 * Docenti: Prencipe, Torquati
 *
 */
/**
 * @file config.h
 * @brief File contenente alcune define con valori massimi utilizzabili
 */

#if !defined(CONFIG_H_)
#define CONFIG_H_

#if _POSIX_C_SOURCE < 200809L
    #undef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 200809L
#endif

#if !defined(UNIX_PATH_MAX)
    #define UNIX_PATH_MAX  64
#endif

/**
 * @brief Definisce la lunghezza massima del nuome di un utente
 */
#define MAX_NAME_LENGTH                  32

/**
 * @brief Definisce la lunghezza massima di una riga del file di configurazione
 */
#define MAX_CONFIG_FILE_LINE_LENGTH 512

/**
 * @brief Definisce il numero di righe della tabella utenti (@ref usersTable)
 */
 #define USERS_TABLE_SIZE 32768

/**
 * @brief Definisce l'ampiezza del blocco di mutua esclusione all'interno della tabella utenti (@ref usersTable)
 */
#define USERS_MUTEX_BLOCK_SIZE 64

/**
 * @brief Definisce il numero di rige della tabella (@ref groupsTable) e data la sua struttura, il numero massimo
 * di gruppi (@ref group) registrabili nel sistema.
 */
#define GROUPS_TABLE_SIZE 128

/**
 * @brief Definisce l'ampiezza del blocco di mutua esclusione all'interno della tabella dei gruppi (@ref groupsTable)
 */
#define GROUPS_MUTEX_BLOCK_SIZE 8

/**
 * @brief Definisce il valore massimo di connessioni simultanee per lo
 * stesso utente
 */
#define MAX_USER_SIMULTANEUS_CONNECTIONS 5

/**
 * @brief Definisce il numero massimo di gruppi a cui un utente può
 * appartenere simultaneamente.
 */
#define MAX_USER_SIMULTANEUS_GROUPS 5

/**
 * @brief Definisce il valore di terminazione della coda @ref incomeList
 */
#define INCOME_LIST_TERM -111

// to avoid warnings like "ISO C forbids an empty translation unit"
typedef int make_iso_compilers_happy;

#endif /* CONFIG_H_ */
