#include "funzioni.h"


void stampaMexAiuto(FILE* stream, int exitCode)
{
	fprintf(stream, "\nSintassi da seguire: %s -op [outputfile] -on [outputfile] -i [inputfile] -q [quantum]\n\n", nomeProgramma);
    fprintf(stream,
		   "Il simulatore deve accettare i seguenti parametri (obbligatori) da linea di comando:\n"
		   "-op | --output-preemption		Il file di output con i risultati dello scheduler con preemption.\n"
		   "-on | --output-no-preemption		Il file di output con i risultati dello scheduler senza preemption.\n"
		   "-i  | --input				Il file di input contenente la lista dei job.\n"
		   "-q  | --quantum				La durata di un quanto di tempo (misurato in cicli di clock) sullo scheduler con preemption.\n\n"
		   "(Opzionale)\n"
		   "-h  | --help				Display this help information.\n");
	exit(exitCode);
}

void inserimentoParametri(int argc, char** argv) 
{
	int nextOption;

    const char* const shortOptions = "hi:o:q:";

    const struct option longOptions[] =
    {
        { "help",       0, NULL, 'h' },
        { "input",      1, NULL, 'i' },
        { "quantum",    1, NULL, 'q' },
        { NULL,         0, NULL,  0  }
    };	
    
    //Strutture per il passaggio dati agli scheduler.
	quantum = 0;
    nomeProgramma = argv[0];
	
	if(argc != 9)//Se parametri < 4.
	{
		if(argc == 1)
			fprintf(stderr, "\nNessun parametro inserito!\n\n");
		else
			if(argc < 9)
				fprintf(stderr, "\nI parametri inseriti sono insufficienti.\n\n");
			else
				if(argc > 9)
					fprintf(stderr, "\nI parametri inseriti sono eccessivi.\n\n");
		stampaMexAiuto(stderr, -1);
	}

	int cntrl = 0;
	for(int i = 1; i < argc ; i += 2)//Acquisizione parametri -op & -on.
	{		
		if(!strcmp(argv[i], "-op") || !strcmp(argv[i], "--output-preemption"))//Controllo se è presente il paramentro -op.
		{		
			fprintf(stdout, "\nSto creando il file di output per i risultati dello scheduler con preemption.\n");	
			cntrlFile = argv[i + 1];
			filePre = fopen(cntrlFile, "w");//Dichiara e apre lo stream del file.
			
			if(filePre == NULL)//Controlla se il file viene aperto.
			{
				fprintf(stderr, "\nSi è verificato un errore. Il file %s non è stato creato.\n", argv[i + 1]);
				exit(-1);
			}
			else
			{
				fprintf(stdout, "\nIl file %s è stato creato.\n", argv[i + 1]);
			}
			cntrl++;
		}
		
		if(!strcmp(argv[i], "-on") || !strcmp(argv[i], "--output-no-preemption"))//Controllo se è presente il paramentro -on.
		{
			fprintf(stdout, "\nSto creando il file di output per i risultati dello scheduler senza preemption.\n");
					cntrlFile_II = argv[i + 1];
					fileNotPre = fopen(cntrlFile_II, "w");//Dichiara e apre lo stream del file.
		
					if(fileNotPre == NULL)//Controlla se il file viene aperto.
					{
						fprintf(stderr, "\nSi è verificato un errore. Il file %s non è stato creato.\n", argv[i + 1]);
						exit(-1);
					}
					else
					{
						fprintf(stdout, "\nIl file %s è stato creato.\n", argv[i + 1]);
					}			
			cntrl++;
		}
	}
	if(cntrl != 2)
	{
		stampaMexAiuto(stderr, -1);
	}
	if(!strcmp(cntrlFile, cntrlFile_II))
	{
		fprintf(stderr, "\nSi è verificato un errore. I file in scrittura hanno lo stesso nome.\n");
		exit(-1);
	}
	//Acquisizione altri parametri.
    do{
        nextOption = getopt_long(argc, argv, shortOptions, longOptions, NULL);
  
		switch(nextOption)
		{
            case 'h':
                stampaMexAiuto(stdout, 0);
                
			case 'o':
				break;
				
			case 'i':
					inputFilename = optarg;
					fileInput = fopen(inputFilename, "r");
					if (fileInput == NULL) 
					{
						fprintf(stderr, "\nIl file di input %s non esiste.\n", inputFilename);
						exit(-1);
					}
					else
					{
						fprintf(stdout, "\nIl file di input %s esiste.\n", inputFilename);
						fclose(fileInput);
					}
					break;	
					
			case 'q':	
					quantum = atoi(optarg);//Conversione ASCII -> dec.
					if(quantum <= 0)
					{
						fprintf(stderr, "E' stata inserita un'opzione non valida.\n");
						exit(-1);
					}
					break;

            case -1:
                break;

            default:
                abort();
        }
    }while(nextOption != -1);
}

void numJobsInstr(FILE* stream)
{
	totJobs = 0;
	totInstr_Global = 0;
	
	stream = fopen(inputFilename, "r");
	char numInput;
	
	do
	{
		numInput = fgetc(stream);//più veloce di fscanf(stream, "%c%, &numInput).
		if(numInput == 'j')
		{
			totJobs++;
		}
		if(numInput == 'i')
		{
			totInstr_Global++;
		}
	}while(numInput != EOF);
	
	fclose(stream);
}

