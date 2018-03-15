//
// Created by chester on 1/20/18.
//
#include<stdio.h>
#include<string.h>
#include <stdlib.h>
int main(){
    FILE *fp1,*fp2;
    char str[26];  //since the length of the input line is 25.
    if((fp1=fopen("input.txt","r"))==NULL){
        /*Open the file with READ only */
        printf("The input file cannot open correctly/n");
        exit(EXIT_SUCCESS);
    }
    if((fp2=fopen("mapper_output.txt","w"))==NULL){
        /*Open the file with WRITE only*/
        printf("The output file cannot open correctly/n");
        exit(EXIT_SUCCESS);
    }
    while(fgets(str,26,fp1)){
        char output[26];
        char name[4];
        char score_title[1];
        int score;
        char subject[15];
        output[0]=str[0];
        for(int i=0;i<4;i++){
            name[i]=str[i+1];
            output[i+1]=str[i+1];
        }
        output[5]=',';// add the space
        score_title[0]=str[6];
        for(int j=0;j<15;j++){
            subject[j]=str[8+j];
            output[j+6]=str[8+j];
        }
        /* Change the title to number */
        switch(score_title[0]){
            case 'P':
                score=50;
                break;
            case 'L':
                score=20;
                break;
            case 'D':
                score=-10;
                break;
            case 'C':
                score=30;
                break;
            case 'S':
                score=40;
                break;
            default:
                printf("Input is wrong");
                break;
        }
        for(int j=15;j>=0;j--) {
            if (subject[j] == ' ') {
                subject[j] = '\0';
            }
        }
        char score2[10];
        sprintf(score2,"%d",score);
        output[21]=',';
        for(int j=0;j<strlen(score2);j++){
            output[j+22]=score2[j];
        }
        if(strlen(score2)==2){
            output[24]=' ';
        }
        output[25]=')';
        output[26]='\0';
        fputs(output,fp2);
        fputs("\n",fp2);
        printf("%s\n",output);
        //fputs(output,stdout);
        //fputs("\n",stdout);
    }
    fclose(fp1);
    fclose(fp2);
    return(0);

}
