#include<pthread.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<math.h>
#include<string.h>
#include <pthread.h>
#include <stdio.h>

char **space;
int end = 0;
int number_of_buffer = 7;
int length = 26;
int number_of_slot = 4;
int token = 100;
int *pointer,*done,*pos;

static pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t full=PTHREAD_COND_INITIALIZER;
static pthread_cond_t empty=PTHREAD_COND_INITIALIZER;

void adjust(char A, char In[])
{
    char *P = ",50)";
    char *L = ",20)";
    char *D = ",-10)";
    char *C = ",30)";
    char *S = ",40)";
    char *p,*q,*m,*n;
    int i = 0;

    switch(A)
    {
        case 'P': m = P;break;
        case 'L': m = L;break;
        case 'D': m = D;break;
        case 'C': m = C;break;
        case 'S': m = S;break;
        default: break;
    }

    for(p=In,q=In;*p!='\0';p++)
    {
        i++;
        *q=*p;
        if(i!=7 && i!=8)
        {
            q++;
        }
    }
    *q = '\0';

    for(p=In,n=In;*(p+1)!=')';p++)
    {
        *n=*p;
        n++;
    }

    for(m;*m!='\0';m++)
    {
        *n=*m;
        n++;
    }
    *n = '\n';
    n++;
    *n = '\0';
}

void* mapper(void* param)
{
    int line = 0,i,j,ok=0;
    char output[length],ID[4],input[length],IDs[number_of_buffer][4];
    char x;
    int start[number_of_buffer];
    char *file = (char *) param;
    FILE *pToFileForm = fopen(param, "r");
    FILE *f = fopen("mapper_output.txt", "w");
    for(i=0; i<number_of_buffer; i++)
    {
        for(j=0; j<4; j++)
        {
            IDs[i][j] = 'x';
        }
        start[i] = 0;
    }

    printf("input.txt: \n");
    while(1)
    {
        pthread_mutex_lock(&lock);
        while(fgets(input, length, pToFileForm))
        {
            line++;

            for (i = length; i--; i > 0) {
                output[i] = input[i];
            }

            char *add = &input[6];
            x = *add;
            for(i=1; i<5; i++)//transmit the user name from the input file!
            {
                ID[i-1] = input[i];
            }
            adjust(x, output);

            for(i=0; i<number_of_buffer; i++) //fill the data into the right buffer!
            {
                if (ID[3] == IDs[i][3])
                {
                    for (j = pos[i]; j < length+pos[i]; j++)
                    {
                        if(output[j-pos[i]]=='\n')
                        {
                            space[i][j] = '\n';
                            pos[i] = j+1;
                            break;
                        }
                        else
                            space[i][j] = output[j-pos[i]];
                    }

                    ok = 1;
                    pointer[i]++;
                }
            }
            i = 0;
            while(ok == 0)
            {
                if (start[i] == 0)
                {
                    for (j = pos[i]; j < length+pos[i]; j++)
                    {
                        if(output[j-pos[i]]=='\n')
                        {
                            space[i][j] = '\n';
                            pos[i] = j+1;
                            break;
                        }
                        else
                            space[i][j] = output[j-pos[i]];
                    }
                    for (j = 0; j < 4; j++)
                    {
                        IDs[i][j] = ID[j];
                    }
                    ok = 1;
                    pointer[i]++;
                    start[i] = 1;
                }
                i++;
            }
            if(ok == 1)
            {
                char *text = output;
                printf(output);
                fprintf(f, output, text);
                fflush(f);
            }
            ok = 0;
            for(i=0; i<number_of_buffer; i++)//3. Full: if there exists any buffer is full!
            {
                while(pointer[i] == number_of_slot)
                {
                    token = i;
                    pthread_cond_broadcast(&empty);
                    pthread_cond_wait(&full, &lock);
                }
            }
        }
        end = 1;

        token = 0;
        pthread_cond_broadcast(&empty);
        pthread_mutex_unlock(&lock);
        break;
    }

    fclose(f);
    fclose(pToFileForm);
    printf("Bye,Mapper!\n");
    return 0;
}

