#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
pthread_mutex_t mux=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full;
pthread_cond_t empty;
int buffer[20]={0};
int exitflag=0;
int NUM_THREADS;
int NUM_SLOTS;
int count=0;
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
// store all of the information
struct thread_data threadData[10];
struct buffer_data turple[100];
void *mapper(void *threadid){ // create the mapper thread
    printf("Enter the Mapper Thread\n");
    int tid=*(int*)threadid;
    //freopen("input.txt",'r',stdin);
    FILE *fp1,*fp2;
    char str[26];
    fp1=fopen("input.txt","r");
    if ((fp2 = fopen("mapper_output.txt", "w")) == NULL) {
        /*Open the file with WRITE only*/
        printf("The output file cannot open correctly\n");
        exit(EXIT_SUCCESS);
    }
    while(1) {
        pthread_mutex_lock(&mux);
        if (exitflag == 1) {
            printf("The file is done!");
            pthread_mutex_unlock(&mux);
        }
        while(fgets(str,26,fp1)){
            printf("the content of str %s\n",str);
            for(int i=0;i<4;i++){
                turple[count].buffer_name[i]=str[1+i];
            }
            turple[count].buffer_name[4]='\0';
            turple[count].score_c[0]=str[6];
            for(int i=0;i<15;i++){
                turple[count].subject[i]=str[8+i];
            }
            turple[count].subject[15]='\0';
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
            for (int k = 0; k < NUM_THREADS; k++) {
                if (buffer[k] == NUM_SLOTS) {// it means buffer[k] is full
                    printf("the buffer %d is full \n",k);
                    pthread_cond_wait(&full, &mux);
                    //pthread_cond_broadcast(&empty);
                    pthread_mutex_unlock(&mux);
                    //break;
                }
            }
            if (count == 1) {
                buffer[0] = buffer[0] + 1;
                strncpy(threadData[0].name,turple[0].buffer_name,5);//put the name into thread name
                turple[0].buffer_num = 0;
                threadData[0].nameregist=1;
            }
            else{
                int flag = 0;
                for (int i = 0; i < NUM_THREADS; i++) {
                    printf("the name of thread %d is :%s\n",i,threadData[i].name);
                    if(threadData[i].nameregist!=1){
                        break;
                    }
                    if (strcmp(turple[count-1].buffer_name,threadData[i].name)==0) {
                        flag = 1;
                        buffer[threadData[i].thread_id]++;
                        turple[count-1].buffer_num=threadData[i].thread_id;
                    }
                }
                if (flag != 1) {// it means the name is not registered yet
                    for (int i = 0; i < NUM_THREADS; i++) {
                        if (threadData[i].nameregist == 0) {//its a empty buffer;
                            buffer[i];
                            threadData[i].nameregist=1;
                            turple[count-1].buffer_num = i;
                            strncpy(threadData[i].name,turple[count-1].buffer_name,5);
                            break;
                        }
                    }
                }
            }
            for(int i=0;i<NUM_THREADS;i++){
                printf("the state of buffer %d is %d\n",i,buffer[i]);
            }
            fputs("(",fp2);
            fputs(turple[count-1].buffer_name, fp2);
            fputs(",",fp2);
            fputs(turple[count-1].subject, fp2);
            fputs(",",fp2);
            char ss[3];
            sprintf(ss, "%d", turple[count-1].score);
            fputs(ss, fp2);
            fputs(")",fp2);
            fputs("\n", fp2);
            pthread_cond_broadcast(&empty);
            pthread_mutex_unlock(&mux);

        }
        fclose(fp1);
        exitflag = 1;
        printf("Mapper %d signalled done!\n", tid);
        printf("the state of exit is %d\n",exitflag);
        pthread_cond_broadcast(&empty);
        pthread_cond_broadcast(&full);
        pthread_mutex_unlock(&mux);
        break;
    }
    return threadid;
}
void *reducer(void *threadid) {
    //struct buffer_data totaldata[100];
    int tid = *(int *) threadid;
    threadData[tid].thread_id = tid;
    printf("Enter the Reducer Thread ID:  %d\n", tid);
    FILE *fp2;
    if ((fp2 = fopen("reducer_output.txt", "w")) == NULL) {
        /*Open the file with WRITE only*/
        printf("The output file cannot open correctly\n");
        exit(EXIT_SUCCESS);
    }
    int temp=0;
    while (1) {
        pthread_mutex_lock(&mux);
        if(exitflag == 1 && buffer[tid] == 0) {
            printf("The Thread  %d", tid);
            printf("finished !\n");
            pthread_mutex_unlock(&mux);
            break;
        }
        while(buffer[tid] == 0 && exitflag != 1) { // the buffer is empty
            pthread_cond_wait(&empty, &mux);
            //pthread_cond_broadcast(&full);
            //pthread_mutex_unlock(&mux);
        }
        if ((buffer[tid] == NUM_SLOTS)) {
            /*
            for(int i=0;i<count;i++){
                if(turple[i].buffer_num==tid){
                    strcmp(totaldata[temp].buffer_name,turple[i].buffer_name);
                    strcmp(totaldata[temp].subject,turple[i].subject);
                    totaldata[temp].score=turple[i].score;
                    temp++;
                    buffer[tid]--;
                }
            }
            */
            for (int i = 0; i < count-1; i++) {
                for (int k = i+1; k < count; k++) {
                    if (turple[i].buffer_num==tid && turple[k].buffer_num==tid){
                        if(strcmp(turple[i].subject,turple[k].subject)==0){
                            turple[i].score = turple[i].score + turple[k].score;
                            turple[k].buffer_name[0] ='X';//change the subject and will not be used again.
                            buffer[tid]--;
                        }
                    }
                }
            }
            printf("the number of buffer is %d\n",buffer[tid]);
            printf("the number of exitflag is %d\n",exitflag);
            pthread_cond_wait(&empty,&mux);
            //pthread_cond_signal(&full);
            //pthread_mutex_unlock(&mux);
        }
        while(buffer[tid]!=0 && exitflag!=1){
            pthread_cond_wait(&empty, &mux);
            //pthread_mutex_unlock(&mux);
            pthread_cond_signal(&full);
            //pthread_mutex_unlock(&mux);
        }
        if(buffer[tid]!=0 && exitflag==1){
            for (int i = 0; i < count; i++) {
                if (strcmp(turple[i].buffer_name,threadData[tid].name)==0){
                    fputs("(",fp2);
                    fputs(turple[i].buffer_name,fp2);
                    fputs(",",fp2);
                    fputs(turple[i].subject,fp2);
                    fputs(",",fp2);
                    char ss[3];
                    sprintf(ss, "%d", turple[i].score);
                    fputs(ss, fp2);
                    fputs(")",fp2);
                    fputs("\n",fp2);
                }
            }
        }
    }
    // all of the same name same subject processed! print to file

    fclose(fp2);
    pthread_cond_signal(&full);
    pthread_mutex_unlock(&mux);
}
int main(int argc,char *argv[]){
    //if(argc<4){
    //    printf("The Number of Arguments is Not Right\n");
    //    exit(EXIT_SUCCESS);
    //}
    //int NUM_SLOTS=atoi(argv[1]);
    //int NUM_THRE
    NUM_SLOTS=10;
    NUM_THREADS=7;
    printf("the num of thread is %d\n",NUM_THREADS);
    printf("the num of slots is %d\n",NUM_SLOTS);
    pthread_t mapper_thread;
    pthread_t threads[NUM_THREADS];// The number of Reducer
    int t=0;
    int p=0;
    for(int i=0;i<NUM_THREADS;i++){
        printf("the number of buffernum is %d\n",buffer[i]);
    }
    pthread_create(&mapper_thread,NULL,mapper,(void *)&p);
    for(t=0;t<NUM_THREADS;t++){
        printf("Creating the reducer thread %d\n",t);
        pthread_create(&threads[t],NULL,reducer,(void *)&t);
    }
    pthread_join(mapper_thread,NULL);
    for(int i=0;i<NUM_THREADS;i++){
        pthread_join(threads[i],NULL);
    }
    pthread_mutex_destroy(&mux);
    pthread_cond_destroy(&full);
    pthread_cond_destroy(&empty);
}
