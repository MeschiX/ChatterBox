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
/**
 * @file settings.c
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Contiene la definizione delle funzioni di confiurazione
 * del server
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "config.h"
#include "settings.h"
#include "utils.h"


int setUpConfiguration(char* configurationFileName)
{
    if(configurationFileName == NULL){errno = EINVAL; return -1;}

    int valueCounter = 0;

    char* buf = (char*)calloc(MAX_CONFIG_FILE_LINE_LENGTH, 1);
    if(buf == NULL){return -1;}
    FILE* fp = fopen(configurationFileName, "r");
    if(fp == NULL){free(buf); return -1;}

    char* saveptr;
    while(fgets(buf, MAX_CONFIG_FILE_LINE_LENGTH, fp) != NULL)
    {
        saveptr = buf;

        // Non considera eventuali spazi all'inizio della riga
        while(*buf==' '){buf++;}

        if(*buf!='#' && isalpha((int)(*buf)))
        {
            char identifier[UNIX_PATH_MAX];
            char* token = NULL;

            token = strtok(buf, " \t\n");
            strcpy(identifier, token);

            token = strtok(NULL, " \t\n");
            // Adesso token sarà "="

            token = strtok(NULL, " \t\n");
            // Adesso token conterrà il valore della variabile

            if(strcmp(identifier, "UnixPath")==0)
            {
                memset(UnixPath, 0, UNIX_PATH_MAX);
                strcpy(UnixPath, token);
                valueCounter++;
            }
            else if(strcmp(identifier, "MaxConnections")==0)
            {
                MaxConnections = atoi(token);
                valueCounter++;
            }
            else if(strcmp(identifier, "ThreadsInPool")==0)
            {
                ThreadsInPool = atoi(token);
                valueCounter++;
            }
            else if(strcmp(identifier, "MaxMsgSize")==0)
            {
                MaxMsgSize = atoi(token);
                valueCounter++;
            }
            else if(strcmp(identifier, "MaxFileSize")==0)
            {
                MaxFileSize = atoi(token);
                valueCounter++;
            }
            else if(strcmp(identifier, "MaxHistMsgs")==0)
            {
                MaxHistMsgs = atoi(token);
                valueCounter++;
            }
            else if(strcmp(identifier, "DirName")==0)
            {
                memset(DirName, 0, UNIX_PATH_MAX);
                strcpy(DirName, token);
                valueCounter++;
            }
            else if(strcmp(identifier, "StatFileName")==0)
            {
                memset(StatFileName, 0, UNIX_PATH_MAX);
                strcpy(StatFileName, token);
                valueCounter++;
            }
        }

        buf = saveptr;
    }

    free(buf);
    fclose(fp);

    return valueCounter;
}


int printSettingValues()
{
    int test = 0;

    test += printf("\tUnixpath: ");
    test += print("\t\"%s\"\n", eBOLDBLUE, UnixPath);
    test += printf("\tMaxConnections: ");
    test += print("\"%d\"\n", eBOLDBLUE, MaxConnections);
    test += printf("\tTheadsInPool: ");
    test += print("\t\"%d\"\n", eBOLDBLUE, ThreadsInPool);
    test += printf("\tMaxMsgSize: ");
    test += print("\t\"%d\"\n", eBOLDBLUE, MaxMsgSize);
    test += printf("\tMaxFileSize: ");
    test += print("\t\"%d\"\n", eBOLDBLUE, MaxFileSize);
    test += printf("\tMaxHistMsgs: ");
    test += print("\t\"%d\"\n", eBOLDBLUE, MaxHistMsgs);
    test += printf("\tDirName: ");
    test += print("\t\"%s\"\n", eBOLDBLUE, DirName);
    test += printf("\tStatFileName: ");
    test += print("\t\"%s\"\n", eBOLDBLUE, StatFileName);
    test += printf("\n\n");

    return test;
}
