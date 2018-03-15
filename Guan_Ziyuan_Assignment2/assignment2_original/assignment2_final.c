#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
pthread_mutex_t mux=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full=PTHREAD_COND_INITIALIZER;
pthread_cond_t empty=PTHREAD_COND_INITIALIZER;
char *addr;
int exitflag=0;
int NUM_THREADS;
int NUM_SLOTS;
int buffer[7];
int count=0;
int ticket=0;
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
FILE *fp2;
// store all of the information
struct thread_data threadData[7];
struct buffer_data turple[100];
void *mapper(void *threadid){ // create the mapper thread
    printf("Enter the Mapper Thread\n");
    int tid=*(int*)threadid;
    //freopen("input.txt",'r',stdin);
    FILE *fp1,*fp3;
    char str[26];
    fp1=fopen("input.txt","r");
    if ((fp3 = fopen("mapper_output.txt", "w")) == NULL) {
        /*Open the file with WRITE only*/
        printf("The output file cannot open correctly\n");
        exit(EXIT_SUCCESS);
    }
    while(1) {
        pthread_mutex_lock(&mux);
        if (exitflag == 1) {
            printf("The file is done!");
            //pthread_cond_broadcast(&empty);
           // pthread_cond_wait(&full,&mux);
            //pthread_mutex_unlock(&mux);
            break;
        }
        while(fgets(str,26,fp1)){
            exitflag=0;
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
                printf("The buffer %d 's name is : %s  registstate is %d\n",k,threadData[k].name,threadData[k].nameregist);
                while(buffer[k] == NUM_SLOTS){// it means buffer[k] is full
                    printf("the buffer %d is full \n",k);
                    printf("the mapper is waiting \n");
                    ticket=k;
                    printf("The value of ticket in mapper is : %d\n",ticket);
                    pthread_cond_wait(&full, &mux);
                    k=k-1;
                }
            }
            printf("the value of count is %d\n",count);
            if (count == 1) {
                buffer[0] = buffer[0] + 1;
                strncpy(threadData[0].name,turple[0].buffer_name,5);//put the name into thread name
                turple[0].buffer_num = 0;
                threadData[0].nameregist=1;
            }
            else{
                int flag = 0;
                for(int i=0;i<count-1;i++){
                    if(strcmp(turple[count-1].buffer_name,threadData[turple[i].buffer_num].name)==0){
                        buffer[turple[i].buffer_num]++;
                        turple[count-1].buffer_num=turple[i].buffer_num;
                        flag=1;
                        break;
                    }
                }
                if(flag!=1){
                    for (int i = 0; i < NUM_THREADS; i++) {
                        if (threadData[i].nameregist == 0) {//its a empty buffer;
                            buffer[i]++;
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
            fputs("(",fp3);
            fputs(turple[count-1].buffer_name, fp3);
            fputs(",",fp3);
            fputs(turple[count-1].subject, fp3);
            fputs(",",fp3);
            char ss[3];
            sprintf(ss, "%d", turple[count-1].score);
            fputs(ss, fp3);
            fputs(")",fp3);
            fputs("\n", fp3);
            //pthread_cond_broadcast(&empty);
            //pthread_mutex_unlock(&mux);

        }
        exitflag = 1;
        //ticket=0;
        printf("the value of ticket right now is :%d\n",ticket);
        printf("Mapper signalled done!\n");
        printf("the state of exit is %d\n",exitflag);
        pthread_cond_broadcast(&empty);
        pthread_mutex_unlock(&mux);
        break;
    }
    pthread_cond_broadcast(&empty);
    pthread_cond_broadcast(&full);
    pthread_mutex_unlock(&mux);
    fclose(fp1);
    return 0;
}
void *reducer(void *threadid) {
    //struct buffer_data totaldata[100];
    int tid=0;
    tid = *(int *) threadid;
    if(tid>=NUM_THREADS){
        pthread_cond_broadcast(&full);
        pthread_cond_broadcast(&empty);
        pthread_mutex_unlock(&mux);
        ticket=0;
    }
    printf("thread %d the tid is : %d\n",threadData[tid].thread_id,tid);
    printf("Enter the Reducer Thread ID:  %d\n", tid);
    printf("the number of ticket when enter reducer is %d\n",ticket);

    if ((fp2 = fopen("reducer_output.txt", "w")) == NULL) {
        /*Open the file with WRITE only*/
        printf("The output file cannot open correctly\n");
        exit(EXIT_SUCCESS);
    }
    //int temp=0;
    while (1) {
        pthread_mutex_lock(&mux);
        if(ticket==tid){
            if(exitflag == 1 && buffer[tid] == 0) {
                printf("The Thread  %d", tid);
                printf("finished !\n");
                pthread_mutex_unlock(&mux);
                break;
            }
            while(buffer[tid] < NUM_SLOTS && exitflag != 1) { // the buffer is empty
                printf("thread %d is waiting \n",tid);
                //pthread_cond_broadcast(&full);
                //ticket++;
                pthread_cond_wait(&empty, &mux);
            }
            for (int i = 0; i < count-1; i++) {
                for (int k = i+1; k < count-1; k++) {
                    if (turple[i].buffer_num==tid && turple[k].buffer_num==tid){
                        if(strcmp(turple[i].subject,turple[k].subject)==0){
                            turple[i].score = turple[i].score + turple[k].score;
                            turple[k].buffer_name[0] ='X';//change the subject and will not be used again.
                            buffer[tid]--;
                        }
                    }
                }
            }
            /*
            for (int i = 0; i < count-1; i++) {
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
                    turple[i].buffer_name[0]='X';
                    buffer[tid]--;
                }
            }
             */
            printf("thread %d:the buffer num for the buffer %d is : %d\n",tid,tid,buffer[tid]);
        }
        /*
        if(buffer[tid]==0){
            pthread_cond_broadcast(&full);
            ticket++;
            if(ticket>=NUM_THREADS){
                ticket=0;
            }
            //pthread_mutex_unlock(&mux);
            printf("thread %d:the buffer num for the buffer %d is : %d\n",tid,tid,buffer[tid]);
        }
         */
        pthread_cond_broadcast(&full);
        ticket++;
        if(ticket>=NUM_THREADS){
            ticket=0;
        }
        printf("thread %d :The value of ticket in reducer is : %d\n",tid,ticket);
        printf("the value of tid is: %d\n",tid);
        break;
    }
    printf("thread %d back to the mapper!\n",tid);
    pthread_cond_broadcast(&full);
    pthread_mutex_unlock(&mux);
    // all of the same name same subject processed! print to file
    return threadid;
}
int main(int argc,char *argv[]){
    if(argc<4){
        printf("The Number of Arguments is Not Right\n");
        exit(EXIT_SUCCESS);
    }
    NUM_SLOTS=atoi(argv[1]);
    NUM_THREADS=atoi(argv[2]);
    addr=argv[3];
    printf("the num of thread is %d\n",NUM_THREADS);
    printf("the num of slots is %d\n",NUM_SLOTS);
    pthread_t mapper_thread;
    pthread_t threads[NUM_THREADS];// The number of Reducer
    int t=0;
    int p=0;
    for(int i=0;i<NUM_THREADS;i++){
        printf("the number of buffernum is %d\n",buffer[i]);
    }
    pthread_create(&mapper_thread,NULL,mapper,(void *)&addr);
    sleep(1);
    for(t=0;t<NUM_THREADS;t++){
        printf("Creating the reducer thread %d\n",t);
        sleep(1);
        threadData[t].thread_id=t;
        printf("in main the thread id is %d\n",t);
        pthread_create(&threads[t],NULL,reducer,(void *)&t);
    }
    pthread_join(mapper_thread,NULL);
    for(int i=0;i<NUM_THREADS;i++){
        pthread_join(threads[i],NULL);
    }
    for (int i = 0; i < count; i++) {
        if (turple[i].buffer_name[0]!='X'){
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
            //turple[i].buffer_name[0]='X';
            //buffer[tid]--;
        }
    }

}
