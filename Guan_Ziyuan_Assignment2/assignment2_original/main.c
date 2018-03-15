#include <stdio.h>
#include<pthread.h>
#include <stdlib.h>
#include<string.h>
#define NUM_THREADS 5 // number of reducer
char *addr;
int flag[NUM_THREADS]={0};// create a statement flag
int temp[NUM_THREADS]={0};
int exitflag=0;
FILE *fp1,*fp2;
char buffer[NUM_THREADS][500];
char result[NUM_THREADS][500];//used to store the results
char score2[3];
pthread_mutex_t mux=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full;
pthread_cond_t empty;
/*
struct thread_data{
    int thread_id;
    int sum;
    char *message;
};
 */
//struct thread_data thread_data_array[NUM_THREADS];//reducer
void *mapper(void * NUM_SLOTS){ // create the mapper thread
    printf("Enter the Mapper Thread\n");
    char str[26];
    char name_buffer[NUM_THREADS][4];
    for(int i=0;i<NUM_THREADS;i++){
        for(int p=0;p<4;p++){
            name_buffer[i][p]=buffer[i][1+p];
        }
    }
    //int NUM_S=*(int *)NUM_SLOTS;
    int NUM_S=*(int *) NUM_SLOTS;
    while(fgets(str,26,fp1)){  //judge whether the file is end or not
        pthread_mutex_lock(&mux);
        for(int i=0;i<NUM_THREADS;i++){
            if(buffer[i][NUM_S*26]!='X'){  // the buffer[i] is full
                char bname[4];
                for(int k=1;k<=4;k++){
                    bname[k-1]=buffer[i][k];
                }
                printf("The Buffer for ID :");
                printf("%s",bname);
                printf(" is  full! Mapper Waits");
                pthread_cond_wait(&full,&mux);// mapper_thread wait.
                flag[i]=1;
            }
        } // all of the buffer are available
        char name[4];
        for(int k=0;k<4;k++){
            name[k]=str[1+k];
        }

        for(int m=0;m<NUM_THREADS;m++){
            if(buffer[m][0]=='X'){  // empty buffer
                temp[m]=1;
                for(int c=0;c<26;c++){
                    buffer[m][c]=str[c];//store the new turple;
                }
            }
            else{
                if((strcmp(name,name_buffer[m])==0)&&(buffer[m][NUM_S*26]!='X')){
                    for(int l=0;l<NUM_S*26;l++){
                        if(buffer[m][l]=='X'){
                            for(int c=0;c<26;c++){
                                buffer[m][l+c]=str[c];//store the new turple;
                            }
                        }
                    }
                }
            }
        }
        pthread_cond_broadcast(&empty);
        pthread_mutex_unlock(&mux);
    }
    pthread_mutex_unlock(&mux);
    printf("The Input File has all been registered!");
    printf("The Mapper is exiting");
    // let the reducer know and finish the program
    //pthread_cond_broadcast(&empty);
    exitflag=1;

}
void *reducer(void *threadid){
    /*struct thread_data *my_data;
    long tid;
    my_data=(struct thread_data *) threadarg;
    int taskid = my_data->thread_id;
    int sum=my_data->sum;
    char *msg=my_data->message;
    */
    int tid=*(int *)threadid;
    printf("Enter the Reducer Thread ID:  %d\n",tid);
    while(1){
        pthread_mutex_lock(&mux);
        if(exitflag==1&&temp[tid]==1){
            printf("The Thread  %d",tid);
            printf("finished !\n");
            pthread_mutex_unlock(&mux);
            break;
        }
        if(temp[tid]==1&&exitflag!=1){
            pthread_cond_wait(&empty,&mux);
        }
        if(flag[tid]==0&&temp[tid]==0){
            pthread_cond_wait(&empty,&mux);
        }
        if(flag[tid]==1) {
            int count = 0;
            while (count < 260) {
                for (int t = 0; t <= 5; t++) {
                    result[tid][t + count] = buffer[tid][t + count];
                }
                char score_buffer = buffer[tid][count + 6];
                int score = 0;
                switch (score_buffer) {
                    case 'P':
                        score = 50;
                        break;
                    case 'L':
                        score = 20;
                        break;
                    case 'D':
                        score = -10;
                        break;
                    case 'C':
                        score = 30;
                        break;
                    case 'S':
                        score = 40;
                        break;
                    default:
                        printf("Input is wrong");
                        break;
                }
                sprintf(score2, "%d", score);
                for (int k = 0; k < 15; k++) {
                    result[tid][count + 6 + k] = buffer[tid][count + 8 + k];
                }
                result[tid][count + 21] = ',';
                for (int n = 0; n < 3; n++) {
                    result[tid][count + 21 + n] = score2[n];
                }
                result[tid][count + 24] = ')';
                printf("the result is : %s\n", result[tid]);
                fputs(result[tid],fp2);
                fputs("\n",fp2);
                count = count + 26;
            }
            printf("the buffer ID: %d",tid);
            printf(" is cleared ! \n");
            pthread_cond_signal(&full);
            pthread_mutex_unlock(&mux);
        }
    }

}
int main(int argc,char *argv[]) {
    char str[26];
 //   if(argc<3){
 //       printf("The Number of Arguments is Not Right");
 //       exit(EXIT_SUCCESS);
 //   }
 //   int NUM_SLOTS=atoi(argv[1]);
    int NUM_SLOTS=10;
    for(int i=0;i<NUM_THREADS;i++){
        for(int j=0;j<NUM_SLOTS*26;j++){
            buffer[i][j]='X';
        }
    }
    printf("The last digit is ");
    printf("%c\n",buffer[4][259]);
    addr=argv[2];
    //argv[3]stores the input file
    if((fp1=fopen(addr,"r"))==NULL){
        /*Open the file with READ only */
        printf("The input file cannot open correctly\n");
        exit(EXIT_SUCCESS);
    }
    if((fp2=fopen("mapper_output.txt","w"))==NULL){
        /*Open the file with WRITE only*/
        printf("The output file cannot open correctly\n");
        exit(EXIT_SUCCESS);
    }
    pthread_mutex_init(&mux, NULL);
    pthread_cond_init(&full, NULL);
    pthread_cond_init(&empty, NULL);
    pthread_t mapper_thread;
    pthread_t threads[NUM_THREADS];// The number of Reducer
    int rc;
    int t;
    pthread_create(&mapper_thread,NULL,mapper, (void *)&NUM_SLOTS);
    for(t=0;t<NUM_THREADS;t++){
        printf("Creating the thread %d\n",t);
        rc=pthread_create(&threads[t],NULL,reducer,(void *)&t);
        printf("In Main: Creating Reducer Thread ID: %d\n",t);
    }
    pthread_join(mapper_thread,NULL);
    for(int i=0;i<NUM_THREADS;i++){
        pthread_join(threads[t],NULL);
    }

}
