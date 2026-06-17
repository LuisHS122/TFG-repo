#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include"parser.h"
#define MESSAGE_BYTES 40*5+5

void rightShift(char *str, char new_char){
    if (str == NULL || strlen(str) == 0) {
        return;
    }
    
    int len = strlen(str);
    
    // Shift all characters one position to the right
    for (int i = len - 1; i > 0; i--) {
        str[i] = str[i - 1];
    }
    
    
    str[0] = new_char;  

}

int parse(char *file_name){
    
    char full_fname[100] = "";
    char fname_out[100] = "Decoded_audios/";
    strcat(fname_out,file_name);
    strcat(fname_out,".sbc");

    strcat(full_fname,file_name);
    strcat(full_fname,".txt");
    printf("Opening files to parse fin: %s  fout: %s \n",full_fname,fname_out);
    FILE *fin = fopen(full_fname, "rb");
    FILE *fout = fopen(fname_out, "wb");
    unsigned char last_5_bytes[6] = {'0','0','0','0','0','\0'};
    uint16_t bytesWriten = 0;
    unsigned char byte;
    unsigned char bytes[3];
    int temp;
    char toWrite;
    long hex;
    int i = 0;
    bool endOfPacket = false;
    if (!fin || !fout)
    {
        printf("Error opening files\n");
        if (fin)
            printf("Closing fin.\n");
            fclose(fin);
        if (fout)
            printf("Closing fout.\n");
            fclose(fout);
        return -1;
    }

    while(!feof(fin)){
        fread(&byte,sizeof(byte),1,fin);
        rightShift(last_5_bytes,byte);
        //printf("%s\n",last_5_bytes);
        if(strcmp(last_5_bytes,"12-C1") == 0){
            fread(&byte,sizeof(byte),1,fin); //reads the next '-'
            printf("%c",byte);
            endOfPacket = false;
            
            while(endOfPacket == false){
                //fscanf(fin,"%x",&temp); // reads the next 2 chars that form the hex value.
                fread(&bytes,2,1,fin); //fread because fscanf was giving errors. It will read 2 bytes and store them in a 2 position array ended with \0.
                bytes[2] = '\0';
                printf("%s",bytes);
                hex = strtol(bytes,NULL,16);
                //printf(" %ld",hex);
                toWrite = (char)hex;
                //fwrite(&hex,sizeof(hex),1,fout);
                fwrite(&toWrite,sizeof(toWrite),1,fout);
                //printf("write complete ");
                bytesWriten++;
                fread(&byte,sizeof(byte),1,fin);
                //printf("%c",byte);
                if(byte != '-') endOfPacket = true;
                //scanf("%c",&byte);
                
            }
            //scanf("%c",byte);
            
            //printf("%s\n",last_5_bytes);
            //printf("%d, %d",ferror(fin),feof(fin));
            //scanf("%c",byte);

        }
        
    }
    printf("%d, %d",ferror(fin),feof(fin));
    fclose(fin);
    fclose(fout);
    if(bytesWriten < 400){
        
        printf("Bytes writen: %d \n",bytesWriten);
        return -1;
    }
    
    return 0;
}