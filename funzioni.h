#ifndef FUNZIONI_H
#define FUNZIONI_H

#include "librerie.h"
#include "scheduler.h"

FILE *filePre, *fileNotPre, *fileInput;
const char *nomeProgramma, *inputFilename, *cntrlFile, *cntrlFile_II;
int totJobs, quantum;
long int totInstr_Global;
infoJob *sJobs, *copiaJobs;
istruzione *sInstr, *copiaInstr;


void stampaMexAiuto(FILE* stream, int exitCode);

void inserimentoParametri(int argc, char** argv);

void numJobsInstr(FILE* stream);

void inputJobsInstr(FILE* stream, infoJob* datiJobs, istruzione* datiInstr);

void initVariabili();

void copiaData();

void erroreFork();

void erroreThread(int err, int numCore, bool qualeScheduler);

void messaggioProcessi(pid_t pid, int status);

void deallMemDinamica(infoJob* datiJobs, istruzione* datiInstr);//da mettere nei due processi scheduler.

#endif
