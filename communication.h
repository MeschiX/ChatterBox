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

#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include "logIn.h"
#include "message.h"
#include "users.h"

/**
 * @file communication.h
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Contiene la dichiarazione di diverse funzioni di comunicazione utilizzate
 * dal server per soddisfare alcune richieste
 */

/**
 * @brief Legge l'header di un messaggio
 * @param fd Descrittore della connessione
 * @param hdr Puntatore ad una struttura @ref message_hdr_t che conterrà
 * il risultato della lettura
 * @retval N: Numero di bytes letti in caso di successo
 * @retval -17: Se il client ha chiuso il socket durante la lettura
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                  |
 * | :----------: | :-------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere la documentazione di
 * readv().
 */
int sReadHeader(long fd, message_hdr_t* hdr);

/**
 * @brief Legge la parte dati di un  messaggio
 * @param fd Descrittore della connessione
 * @param data Puntatore ad una struttura @ref message_data_t che conterrà
 * il risultato della lettura
 * @retval N: Numero di bytes letti in caso di successo
 * @retval -17: Se il client ha chiuso il socket durante la lettura
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                  |
 * | :----------: | :-------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere la documentazione di
 * readv(), malloc(), read().
 */
int sReadData(long fd, message_data_t* data);

/**
 * @brief Legge la parte dati di un messaggio che contiene un file
 * @details La funzione esegue le stesse operazioni di sReadData()
 * ma esegue un controllo sulla dimensione del file che viene comunicata
 * nella lettura dell'intestazione della parte dati (@ref message_data_hdr_t)
 * @param fd Descrittore della connessione
 * @param data Puntatore ad una struttura  @ref message_data_t che conterrà
 * il risultato della lettura
 * @retval N: Numero di bytes letti in caso di successo
 * @retval -17: Se il client ha chiuso il socket durante la lettura
 * @retval -18: Se il file contenuto nel messaggio ha una dimensione superiore a quella consentita
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                  |
 * | :----------: | :-------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere la documentazione di
 * readv(), malloc(), read().
 */
int sReadFile(long fd, message_data_t* data);

/**
 * @brief Legge un intero messaggio
 * @param fd Descrittore della connessione
 * @param msg Puntatore ad una struttura @ref message_t che conterrà
 * il risultato della lettura
 * @retval N: Numero di bytes letti in caso di successo
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                  |
 * | :----------: | :-------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere la documentazione di
 * sReadHeader(), sReadData().
 */
int sReadMsg(long fd, message_t *msg);

/**
 * @brief Scrive l'intestazione di un messaggio sul socket identificato da @p fd
 * @param fd Descrittore della connessione
 * @param hdr Puntatore alla struttura @ref message_hdr_t che si intende
 * scrivere sul socket
 * @retval N: Numero di bytes scritti in caso di successo
 * @retval -17: Se il client ha chiuso il socket durante la scrittura
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                  |
 * | :----------: | :-------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere la documentazione di
 * writev().
 */
int sSendHeader(long fd, message_hdr_t* hdr);

/**
 * @brief Scrive la parte dati di un messagio sul socket identificato da @p fd
 * @param fd Descrittore della connessione
 * @param data Puntatore alla struttura @ref message_data_t che si intende
 * scrivere sul socket
 * @retval N: Numero di bytes scritti in caso di successo
 * @retval -17: Se il client ha chiuso il socket durante la scrittura
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                  |
 * | :----------: | :-------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere la documentazione di
 * writev().
 */
int sSendData(long fd, message_data_t *data);

/**
 * @brief Scrive un intero messagio sul socket identificato da @p fd
 * @param fd Descrittore della connessione
 * @param msg Puntatore alla struttura @ref message_t che si intende
 * scrivere sul socket
 * @retval N: Numero di bytes scritti in caso di successo
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                  |
 * | :----------: | :-------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere la documentazione di
 * sSendHeader(), sSendData().
 */
int sSendMsg(long fd, message_t* msg);


