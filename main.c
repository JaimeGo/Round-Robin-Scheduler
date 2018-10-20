#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>

struct Process{
    pid_t pid;

    char name[256];
    char initialTime[256];
    char n[256];
    char sequence[256];

    char state[10];

    int timeLeftInCpu;
    int timeLeftInIo;
    int lastInSeq;
    int burstsReady;
    int del;


};

struct Queue{
    char name[256];
    struct Process *processes;
};


int main(int argc, char *argv[]) {

    struct Queue mainQueue;
    mainQueue.processes=NULL;

    struct Process *allProcesses=NULL;

    int numProcesses=0;

    printf("Input from console: 1) %s, 2) %s, 3) %s\n", argv[1], argv[2], argv[3]);


    FILE *inputFile = fopen( argv[1], "r" );
    FILE *outputFile = fopen( argv[2], "w" );
    int quantum= (int)strtol(argv[3], NULL, 10);
    printf("Quantum is: %d\n", quantum);

    if (inputFile == NULL)
    {
        printf("Error opening file");

        exit(1);
    }

    printf("File content is:\n");

    char line[512];
    for (int k=0;k< sizeof(line);k++){line[k]='E';}

    int countLines=0;


    while(fgets(line,sizeof line, inputFile)){
        printf("LINE: %s\n", line);


        struct Process ProcessI={};
        ProcessI.lastInSeq=0;
        ProcessI.burstsReady=0;
        ProcessI.del=0;



        struct Process *moreProcesses=NULL;



        int i=0;
        char currentChar=line[i];
        int readingVar=0;

        int iname=0, itime=0, inn=0, iseq=0;



        while(line[i]!= EOF && line[i] != '\n' && i< sizeof(line)){



            if(currentChar!=' ' || readingVar==3)
                switch (readingVar){
                    case 0:
                        ProcessI.name[iname]=currentChar;
                        iname++;


                        break;

                    case 1:
                        ProcessI.initialTime[itime]=currentChar;
                        itime++;

                        break;

                    case 2:
                        ProcessI.n[inn]=currentChar;
                        inn++;

                        break;

                    case 3:

                        ProcessI.sequence[iseq]=currentChar;
                        iseq++;

                        break;

                    default:
                        printf("DEFAULT");
                        break;

                }

            else if (readingVar<3){readingVar++;}

            i++;
            currentChar=line[i];



        }

        moreProcesses = (struct Process*) realloc (allProcesses, sizeof(allProcesses) + sizeof(ProcessI));

        allProcesses=moreProcesses;

        allProcesses[countLines]=ProcessI;
        numProcesses++;


        printf("Process name is: %s,\n initial time is %s,\n n is %s,\n sequence is %s\n", ProcessI.name,ProcessI.initialTime,ProcessI.n,ProcessI.sequence);

        countLines++;


    }



    //Inicio Scheduler
    int isFirst=1;
    int time=0;
    int counterAllProcesses = numProcesses;
    int counterMainQueue = 0;

    while (1) {


        char ready[256] = "READY";
        char running[256] = "RUNNING";
        char waiting[256] = "WAITING";
        char finished[256] = "FINISHED";




        //Creation
        for (int i = 0; i < counterAllProcesses; i++) {
            int initialTime = atoi(allProcesses[i].initialTime);

            if (initialTime == time) {

                printf("[t = %d] El proceso %s ha sido creado\n", time, allProcesses[i].name);

                if(isFirst){
                    strcpy(allProcesses[i].state, "RUNNING");
                    printf("[t = %d] El proceso %s ha pasado a estado RUNNING\n", time, allProcesses[i].name);
                    isFirst=0;
                }
                else {
                    strcpy(allProcesses[i].state, "READY");

                }


                struct Process *extraProcesses = (struct Process *) realloc(mainQueue.processes,
                                                                            sizeof(mainQueue.processes) +
                                                                            sizeof(allProcesses[i]));

                mainQueue.processes = extraProcesses;


                mainQueue.processes[counterMainQueue] = allProcesses[i];
                printf("STATE %s",mainQueue.processes[counterMainQueue].state);

                char temp[256];
                char *seq = mainQueue.processes[counterMainQueue].sequence;
                for (int j = mainQueue.processes[counterMainQueue].lastInSeq; j < strlen(seq); j++) {
                    if (seq[j] == ' ') {
                        mainQueue.processes[counterMainQueue].lastInSeq = j + 1;
                        break;
                    }
                    temp[j] = seq[j];

                }

                mainQueue.processes[counterMainQueue].timeLeftInCpu = atoi(temp);
                printf("TIME LEFT %d",mainQueue.processes[counterMainQueue].timeLeftInCpu);

                counterMainQueue++;
            }
        }

        int instantCounterMain=counterMainQueue;


        //Main loop
        if (time!=0) {
            for (int i = 0; i < instantCounterMain; i++) {

                struct Process p = mainQueue.processes[i];


                if (p.del == 0) {

                    //CPU Burst Running
                    if (strcmp(p.state, running) == 0) {


                        p.timeLeftInCpu--;

                        //Interrupt - Waiting
                        if (p.timeLeftInCpu == 0) {


                            struct Process *extraProcesses = (struct Process *) realloc(mainQueue.processes,
                                                                                        sizeof(mainQueue.processes) +
                                                                                        sizeof(p));

                            mainQueue.processes = extraProcesses;

                            mainQueue.processes[counterMainQueue] = p;

                            counterMainQueue++;

                            mainQueue.processes[i].del = 1;

                            struct Process pNew = mainQueue.processes[counterMainQueue - 1];


                            pNew.burstsReady++;

                            //finished
                            if (pNew.burstsReady == 2 * atoi(pNew.n) - 1) {
                                printf("[t = %d] El proceso %s ha pasado a estado FINISHED\n", time, pNew.name);

                                strcpy(pNew.state, finished);
                            }

                                //waiting
                            else {

                                printf("[t = %d] El proceso %s ha pasado a estado WAITING\n", time, p.name);

                                strcpy(pNew.state, "WAITING");
                            }

                            //Running first element
                            for (int k = 0; k < counterMainQueue; k++) {
                                struct Process pK = mainQueue.processes[k];
                                if (strcmp(pK.state, ready) == 0) {
                                    printf("[t = %d] El proceso %s ha pasado a estado RUNNING\n", time, pK.name);

                                    strcpy(pK.state, running);

                                    break;
                                }

                            }


                        }

                            //CPU burst finished
                        else if (time % quantum == 0) {



                            printf("[t = %d] El proceso %s ha pasado a estado READY\n", time, p.name);


                            struct Process *extraProcesses = (struct Process *) realloc(mainQueue.processes,
                                                                                        sizeof(mainQueue.processes) +
                                                                                        sizeof(p));

                            mainQueue.processes = extraProcesses;

                            mainQueue.processes[counterMainQueue] = p;

                            counterMainQueue++;

                            mainQueue.processes[i].del = 1;

                            struct Process pNew = mainQueue.processes[counterMainQueue - 1];


                            pNew.burstsReady++;

                            strcpy(pNew.state, "READY");

                            //Running first element
                            for (int k = 0; k < counterMainQueue; k++) {
                                struct Process pK = mainQueue.processes[k];
                                if (strcmp(pK.state, ready) == 0) {
                                    printf("[t = %d] El proceso %s ha pasado a estado RUNNING\n", time, pK.name);

                                    strcpy(pK.state, running);

                                    break;
                                }

                            }


                        }


                    }

                        //IO Burst Finished
                    else if (strcmp(p.state, waiting) == 0) {


                        p.timeLeftInIo--;

                        if (p.timeLeftInIo == 0) {


                            printf("[t = %d] El proceso %s ha pasado a estado READY\n", time, p.name);


                            struct Process *extraProcesses = (struct Process *) realloc(mainQueue.processes,
                                                                                        sizeof(mainQueue.processes) +
                                                                                        sizeof(p));

                            mainQueue.processes = extraProcesses;

                            mainQueue.processes[counterMainQueue] = p;

                            printf("P4 %d",counterMainQueue);
                            counterMainQueue++;

                            mainQueue.processes[i].del = 1;

                            struct Process pNew = mainQueue.processes[counterMainQueue - 1];


                            pNew.burstsReady++;

                            strcpy(pNew.state, "READY");

                            char temp[256];
                            char *seq = mainQueue.processes[counterMainQueue].sequence;
                            for (int j = mainQueue.processes[counterMainQueue].lastInSeq; j < strlen(seq); j++) {
                                if (seq[j] == ' ') {
                                    mainQueue.processes[counterMainQueue].lastInSeq = j + 1;
                                    break;
                                }
                                temp[j] = seq[j];

                            }

                            mainQueue.processes[counterMainQueue].timeLeftInCpu = atoi(temp);

                        }


                    }

                }


            }
        }



        time++;


        int isOver=1;
        for (int i=0; i<counterMainQueue;i++){

            struct Process pF=mainQueue.processes[i];

            if (strcmp(pF.state,finished)!=0 && pF.del==0){

                isOver=0;
            }
        }

        if (isOver){
            break;
        }




    }


    //Fin Scheduler

    //for (int k=0;k< sizeof(allProcesses);k++){free(allProcesses[k].sequence);}

    free (allProcesses);
    free (mainQueue.processes);

    fclose(inputFile);
    fclose(outputFile);
    return 0;
}






