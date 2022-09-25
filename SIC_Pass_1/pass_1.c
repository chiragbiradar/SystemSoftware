/*
*   Author:       Prajwal K. Patil
*   Program:      SIC - Pass 1 
*   Requirements: input.txt 
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define MAX_SIZE 14
#define MAX_OBJ_SIZE 100
#define MAX_OPTAB_SIZE 22

int intial_address = 0;
int start = 0;
int optab_index = -1;

struct obj{
    char label[MAX_SIZE];
    char opcode[MAX_SIZE];
    char operand[MAX_SIZE];
};

struct optab{
    char name[MAX_SIZE];
    long long code;
};

typedef struct obj OBJECT;
typedef struct optab OPTAB;

OPTAB*optab_all = NULL;


void create_optab(){
    if(optab_all == NULL){
        optab_all = (OPTAB*)malloc(sizeof(OPTAB)*22);
    }
    else{
        return;
    }
    char temp_string[5];
    FILE*fp = fopen("optab.txt","r+");
    if(fp == NULL){
        printf("Error ");
        exit(-1);
    }
    while(!feof(fp)){
        optab_index++;
        fscanf(fp,"%s %s",optab_all[optab_index].name,temp_string);
        optab_all[optab_index].code = strtol(temp_string,NULL,16);
    }
    fclose(fp);
}

int add_locctr(OBJECT o){
    if(optab_all == NULL){
        create_optab();
    }
    long long temp_num = 0;
    char temp_str[MAX_SIZE];
    for(int i =0; i<=optab_index;i++){
        if(!strcmp(optab_all[i].name,o.opcode)){
            return 3;
        }
    }
    if(!strcmp(o.opcode,"WORD")){
        return 3;
    }
    else if(!strcmp(o.opcode,"RESW")){
        temp_num = strtol(o.operand,NULL,16);
        return 3 * temp_num;
    }
    else if(!strcmp(o.opcode,"RESB")){
        temp_num = strtol(o.operand,NULL,10);
        return temp_num;
    }
    else if(!strcmp(o.opcode,"BYTE")){
        if(o.operand[0] == 'X'){
            return (int)(strlen(o.operand)-3)/2;
        }
        else if(o.operand[0] == 'C'){
            return (int)(strlen(o.operand)-3);
        }
    }
    else if(!strcmp(o.opcode,"END")){
        return 0;
    }
    else{
        printf("Opcode error! - %s",o.opcode);
        exit(-1);
    }
}

OBJECT insert_tokens(char *s){
    OBJECT obj;
    strcpy(obj.label," ");
    strcpy(obj.operand," ");
    strcpy(obj.opcode," ");

    char *token = NULL;
    char s1[MAX_SIZE];
    char s2[MAX_SIZE];
    char s3[MAX_SIZE];

    token = strtok(s, " ");
    if(token == NULL)
        strcpy(s1," ");
    else
        strcpy(s1,token);


    token = strtok(NULL, " ");
    if(token == NULL)
        strcpy(s2," ");
    else
        strcpy(s2,token);


    token = strtok(NULL, " ");
    if(token == NULL)
        strcpy(s3," ");
    else
        strcpy(s3,token);

    if(!strcmp(s2," ") && !strcmp(s3," ")){
        strcpy(obj.opcode,s1);
    }
    else if(!strcmp(s3," ")){
        strcpy(obj.opcode,s1);
        strcpy(obj.operand,s2);
    }
    else{
        strcpy(obj.label,s1);
        strcpy(obj.opcode,s2);
        strcpy(obj.operand,s3);
    }
    return obj;
}

int main(){
    FILE* fp;
    OBJECT temp_obj;
    OBJECT *objects;
    char temp_string[MAX_SIZE];
    objects = (OBJECT*)malloc(sizeof(OBJECT)*MAX_OBJ_SIZE);
    if(objects == NULL){
        printf("Error in allocating memory!");
        exit(-1);
    }

    fp = fopen("input2.txt","r+"); 
    if(fp == NULL){
        printf("Couldn't open the file.");
        exit(-1);
    }

    int i = 0;
    int obj_index = 0;
    while(!feof(fp)){
        fgets(temp_string,MAX_OBJ_SIZE,fp);
        i = 0;
        while(temp_string[i]!= '\0'){
            if(temp_string[i] == '\n'){
                temp_string[i] = '\0';
                break;
            }
            i++;
        }
        if(temp_string[0] != '.'){
            temp_obj = insert_tokens(temp_string);
            printf("%-10s %-10s %-10s\n",temp_obj.label,temp_obj.opcode,temp_obj.operand);
            objects[obj_index++] = temp_obj;
        }
    }
    fclose(fp);

    FILE*fp2 = fopen("output.txt","w+");
    if(fp2 == NULL){
        printf("Error in opening file");
        exit(-1);
    }
    for(int j =0;j < obj_index;j++){
        if(!strcmp(objects[j].opcode,"START")){
            start = strtol(objects[j].operand,NULL,16);
            intial_address = start;
            fprintf(fp2,"%-10s %-10s %-10s %-10s\n"," ",objects[j].label,objects[j].opcode,objects[j].operand);
        }
        else{
            fprintf(fp2,"%-10X %-10s %-10s %-10s\n",start,objects[j].label,objects[j].opcode,objects[j].operand);
            start += add_locctr(objects[j]);
        }
    }
    fclose(fp2);

    FILE*fp3 = fopen("symtab.txt","w+");
    if(fp3 == NULL){
        printf("Error in opening file");
        exit(-1);
    }
    unsigned int dup_index = 0;
    for (int i =0;i<obj_index;i++){
        for(int j=0;j<obj_index;j++){
            if((!strcmp(objects[i].label,objects[j].label)) && i != j && (strcmp(objects[i].label," "))){
                if(i > j){
                    dup_index = i;
                }
                else{
                    dup_index = j;
                }
                printf("\nError! Redeclaration of Label on Line(%d)\n", dup_index);
                exit(-1);
            }
        }
    }
    for(int j =0;j < obj_index;j++){
        if(!strcmp(objects[j].opcode,"START")){
            start = strtol(objects[j].operand,NULL,16);
        }
        else{
            if(strcmp(objects[j].label," "))
                fprintf(fp3,"%-10X %-10s\n",start,objects[j].label);
            start += add_locctr(objects[j]);
        }
    }
    fclose(fp3);
    printf("\n%d bytes is the size of the program.\n",(start-intial_address));
    printf("Pass 1 completed successfuly.");
    return 0;
}