#ifndef LIBRERIE_H
#define LIBRERIE_H

#include <errno.h>
#include <getopt.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <pthread.h>
#include <semaphore.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sysexits.h>


typedef enum {New, Ready, Running, Blocked, Exit} faseStato;

typedef struct istruzione
{
	bool typeFlag;
	int length;
	int ioMax;
}istruzione;

typedef struct state
{
	faseStato fase;
	int numeroCore;
	int clockBlock;
}state;

typedef struct job
{
	int id;
	int arrivalTime;
	long int listaInstr;
	state statoJob;

}job;

typedef struct log
{
	faseStato fase;
	int idCore;
}log;

typedef struct infoJob
{
	job Job;
	int totInstr;
	int currentInstr;
	int lengthTot;
}infoJob;

#endif
