#include "scheduler.h"

void stampaLog(log coreLog, bool qualeFile)//qualeFile = 0 è Preemptive, 1 NPreemptive.
{		
	long int clockVar;
	int id = coreLog.idCore;
		
	if(id == 0) {
		clockVar = clock_0;
	} else if(id == 1) {
		clockVar = clock_I;
	}
	char* fase_S;

	switch(coreLog.fase) {
		case New :
			printf("\nErrore in stampaLog. Fase: New.\n");
			break;
			
		case Ready :
			fase_S = "Ready";
			break;
			
		case Running :
			fase_S = "Running";
			break;
			
		case Blocked :
			fase_S = "Blocked";
			break;
			
		case Exit :
			fase_S = "Exit";
			break;
			
		default:
			printf("\nErrore in stampaLog.\n");
			break;
	}
	
	if(qualeFile == 0) {
		fprintf(filePre, "\ncore%d, %ld, %d, %s.\n", id, clockVar, copiaJobs[currentJob].Job.id, fase_S);//core, clockVar, idJob, faseStato.
	} else
		fprintf(fileNotPre, "\ncore%d, %ld, %d, %s.\n", id, clockVar, copiaJobs[currentJob].Job.id, fase_S);
}

void clockPlus(int id, bool qualeFile)
{
	switch(id) {
		case 0:
			clock_0++;
			
			for(int i = numExit; i < totJobs; i++) {
				if(totBlock_0 == 0) break;
				
				if((copiaJobs[i].Job.statoJob.fase == Blocked) && (copiaJobs[i].Job.statoJob.numeroCore == 0)) {	
					copiaJobs[i].Job.statoJob.clockBlock--;
					 
					if(copiaJobs[i].Job.statoJob.clockBlock == 0) {
						copiaJobs[i].Job.statoJob.fase = Ready; //qua c'è un [currentStato] che potrebbe dare noie.
						
						totBlock_0--;

						if(qualeFile == 0) {
							fprintf(filePre, "\ncore%d, %ld, %d, Ready.", id, clock_0, copiaJobs[i].Job.id);
						} else
							fprintf(fileNotPre, "\ncore%d, %ld, %d, Ready.", id, clock_0, copiaJobs[i].Job.id);
					}
				}
			}
			break;
		
		case 1:
			clock_I++;
			
			for(int i = numExit; i < totJobs; i++)
			{
				if(totBlock_I == 0)
				{
					break;
				}
		
				if((copiaJobs[i].Job.statoJob.fase == Blocked) && (copiaJobs[i].Job.statoJob.numeroCore == 1))
				{
					copiaJobs[i].Job.statoJob.clockBlock--;
						 
					if(copiaJobs[i].Job.statoJob.clockBlock == 0)
					{
						copiaJobs[i].Job.statoJob.fase = Ready; //qua c'è un [currentStato] che potrebbe dare noie.
						
						totBlock_I--;
						
						if(qualeFile == 0)
						{
							fprintf(filePre, "\ncore%d, %ld, %d, Ready.", id, clock_I, copiaJobs[i].Job.id);
						}
						else
							fprintf(fileNotPre, "\ncore%d, %ld, %d, Ready.", id, clock_I, copiaJobs[i].Job.id);
					}
				}
			}
			break;
			
		default:
			abort();
	}
}

int checkException_LastJob(int qualeCore)
{	
	if(qualeCore == 0)
	{
		if((jobsLeft - totBlock_I) == 0)
		{
			if(jobsLeft != 0)
			{
				return 1;
			}
		}
	}
	
	else
		if(qualeCore == 1)
		{
			if((jobsLeft - totBlock_0) == 0)
			{
				if(jobsLeft != 0)
				{
					return 1;
				}
			}
		}
		
	return 0;
}

void solveException_AllBlockedJobs(int check_0, void* argc)
{
	check_0 = checkException_LastJob(coreLog.idCore);
		
	while(check_0 != 0)
	{	
		printf("\nSono nel solveException.\n");
		if(check_0 == 1)//manda in sleep il primo core, che non ha più job Ready, per lasciare al secondo il lock.
		{
			ctrl = pthread_mutex_unlock(&jobMutex);
			if(ctrl != 0)
			{
				printf( "\nMutex unlock failed: %d - Quit\n", ctrl); 
				exit(-1);
			}
					
			usleep(100000);//0.1 secondi. (Esiste un modo migliore?)
					
			ctrl = pthread_mutex_lock(&jobMutex);
	
			if(ctrl != 0)
			{
				printf( "\nMutex lock failed: %d - Quit\n", ctrl); 
				exit(-1);
			}
		}
				
		coreLog.idCore = *((int*) argc);
		check_0 = checkException_LastJob(coreLog.idCore);
	}	
}