/**
 * @brief Effettua l'operazione di registrazione di un utente
 * nel sistema.
 * @details Inserisce un nuovo utente (@ref user), con username (@ref user.nickName)
 * uguale alla stringa @p nickName passata come parametro, all'interno della
 * tabella degli utenti di sistema (@ref usrTable), ammesso che non ve ne sia
 * già presente uno con lo stesso nickName.
 * L'indice di riga dove verrà posizionata la nuova struttura @ref user verrà
 * stabilita tramite la funzione userHash() e l'inserimento all'interno della riga (la quale
 * coniste in una @ref usersList) verrà effettuato tramite la funzione addUserNode().
 * @param nickName Nickname dello @ref user da registrare e inserire nella tabella
 * @retval 0: Se l'inserimento è avvenuto con successo.
 * @retval -2: un utente con lo stesso username è già registrato nel sistema
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno       | Motivo                                   |
 * | :------------: | :--------------------------------------: |
 * |@c EINVAL       | Almeno uno degli argomenti non è valido  |
 *
 * @see Per gli altri codici di errore vedere il manuale di
 * userHash(), addUserNode().
 */
int registerUser(char* nickName);


/**
 * @brief Effettua l'oerazione di deregistrazione di un utente
 * dal sistema
 * @details Viene anzitutto effettuata una ricerca nella tabella degli utenti
 * per verificare che il nome utente @p nickName sia effettivamente registrato.
 * In caso affermativo viene passata in rassegna la struttura @ref membership
 * relativa all'utente selezionato e viene invocata la procedura removeMember()
 * su tutti i gruppi che vi risiedono, ossia l'utente viene dissociato
 * da ogni gruppo a cui si era registrato.
 * Dopodiché si procede con il logout di tutti i client connessi al
 * servizio con questo utente tranne quello che ha inoltrato la richiesta di
 * deregistrazione.
 * A questo punto si procede con la rimozione della struttura @ref user dalla
 * tabella utenti di sistema @ref usrTable.
 * @param nickName Nickname dello @ref user da rimuovere.
 * @param client File descriptor del client connesso al servizio che ha richiesto
 * la deregistrazione
 * @retval 0: Se la rimozione è avvenuta con successo
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno | Motivo                                     |
 * | :------: | :----------------------------------------: |
 * |@c EINVAL | Almeno uno degli argomenti non è valido    |
 *
 * @see Per gli altri codici di errore vedere il manuale di
 * userHash(), getUserNode(), removeUserNode().
 */
int unregisterUser(char* nickName, long client);


/**
 * @brief Funzione di servizio che effettua un controllo su un certo
 * username determinando se quest'ultimo è presente nella tabella degli
 * utenti di sistema @ref usrTable o meno
 * @param nickName Username dell'utente di cui si vuole verificare la registrazione
 * @retval 1: Se l'utente con username @p nickName è registrato
 * @retval 0: Se l'utente con username @p nickName non è registrato
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                  |
 * | :----------: | :-------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere la documentazione di
 * getUserNode().
 */
int checkRegistration(char* nickName);

/**
 * @brief Effettua la procedura di login completa di un utente
 * @details Viene anzitutto effettuata una ricerca nella tabella degli utenti
 * per verificare che il nome utente @p nickName sia effettivamente registrato.
 * Dopodiché verifica che il numero di login simultanei per l'utente non superi la
 * soglia massima (@ref MAX_USER_SIMULTANEUS_CONNECTIONS).
 * A questo punto viene aggiunta un'istanza di login contenente il nome @p nickName
 * all'indirizzo @p address nella tabella di logIns di sistema (@ref logIn),
 * tale indirizzo viene poi inserito nel vetore degli indirizzi (@ref delivery)
 * relativo all'utente.
 * @param nickName Nome dell'utente che richiede un logIn nel sistema
 * @param address File descriptor del socket su cui il client è connesso ed
 * ha inviato la richiesta
 * @retval 0: Se l'operazione di logIn è avvenuta con successo
 * @retval -14: Se il nome @p nickName non corrisponde al'username di nessun
 * utente registrato in @ref usrTable
 * @retval -7: Se la soglia di login simultanei per uno stesso utente è stata superata
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                  |
 * | :----------: | :-------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere la documentazione di
 * getUserNode(), logIn(), addAddress().
 */
int logInUser(char* nickName, int address);


