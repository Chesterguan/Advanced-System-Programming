#include<pthread.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<semaphore.h>
#include<sys/mman.h>

char **buffer;
int *pointer,*pos,*num,*end;
int number_of_buffer = 7;
int buffer_size = 26;
int whole_size;
int number_of_slot = 4;
static sem_t *mutex, *full, *empty;

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

void output(int check,char save[50][50])
{
    pos[check] = 0;
    int j=0,k=0,l=0,t=0,y=0,z=0;
    char compare[15];
    char standard[15];
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
}

void mapper(void)
{
    //char *file = (char *) param;
    int i,j,ok = 0;
    char x;
    char output[buffer_size],ID[4],input[buffer_size],IDs[number_of_buffer][4];
    int start[number_of_buffer];
    //FILE *pToFileForm = fopen("input.txt", "r");
    FILE *f = fopen("mapper_output.txt", "w");
    for(i=0; i<number_of_buffer; i++)
    {
        for(j=0; j<4; j++)
        {
            IDs[i][j] = 'x';
        }
        start[i] = 0;
    }

    while(fgets(input, buffer_size, stdin))
    {
        sem_wait(&empty[0]);
        sem_wait(&mutex[0]);
        msync(empty,1,MS_SYNC);
        msync(mutex,1,MS_SYNC);
        printf("Enter The Mapper!\n");
        for (i = buffer_size; i--; i > 0)
        {
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
                for (j = pos[i]; j < buffer_size+pos[i]; j++)
                {
                    if(output[j-pos[i]]=='\n')
                    {
                        buffer[i][j] = '\n';
                        pos[i] = j+1;
                        break;
                    }
                    else
                        buffer[i][j] = output[j-pos[i]];
                }

                ok = 1;
                pointer[i]++;
                msync(pointer[i],1,MS_SYNC);
                msync(pos[i],1,MS_SYNC);
                msync(buffer[i], buffer_size, MS_SYNC);
            }
        }
        i = 0;
        while(ok == 0)
        {
            if (start[i] == 0)
            {
                for (j = pos[i]; j < buffer_size+pos[i]; j++)
                {
                    if(output[j-pos[i]]=='\n')
                    {
                        buffer[i][j] = '\n';
                        pos[i] = j+1;
                        break;
                    }
                    else
                        buffer[i][j] = output[j-pos[i]];
                }
                for (j = 0; j < 4; j++)
                {
                    IDs[i][j] = ID[j];
                }
                ok = 1;
                pointer[i]++;
                start[i] = 1;
                msync(pointer[i],1,MS_SYNC);
                msync(pos[i],1,MS_SYNC);
                msync(buffer[i], buffer_size, MS_SYNC);
            }
            i++;
        }
        if(ok == 1)
        {
            char *text = output;
            //printf(output);
            fprintf(f, output, text);
            fflush(f);
        }
        ok = 0;

        for(i=0; i<number_of_buffer; i++)//3. Full: if there exists any buffer is full!
        {
            if(pointer[i] == number_of_slot)
            {
		printf("The Buffer %d is full ! \n",i);
                sem_post(&mutex[0]);
                sem_post(&full[i]);
                msync(full,number_of_buffer,MS_SYNC);
                msync(mutex,1,MS_SYNC);
                break;
            }
            else if(i==(number_of_buffer-1))
            {
                sem_post(&mutex[0]);
                sem_post(&empty[0]);
                msync(empty,1,MS_SYNC);
                msync(mutex,1,MS_SYNC);
            }
        }
    }
    sleep(1);
    end[0] = 1;
    msync(end,1,MS_SYNC);
    sem_post(&full[0]);
    fclose(f);
    printf("Finish the Mapper!\n");
    exit(0);
}