int trovaJob_Pre(int id)
{
	switch(id)
	{
		case 0:
			while(1)
			{
				if(copiaJobs[currentJob].Job.id == 0)
				{
					while(copiaJobs[currentJob].Job.arrivalTime > clock_0)
					{
						clockPlus(id, 0);
					}
					
					if((copiaJobs[currentJob].Job.statoJob.fase == New) || (copiaJobs[currentJob].Job.statoJob.fase == Ready))
					{
						return currentJob;
					}
				}
				
				if(((copiaJobs[currentJob].Job.statoJob.fase == New) || (copiaJobs[currentJob].Job.statoJob.fase == Ready)) && (copiaJobs[currentJob].Job.arrivalTime <= clock_0))
				{
					return currentJob;
				}
				
				for(int i = numExit; i < currentJob; i++)
				{ 
					if(((copiaJobs[i].Job.statoJob.fase == New) || (copiaJobs[i].Job.statoJob.fase == Ready)) && (copiaJobs[i].Job.arrivalTime <= clock_0))
					{
						return i;
					}
					
				}
					
				if(clock_0 <= 5000)
				{
					if(currentJob < 2047)
					{
						currentJob++;
					}
				}
				
				if(clock_0 > 5000)
				{
					for(int i = (currentJob + 1); i < totJobs; i++)
					{ 
						if(((copiaJobs[i].Job.statoJob.fase == New) || (copiaJobs[i].Job.statoJob.fase == Ready)) && (copiaJobs[i].Job.arrivalTime <= clock_0))
						{
							return i;
						}
					}
				}
				
				clockPlus(id, 0);
			}
			break;
		
		case 1:
			while(1)
			{	
				if(copiaJobs[currentJob].Job.id == 0)
				{
					while(copiaJobs[currentJob].Job.arrivalTime > clock_I)
					{
						clockPlus(id, 0);
					}
					
					if((copiaJobs[currentJob].Job.statoJob.fase == New) || (copiaJobs[currentJob].Job.statoJob.fase == Ready))
					{
						return currentJob;
					}
				}
				
				if(((copiaJobs[currentJob].Job.statoJob.fase == New) || (copiaJobs[currentJob].Job.statoJob.fase == Ready)) && (copiaJobs[currentJob].Job.arrivalTime <= clock_I))
				{
					return currentJob;
				}
				
				for(int i = numExit; i < currentJob; i++)
				{ 
					if(((copiaJobs[i].Job.statoJob.fase == New) || (copiaJobs[i].Job.statoJob.fase == Ready)) && (copiaJobs[i].Job.arrivalTime <= clock_I))
					{
						return i;
					}
					
				}
				
				if(clock_I <= 5000)
				{
					if(currentJob < 2047)
					{
						currentJob++;
					}
				}
				
				if(clock_I > 5000)
				{
					for(int i = (currentJob + 1); i < totJobs; i++)
					{ 
						if(((copiaJobs[i].Job.statoJob.fase == New) || (copiaJobs[i].Job.statoJob.fase == Ready)) && (copiaJobs[i].Job.arrivalTime <= clock_I))
						{
							return i;
						}
					}
				}
				
				clockPlus(id, 0);
			}
			break;
		
		default:
			perror("\nErrore nella ricerca di un nuovo job per lo schedulerPre.\n");
			abort();
			break;
	}
}

