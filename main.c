#include "scheduler.h"

int main(int argc, char** argv)
{	
	inserimentoParametri(argc, argv);
	
	numJobsInstr(fileInput);
	printf("\ntotJobs = %d, totInstr = %ld.\n", totJobs, totInstr_Global);
	
	initVariabili();
	
	//creo i semafori, in maniera tale da far accedere solo il Master.
	
	pid_t pidMaster; 
	pid_t pidPreemptive;	
	pid_t pidNotPreemptive;
	
	int statusProcessi;

	key_t chiaveJobs = 9432;
	key_t chiaveInstr = 5678; 

	int shmSize_Jobs = totJobs*sizeof(infoJob);
				
	long int shmSize_Instr = totInstr_Global*sizeof(istruzione);

	pidMaster = fork();
	
	if(pidMaster == -1)
		erroreFork();
		
	if(pidMaster == 0)//Master.
	{	
		int shmID_Jobs;
	
		int shmID_Instr; 
				
		printf("\nMaster: creazione memoria condivisa.\n");
				
		if((shmID_Jobs = shmget(chiaveJobs, shmSize_Jobs, IPC_CREAT | 0666)) < 0)
		{ 
			fprintf(stderr, "\nErrore nella 'shmget' dei job: %s.\n", strerror(errno));
			exit(-1);
		}

		sJobs = shmat(shmID_Jobs, 0, 0);//SHM_RDONLY nel terzo parametro, per gli scheduler.
	
		if(sJobs == (infoJob*)(-1))
		{
			fprintf(stderr, "\nErrore nella 'shmat' dei job: %s.\n", strerror(errno));
			exit(-1);
		}
				
		if((shmID_Instr = shmget(chiaveInstr, shmSize_Instr, IPC_CREAT | 0666)) < 0)
		{
			fprintf(stderr, "\nErrore nella 'shmget' delle istruzioni: %s.\n", strerror(errno));
			exit(-1);
		}

		sInstr = shmat(shmID_Instr, 0, 0);
	
		if(sInstr == (istruzione*)(-1))
		{
			fprintf(stderr, "\nErrore nella 'shmat' delle istruzioni: %s.\n", strerror(errno));
			exit(-1);
		}
		
		inputJobsInstr(fileInput, sJobs, sInstr);
		
		//faccio una sem_wait per bloccare il Master, e sbloccare gli scheduler.
		
		while(1)
		{
			if((sJobs[totJobs - 1].Job.id == 666) && (sJobs[totJobs - 2].Job.id == 555))
			{
				break;
			}
			
			sleep(1);			
		}
			
		shmctl(shmID_Instr, IPC_RMID, 0);
		shmctl(shmID_Jobs, IPC_RMID, 0); 
		
		printf("\nMaster: memoria condivisa deallocata.\n");
		
		exit(0);
	}
	
	if(pidMaster > 0)//main che continua.
	{
		printf("\nmain: fork del Master.\nPID: %d.\n", pidMaster);

		fflush(stdout);

		pidPreemptive = fork();
		
		if(pidPreemptive == -1)
			erroreFork();
	}
	
	if(pidPreemptive == 0)//schedulerPre.
	{
		printf("\n\nPreemptive: attendo che il Master crei la memoria condivisa.\n");
		
		sleep(1);
		
		int shmID_Jobs;
	
		int shmID_Instr; 
				
		if((shmID_Jobs = shmget(chiaveJobs, shmSize_Jobs, 0666)) < 0)
		{ 
			fprintf(stderr, "\nErrore nella 'shmget' dei job nel Preemptive: %s.\n", strerror(errno));
			exit(-1);
		}

		sJobs = shmat(shmID_Jobs, 0, 0);//SHM_RDONLY nel terzo parametro, per gli scheduler.
	
		if(sJobs == (infoJob*)(-1))
		{
			fprintf(stderr, "\nErrore nella 'shmat' dei job nel Preemptive: %s.\n", strerror(errno));
			exit(-1);
		}
				
		if((shmID_Instr = shmget(chiaveInstr, shmSize_Instr, 0666)) < 0)
		{
			fprintf(stderr, "\nErrore nella 'shmget' delle istruzioni nel Preemptive: %s.\n", strerror(errno));
			exit(-1);
		}

		sInstr = shmat(shmID_Instr, 0, SHM_RDONLY);
	
		if(sInstr == (istruzione*)(-1))
		{
			fprintf(stderr, "\nErrore nella 'shmat' delle istruzioni nel Preemptive: %s.\n", strerror(errno));
			exit(-1);
		}

		do{ 
			if(sJobs[totJobs - 1].Job.id == (totJobs - 1))//attende il caricamento della memoria.
			{
				printf("\n\nPreemptive: caricamento memoria condivisa completo.\n");
				break;
			}

		}while(1);
		
		copiaData();
		
		while(1)
		{ 
			if(copiaJobs[totJobs - 1].Job.id == (totJobs - 1))//attende la copiatura della memoria.
			{
				printf("\n\nPreemptive: copiatura memoria condivisa completo. Inizio simulazione.\n");
				break;
			}
		}

		int schedParametro_0 = 0;
		int schedParametro_I = 1;

		int err;

		pthread_mutex_init(&jobMutex, NULL);
		quantumCopia = quantum;
		checkReturn = 0;
		numExit = 0;
		
		printf("\nPreemptive: avviamento thread_%d.\n", schedParametro_0);
		
		err = pthread_create(&(core_0), NULL, schedulerPre, (void*)&(schedParametro_0));
		erroreThread(err, schedParametro_0, 0);
		
		fflush(stdout);

		printf("\nPreemptive: avviamento thread_%d.\n", schedParametro_I);
		
		err = pthread_create(&(core_I), NULL, schedulerPre, (void*)&(schedParametro_I));
		erroreThread(err, schedParametro_I, 0);
		
		fflush(stdout);
						
		pthread_join(core_0, NULL);
		printf("\nPreemptive: thread_0 terminato.\n");
		
		pthread_join(core_I, NULL);		
		printf("\nPreemptive: thread_I terminato.\n");

		fclose(filePre);
		
		pthread_mutex_destroy(&jobMutex);

		//faccio una sem_post, per dare il segnale al Master di aver finito.
		
		deallMemDinamica(copiaJobs, copiaInstr);	
	
		sJobs[totJobs - 1].Job.id = 666;
	
	   	exit(0);
	}
	
	if(pidPreemptive > 0)//main che continua.
	{
		printf("\n\nmain: fork del Preemptive.\nPID: %d.\n", pidPreemptive);

		fflush(stdout);

		pidNotPreemptive = fork();
		
		if(pidNotPreemptive == -1)
			erroreFork();
	}
	
	if(pidNotPreemptive == 0)//schedulerNotPre.
	{	
		printf("\n\nNotPreemptive: attendo che il Master crei la memoria condivisa.\n");
		
		sleep(1);
		
		int shmID_Jobs;
	
		int shmID_Instr; 
				
		if((shmID_Jobs = shmget(chiaveJobs, shmSize_Jobs, 0666)) < 0)
		{ 
			perror("\nErrore nella 'shmget' dei job nel NotPreemptive.\n");
			exit(1);
		}

		sJobs = shmat(shmID_Jobs, 0, 0);//SHM_RDONLY nel terzo parametro, per gli scheduler.
	
		if(sJobs == (infoJob*)(-1))
		{
			perror("\nErrore nella 'shmat' dei job nel NotPreemptive.\n");
		}
				
		if((shmID_Instr = shmget(chiaveInstr, shmSize_Instr, 0666)) < 0)
		{
			perror("\nErrore nella 'shmget' delle istruzioni nel NotPreemptive.\n");
			exit(1);
		}

		sInstr = shmat(shmID_Instr, 0, SHM_RDONLY);
	
		if(sInstr == (istruzione*)(-1))
		{
			perror("\nErrore nella 'shmat' dei job nel NotPreemptive.\n");
		}

		do{ 
			if(sJobs[totJobs - 1].Job.id == (totJobs - 1))//attende il caricamento della memoria.
			{
				printf("\n\nNotPreemptive: caricamento memoria condivisa completo.\n");
				break;
			}

		}while(1);
		
		copiaData();
		
		while(1)
		{ 
			if(copiaJobs[totJobs - 1].Job.id == (totJobs - 1))//attende la copiatura della memoria.
			{
				printf("\n\nNotPreemptive: copiatura memoria condivisa completo. Inizio simulazione.\n");
				break;
			}
		}

		int schedParametro_0 = 0;
		int schedParametro_I = 1;

		int err;

		pthread_mutex_init(&jobMutex, NULL);
		uscitaRunning = 3;
		checkReturn = 0;
		numExit = 0;	
		
		printf("\nNotPreemptive: thread_%d avviato.\n", schedParametro_0);
		
		err = pthread_create(&(core_0), NULL, schedulerNotPre, (void*)&(schedParametro_0));
		erroreThread(err, schedParametro_0, 1);
		
		fflush(stdout);

		printf("\nNotPreemptive: thread_%d avviato.\n", schedParametro_I);
		
		//fflush(stdout);
		
		err = pthread_create(&(core_I), NULL, schedulerNotPre, (void*)&(schedParametro_I));
		erroreThread(err, schedParametro_I, 1);
		
		fflush(stdout);
						
		pthread_join(core_0, NULL);
		printf("\nNotPreemptive: thread_0 terminato.\n");
		
		pthread_join(core_I, NULL);		
		printf("\nNotPreemptive: thread_I terminato.\n");
		
		fclose(fileNotPre);

		pthread_mutex_destroy(&jobMutex);
		
		//faccio una sem_post, per dare il segnale al Master di aver finito.
		
		deallMemDinamica(copiaJobs, copiaInstr);
		
		sJobs[totJobs - 2].Job.id = 555;
	
	   	exit(0);
	}
 
	if(pidNotPreemptive > 0)//main che termina.
	{
		printf("\n\nmain: fork del NotPreemptive.\nPID: %d.\n", pidNotPreemptive);
		
		waitpid(pidPreemptive, &statusProcessi, 0);
		printf("\n\nmain: attendo la fine del Preemptive.\n");	
		messaggioProcessi(pidPreemptive, statusProcessi);
		
		waitpid(pidNotPreemptive, &statusProcessi, 0);
		printf("\n\nmain: attendo la fine del NotPreemptive.\n");
		messaggioProcessi(pidNotPreemptive, statusProcessi);

		waitpid(pidMaster, &statusProcessi, 0);
		printf("\n\nmain: attendo la fine del Master.\n");	
		messaggioProcessi(pidMaster, statusProcessi);
		
		//distruggo i semafori con sem_destroy.
	
		printf("\n\nMollane una e non due, eh.\n");
	}
	
	/*sem_t semMaster, semPre, semNotPre;
	
	sem_init(&semMaster, 1, 0);
	sem_init(&semPre, 1, 0);
	sem_init(&semNotPre, 1, 0);
	
	//Master.
	sem_post(&semPre);
	sem_post(&semNotPre);
	sem_wait(&semMaster);
	sem_wait(&semMaster);
	
	//scedulerPre.
	sem_wait(&semPre);
	sem_post(&semMaster);
	
	//schedulerNotPre
	sem_wait(&semNotPre);
	sem_post(&semMaster);
	
	sem_destroy(&semMaster);
	sem_destroy(&semPre);
	sem_destroy(&semNotPre);*/
	
	exit(0);
}
