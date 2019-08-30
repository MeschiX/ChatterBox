
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
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdarg.h>
#include <libgen.h>

#include "utils.h"

/**
 * @file utils.c
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Contiene la definizione delle funzioni di stampa e di
 * utilità
 */


/*-----------------------FUNZIONI DI UTILITA'---------------------------*/
int copyToBinary(void* data, size_t size, char** buff)
{
    if(data == NULL || size <= 0){errno = EINVAL; return -1;}

    if((*buff) != NULL){free(*buff); (*buff) = NULL;}

    (*buff) = (char*)malloc(size);
    if(*buff == NULL){return -1;}

    memcpy(*buff, (unsigned char*)data, size);

    return size;
}


int removeDir(char* path)
{
    if(path == NULL){errno = EINVAL; return -1;}

    int stop = 0;
    size_t path_len = strlen(path);
    struct dirent *p;
    DIR* dir = opendir(path);
    if(dir == NULL){return -1;}

    while (!stop && (p=readdir(dir)))
    {
        int stop2 = -1;
        char *buf;
        size_t len;

        // Si evita di considerare le cartelle "." e ".."
        if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
            continue;

        len = path_len + strlen(p->d_name) + 2;
        buf = (char*)malloc(len);
        if(buf == NULL){closedir(dir); return -1;}

        struct stat statbuf;

        snprintf(buf, len, "%s/%s", path, p->d_name);

        if (!stat(buf, &statbuf))
        {
            if (S_ISDIR(statbuf.st_mode))
                stop2 = removeDir(buf);
            else
                stop2 = unlink(buf);
        }

        free(buf);


        stop = stop2;
    }

    closedir(dir);

    if (!stop)
        stop = rmdir(path);

    return stop;
}

int getNameFromPath(char* path, char** save)
{
    if (path == NULL){errno = EINVAL; return -1;}

    bool stop = FALSE;
    char* name = NULL;

    for(size_t i = strlen(path) - 1; i && !stop; i--)
    {
        if (path[i] == '/')
        {
            name = (char*)calloc(strlen( &path[i+1])+1, sizeof(char));
            if (name == NULL){return -1;}
            strcpy(name,  &path[i+1]);
            stop = TRUE;
        }
    }

    if(!stop)
    {
        name = (char*)calloc(strlen(path)+1, sizeof(char));
        if (name == NULL){return -1;}
        strcpy(name, path);
    }

    *save = name;

    return 0;
}
/*----------------------------------------------------------------------*/


/*-----------------FUNZIONI GENERAZIONE DI OUTPUT COLORATO--------------*/
int print(char* format, color_t c, ...)
{
    if(format == NULL){return 0;}

    int test;
    char* buff;
    va_list argList;

    if(!colorful || c == eNORMAL)
    {
        va_start(argList, c);
        test = vprintf(format, argList);
        va_end(argList);

        return test;
    }

    buff = (char*)calloc(strlen(format) + 30, 1);
    if (buff == NULL) {return -1;}

    switch (c) {
    case eBOLD:
        strcat(buff, BOLD);
        break;
    case eRED:
        strcat(buff, RED);
        break;
    case eGREEN:
        strcat(buff, GREEN);
        break;
    case eYELLOW:
        strcat(buff, YELLOW);
        break;
    case eBLUE:
        strcat(buff, BLUE);
        break;
    case eMAGENTA:
        strcat(buff, MAGENTA);
        break;
    case eCYAN:
        strcat(buff, CYAN);
        break;
    case eWHITE:
        strcat(buff, WHITE);
        break;
    case eBOLDRED:
        strcat(buff, BOLDRED);
        break;
    case eBOLDGREEN:
        strcat(buff, BOLDGREEN);
        break;
    case eBOLDYELLOW:
        strcat(buff, BOLDYELLOW);
        break;
    case eBOLDBLUE:
        strcat(buff, BOLDBLUE);
        break;
    case eBOLDMAGENTA:
        strcat(buff, BOLDMAGENTA);
        break;
    case eBOLDCYAN:
        strcat(buff, BOLDCYAN);
        break;
    case eBOLDWHITE:
        strcat(buff, BOLDWHITE);
        break;
    default:
        break;
    }

    strcat(buff, format);
    strcat(buff, RESET);

    va_start(argList, c);
    test = vprintf(buff, argList);
    va_end(argList);

    free(buff);

    return test;
}


int vprint(char* format, color_t c, va_list argList)
{
    if(format == NULL){return 0;}

    int test;
    char* buff;

    if(!colorful || c == eNORMAL)
    {
        test = vprintf(format, argList);

        return test;
    }

    buff = (char*)calloc(strlen(format) + 30, 1);
    if (buff == NULL) {return -1;}

    switch (c) {
    case eBOLD:
        strcat(buff, BOLD);
        break;
    case eRED:
        strcat(buff, RED);
        break;
    case eGREEN:
        strcat(buff, GREEN);
        break;
    case eYELLOW:
        strcat(buff, YELLOW);
        break;
    case eBLUE:
        strcat(buff, BLUE);
        break;
    case eMAGENTA:
        strcat(buff, MAGENTA);
        break;
    case eCYAN:
        strcat(buff, CYAN);
        break;
    case eWHITE:
        strcat(buff, WHITE);
        break;
    case eBOLDRED:
        strcat(buff, BOLDRED);
        break;
    case eBOLDGREEN:
        strcat(buff, BOLDGREEN);
        break;
    case eBOLDYELLOW:
        strcat(buff, BOLDYELLOW);
        break;
    case eBOLDBLUE:
        strcat(buff, BOLDBLUE);
        break;
    case eBOLDMAGENTA:
        strcat(buff, BOLDMAGENTA);
        break;
    case eBOLDCYAN:
        strcat(buff, BOLDCYAN);
        break;
    case eBOLDWHITE:
        strcat(buff, BOLDWHITE);
        break;
    default:
        break;
    }

    strcat(buff, format);
    strcat(buff, RESET);

    test = vprintf(buff, argList);

    free(buff);

    return test;
}


int threadPrint(int ID, char* format, color_t c, ...)
{
    int test;
    va_list argList;

    if(!colorful)
    {
        if(ID == 0)
            test = printf("Main thread: ");
        else
            test = printf("\tThread %d: ", ID);

        va_start(argList, c);
        test += vprintf(format, argList);
        va_end(argList);

        return test;
    }

    if(ID == 0)
        test = print("Main thread: ", eBOLDMAGENTA);
    else
        test = print("\tThread %d: ", eBOLDMAGENTA, ID);


    if(format == NULL)
        return test;

    va_start(argList, c);
    if(c == eNORMAL)
        test += vprintf(format, argList);
    else
        test += vprint(format, c, argList);
    va_end(argList);


    return test;
}


int vthreadPrint(int ID, char* format, color_t c, va_list argList)
{
    int test;

    if(!colorful)
    {
        if(ID == 0)
            test = printf("Main thread: ");
        else
            test = printf("\tThread %d: ", ID);

        test += vprintf(format, argList);

        return test;
    }

    if(ID == 0)
        test = print("Main thread: ", eBOLDMAGENTA);
    else
        test = print("\tThread %d: ", eBOLDMAGENTA, ID);

    if(format == NULL)
        return test;

    if(c == eNORMAL)
    {
        test += vprintf(format, argList);

        return test;
    }

    test += vprint(format, c, argList);

    return test;
}

/*----------------------------------------------------------------------*/