void reducer(int param)
{   
    int check = param;
    printf("Enter the Consumer : %d\n",check);
    int g = 0,op = 0;
    int ip,jp;
    char save[50][50];
    for (ip = 0; ip < 50; ip++)
    {
        for (jp = 0; jp < 50; jp++) {
            save[ip][jp] = '0';
        }
    }
    while(1)
    {
        sem_wait(&full[check]);
        sem_wait(&mutex[0]);
        msync(full,number_of_buffer,MS_SYNC);
        msync(mutex,1,MS_SYNC);
        int s = 0;
        if(pointer[check] != 0)
        {
            for (; buffer[check][g] != '\n'; g++, s++)
            {
                save[op][s] = buffer[check][g];
            }
            g++;
            s++;
            pointer[check]--;
            save[op][s] = '\0';
            msync(pointer[check],1,MS_SYNC);
        }
        if(pointer[check] == 0)
        {
            pos[check]=0;
            msync(pos[check],1,MS_SYNC);
            g=0;
            if(end[0] == 1)
            {
                output(check,save);
                if(check < number_of_buffer)
                {
                    sem_post(&full[check+1]);
                    sem_post(&mutex[0]);
                    msync(full,number_of_buffer,MS_SYNC);
                    msync(mutex,1,MS_SYNC);
                }
                break;
            }
            sem_post(&mutex[0]);
            sem_post(&empty[0]);
            msync(empty,1,MS_SYNC);
            msync(mutex,1,MS_SYNC);
        }
        else
        {
            sem_post(&mutex[0]);
            sem_post(&full[check]);
            msync(full,number_of_buffer,MS_SYNC);
            msync(mutex,1,MS_SYNC);
        }
        op++;
    }
    printf("Condumer : %d Finished!\n",check);
    exit(0);
}

int main(int argc, char *argv[])
{
    int i,j,status;
    char *x, *y;
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


    whole_size = number_of_slot * 30;

    buffer = mmap(NULL,number_of_buffer, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    for (i = 0; i < number_of_buffer; i++)
    {
        buffer[i] = mmap(NULL,whole_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    }
    for (i = 0; i < number_of_buffer; ++i)
    {
        msync(buffer[i],whole_size,MS_SYNC);
    }

    pointer = mmap(NULL,number_of_buffer, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    pos = mmap(NULL,number_of_buffer, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    for (i = 0; i < number_of_buffer; ++i)
    {
        pointer[i] = 0;
        pos[i] = 0;
    }
    msync(pointer,number_of_buffer,MS_SYNC);
    msync(pos,number_of_buffer,MS_SYNC);
    num = mmap(NULL,1, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    num[0] = 0;
    msync(num,1,MS_SYNC);
    end = mmap(NULL,1, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    end[0] = 0;
    msync(end,1,MS_SYNC);

    printf("Coding Start!\n");
    pid_t producer, consumer[number_of_buffer];

    mutex = mmap(NULL,1, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    empty = mmap(NULL,1, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    full = mmap(NULL,number_of_buffer, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    sem_init(&mutex[0],1,1);
    sem_init(&empty[0],1,1);
    for (i = 0; i < number_of_buffer; i++)
    {
        sem_init(&full[i],1,0);
    }
    msync(full,number_of_buffer,MS_SYNC);
    msync(empty,1,MS_SYNC);
    msync(mutex,1,MS_SYNC);
    producer = fork();
    if(producer == 0)
    {
        mapper();
    }
    else
    {
        int o = 1;
        while(o)
        {
            int m = num[0];
            if(m >= number_of_buffer)
                break;
            msync(num,1,MS_SYNC);
            num[0] = num[0] + 1;
            consumer[m] = fork();
            if(consumer[m]==0)
                reducer(m);
        }
    }
    sleep(2);

    for (i = 0; i < number_of_buffer; ++i)
    {
        munmap(buffer[i],whole_size);
    }
    munmap(pointer,number_of_buffer);
    munmap(pos,number_of_buffer);
    munmap(num,1);
    munmap(end,1);
    printf("Coding Finish!\n");
    return 0;
}