void* schedulerPre(void* argc)
{ 
	do
	{
		ctrl = pthread_mutex_lock(&jobMutex);
		
		if(ctrl != 0)
		{
			printf( "\nMutex lock failed: %d - Quit\n", ctrl); 
			exit(-1);
		}

		coreLog.idCore = *((int*) argc);
		
		if(jobsLeft <= 50)
		{
			solveException_AllBlockedJobs(checkExc, argc);
		}
		
		if(jobsLeft == 0)
		{
			if(checkReturn == 0)
			{
				checkReturn++;
				printf("\nSchedulerPre ha raggiunto 'return 0', con %d job da eseguire.\n", jobsLeft);
			}
			return 0;
		}
			
		currentJob = trovaJob_Pre(coreLog.idCore);
		
		while(quantumCopia != 0)
		{	
			switch(copiaJobs[currentJob].Job.statoJob.fase)
			{
				case New:
					copiaJobs[currentJob].Job.statoJob.fase = Ready;
					
					clockPlus(coreLog.idCore, 0);
					
					coreLog.fase = Ready;//cambia da New a Ready.
					stampaLog(coreLog, 0);		
		
					quantumCopia = quantum;//impedisce l'uscita dal while.							
					break;
			
				case Ready:
					if(jobsLeft == 1)
					{ 
						lastJob = currentJob;
					}

					copiaJobs[currentJob].Job.statoJob.fase = Running;//cambia da Ready a Running	
					clockPlus(coreLog.idCore, 0);			

					coreLog.fase = Running;
					stampaLog(coreLog, 0); 
							
					quantumCopia = quantum;//impedisce l'uscita dal while.
					break;

				case Running:
					currentInstr_Global = copiaJobs[currentJob].Job.listaInstr + copiaJobs[currentJob].currentInstr;
					if(copiaInstr[currentInstr_Global].length < 0)
					{
						printf("\njob = %d, instr = %ld, ", currentJob, currentInstr_Global);
						printf("instrTot = %d, lengthError = %d.\n",  copiaJobs[currentJob].totInstr, copiaInstr[currentInstr_Global].length);		
						exit(-1);
					}

					if(copiaInstr[currentInstr_Global].typeFlag == 1)//Instruction bloccante.
					{
				
						if(coreLog.idCore == 0)
						{
							totBlock_0++;
						}
			
						if(coreLog.idCore == 1)
						{
							totBlock_I++;
						}

						srand(time(NULL));								
						
						copiaJobs[currentJob].Job.statoJob.fase = Blocked;
						copiaInstr[currentInstr_Global].typeFlag = 0;//cambio tipo all'istruzione.
							
								
						currentBlock = (rand() % copiaInstr[currentInstr_Global].ioMax) + 1;//calcolo il numero di cicli di blocco.
										
						copiaJobs[currentJob].Job.statoJob.numeroCore = coreLog.idCore;
						copiaJobs[currentJob].Job.statoJob.clockBlock = currentBlock;
									
						clockPlus(coreLog.idCore, 0);				
									
						coreLog.fase = Blocked;//cambia da Running a Blocked.		
						stampaLog(coreLog, 0);

						if(jobsLeft == 1) 
						{
							currentJob = lastJob;
							while(copiaJobs[currentJob].Job.statoJob.fase != Ready)
							{
								clockPlus(coreLog.idCore, 0);//clocca finchè non si sblocca.
							}
						}
						
						if(currentJob < 2047)
						{
							currentJob++;
						}
						
						quantumCopia = 0;//esce dal while.
														
						break;
					}	
						
					copiaInstr[currentInstr_Global].length--;
									
					if(copiaInstr[currentInstr_Global].length == 0)
					{
						copiaJobs[currentJob].currentInstr++;
					}
									
					if(copiaJobs[currentJob].currentInstr == copiaJobs[currentJob].totInstr)//il job è terminato, va in Exit.
					{												
						copiaJobs[currentJob].Job.statoJob.fase = Exit;
							
						clockPlus(coreLog.idCore, 0);
		
						coreLog.fase = Exit;//cambia da Running a Exit.		
						stampaLog(coreLog, 0);

						jobsLeft--;
						
						swapExit = copiaJobs[currentJob];//riduce il numero di job da confrontare nella trovaJob.
						copiaJobs[currentJob] = copiaJobs[numExit];
						copiaJobs[numExit] = swapExit;
						numExit++;
						
						if(currentJob < 2047)
						{
							currentJob++;
						}
									
						quantumCopia = 0;

						break;

					}
								
					quantumCopia--; //decrementa il quanto di tempo

					if(quantumCopia == 0 && (copiaJobs[currentJob].Job.statoJob.fase == Running))//Timeout.
					{
						copiaJobs[currentJob].Job.statoJob.fase = Ready;
						
						clockPlus(coreLog.idCore, 0);
						
						coreLog.fase = Ready;
						
						stampaLog(coreLog, 0);
						
						if(jobsLeft == 1)
						{
							currentJob = lastJob;
							break;
						}
						
						if(currentJob < 2047)
						{
							currentJob++;
						}
			
						break;
					}
							
					clockPlus(coreLog.idCore, 0);
					break;

				case Blocked:
					printf("\nSono in un job Blocked: ERRORE.\n");
					exit(-1);
					break;
					
				case Exit:
					printf("\nSono in un job Exit: ERRORE.\n");
					exit(-1);
					break;
					
				default :
					perror("\nErrore dello switch schedulerPre.\n");
					abort();
					break;	
			}
		}
		
		quantumCopia = quantum;
				
		ctrl = pthread_mutex_unlock(&jobMutex);
		
		if(ctrl != 0)
		{
			printf( "\nMutex unlock failed: %d - Quit\n", ctrl); 
			exit(-1);
		}
		
	}while(jobsLeft != 0);
	
	ctrl = pthread_mutex_lock(&jobMutex);
		
	if(ctrl != 0)
	{
		printf( "\nMutex lock failed: %d - Quit\n", ctrl); 
		exit(-1);
	}
	
	if(checkReturn == 0)
	{
		checkReturn++;
		printf("\nSchedulerPre ha raggiunto 'return 0', con %d job da eseguire.\n", jobsLeft);
	}
	
	ctrl = pthread_mutex_unlock(&jobMutex);
		
	if(ctrl != 0)
	{
		printf( "\nMutex unlock failed: %d - Quit\n", ctrl); 
		exit(-1);
	}
		
	return 0;
}

