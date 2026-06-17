#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "lc3plus/lc3plus.h"
//#include"functions.h"

#define BITRATE 32000
//#define DISABLE_HR_MODE
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


/*
LC3PLUS_Enc *enc = NULL;
LC3PLUS_Dec *dec = NULL;
*/


/*
lc3_encoder_mem_16k_t encoder_mem;
lc3_encoder_t encoder;

lc3_decoder_mem_16k_t decoder_mem;
lc3_decoder_t decoder;

*/


int read_wav_header(FILE *file, WavHeader *header)
{
    fseek(file, 0, SEEK_SET);
    size_t read = fread(header, sizeof(WavHeader), 1, file);
    return (read == 1);
}



int encode(LC3PLUS_Enc *enc){
    
    FILE *fin = fopen("","rb");
    FILE *fout = fopen("","wb");
    int error = 0;

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


    printf("u");
    
    error = lc3plus_enc_init(enc,16000,1,NULL);
    printf("a, %d",error);
    if(error != 0) printf("ERROR init %d",error);
    error = lc3plus_enc_set_frame_dms(enc,LC3PLUS_FRAME_DURATION_10MS);
    if(error != 0) printf("ERROR frame %d",error);
    error = lc3plus_enc_set_bitrate(enc,BITRATE);
    if(error != 0) printf("ERROR bitrate %d",error);

    printf("eee");

    int frames_encoded = 0;
    
    int16_t **PCM_DATAS; 
    int16_t PCM_DATA[160];

    PCM_DATAS[0] = PCM_DATA;
    int nbytes = lc3plus_enc_get_num_bytes(enc);
    int consumed = lc3plus_enc_get_input_samples(enc);
    int sratch_size = lc3plus_enc_get_scratch_size(enc);
    //int nbytes = lc3_frame_bytes(10000,BITRATE);
    int scratch[400];
    printf("nbytes= %d,consumed= %d, scrth size= %d \n",nbytes,consumed,sratch_size);


    int8_t enc_buffer[400];

    while(!feof(fin)){
        fread(PCM_DATAS[0],sizeof(int16_t),160,fin);
        //error = lc3_encode(enc,LC3_PCM_FORMAT_S16,PCM_DATA,1,nbytes,enc_buffer);
        error = lc3plus_enc16(enc,PCM_DATAS,enc_buffer,&nbytes,scratch);
        if(error != 0){
            printf("ENCODER ERROR");
        }
        frames_encoded++;
        
        fwrite(enc_buffer,sizeof(int8_t),nbytes,fout);

    }


    return 0;

}

int decode(LC3PLUS_Dec *dec){
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
    lc3plus_dec_init(dec,16000,1,LC3PLUS_PLC_ADVANCED);

    
    lc3plus_dec_set_frame_dms(dec,LC3PLUS_FRAME_DURATION_10MS);
    
    

    int frames_decoded = 0;
    int error = 0;
    int16_t **PCM_DATAS;
    int16_t PCM_DATA[160];

    PCM_DATAS[0] = PCM_DATA;
    int scratch[400];
    //int nbytes = lc3_frame_bytes(10000,BITRATE);
    
    int nbytes = 100;
    int sratch_size = lc3plus_dec_get_scratch_size(dec);
    printf("scratch=  %d",sratch_size);


    int8_t enc_buffer[100];
    while(!feof(fin)){
        fread(enc_buffer,sizeof(int8_t),nbytes,fin);
        //error = lc3_decode(dec,enc_buffer,nbytes,LC3_PCM_FORMAT_S16,PCM_DATA,1);
        error = lc3plus_dec16(dec,enc_buffer,100,PCM_DATAS,scratch,0);
        if(error != 0){
            printf("DECODER ERROR");
        }
        frames_decoded++;
        
        fwrite(PCM_DATAS[0],sizeof(int16_t),160,fout);

    }


    return 0;


}

int main(){
    
    
LC3PLUS_Enc *enc = malloc(LC3PLUS_ENC_MAX_SIZE);    
LC3PLUS_Dec *dec = malloc(lc3plus_dec_get_size(16000,1,LC3PLUS_PLC_ADVANCED));


printf("%d\n",lc3plus_enc_get_size(16000,1));
enc = (LC3PLUS_Enc*)malloc(lc3plus_enc_get_size(16000,1));
if(enc == NULL) printf("aaaa");
dec = malloc(lc3plus_dec_get_size(16000,1,LC3PLUS_PLC_ADVANCED));
encode(enc);
decode(dec);


return 0;

}