/**
 * @brief Effettua la proocedura di logout completa di un utente
 * @details Viene anzitutto effettuata una ricerca nella tabella degli utenti
 * per verificare che il nome utente collegato all'indirizzo @p address
 * nella tabella @ref lgTable sia effettivamente registrato.
 * A questo punto viene rimosso l'indirizzo @p address dal vettore degli indirizzi
 * relativo all'utente dopodiché viene rimossa l'istanza di logIn all'indirizzo
 * @p address nella tabella dei login di sistema @ref lgTable
 * @param address File descriptor del socket dove si è verificata una chiusura
 * dal lato client
 * @retval 0: Se l'operazione di logOut è avvenuta con successo
 * @retval -14: Se il nome utente che corrisponde all'indirizzo nella tabella dei login
 * di sistema non corrisponde all'username di nessun utente registrato
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                  |
 * | :----------: | :-------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere la documentazione di
 * getUserNode(), logOut(), removeAddress().
 */
int logOutUser(int address);


/**
 * @brief Effettua l'operazione di creazione di un gruppo.
 * @details Viene prima effettuata una verifica sull'esistenza
 * dell'utente identificato da @p fnd e che sia effettivamente
 * registrato nella tabella degli utenti di sistema @ref usrTable
 * dopodiché, se non è già presente un gruppo con lo stesso
 * groupName nella tabella dei gruppi di sistema @ref grTable ,
 * viene inserito il nuovo gruppo con nome @p grName e fondatore
 * @p fnd .
 * @param grName Nome del nuovo grupo che si intende creare
 * @param fnd Username dell'utente che ha fatto richiesta di
 * creazione del gruppo
 * @retval 0: Se la creazione è avvenuta con successo
 * @retval -14: Se @p fnd non corrisponde allo username di nessun
 * utente registrato
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                  |
 * | :----------: | :-------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere la documentazione di
 * getUserNode(), insertGroup().
 */
int createGroup(char* grName, char* fnd);


/**
 * @brief Effettua l'operazione di associazione di un utente ad un gruppo
 * @details Viene anzitutto effettuata una ricerca nella tabella degli utenti
 * per verificare che il nome utente @p nickName sia effettivamente registrato.
 * A questo punto viene inserito @p grName nel vettore di
 * appartenenza (@ref membership) relativo all'utente dopodiché viene aggiunto
 * @p nickName alla lista dei membri del gruppo identificato da @p grName
 * @param grName Nome del gruppo nel quale si intende associare l'utente
 * @param nickName Nome dell'utente che richiede l'associazione al gruppo
 * @retval 0: Se l'operazione di associazione è avvenuta con successo
 * @retval -14: Se il nome @p nickName non corrisponde al'username di nessun
 * utente registrato in @ref usrTable
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                  |
 * | :----------: | :-------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere la documentazione di
 * getUserNode(), getGroupNode(), addMember(), association().
 */
int joinUser(char* grName, char* nickName);


/**
 * @brief Effettua l'oerazione di dissociazione di un utente da un gruppo
 * @details Viene anzitutto effettuata una ricerca nella tabella degli utenti
 * per verificare che il nome utente @p nickName sia effettivamente registrato.
 * A questo punto viene rimosso @p grName dal vettore di
 * appartenenza (@ref membership) relativo all'utente dopodiché viene rimosso
 * @p nickName dalla lista dei membri del gruppo identificato da @p grName
 * @param grName Nome del gruppo dal quale si intende efettuare la dissociazione
 * @param nickName Nome dell'utente da dissociare
 * @retval 0: Se l'operazione di dissociazione è avvenuta con successo
 * @retval -14: Se il nome @p nickName non corrisponde al'username di nessun
 * utente registrato in @ref usrTable
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                  |
 * | :----------: | :-------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere la documentazione di
 * getUserNode(), getGroupNode(), removeMember(), dissociation().
 */
int disjoinUser(char* grName, char* nickName);


/**
 * @brief Effettua l'operazione di cancellazione di un gruppo
 * @details Innanzitutto si controlla che esistano sia l'utente che
 * ha richiesto la cancellazione sia il gruppo che si intende cancellare,
 * poi si verifiche che l'utente che ha richiesto la cancellazione
 * sia effettivamente il fondatore del gruppo dopodiché viene passata
 * in rassegna la lista dei membri relativa al gruppo identificato da
 * @p grName e per ogni membro, viene chiamata la funzione dissociation(),
 * che rimuove il nome del gruppo che si intende eliminare dalla lista di
 * appartenenza (@ref membership) dell'utente. A questo punto si elimina
 * il gruppo @p grName dalla tabella dei gruppu di sistema @ref grTable
 * @param grName Nome del gruppo che si intende eliminare
 * @param applicant Username dell'utente che ha richiesto la
 * cancellazione del gruppo
 * @retval 0: Se la cancellazione è avvenuta con successo
 * @retval -13: Se il gruppo identificato da @p grName non è presente
 * nella tabella dei grupi di sistema (@ref grTable)
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                  |
 * | :----------: | :-------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere la documentazione di
 * getUserNode(), getGroupNode(), dissociation(), destroyGroup().
 */
