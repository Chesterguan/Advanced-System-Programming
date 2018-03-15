#include <stdio.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

static sem_t mutex,empty,full[30];
int exitflag=0;
int NUM_THREADS;
int NUM_SLOTS;
int buffer[7];
int count=0;
int count2=0;
FILE *fp2;
struct thread_data{
    int thread_id;
    char name[5] ;
    int nameregist;
};
struct buffer_data{
    int score;
    int buffer_num;
    char buffer_name[5];
    char subject[16];
    char score_c[1];
};
struct buffer_data result[100];
struct thread_data threadData[7];
struct buffer_data turple[100];
void *mapper(void *threadid){ // create the mapper thread
    printf("Enter the Mapper Thread\n");
    int tid=*(int*)threadid;
    ////freopen("input.txt",'r',stdin);
    FILE *fp3,*fp1;
    char str[26];
    if ((fp1 = fopen("input.txt", "r")) == NULL) {
        /*Open the file with read only*/
        printf("The output file cannot open correctly\n");
        exit(EXIT_SUCCESS);
    }
    if ((fp3 = fopen("mapper_output.txt", "w")) == NULL) {
        /*Open the file with WRITE only*/
        printf("The output file cannot open correctly\n");
        exit(EXIT_SUCCESS);
    }
    //;// to make sure the mapper should enter firstly
    while (fgets(str, 26, fp1)) {
        sem_wait(&empty);
        sem_wait(&mutex);
        exitflag = 0;
        printf("the content of str %s\n", str);
        int i;
        for (i = 0; i < 4; i++) {
            turple[count].buffer_name[i] = str[1 + i];
        }
        turple[count].buffer_name[4] = '\0';
        turple[count].score_c[0] = str[6];
        for ( i = 0; i < 15; i++) {
            turple[count].subject[i] = str[8 + i];
        }
        turple[count].subject[15] = '\0';
        switch (turple[count].score_c[0]) {
            case 'P':
                turple[count].score = 50;
                break;
            case 'L':
                turple[count].score = 20;
                break;
            case 'D':
                turple[count].score = -10;
                break;
            case 'C':
                turple[count].score = 30;
                break;
            case 'S':
                turple[count].score = 40;
                break;
            default:
                printf("Input is wrong\n");
                break;
        }
        printf("The name of buffer is %s\n", turple[count].buffer_name);
        printf("the subject of buffer is %s\n", turple[count].subject);
        printf("the score of the subject is %d\n", turple[count].score);
        count++;
        if (count == 1) {
            buffer[0] = buffer[0] + 1;
            strncpy(threadData[0].name, turple[0].buffer_name, 5);//put the name into thread name
            turple[0].buffer_num = 0;
            threadData[0].nameregist = 1;
        }
        else {
            int flag = 0;
            int j;
            for (j = 0; j < count - 1; j++) {
                if (strcmp(turple[count - 1].buffer_name, threadData[turple[j].buffer_num].name) == 0) {
                    buffer[turple[j].buffer_num]++;
                    turple[count - 1].buffer_num = turple[j].buffer_num;
                    flag = 1;
                    break;
                }
            }
            if (flag != 1) {
                int k;
                for (k = 0; k < NUM_THREADS; k++) {
                    if (threadData[k].nameregist == 0) {//its a empty buffer;
                        buffer[k]++;
                        threadData[k].nameregist = 1;
                        turple[count - 1].buffer_num = k;
                        strncpy(threadData[k].name, turple[count - 1].buffer_name, 5);
                        break;
                    }
                }
            }
        }
        int v;
        for (v = 0; v < NUM_THREADS; v++) {
            printf("the state of buffer %d is %d\n", v, buffer[v]);
        }
        fputs("(", fp3);
        fputs(turple[count - 1].buffer_name, fp3);
        fputs(",", fp3);
        fputs(turple[count - 1].subject, fp3);
        fputs(",", fp3);
        char ss[3];
        sprintf(ss, "%d", turple[count - 1].score);
        fputs(ss, fp3);
        fputs(")", fp3);
        fputs("\n", fp3);
        int p;
        for (p = 0; p < NUM_THREADS; p++) {
            printf("The buffer %d 's name is : %s  registstate is %d\n", p, threadData[p].name,
                   threadData[p].nameregist);
            if (buffer[p] == NUM_SLOTS) {// it means buffer[k] is full
                printf("the buffer %d is full \n", p);
                printf("the mapper is waiting \n");
                sem_post(&mutex);
                sem_post(&full[p]);
                break;
            }
            else if (p == NUM_THREADS - 1) {
                sem_post(&mutex);
                sem_post(&empty);
            }
        }
        printf("the value of count is %d\n", count);
    }
    sleep(3);
    exitflag = 1;
    sem_post(&full[0]);
    fclose(fp3);
    printf("Mapper signalled done!\n");
    printf("the state of exit is %d\n", exitflag);
    return 0;
}
void *reducer(void *threadid) {
    int tid=(int)threadid;
    printf("enter the reducer %d\n",tid);
    while(1){
        sem_wait(&full[tid]);
        sem_wait(&mutex);
        printf("tid%d\n",tid);
        if(buffer[tid]!=0){
            int d;
            for( d=0; d<count;d++){
                if(turple[d].buffer_num==tid){
                    result[count2].buffer_num=tid;
                    strcpy(result[count2].subject,turple[d].subject);
                    strcpy(result[count2].buffer_name,turple[d].buffer_name);
                    result[count2].score=turple[d].score;
                    turple[d].buffer_num=20;
                    turple[d].buffer_name[0]='X';
                    count2++;
                    buffer[tid]--;// save the turple into result
                }
            }
            printf("buffer[o]num is %d\n",buffer[0]);
            printf("the thread %d buffer num is :%d\n",tid,buffer[tid]);
        }
        int t;
        for( t=0;t<NUM_THREADS;t++){
            printf("the number of buffer is %d\n",buffer[t]);
        }
        printf("the number of count2 is %d\n",count2);
        if(buffer[tid]==0){
            if(exitflag==1){// output time
                printf("outputtime\n");
                int i,k;
                for ( i = 0; i <count2-1; i++) {
                    for ( k = i+1; k <count2; k++) {
                        if ((strcmp(result[i].buffer_name,result[k].buffer_name)==0)&&(result[i].buffer_num==tid)&&(result[k].buffer_num==tid)){
                            if(strcmp(result[i].subject,result[k].subject)==0){
                                result[i].score = result[i].score + result[k].score;
                                result[k].buffer_name[0] ='X';//change the subject and will not be used again.
                            }
                        }
                    }
                }
                if(tid<NUM_THREADS){
                    printf("thread %d is done \n",tid);
                    sem_post(&full[tid+1]);
                    sem_post(&mutex);

                }
                break;
            }
            printf("thread %d\n",tid);
            printf("thread %d is waiting \n",tid);
            sem_post(&empty);
            sem_post(&mutex);

        }
        else{
            sem_post(&full[tid]);
            sem_post(&mutex);

        }
    }
    printf("Thread %d said: Bye!\n",tid);
    return 0;
}
int main(int argc,char *argv[]) {
    close(0);
    printf("Start!\n");
    if(argc<3){
        printf("The Number of Arguments is Not Right\n");
        exit(EXIT_SUCCESS);
    }
    NUM_SLOTS=atoi(argv[1]);
    NUM_THREADS=atoi(argv[2]);
    //NUM_THREADS=7;
    //NUM_SLOTS=10;
    printf("the num of thread is %d\n",NUM_THREADS);
    printf("the num of slots is %d\n",NUM_SLOTS);
    if ((fp2 = fopen("reducer_output.txt", "w")) == NULL) {
        /*Open the file with WRITE only*/
        printf("The output file cannot open correctly\n");
        exit(EXIT_SUCCESS);
    }
    pthread_t mapper_thread;
    pthread_t threads[NUM_THREADS];// The number of Reducer
    sem_init(&mutex,0,1);
    sem_init(&empty,0,1);
    for (int v = 0; v < NUM_THREADS; v++){
        sem_init(&full[v],0,0);
    }
    int p=0;
    pthread_create(&mapper_thread,NULL,mapper,(void *)&p);
    //sleep(1);
    int t=0;
    while(t<=NUM_THREADS-1){
        printf("Creating the reducer thread %d\n",t);
        threadData[t].thread_id=t;
        //sleep(1);
        pthread_create(&threads[t],NULL,reducer,(void *)t);
        ++t;
        printf("thread %d\n",t);
    }
    pthread_join(mapper_thread,NULL);
    int i=0;
    for(;i<NUM_THREADS;i++){
        pthread_join(threads[i],NULL);
    }
    for (int i = 0; i <count; i++) {
        if (result[i].buffer_name[0]!='X'){
            fputs("(",fp2);
            fputs(result[i].buffer_name,fp2);
            fputs(",",fp2);
            fputs(result[i].subject,fp2);
            fputs(",",fp2);
            char ss[3];
            sprintf(ss, "%d", result[i].score);
            fputs(ss, fp2);
            fputs(")",fp2);
            fputs("\n",fp2);
            result[i].buffer_name[0]='X';// this record has beed used
            fflush(fp2);
        }
    }
    printf("finish!\n");
    return 0;
}
