#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
pthread_mutex_t mux=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full;
pthread_cond_t empty;
int buffer[10]={0};
int exitflag=0;
int NUM_THREADS;
int NUM_SLOTS;
int count=0;
struct thread_data{
    int thread_id;
    char *name ;
};
struct buffer_data{
    int score;
    int buffer_num;
    char buffer_name[4];
    char subject[15];
    char score_c[1];
};
// store all of the information
struct thread_data threadData[10];
struct buffer_data turple[100];
void *mapper(void *threadid){ // create the mapper thread
    printf("Enter the Mapper Thread\n");
    int tid=*(int*)threadid;
    FILE *fp2;
    if ((fp2 = fopen("mapper_output.txt", "w")) == NULL) {
        /*Open the file with WRITE only*/
        printf("The output file cannot open correctly\n");
        exit(EXIT_SUCCESS);
    }
    int NUM_S=NUM_SLOTS;
    while(1) {
        pthread_mutex_lock(&mux);
        if (exitflag == 1) {
            printf("The file is done!");
            pthread_mutex_unlock(&mux);
        }
        int r = scanf("(%[^,],%[^,],%[^)])\n",turple[count].buffer_name,turple[count].score_c,turple[count].subject);
	//char mm=getchar();        
	count++;
        if (r != 3) {
            exitflag = 1;
            pthread_cond_broadcast(&empty);
            pthread_cond_broadcast(&full);
            printf("Producer %ld signalled done!\n", tid);
            pthread_mutex_unlock(&mux);
            break;
        }     
	printf("the state of input is %d\n", r);
        printf("the score is \n",turple[count].score_c[0]);
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
        for (int k = 0; k < NUM_THREADS; k++) {
            if (buffer[k] == NUM_S) {// it means buffer[k] is full
                pthread_cond_wait(&full, &mux);
            }
        }
        if (count == 1) {
            buffer[0] = buffer[0] + 1;
            threadData[0].name = turple[0].buffer_name;
            turple[0].buffer_num = 0;
        }
        int flag = 0;
        for (int i = 0; i < count; i++) {
            if ((turple[count-1].buffer_name == turple[i].buffer_name)&&(count>1)) {
                buffer[turple[i].buffer_num]++;
                threadData[turple[i].buffer_num].name = turple[i].buffer_name;
                flag = 1;
                break;
            }
        }
        if (flag != 1) {// it means the name is not registered yet
            for (int i = 0; i < NUM_THREADS; i++) {
                if (buffer[i] == 0) {//its a empty buffer;
                    buffer[i]++;
                    turple[count-1].buffer_num = i;
                    threadData[i].name = turple[count-1].buffer_name;
                    break;
                }
            }
        }
        fputs(turple[count-1].buffer_name, fp2);
        fputs(turple[count-1].subject, fp2);
        char ss[3];
        sprintf(ss, "%d", turple[count-1].score);
        fputs(ss, fp2);
        fputs("\n", fp2);
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mux);
    }
    return threadid;
}
void *reducer(void *threadid) {
    int tid = *(int *) threadid;
    threadData[tid].thread_id = tid;
    printf("Enter the Reducer Thread ID:  %d\n", tid);
    FILE *fp2;
    if ((fp2 = fopen("reducer_output.txt", "w")) == NULL) {
        /*Open the file with WRITE only*/
        printf("The output file cannot open correctly\n");
        exit(EXIT_SUCCESS);
    }
    while (1) {
        pthread_mutex_lock(&mux);
        if (exitflag == 1 && buffer[tid] == 0) {
            printf("The Thread  %d", tid);
            printf("finished !\n");
            pthread_mutex_unlock(&mux);
            break;
        }
        if (buffer[tid] == 0 && exitflag != 1) { // the buffer is empty
            pthread_cond_wait(&empty, &mux);
        }
        if ((buffer[tid] < NUM_SLOTS) && exitflag != 1) {//how to process this condition??
            pthread_cond_broadcast(&full);
            pthread_mutex_unlock(&mux);
        }
        if ((buffer[tid] == NUM_SLOTS) || ((buffer[tid] > 0) && (exitflag == 1))) {
            for (int i = 0; i < count; i++) {
                for (int k = i; k < count; k++) {
                    if ((turple[i].buffer_num == tid) && (turple[k].buffer_num == tid)&&(turple[i].subject == turple[k].subject)) {
                        turple[i].score = turple[i].score + turple[k].score;
                        turple[k].buffer_num =100;//change the subject and will not be used again.
                        buffer[tid]--;
                        pthread_cond_signal(&full);
                        pthread_mutex_unlock(&mux);
                    }
                }

            }
            for (int i = 0; i < count; i++) {
                if (turple[i].buffer_num == tid) {
                    fputs(turple[i].buffer_name, fp2);
                    fputs(turple[i].subject, fp2);
                    char str[3];
                    sprintf(str, "%d", turple[i].score);
                    fputs(str, fp2);
                    fputs("\n", fp2);
                    buffer[tid]--;
                    pthread_cond_signal(&full);
                    pthread_mutex_unlock(&mux);
                }
            }
        }
    }
}
int main(int argc,char *argv[]){
    //if(argc<4){
    //    printf("The Number of Arguments is Not Right\n");
    //    exit(EXIT_SUCCESS);
    //}
    int NUM_SLOTS=atoi(argv[1]);
    int NUM_THREADS=atoi(argv[2]);
    //int NUM_SLOTS=10;
    //int NUM_THREADS=5;
    printf("the num of thread is %d\n",NUM_THREADS);
    printf("the num of slots is %d\n",NUM_SLOTS);
    pthread_t mapper_thread;
    pthread_t threads[NUM_THREADS];// The number of Reducer
    int rc;
    int t;
    int p=0;
    pthread_create(&mapper_thread,NULL,mapper,(void *)&p);
    for(t=0;t<NUM_THREADS;t++){
        printf("Creating the reducer thread %d\n",t);
        rc=pthread_create(&threads[t],NULL,reducer,(void *)&t);
        if(rc!=0){
            printf("Thread create fail!\n");
        }
        else{
            printf("In Main: Creating Reducer Thread ID: %d\n",t);
        }

    }
    pthread_join(mapper_thread,NULL);
    for(int i=0;i<NUM_THREADS;i++){
        pthread_join(threads[t],NULL);
    }
    pthread_mutex_destroy(&mux);
    pthread_cond_destroy(&full);
    pthread_cond_destroy(&empty);
}
