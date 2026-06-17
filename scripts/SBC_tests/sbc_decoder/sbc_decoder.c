
#include"sbc/include/sbc.h"
#include "sbc_decoder.h"

#define BITRATE 32000

static sbc_t sbc;
static struct sbc_frame frame;


/*

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

*/


/*

int read_wav_header(FILE *file, WavHeader *header)
{
    fseek(file, 0, SEEK_SET);
    size_t read = fread(header, sizeof(WavHeader), 1, file);
    return (read == 1);
}

*/
/*
int encode(lc3_encoder_t enc){
    
    FILE *fin = fopen("C:/Users/Luis/OneDrive/Laptop-Desktop/Coding/C/Opus_tests/wav inputs/balatro.wav","rb");
    FILE *fout = fopen("C:/Users/Luis/OneDrive/Laptop-Desktop/Coding/C/lc3_tests/lc3_test1/output_enc.lc3","wb");

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

*/

int decode(const char *input_file){
    char full_fname[100] = "Decoded_audios/";
    char full_fnameOut[100] = "Decoded_audios/";
    strcat(full_fname,input_file);
    strcat(full_fname,".sbc");
    
    strcat(full_fnameOut,input_file);
    strcat(full_fnameOut,"Dec.raw");
    FILE *fin = fopen(full_fname, "rb");
    if (!fin)
    {
        printf("Error opening input file\n");
        return -1;
    }

    FILE *fout = fopen(full_fnameOut, "wb");
    if (!fout)
    {
        printf("Error opening output file\n");
        fclose(fin);
        return -1;
    }


    frame.msbc=true;
    frame.mode = SBC_MODE_MONO;
    frame.freq = SBC_FREQ_16K;
    frame.bam = SBC_BAM_LOUDNESS;
    frame.nsubbands = 8;
    frame.nblocks = 15;
    frame.bitpool = 26;
    

    int frames_decoded = 0;
    int error = 0;
    int16_t PCM_DATA[120];
    
    //int nbytes = lc3_frame_bytes(10000,BITRATE);
    //printf("nbytes= %d",nbytes);

    //int nerrors = 0;
    int8_t enc_buffer[100];
    sbc_reset(&sbc);

    printf("frame size %d ",sbc_get_frame_size(&frame));
    while(!feof(fin)){
        fread(enc_buffer,sizeof(int8_t),57,fin);
        error = sbc_decode(&sbc,enc_buffer,sizeof(enc_buffer),&frame,PCM_DATA,1,PCM_DATA,1);
        if(error != 0){
            printf("DECODER ERROR %d ",error);
            //nerrors++;
        }else{
            frames_decoded++;
        }
        
        
        fwrite(PCM_DATA,sizeof(int16_t),120,fout);


    }

   // printf("\nNerrors: %d   Frames decoded: %d\n",nerrors,frames_decoded);


    return 0;


}

/*


int main(){
    
encoder = lc3_setup_encoder(10000,16000,0,&encoder_mem);

decoder = lc3_setup_decoder(10000,16000,0,&decoder_mem);
if(encoder == NULL){
    printf("NULL ENCODER \n");
    return -1;
}
encode(encoder);
decode(decoder);


return 0;

}
*/