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

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include "utils.h"
#include "settings.h"
#include "errors.h"
#include "connections.h"

/**
 * @file connections.c
 *
 * @author Alessandro Meschi
 *
 * @date 7 Maggio 2019
 *
 * @brief Contiene la definizione delle funzioni che implementano il protocollo
 * tra i clients ed il server
 */

/*--------------------------CLIENT SIDE----------------------------------*/
int openConnection(char* path, unsigned int ntimes, unsigned int secs)
{
    if(path == NULL || ntimes <= 0 || secs <= 0) {errno = EINVAL; return -1;}

    int test;
    int sockFD;
    struct sockaddr_un address;

    memset(&address, 0, sizeof(struct sockaddr_un));
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, path);

    sockFD = socket(AF_UNIX, SOCK_STREAM, 0);
    if( sockFD == -1 ){return -1;}

    for(unsigned int i=0; i<ntimes; i++)
    {
        test = connect(sockFD, (struct sockaddr*)&address, sizeof(address));
        if(test == -1)
            sleep(secs);
        else
            break;
    }

    if(test == -1)
    {
        errno = ENOTCONN;
        return -1;
    }

    return sockFD;
}

int readHeader(long fd, message_hdr_t *hdr)
{
    if(hdr == NULL || fd < 0) {errno = EINVAL; return -1;}

    int test;
    struct iovec iov;

    iov.iov_base = hdr;
    iov.iov_len = sizeof(message_hdr_t);

    test = readv(fd, &iov, 1);
    if(test == -1)
        return test;
    if(test == 0)
    {
        errno = ECONNRESET;
        return -1;
    }

    return test;
}

int readData(long fd, message_data_t *data)
{
    if(data == NULL || fd < 0) {errno = EINVAL; return -1;}

    int test;
    char* ph;
    unsigned int bytesRead;
    unsigned int bufRead = 0;
    struct iovec iov;

    iov.iov_base = &(data->hdr);
    iov.iov_len = sizeof(message_data_hdr_t);

    test = readv(fd, &iov, 1);
    if(test == -1)
        return test;
    if(test == 0)
    {
        errno = ECONNRESET;
        return -1;
    }

    bytesRead = test;

    if(data->hdr.len != 0)
    {
        data->buf = malloc(data->hdr.len);
        if(data->buf == NULL){return -1;}

        ph = data->buf;

        while (bufRead < data->hdr.len)
        {
            test = read(fd, ph, data->hdr.len - bufRead);
            if(test == -1)
                return test;
            if(test == 0)
            {
                errno = ECONNRESET;
                return -1;
            }

            bufRead += test;
            ph += test;
        }

        bytesRead += bufRead;
    }

    return bytesRead;
}

int readMsg(long fd, message_t *msg)
{
    if(msg == NULL || fd < 0) {errno = EINVAL; return -1;}

    int test;

    test = readHeader(fd, &(msg->hdr));
    if(test == -1){return -1;}

    test += readData(fd, &(msg->data));
    return test;
}


int sendData(long fd, message_data_t *msg)
{
    if(msg == NULL || fd < 0) {errno = EINVAL; return -1;}

    int test;
    char* ph;
    unsigned int written;
    unsigned int bufWritten = 0;
    struct iovec iov;

    iov.iov_base = &(msg->hdr);
    iov.iov_len = sizeof(message_data_hdr_t);

    test = writev(fd, &(iov), 1);
    if(test == -1)
        return test;
    if(test == 0)
    {
        errno = ECONNRESET;
        return -1;
    }

    written = test;

    if(msg->hdr.len != 0)
    {
        ph = msg->buf;

        while (bufWritten < msg->hdr.len)
        {
            test = write(fd, ph, msg->hdr.len - bufWritten);
            if(test == -1)
                return test;
            if(test == 0)
            {
                errno = ECONNRESET;
                return -1;
            }

            bufWritten += test;
            ph += test;
        }

        written += bufWritten;
    }

    return written;
}


int sendRequest(long fd, message_t* msg)
{
    if(msg == NULL || fd < 0) {errno = EINVAL; return -1;}

    int test;
    int written;
    struct iovec iov;

    iov.iov_base = &(msg->hdr);
    iov.iov_len = sizeof(message_hdr_t);

    test = writev(fd, &iov, 1);
    if(test == -1)
        return test;
    if(test == 0)
    {
        errno = ECONNRESET;
        return -1;
    }

    written = test;

    test = sendData(fd, &(msg->data));
    if(test == -1)
        return -1;

    written += test;

    return written;
}
/*-----------------------------------------------------------------------*/