void inputJobsInstr(FILE* stream, infoJob* datiJobs, istruzione* datiInstr)
{	
	stream = fopen(inputFilename, "r");
	
	char qualeInput;
	
	long int count_I = 0;//contatore globale delle istruzioni. 
	
	int count_J = 0;  
	int numInstr = 0;//contatore locale delle istruzioni.
	
	int varInput = 0;

	qualeInput = fgetc(stream);
	
	do{
						  
		if(qualeInput == 'j')
		{
			if(count_J != 0)
				datiJobs[count_J - 1].totInstr = numInstr;
				
			numInstr = 0;//ad ogni job rinizializza il contatore delle istruzioni.
		
			fgetc(stream);//passo il char ',' da stream; 
			fscanf(stream, "%d", &varInput); 

			datiJobs[count_J].Job.id = varInput; 
		 
			fgetc(stream);
			fscanf(stream, "%d", &varInput); 
			
			datiJobs[count_J].Job.arrivalTime = varInput; 

			datiJobs[count_J].Job.statoJob.fase = New;
			datiJobs[count_J].currentInstr = 0;
			
			count_J++;
		}
			 							 
		if(qualeInput == 'i')
		{
			numInstr++;
			
			fgetc(stream);
			qualeInput = fgetc(stream);
			if(qualeInput == '0')
			{
				datiInstr[count_I].typeFlag = 0;
			}
				
			if(qualeInput == '1')
			{
				datiInstr[count_I].typeFlag = 1;
			}

			fgetc(stream);
			fscanf(stream, "%d", &varInput); 
										      
			datiInstr[count_I].length = varInput; 

			datiJobs[count_J - 1].lengthTot += varInput;   
			 				
			fgetc(stream);
			fscanf(stream, "%d", &varInput); 
						                                      
			datiInstr[count_I].ioMax = varInput;  

			count_I++; 
		}
		 
		qualeInput = fgetc(stream);
		if(qualeInput == EOF)
		{
			datiJobs[count_J - 1].totInstr = numInstr;
		}                 
						                
	}while(qualeInput != EOF);
	
	fclose(stream);
	
	for(int i = 0; i < totJobs; i++)//posizione della prima istruzione di ogni job.
	{
		if(i == 0)
		{
			datiJobs[i].Job.listaInstr = 0;
		}
					
		else
		{
			datiJobs[i].Job.listaInstr = datiJobs[i - 1].Job.listaInstr + datiJobs[i - 1].totInstr;
		}
	}
}

void initVariabili()
{
	totBlock_0 = 0;
	totBlock_I = 0;
	
	currentJob = 0;
	jobsLeft = totJobs;
	lastJob = 0;
	
	currentBlock = 0;
	
	clock_0 = 0;
	clock_I = 0;
	
	currentInstr_Global = 0;
}

void copiaData()//bisogna fare la free(), a fine void*.
{
	copiaJobs = (infoJob*) malloc(totJobs*sizeof(infoJob));//puntatore al primo elemento del vettore dinamico di infoJob.
	copiaInstr = (istruzione*) malloc(totInstr_Global*sizeof(istruzione)); 
	
	for(long int i = 0; i < totInstr_Global; i++)
	{
		copiaInstr[i] = sInstr[i];		 
	}
	
	for(int i = 0; i < totJobs; i++)
	{
		copiaJobs[i] = sJobs[i];			    
	}
}

void erroreFork()
{
	extern int errno;
	
	fprintf(stderr, "\nValore di errno: %d.\n", errno);
	fprintf(stderr, "\nErrore durante l'esecuzione della fork: %s.\n", strerror(errno));
	
	exit(EX_OSERR);
}

void erroreThread(int err, int numCore, bool qualeScheduler)
{
	if(!qualeScheduler)
	{
		if(err != 0)
		{
			fprintf(stderr,"\nSi è verificato un problema nella creazione del thread_%i dello schedulerPre per il seguente motivo: '%s'.", numCore, strerror(err));
			exit(EX_OSERR);
		}
		else
			printf("\nIl thread_%i dello schedulerPre è stato creato correttamente.\n", numCore);
	}
		
	else
	{
		if(err != 0)
		{
			fprintf(stderr,"\nSi è verificato un problema nella creazione del thread_%i dello schedulerNotPre per il seguente motivo: '%s'.", numCore, strerror(err));
			exit(EX_OSERR);
		}
		else
			printf("\nIl thread_%i dello schedulerNotPre è stato creato correttamente.\n", numCore);
	}
}

void messaggioProcessi(pid_t pid, int status)
{
	if(WIFEXITED(status))
		fprintf(stdout, "\nIl processo %i è uscito normalmente con exit code: %d.\n", pid, WEXITSTATUS(status));
		
	else
		fprintf(stderr, "\nIl processo %i è uscito in modo anomalo.\n", pid);
}

void deallMemDinamica(infoJob* datiJobs, istruzione* datiInstr)
{
	free(datiJobs);
	free(datiInstr);
}
