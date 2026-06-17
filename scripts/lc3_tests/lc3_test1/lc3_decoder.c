#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "lc3/include/lc3.h"

#define BITRATE 32000

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


lc3_encoder_mem_16k_t encoder_mem;
lc3_encoder_t encoder;

lc3_decoder_mem_16k_t decoder_mem;
lc3_decoder_t decoder;


int read_wav_header(FILE *file, WavHeader *header)
{
    fseek(file, 0, SEEK_SET);
    size_t read = fread(header, sizeof(WavHeader), 1, file);
    return (read == 1);
}



int encode(lc3_encoder_t enc){
    
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
    int16_t PCM_DATA[160];
    printf("ee");
    int nbytes = lc3_frame_bytes(10000,BITRATE);
    printf("nbytes= %d",nbytes);


    int8_t enc_buffer[400];
    while(!feof(fin)){
        fread(PCM_DATA,sizeof(int16_t),160,fin);

        error = lc3_encode(enc,LC3_PCM_FORMAT_S16,PCM_DATA,1,nbytes,enc_buffer);
        
        if(error != 0){
            printf("ENCODER ERROR");
        }
        frames_encoded++;
        
        fwrite(enc_buffer,sizeof(int8_t),nbytes,fout);

    }


    return 0;

}

int decode(lc3_decoder_t dec){
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
    int16_t PCM_DATA[160];
    int nbytes = lc3_frame_bytes(10000,BITRATE);
    printf("nbytes= %d",nbytes);


    int8_t enc_buffer[nbytes];
    while(!feof(fin)){
        fread(enc_buffer,sizeof(int8_t),nbytes,fin);
        error = lc3_decode(dec,enc_buffer,nbytes,LC3_PCM_FORMAT_S16,PCM_DATA,1);
        if(error != 0){
            printf("DECODER ERROR");
        }
        frames_decoded++;
        
        fwrite(PCM_DATA,sizeof(int16_t),160,fout);

    }


    return 0;


}

int main(){
    
encoder = lc3_setup_encoder(10000,16000,0,malloc(lc3_encoder_size(10000,16000)));

lc3_encoder_disable_ltpf(encoder);


decoder = lc3_setup_decoder(10000,16000,0,malloc(lc3_decoder_size(10000,16000)));



if(encoder == NULL || decoder == NULL){
    printf("NULL ENCODER \n");
    return -1;
}
encode(encoder);
decode(decoder);


return 0;

}
