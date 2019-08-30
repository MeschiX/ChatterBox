
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

#ifndef __LISTENER_H__
#define __LISTENER_H__

/**
 * @file  listener.h
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Contiene la dichiarazione della funzione di routine eseguita da ogni
 * thread worker generato dal server.
 */

/**
 * @brief Funzione di routine per i threads worker.
 * @details I threads che assumono questa funzione come funzione di routine, eseguono
 * un ciclo potenzialmente infinito composto da 6 fasi:
 * - Estrazione di una richiesta (@ref income) dalla @ref incomeList di sistema (@ref service)
 *   tramite la chiamata di @ref popIncome.
 *    - Se la parte @c fd (@ref income.fd) della richiesta è diversa dal valore
 *       di terminazione della catena (@ref INCOME_LIST_TERM) il ciclo del thread prosegue normalmente.
 *    - Se la parte @c fd (@ref income.fd) della richiesta è uguale valore
 *       di terminazione della catena (@ref INCOME_LIST_TERM) allora il thread interrompe il
 *       ciclo, esegue alcune operazioni di pulizia e termina.
 * - Lettura della richiesta: viene fatta una lettura (@ref sReadMsg) dal file
 *   descriptor contenuto nella struttura @ref income appena estratta.
 *   Il messaggio letto consisterà in una richiesta di una specifica operazione
 *   (@ref op_t) da parte del client.
 * - Gestione della richiesta: Viene confrontato il codice di operazione ricevuto
 *   nella richiesta con tutte le possibili alternative (vedi @ref op_t).
 * - Servizio della richiesta: La richiesta viene elaborata modificando e/o leggendo
 *   dati presenti nelle strutture del server.
 * - Risposta alla richiesta: in seguito al servizio della richiesta viene inviato
 *   un messaggio di conferma (@ref OP_OK) se la richiesta è stata servita correttamente,
 *   altrimenti viene inviato un messaggio di errore contenente uno codice di errore
 *   che descrive l'errore accaduto in fase di servizio.
 * - Pulizia dei dati temporanei.
 * - Se non viene imposta la disconnessine del client, appena servito viene reinserito
 *   nell'insieme di file descriptors del server (@ref masterSet), a questo punto il ciclo
 *   ricomincia dal primo punto.
 * @param args Argomenti del thread. In  questo sistema verrà passato solo un ID numerico
 * che si riferisce all'identificatore del thread
 */
void* listener(void* args);

#endif