int trovaJob_NotPre(int id)
{
	int posizione;
	int lengthMin;
	bool flag;
		
	switch(id)
	{
		case 0:
			do 
			{
				for(int i = numExit;((copiaJobs[i].Job.arrivalTime <= clock_0) && (i < totJobs)); i++)
				{	
					flag = 1;

					if(copiaJobs[i].Job.statoJob.fase == New)
					{
						posizione = i;
						lengthMin = copiaJobs[i].lengthTot;
						flag = 0;
					}

					if(copiaJobs[i].Job.statoJob.fase == Ready)
					{
						posizione = i;
						lengthMin = copiaJobs[i].lengthTot;
						flag = 0;
					}

					if(flag == 0)
					{
						for(int j = i + 1; ((copiaJobs[j].Job.arrivalTime <= clock_0) && (j < totJobs)); j++)
						{
							if((copiaJobs[j].Job.statoJob.fase == New) && (copiaJobs[j].lengthTot < lengthMin))
							{
								posizione = j;
								lengthMin = copiaJobs[j].lengthTot;
							}
			
							if((copiaJobs[j].Job.statoJob.fase == Ready) && (copiaJobs[j].lengthTot < lengthMin))
							{
								posizione = j;
								lengthMin = copiaJobs[j].lengthTot;
							}
						}
						
						return posizione;
					}
				}
			
				clockPlus(id, 1);//clocca finchè non soddisfa il 'for'.

			}while(1);
			break;
				
		case 1:
			do
			{
				for(int i = numExit;((copiaJobs[i].Job.arrivalTime <= clock_I) && (i < totJobs)); i++)
				{	
					flag = 1;

					if(copiaJobs[i].Job.statoJob.fase == New)
					{
						posizione = i;
						lengthMin = copiaJobs[i].lengthTot;
						flag = 0;
					}

					if(copiaJobs[i].Job.statoJob.fase == Ready)
					{
						posizione = i;
						lengthMin = copiaJobs[i].lengthTot;
						flag = 0;
					}

					if(flag == 0)
					{
						for(int j = i + 1; ((copiaJobs[j].Job.arrivalTime <= clock_I) && (j < totJobs)); j++)
						{
							if((copiaJobs[j].Job.statoJob.fase == New) && (copiaJobs[j].lengthTot < lengthMin))
							{
								posizione = j;
								lengthMin = copiaJobs[j].lengthTot;
							}
			
							if((copiaJobs[j].Job.statoJob.fase == Ready) && (copiaJobs[j].lengthTot < lengthMin))
							{
								posizione = j;
								lengthMin = copiaJobs[j].lengthTot;
							}
						}
						
						return posizione;
					}
				}
			
				clockPlus(id, 1);//clocca finchè non soddisfa il 'for'.
				
			}while(1);
			break;
			
		default:
			perror("\nErrore nella ricerca di un nuovo job per lo schedulerNotPre.\n");
			abort();
			break;
	}
}

