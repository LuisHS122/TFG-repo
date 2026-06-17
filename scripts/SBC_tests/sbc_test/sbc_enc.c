#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "sbc/include/sbc.h"

#define BITRATE 32000



static sbc_t sbc_enc;
    

//Set up of the sbc frame, sbc.c encoder will internally give vallues to the frame attributes according to the msbc standard.
//Doesn't seem to work, all the frame parameters must be defined.
    
static struct sbc_frame frame;


#pragma pack(push, 1)
typedef struct
{
    char chunk_id[4];
    uint32_t chunk_size;
    char format[4];
    char subchunk1_id[4];
    uint32_t subchunk1_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    char subchunk2_id[4];
    uint32_t subchunk2_size;
} WavHeader;
#pragma pack(pop)





int read_wav_header(FILE *file, WavHeader *header)
{
    fseek(file, 0, SEEK_SET);
    size_t read = fread(header, sizeof(WavHeader), 1, file);
    return (read == 1);
}



int encode(){
    //Input and output directories
    FILE *fin = fopen("","rb");
    FILE *fout = fopen("","wb");

    if (!fin || !fout)
    {
        printf("Error opening files\n");
        if (fin){
            fclose(fin);
            printf("fout");
        }
        if (fout){
            fclose(fout);
            printf("fin");
        }    
        return -1;
    }

    WavHeader header;
    if (!read_wav_header(fin, &header))
    {
        printf("Error reading WAV header\n");
        fclose(fin);
        fclose(fout);
        return -1;
    }

    printf("WAV File Info:\n");
    printf("  Channels: %d\n", header.num_channels);
    printf("  Sample Rate: %d\n", header.sample_rate);
    printf("  Bits per Sample: %d\n", header.bits_per_sample);
    printf("  Data Size: %d bytes\n", header.subchunk2_size);

    int frames_encoded = 0;
    int error = 0;
    int16_t PCM_DATA[120];

    //int nbytes = lc3_frame_bytes(10000,BITRATE);
    //printf("nbytes= %d",nbytes);
    sbc_reset(&sbc_enc);

    int8_t enc_buffer[400];
    while(!feof(fin)){
        fread(PCM_DATA,sizeof(int16_t),120,fin);
        error = sbc_encode(&sbc_enc, PCM_DATA,1,PCM_DATA,1,&frame,enc_buffer,sizeof(enc_buffer));

        if(error != 0){
            printf("ENCODER ERROR, %d ",error);
        }

        frames_encoded++;
        printf("Frame encoded, bytes written %d",sbc_get_frame_size(&frame));
        fwrite(enc_buffer,sizeof(int8_t),sbc_get_frame_size(&frame),fout);

    }
    printf("frames decoded: %d \n",frames_encoded);

    return 0;

}

int decode(){
    FILE *fin = fopen("","rb");
    FILE *fout = fopen("","wb");

    if (!fin || !fout)
    {
        printf("Error opening files\n");
        if (fin)
            fclose(fin);
        if (fout)
            fclose(fout);
        return -1;
    }

    

    int frames_decoded = 0;
    int error = 0;
    int16_t PCM_DATA[120];
    uint8_t bytes[5];
    

    sbc_reset(&sbc_enc);
    printf("frame size %d",sbc_get_frame_size(&frame));
    int8_t enc_buffer[100];
    while(!feof(fin)){
        //fread(bytes,SBC_PROBE_SIZE,1,fin);
        fread(enc_buffer,sizeof(int8_t),57,fin);

        error = sbc_decode(&sbc_enc,enc_buffer,sizeof(enc_buffer),&frame,PCM_DATA,1,PCM_DATA,1);

        if(error != 0){
            printf("DECODER ERROR");
        }
        frames_decoded++;
        
        fwrite(PCM_DATA,sizeof(int16_t),120,fout);

    }
    printf("frames decoded: %d \n",frames_decoded);

    return 0;


}

int main(){

    sbc_t sbc_enc;
    

    //Set up of the sbc frame, sbc.c encoder will internally give vallues to the frame attributes according to the msc standard.
    
    
    frame.msbc=true;


    //Frame parameters for mSBC
   
    frame.mode = SBC_MODE_MONO;
    frame.freq = SBC_FREQ_16K;
    frame.bam = SBC_BAM_LOUDNESS;
    frame.nsubbands = 8;
    frame.nblocks = 15;
    frame.bitpool = 26;
    

    


    encode();
    decode();


    return 0;

}
