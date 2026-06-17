#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <opus/opus.h>
#include <opus/opus_custom.h>

#define SAMPLE_RATE 16000
#define CHANNELS 1
#define FRAME_SIZE 320 // 20ms at 16kHz
#define MAX_PACKET_SIZE 4000

// Pragma just in case, so the compiler doesn't add additional bytes in between the wav header

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

// Function to check if file is WAV format
int is_wav_file(FILE *file)
{
    WavHeader header;
    fseek(file, 0, SEEK_SET);
    fread(&header, sizeof(WavHeader), 1, file);

    return (memcmp(header.chunk_id, "RIFF", 4) == 0 &&
            memcmp(header.format, "WAVE", 4) == 0);
}

// Function to read WAV file header
int read_wav_header(FILE *file, WavHeader *header)
{
    fseek(file, 0, SEEK_SET);
    size_t read = fread(header, sizeof(WavHeader), 1, file);
    return (read == 1);
}

// Function to encode WAV to Opus
int encode_wav_to_opus(const char *input_file, const char *output_file)
{
    FILE *fin = fopen(input_file, "rb");
    FILE *fout = fopen(output_file, "wb");

    if (!fin || !fout)
    {
        printf("Error opening files\n");
        if (fin)
            fclose(fin);
        if (fout)
            fclose(fout);
        return -1;
    }

    if (!is_wav_file(fin))
    {
        printf("Error: Not a valid WAV file\n");
        fclose(fin);
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

    if (header.audio_format != 1)
    { // 1 = PCM
        printf("Error: Only PCM WAV files supported\n");
        fclose(fin);
        fclose(fout);
        return -1;
    }

    if (header.bits_per_sample != 16)
    {
        printf("Error: Only 16-bit WAV files supported\n");
        fclose(fin);
        fclose(fout);
        return -1;
    }

    // Create Opus encoder
    int error;
    OpusEncoder *encoder = opus_encoder_create(SAMPLE_RATE, CHANNELS, OPUS_APPLICATION_VOIP, &error);
    if (error != OPUS_OK)
    {
        printf("Error creating encoder: %s\n", opus_strerror(error));
        fclose(fin);
        fclose(fout);
        return -1;
    }

    // Set encoder parameters

    /*

    #define AUDIO_ENCODER_INPUT_SAMPLING_FREQ               16000
#define AUDIO_ENCODER_INPUT_SAMPLE_WIDTH                16
#define AUDIO_ENCODER_INPUT_CHANNELS			1        //mono


    #define AUDIO_ENCODER_FRAME_SIZE	                320      //SAMPLES
    #define AUDIO_ENCODER_BANDWIDTH                         OPUS_BANDWIDTH_MEDIUMBAND
    #define AUDIO_ENCODER_BIT_RATE			        15000
    #define AUDIO_ENCODER_APPLICATION			OPUS_APPLICATION_VOIP
    #define AUDIO_ENCODER_COMPLEXITY			0        //0-10 (10 max quality)
    #define AUDIO_ENCODER_PACKET_LOSS_RESISTANT		0        //0-100
    #define AUDIO_ENCODER_VARIABLE_DATA_RATE		0        //0-1
    #define AUDIO_ENCODER_SIGNAL_TYPE                       OPUS_AUTO
    // #define AUDIO_ENCODER_INBAND_FER                     0
    // #define AUDIO_ENCODER_FRAME_DURATION                 OPUS_FRAMESIZE_20_MS
    #define AUDIO_ENCODER_PREDICTION			0
    #define AUDIO_ENCODER_FORCE_CHANNEL                     OPUS_AUTO
    #define AUDIO_ENCODER_DTX                               0
    #define AUDIO_ENCODER_MAX_OUTPUT_PACKETS		40
    #define AUDIO_ENCODER_OUTPUT_PACKETS                    38
    opus_custom_encoder_ctl(AudioEncPtr, OPUS_SET_BITRATE((AUDIO_ENCODER_BIT_RATE)));
    opus_custom_encoder_ctl(AudioEncPtr, OPUS_SET_BANDWIDTH(AUDIO_ENCODER_BANDWIDTH));
    opus_custom_encoder_ctl(AudioEncPtr, OPUS_SET_VBR(AUDIO_ENCODER_VARIABLE_DATA_RATE));
    opus_custom_encoder_ctl(AudioEncPtr, OPUS_SET_COMPLEXITY(AUDIO_ENCODER_COMPLEXITY));
    opus_custom_encoder_ctl(AudioEncPtr, OPUS_SET_FORCE_CHANNELS(AUDIO_ENCODER_FORCE_CHANNEL));
    opus_custom_encoder_ctl(AudioEncPtr, OPUS_SET_DTX(AUDIO_ENCODER_DTX));
    opus_custom_encoder_ctl(AudioEncPtr, OPUS_SET_PACKET_LOSS_PERC(AUDIO_ENCODER_PACKET_LOSS_RESISTANT));
    opus_custom_encoder_ctl(AudioEncPtr, OPUS_SET_LSB_DEPTH(AUDIO_ENCODER_INPUT_SAMPLE_WIDTH));
    */
    opus_encoder_ctl(encoder, OPUS_SET_BITRATE(16000));
    //opus_encoder_ctl(encoder, OPUS_SET_COMPLEXITY(0));
    //opus_encoder_ctl(encoder, OPUS_SET_BANDWIDTH(OPUS_BANDWIDTH_MEDIUMBAND));
    opus_encoder_ctl(encoder, OPUS_SET_VBR(0));
    

    // Calculate samples count
    int samples_per_channel = header.subchunk2_size / (header.num_channels * (header.bits_per_sample / 8));
    int total_samples = samples_per_channel * header.num_channels;

    // Allocate buffers
    int16_t *pcm_data = (int16_t *)malloc(header.subchunk2_size);
    int16_t *pcm_buffer = (int16_t *)malloc(FRAME_SIZE * CHANNELS * sizeof(int16_t));
    unsigned char opus_data[MAX_PACKET_SIZE];

    if (!pcm_data || !pcm_buffer)
    {
        printf("Memory allocation failed\n");
        fclose(fin);
        fclose(fout);
        free(pcm_data);
        free(pcm_buffer);
        opus_encoder_destroy(encoder);
        return -1;
    }

    // Read PCM data
    fseek(fin, sizeof(WavHeader), SEEK_SET);
    size_t pcm_read = fread(pcm_data, 1, header.subchunk2_size, fin);

    if (pcm_read != header.subchunk2_size)
    {
        printf("Error reading PCM data\n");
        fclose(fin);
        fclose(fout);
        free(pcm_data);
        free(pcm_buffer);
        opus_encoder_destroy(encoder);
        return -1;
    }

    // Write custom header (simple format)
    uint32_t opus_header[] = {
        0x4F505553, // "OPUS" magic number
        SAMPLE_RATE,
        CHANNELS,
        (uint32_t)samples_per_channel};
    fwrite(opus_header, sizeof(opus_header), 1, fout);

    // Encode frames
    int frames_encoded = 0;
    int sample_index = 0;

    while (sample_index < total_samples)
    {
        int samples_to_encode = (total_samples - sample_index < FRAME_SIZE * CHANNELS) ? (total_samples - sample_index) : FRAME_SIZE * CHANNELS;

        // Copy samples to buffer
        memcpy(pcm_buffer, &pcm_data[sample_index], samples_to_encode * sizeof(int16_t));

        // Encode frame
        int bytes_encoded = opus_encode(encoder, pcm_buffer, FRAME_SIZE, opus_data, MAX_PACKET_SIZE);

        if (bytes_encoded < 0)
        {
            printf("Encoding error: %s\n", opus_strerror(bytes_encoded));
            break;
        }

        // Write frame size and data
        printf("Bytes encoded: %d \n", bytes_encoded);
        fwrite(&bytes_encoded, sizeof(int), 1, fout);
        fwrite(opus_data, 1, bytes_encoded, fout);

        frames_encoded++;
        sample_index += FRAME_SIZE * CHANNELS;
    }

    printf("Encoded %d frames successfully\n", frames_encoded);

    // Cleanup
    fclose(fin);
    fclose(fout);
    free(pcm_data);
    free(pcm_buffer);
    opus_encoder_destroy(encoder);

    return 0;
}

// Function to decode Opus to WAV
int decode_opus_to_wav(const char *input_file, const char *output_file)
{
    FILE *fin = fopen(input_file, "rb");
    FILE *fout = fopen(output_file, "wb");

    if (!fin || !fout)
    {
        printf("Error opening files\n");
        if (fin)
            fclose(fin);
        if (fout)
            fclose(fout);
        return -1;
    }

    // Read our custom header
    uint32_t opus_header[4];
    if (fread(opus_header, sizeof(opus_header), 1, fin) != 1)
    {
        printf("Error reading Opus header\n");
        fclose(fin);
        fclose(fout);
        return -1;
    }

    // Verify magic number
    if (opus_header[0] != 0x4F505553)
    { // "OPUS"
        printf("Error: Not a valid Opus file\n");
        fclose(fin);
        fclose(fout);
        return -1;
    }

    uint32_t sample_rate = opus_header[1];
    uint32_t channels = opus_header[2];
    uint32_t total_samples = opus_header[3];

    printf("Opus File Info:\n");
    printf("  Sample Rate: %d\n", sample_rate);
    printf("  Channels: %d\n", channels);
    printf("  Total Samples: %d\n", total_samples);

    // Create Opus decoder
    int error;
    OpusDecoder *decoder = opus_decoder_create(24000, channels, &error);
    if (error != OPUS_OK)
    {
        printf("Error creating decoder: %s\n", opus_strerror(error));
        fclose(fin);
        fclose(fout);
        return -1;
    }

    opus_decoder_ctl(decoder, OPUS_SET_BITRATE((16000)));
    //opus_custom_decoder_ctl(decoder, OPUS_SET_BANDWIDTH(AUDIO_ENCODER_BANDWIDTH));
    opus_decoder_ctl(decoder, OPUS_SET_VBR(0));

    // Prepare WAV header
    WavHeader wav_header = {
        .chunk_id = "RIFF",
        .chunk_size = 36 + total_samples * channels * sizeof(int16_t),
        .format = "WAVE",
        .subchunk1_id = "fmt ",
        .subchunk1_size = 16,
        .audio_format = 1, // PCM
        .num_channels = channels,
        .sample_rate = sample_rate,
        .byte_rate = sample_rate * channels * sizeof(int16_t),
        .block_align = channels * sizeof(int16_t),
        .bits_per_sample = 16,
        .subchunk2_id = "data",
        .subchunk2_size = total_samples * channels * sizeof(int16_t)};
    memcpy(wav_header.chunk_id, "RIFF", 4);
    memcpy(wav_header.format, "WAVE", 4);
    memcpy(wav_header.subchunk1_id, "fmt ", 4);
    memcpy(wav_header.subchunk2_id, "data", 4);

    // Write WAV header
    fwrite(&wav_header, sizeof(WavHeader), 1, fout);

    // Allocate buffers
    int16_t *pcm_buffer = (int16_t *)malloc(FRAME_SIZE * channels * sizeof(int16_t));
    unsigned char opus_data[MAX_PACKET_SIZE];

    if (!pcm_buffer)
    {
        printf("Memory allocation failed\n");
        fclose(fin);
        fclose(fout);
        opus_decoder_destroy(decoder);
        return -1;
    }

    // Decode frames
    int frames_decoded = 0;
    int total_decoded_samples = 0;

    while (!feof(fin))
    {
        int frame_size;

        // Read frame size
        if (fread(&frame_size, sizeof(int), 1, fin) != 1)
        {
            if (feof(fin))
                break;
            printf("Error reading frame size\n");
            break;
        }

        // Read encoded data
        if (fread(opus_data, 1, frame_size, fin) != (size_t)frame_size)
        {
            printf("Error reading frame data\n");
            break;
        }

        // Decode frame
        int samples_decoded = opus_decode(decoder, opus_data, frame_size, pcm_buffer, FRAME_SIZE, 0);

        if (samples_decoded < 0)
        {
            printf("Decoding error: %s\n", opus_strerror(samples_decoded));
            break;
        }

        // Write PCM data
        fwrite(pcm_buffer, sizeof(int16_t), samples_decoded * channels, fout);

        frames_decoded++;
        total_decoded_samples += samples_decoded;
    }

    printf("Decoded %d frames, %d samples\n", frames_decoded, total_decoded_samples);

    // Cleanup
    fclose(fin);
    fclose(fout);
    free(pcm_buffer);
    opus_decoder_destroy(decoder);

    return 0;
}

// Function to print usage
void print_usage(const char *program_name)
{
    printf("Usage: %s <mode> <input_file> <output_file>\n", program_name);
    printf("Modes:\n");
    printf("  encode  - Encode WAV to Opus\n");
    printf("  decode  - Decode Opus to WAV\n");
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        print_usage(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "encode") == 0)
    {
        return encode_wav_to_opus(argv[2], argv[3]);
    }
    else if (strcmp(argv[1], "decode") == 0)
    {
        return decode_opus_to_wav(argv[2], argv[3]);
    }
    else
    {
        printf("Error: Invalid mode. Use 'encode' or 'decode'\n");
        print_usage(argv[0]);
        return 1;
    }
}
