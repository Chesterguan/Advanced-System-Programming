//
// Created by chester on 1/20/18.
//
#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include<math.h>
int main(){
    FILE *fp1,*fp2;
    char str[26];
    char name_temp[4];
    char name[50][5];
    char subject_temp[15];
    char subject[50][16];
    char score_temp[3];
    char score[50][4];
    int counts=0;
    if((fp1=fopen("mapper_output.txt","r"))==NULL){
        /*Open the file with READ only */
        printf("The input file cannot open correctly/n");
        exit(EXIT_SUCCESS);
    }
    if((fp2=fopen("reducer_output.txt","w"))==NULL){
        /*Open the file with WRITE only*/
        printf("The output file cannot open correctly/n");
        exit(EXIT_SUCCESS);
    }
    int count=0;
    while(fgets(str,200,fp1)){
        for(int i=0;i<4;i++){
            name_temp[i]=str[1+i];
        }
        strcpy(name[count],name_temp);
        name[count][5]='\0';
        for(int k=0;k<15;k++){
            subject_temp[k]=str[6+k];
        }
        strcpy(subject[count],subject_temp);
        subject[count][16]='\0';
        for(int p=0;p<3;p++){
            score_temp[p]=str[22+p];
        }
        strcpy(score[count],score_temp);
        score[count][4]='\0';
        //printf("%s\n",name[count]);
        //printf("%s\n",subject[count]);
        //printf("%s\n",score[count]);
        count++;
        counts=count;
    }
    for(int i=0;i<counts-1;i++){
        for (int j=i+1;j<counts;j++){
            if((strcmp(name[i],name[j])==0)&&(strcmp(subject[i],subject[j])==0)){
                int sum;
                int temp1;
                int temp2;
                int temp3;
                int temp4;
                int temp5;
                int temp6;
                if(score[i][0]!='-'){
                    temp1=1;
                    temp2=score[i][0]-'0'; // dimen
                    temp3=score[i][1]-'0';// cent
                }
                else{
                    if(score[i][0]=='-'){
                        temp1=-1;
                        temp2=score[i][1]-'0';
                        temp3=score[i][2]-'0';
                    }
                }
                if(score[j][0]!='-'){
                    temp4=1;
                    temp5=score[j][0]-'0'; // dimen
                    temp6=score[j][1]-'0';// cent
                }
                else{
                    if(score[j][0]=='-'){
                        temp4=-1;
                        temp5=score[j][1]-'0';
                        temp6=score[j][2]-'0';
                    }
                }
                sum=temp1*(10*temp2+1*temp3)+temp4*(10*temp5+1*temp6);
                //printf("%d\n",sum);
                char point_temp[3];
                sprintf( point_temp,"%d", sum);
                //point_temp[4]='\0';
                strcpy(score[i],point_temp);
                name[j][0]='X';
            }
            
        }
    }
    for(int i=0;i<50;i++){
        if(name[i][0]!='X'){
            fputs("(",fp2);
            fputs(name[i],fp2);
            fputs(",",fp2);
            fputs(subject[i],fp2);
            fputs(",",fp2);
            fputs(score[i],fp2);
            fputs(")",fp2);
            fputs("\n",fp2);
        }

    }

}
