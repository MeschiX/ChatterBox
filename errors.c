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

#include <stdio.h>
#include <errno.h>
#include <stdarg.h>

#include "errors.h"
#include "utils.h"

/**
 * @file errors.c
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Contiene la definizione della funzione usata per la stampa formattata
 * del messaggio di errore relativo ad un certo codice
 * @see errors.h
 */

int printError(int ID, int errCode, char* errText, ...)
{
    if(errCode >= 0){errno = EINVAL; return -1;}

    char* errMex;
    int printed = 0;
    va_list argList;

    switch (errCode){
    case USER_ALREADY_REGISTERED:
        errMex = STR_USER_ALREADY_REGISTERED;
        break;
    case USERNAME_TOO_LONG:
        errMex = STR_USERNAME_TOO_LONG;
        break;
    case USERS_QUOTA_EXCEEDED:
        errMex = STR_USERS_QUOTA_EXCEEDED;
        break;
    case LOGIN_QUOTA_EXCEEDED:
        errMex = STR_LOGIN_QUOTA_EXCEEDED;
        break;
    case RECEIVER_UNKNOWN:
        errMex = STR_RECEIVER_UNKNOWN;
        break;
    case USER_LOGIN_QUOTA_EXCEEDED:
        errMex = STR_USER_LOGIN_QUOTA_EXCEEDED;
        break;
    case GROUPS_QUOTA_EXCEEDED:
        errMex = STR_GROUPS_QUOTA_EXCEEDED;
        break;
    case USER_GROUPS_QUOTA_EXCEEDED:
        errMex = STR_USER_GROUPS_QUOTA_EXCEEDED;
        break;
    case USER_ALREADY_JOINT:
        errMex = STR_USER_ALREADY_JOINT;
        break;
    case USER_NOT_JOINT:
        errMex = STR_USER_NOT_JOINT;
        break;
    case GROUP_ALREADY_REGISTERED:
        errMex = STR_GROUP_ALREADY_REGISTERED;
        break;
    case GROUP_NOT_REGISTERED:
        errMex = STR_GROUP_NOT_REGISTERED;
        break;
    case USER_NOT_REGISTERED:
        errMex = STR_USER_NOT_REGISTERED;
        break;
    case IS_GROUPNAME:
        errMex = STR_IS_GROUPNAME;
        break;
    case IS_USERNAME:
        errMex = STR_IS_USERNAME;
        break;
    case CLIENT_CLOSED_CONNECTION:
        errMex = STR_CLIENT_CLOSED_CONNECTION;
        break;
    case TEXT_MESSAGE_TOO_LONG:
        errMex = STR_TEXT_MESSAGE_TOO_LONG;
        break;
    case FILE_MESSAGE_TOO_BIG:
        errMex = STR_FILE_MESSAGE_TOO_BIG;
        break;
    case CLIENT_NOT_LOGGED:
        errMex = STR_CLIENT_NOT_LOGGED;
        break;
    case USER_NOT_FOUNDER:
        errMex = STR_USER_NOT_FOUNDER;
        break;
    default:
        return -1;
    }

    va_start(argList, errText);
    printed = vthreadPrint(ID, errText, eNORMAL, argList);
    va_end(argList);

    printed += print(errMex, eRED);

    return printed;
}