int deleteGroup(char* grName, char* applicant);


/**
 * @brief Effettua l'invio di un messaggio ad un singolo utente
 * @details Questa funzione, dato un messaggio (@ref message_t) e
 * un utente (@ref user), passa un rassegna la struttura @ref delivery
 * dell'utente e per ogni indirizzo contenuto, acquisisce la lock
 * corrispondente all'indirizzo nella tabella @ref lgTable e scrive
 * sul socket il messaggio @p msg.
 * Dopodiché aggiorna la @ref history relativa all'utente con @p msg
 * @param msg Struttura @ref message_t che si intende scrivere sul socket e memorizzare in @ref history
 * @param receiver Utente a cui è destinato il messaggio @p msg
 * @retval N: Numero di messsaggi scritti sui diversi sockets
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                                                                 |
 * | :----------: | :------------------------------------------------------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido                                                |
 * |@c EPERM      | Se si è verificata un'inconsistenza fra la @ref delivery dell'utente e la @ref lgTable |
 *
 * @see Per gli altri codici di errore vedere la documentazione di
 * sSendMsg(), pushMessage().
 */
int sendAtUser(message_t* msg, user_t* receiver);


/**
 * @brief Invia un messaggio di risposta con esito postivo ad un client
 * @details Il messaggio inviato potrà avere o non avere un parte dati
 * che verrà spedita solo se presente. A differenza di  sendAtUser()
 * questa funzione non fa riferimento ad alcun utente ma piuttosto
 * alla sola tabella @ref lgTable (ovvero nessuno dei messaggi inviati
 * con questa funzione verrano memorizzati nella history di un utente).
 * @param client File descriptor del client a cui è sdestinato il messaggio
 * di risposta
 * @param bufLen Lunghezza della parte dat1 se presente (qualsiasi valore minore
 * o uguale a 0 viene interpretato come assenza della parte dati).
 * @param buf Parte dati da inviare se presente (Da impostare a @c NULL se non
 * presente)
 * @retval 0: Se l'invio del messaggio di risposta è avvenuto correttamente
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno   | Motivo                                                                                       |
 * | :--------: | :------------------------------------------------------------------------------------------: |
 * |@c EINVAL   | Almeno uno degli argomenti non è valido                                                      |
 * |@c EPERM    | Se l'indirizzo @p address non risulta attivo nella tabella dei logIn di sistema @ref lgTable |
 *
 * @see Per gli altri codici di errore vedere la documentazione di
 * sSendHeader(), sSendData().
 */
int sendOk(int client, int bufLen, char* buf);


/**
 * @brief Invia un messaggio di risposta con esito negativo ad un client
 * con codice di errore uguale a @p errCode
 * @details Il messaggio inviato avrà solamente una parte di intestazione.
 * A differenza di  sendAtUser() questa funzione non fa riferimento ad alcun utente ma piuttosto
 * alla sola tabella @ref lgTable (ovvero nessuno dei messaggi inviati
 * con questa funzione verrano memorizzati nella history di un utente).
 * @param client File descriptor del client a cui è sdestinato il messaggio
 * di risposta
 * @param errCode Codice di errore da comunicare al client
 * @retval 0: Se l'invio del messaggio di risposta è avvenuto correttamente
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno   | Motivo                                                                                       |
 * | :--------: | :------------------------------------------------------------------------------------------: |
 * |@c EINVAL   | Almeno uno degli argomenti non è valido                                                      |
 * |@c EPERM    | Se l'indirizzo @p address non risulta attivo nella tabella dei logIn di sistema @ref lgTable |
 *
 * @see Per gli altri codici di errore vedere la documentazione di
 * sSendHeader().
 */
int sendError(int client, op_t errCode);