void* schedulerNotPre(void* argc)
{	
	do
	{
		ctrl = pthread_mutex_lock(&jobMutex);
		if(ctrl != 0)
		{
			printf( "\nMutex lock failed: %d - Quit\n", ctrl); 
			exit(-1);
		}
			
		coreLog.idCore = *((int*) argc);
			
		if(jobsLeft <= 50)
		{
			solveException_AllBlockedJobs(checkExc, argc);
		}
		
		if(jobsLeft == 0)
		{
			if(checkReturn == 0)
			{
				checkReturn++;
				printf("\nSchedulerNotPre ha raggiunto 'return 0', con %d job da eseguire.\n", jobsLeft);
			}
			return 0;
		}

		currentJob = trovaJob_NotPre(coreLog.idCore);
		
		while(uscitaRunning != 0)
		{
			switch(copiaJobs[currentJob].Job.statoJob.fase)
			{
				case New:
					copiaJobs[currentJob].Job.statoJob.fase = Ready;
	
					clockPlus(coreLog.idCore, 1);
	
					coreLog.fase = Ready; 		
					stampaLog(coreLog, 1);			
					break;

				case Ready:
					if(jobsLeft == 1)
					{ 
						lastJob = currentJob;
					}
					copiaJobs[currentJob].Job.statoJob.fase = Running;	
											
					clockPlus(coreLog.idCore, 1);		

					coreLog.fase = Running; 		
					stampaLog(coreLog, 1);	
					break;

				case Running:
					currentInstr_Global = copiaJobs[currentJob].Job.listaInstr + copiaJobs[currentJob].currentInstr;
					
					if(copiaInstr[currentInstr_Global].length < 0)
					{
						printf("\njob = %d, instr = %ld, ", currentJob, currentInstr_Global);
						printf("instrTot = %d, lengthError = %d.\n",  copiaJobs[currentJob].totInstr, copiaInstr[currentInstr_Global].length);		
						exit(-1);
					}

					if(copiaInstr[currentInstr_Global].typeFlag == 1)//Instruction bloccante.
					{
						if(coreLog.idCore == 0)
						{
							totBlock_0++;
						}
						
						if(coreLog.idCore == 1)
						{
							totBlock_I++;
						}

						srand(time(NULL));								
			
						copiaJobs[currentJob].Job.statoJob.fase = Blocked;
						copiaInstr[currentInstr_Global].typeFlag = 0;//cambio tipo all'istruzione.
							
								
						currentBlock = (rand() % copiaInstr[currentInstr_Global].ioMax) + 1;//calcolo il numero di cicli di blocco.
									
						copiaJobs[currentJob].Job.statoJob.numeroCore = coreLog.idCore;
						copiaJobs[currentJob].Job.statoJob.clockBlock = currentBlock;
									
						clockPlus(coreLog.idCore, 1);				
									
						coreLog.fase = Blocked;//cambia da Running a Blocked.		
						stampaLog(coreLog, 1);

						if(jobsLeft == 1) 
						{
							currentJob = lastJob;
							while(copiaJobs[currentJob].Job.statoJob.fase != Ready)
							{
								clockPlus(coreLog.idCore, 1);//clocca finchè non si sblocca.
							}
						}
						uscitaRunning = 0;//esce dal while.								
						break;
					}	
				
					copiaJobs[currentJob].lengthTot--; //serve per cercare il corretto SJN.
					copiaInstr[currentInstr_Global].length--;
					if(copiaInstr[currentInstr_Global].length == 0)
					{
						copiaJobs[currentJob].currentInstr++;
					}
					
					if(copiaJobs[currentJob].currentInstr == copiaJobs[currentJob].totInstr)//il job è terminato, va in Exit.
					{	
						copiaJobs[currentJob].Job.statoJob.fase = Exit;
							
						clockPlus(coreLog.idCore, 1);
		
						coreLog.fase = Exit;//cambia da Running a Exit.		
						stampaLog(coreLog, 1);

						jobsLeft--;
						
						swapExit = copiaJobs[currentJob];//riduce il numero di job da confrontare nella trovaJob.
						copiaJobs[currentJob] = copiaJobs[numExit];
						copiaJobs[numExit] = swapExit;
						numExit++;
									
						uscitaRunning = 0;

						break;
					}
					clockPlus(coreLog.idCore, 1);
					break;

				case Blocked:
					printf("\nNotPre. Sono in un job Blocked: ERRORE.\n");
					exit(-1);
					break;
					
				case Exit:
					printf("\nNotPre. Sono in un job Exit: ERRORE.\n");
					exit(-1);
					break;
					
				default :
					perror("\nErrore dello switch schedulerNotPre.\n");
					abort();
					break;	
			}
		}
	
		uscitaRunning = 3;
		
		ctrl = pthread_mutex_unlock(&jobMutex);
		
		if(ctrl != 0)
		{
			printf( "\nMutex unlock failed: %d - Quit\n", ctrl); 
			exit(-1);
		}
		
	}while(jobsLeft != 0);
	
	ctrl = pthread_mutex_lock(&jobMutex);
		
	if(ctrl != 0)
	{
		printf( "\nMutex lock failed: %d - Quit\n", ctrl); 
		exit(-1);
	}
	
	if(checkReturn == 0)
	{
		checkReturn++;
		printf("\nSchedulerNotPre ha raggiunto 'return 0', con %d job da eseguire.\n", jobsLeft);
	}
	
	ctrl = pthread_mutex_unlock(&jobMutex);
		
	if(ctrl != 0)
	{
		printf( "\nMutex unlock failed: %d - Quit\n", ctrl); 
		exit(-1);
	}

	return 0;
}
