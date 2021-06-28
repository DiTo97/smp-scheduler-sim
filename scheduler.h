#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "funzioni.h"

int totBlock_0, totBlock_I, currentJob, jobsLeft, lastJob, currentBlock;
int ctrl, quantumCopia, checkExc, checkReturn, uscitaRunning, numExit;
long int clock_0, clock_I, currentInstr_Global;
infoJob swapExit;
pthread_t core_0, core_I;
pthread_mutex_t jobMutex;
log coreLog;


void stampaLog(log coreLog, bool qualeFile);

void clockPlus(int id, bool qualeFile);

int checkException_LastJob(int qualeCore);

void solveException_AllBlockedJobs(int check_0, void* argc);

int trovaJob_Pre(int id);

void* schedulerPre(void* argc);

int trovaJob_NotPre(int id);

void* schedulerNotPre(void* argc);

#endif