void* reducer(void* param)
{
    int check = (int) param;
    int g = 0,op = 0;
    int ip,jp;
    //int line = 0;
    char save[50][50];
    char compare[7];
    char standard[7];
    for (ip = 0; ip < 50; ip++)//initialize all the data of the array
    {
        for (jp = 0; jp < 50; jp++) {
            save[ip][jp] = '0';
        }
    }
    while(1)
    {
        pthread_mutex_lock(&lock);

        if(token == check)
        {
            char x;
            int s = 0;
            while(pointer[check] == 0) {
                g=0;
                pos[check]=0;
                pthread_cond_broadcast(&full);
                pthread_cond_wait(&empty, &lock);
                if(end == 1 && token == check)
                    break;
            }
            if(pointer[check] != 0) {
                for (; space[check][g] != '\n'; g++, s++) {
                    save[op][s] = space[check][g];
                }
                g++;
                s++;
                pointer[check]--;
                save[op][s] = '\0';
            }
            if (end == 1 && pointer[check] == 0)
            {
                pos[check] = 0;
                int j=0,k=0,l=0,t=0,y=0,z=0;
                for(j=0;j<50;j++)
                {
                    if(save[j][0]=='X')
                        j++;
                    for(l=0;l<7;l++)
                    {
                        standard[l]=save[j][1+l];
                    }
                    for(t=j+1;t<50;t++)
                    {
                        for(z=0;z<7;z++)
                        {
                            compare[z]=save[t][1+z];
                        }
                        int h=1,o=0;
                        if((h=strcmp(compare,standard))==0)
                        {
                            int num1,num2;
                            if(save[j][21]=='-')
                            {
                                num1 = -1;
                                num1 = num1*((save[j][22]-'0')*10+save[j][23]-'0');
                            }
                            else
                            {
                                num1 = 1;
                                num1 = num1*((save[j][21]-'0')*10+save[j][22]-'0');
                            }
                            if(save[t][21]=='-')
                            {
                                num2 = -1;
                                num2 = num2*((save[t][22]-'0')*10+save[t][23]-'0');
                            }
                            else
                            {
                                num2 = 1;
                                num2 = num2*((save[t][21]-'0')*10+save[t][22]-'0');
                            }
                            num1 = num1 + num2;
                            //printf("%d\n",num1);
                            if(num1<0)
                            {
                                save[j][22]='-';
                                num1=num1*(-1);
                                save[j][22]=num1/10+'0';
                                save[j][23]=num1%10+'0';
                                save[j][24]=')';
                                save[j][25]='\0';
                            }
                            else
                            {
                                save[j][21]=num1/10+'0';
                                save[j][22]=num1%10+'0';
                                save[j][23]=')';
                                save[j][24]='\0';
                            }
                            save[t][0]='X';
                        }
                    }
                }
                int r=0;
                FILE *fp = fopen("reducer_output.txt", "a");

                while(r!=50)
                {
                    if(save[r][0]!='X' && save[r][10] != '0')
                    {
                        int d=0,f=0,g=0;
                        char mid[50];
                        mid[0]='(';
                        for(d=1;save[r][d-1]!=')';d++)
                        {
                            mid[d]=save[r][d];
                        }
                        mid[d]='\n';
                        mid[d+1]='\0';
                        char *text = mid;
                        fprintf(fp,mid,text);
                        fflush(fp);
                    }
                    r++;
                }
                fclose(fp);
                done[check] = 1;

            }
            op++;
        }
        if(done[check] == 1)
        {
            //printf("%d\n",token);
            pthread_cond_broadcast(&empty);
            token++;
            if(token >= number_of_buffer)
                token = 0;
            pthread_mutex_unlock(&lock);
            break;
        }
        pthread_mutex_unlock(&lock);

    }
    printf("Bye!");
    printf("%d\n",check);
    return 0;
}

int main(int argc, char *argv[]) {

    char *x, *y;
    int i, l, j, whole_size;

    if (argc < 4) {
        perror("Must pass the arguments!");
    }

    x = argv[1];
    y = argv[2];

    if (*(x + 1) != 0)
        number_of_slot = (*x - '0') * 10 + (*(x + 1) - '0');
    else
        number_of_slot = *x - '0';

    if (*(y + 1) != 0)
        number_of_buffer = (*y - '0') * 10 + (*(y + 1) - '0');
    else
        number_of_buffer = *y - '0';


    whole_size = 400;
    int count[number_of_buffer],over[number_of_buffer],situation[number_of_buffer];
    space = (char**)malloc(sizeof(char*)*number_of_buffer);//build dynamic space
    for (i = 0; i < number_of_buffer; ++i)
    {
        space[i] = (char*)malloc(sizeof(char)*whole_size);
    }
    for (i = 0; i < number_of_buffer; i++)//initialize all the data of the array
    {
        for (j = 0; j < whole_size; j++) {
            space[i][j] = '0';
        }
    }
    for (i = 0; i < number_of_buffer; ++i)
    {
        count[i] = 0;
        over[i] = 0;
        situation[i] = 0;
    }
    pointer = count;
    done = over;
    pos = situation;
    /* Assign storage and initialize values */

    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&full, NULL);
    pthread_cond_init(&empty, NULL);
    printf("Start!\n");

    pthread_t producer, consumer[number_of_buffer];

    pthread_create(&producer, NULL, mapper, (void *) argv[3]);
    for (i = 0; i < number_of_buffer; i++) {
        pthread_create(&consumer[i], NULL, reducer,(void *)i);
    }

    pthread_join(producer, NULL);
    for (i = 0; i < number_of_buffer; i++) {
        pthread_join(consumer[i], NULL);

    }

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&full);
    pthread_cond_destroy(&empty);
    free(space);//free the space
    printf("Finish!");
    return 0;
}