/**
 * @brief Effettua l'operazione di invio messaggio
 * @details Inizialmente si verifica che, nel caso si voglia inviare un messaggio
 * di tipo testuale, la lunghezza del messaggio non superi quella consentita,
 * dopodiché viene determinato il tipo di messaggio:
 *    - Utente unico o gruppo di utenti
 *        - @ref POSTFILE_OP
 *        - @ref POSTTXT_OP
 *    - Tutti gli utenti registrati
 *        - @ref POSTTXTALL_OP
 *
 * Quindi se il tipo di messaggio è uguale a @ref POSTFILE_OP oppure a @ref POSTTXT_OP
 * viene inizialmente effettuato un controllo per verificare se il destinatario del
 * messaggio corrisponde al nome di un gruppo oppure ad un signolo utente.
 * Nel caso corrisponda ad un gruppo il messaggio viene spedito a tutti gli utenti
 * associati al gruppo, passando in rassegna la relativa lista dei membri, tramite
 * la funzione sendAtUser().
 * Nel caso corrisponda invece ad un utente il messaggio viene inviato al singolo utente
 * destinatario sempre attraverso la funzione sendAtUser().
 * Se invece  @p outcome->hdr.op è uguale a @ref POSTTXTALL_OP allora viene passata
 * in rassegna tutta la tabella degli utenti di sistema @ref usrTable e viene inviato
 * il messaggio ad ogni utente che risulta registrato con lo stesso meccanismo usato
 * in sendAtUser().
 * @param outcome Puntatore alla struttura @ref message_t da inviare
 * @retval N: Il numero di messaggi effettivamente inviati (ossia scritti sui socket
 * dei client loggati con gli utenti destinatari).
 * @retval -18: Se @p outcome contiene un messaggio di lunghezza superiore a quella
 * consentita.
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                                                                  |
 * | :----------: | :-------------------------------------------------------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido                                                 |
 * |@c ENOMSG     | Il campo @ref message_hdr_t.op del messaggio @p outcome è errato                          |
 * |@c EPERM      | Se si verifica un inconsistenza fra @ref lgTable e la @ref delivery relativa all'utente |
 *
 * @see Per gli altri codici di errore vedere la documentazione di
 * getUserNode(), getGroupNode(), copyMessage(), sendAtUser(), sSendMsg(), pushMessage().
 *
 * @note Se la funzione ritorna 0 anche se il messaggio ha almeno un destinatario
 * questo non deve essere considerato un errore bensì significa che nessuno
 * dei destinatari è loggato al servizio ma sono comunque registrati e le
 * corrispondenti strutture @ref history sono state aggiornate
 */
int ship(message_t* outcome);


/**
 * @brief Produce la lista degli utenti loggati nel sistema al momento della chiamata
 * @details Viene scansionata la tabella dei logIns di sistema @ref lgTable e vengono
 * trascritti in @p save tutti i nickname degli utenti che risultano loggati.
 * @param save Stringa dove verrà trascritta la lista degli utenti loggati
 * @retval N: La lunghezza della lista se e la generazione è avvenuta con successo
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                  |
 * | :----------: | :-------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere la documentazione di
 * realloc().
 */
int getLoggedUsersList(char** save);


/**
 * @brief Effettua l'operazione di invio dello storico dei messaggi
 * @details Viene anzitutto effettuata una ricerca nella tabella degli utenti
 * di sistema @ref usrTable per verificare che il nome utente @p nickName sia
 * effettivamente registrato.
 * Dopodiché viene inviato un messaggio di OK contenente, nella parte dati,
 * il numero di messaggi presenti nella struttura @ref history relativa
 * all'utente.
 * A questo punto vengono inviati uno per uno i messaggi presenti nello
 * storico.
 * @param nickName Nome dell'utente che ha richiesto l'operazione
 * @param address File descriptor sul quale è connesso il client loggato
 * al sistema che ha richiesto l'operazione
 * @retval 0: Se l'operazione è avvenuta con successo
 * @retval -14: Se l'utente identificato da @p nickName non è registrato
 * nel sistema
 * @retval -17: Se il client loggato come l'utente che ha richiesto l'operazione
 * ha chiuso la connessione prima di aver ricevuto tutti i messaggi presenti
 * nello storico
 * @retval -1: Se si è verificato un errore e viene settata opportunamente
 * la variabile @c errno
 *
 * | @c errno     | Motivo                                  |
 * | :----------: | :-------------------------------------: |
 * |@c EINVAL     | Almeno uno degli argomenti non è valido |
 *
 * @see Per gli altri codici di errore vedere la documentazione di
 * sendOK(), calloc(), sSendMsg().
 */
int sendUserHistory(char* nickName, int address);


#endif
