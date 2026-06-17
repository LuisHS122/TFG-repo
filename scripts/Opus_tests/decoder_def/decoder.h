#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <opus.h>
#include <opus_custom.h>

#define SAMPLE_RATE 16000
#define CHANNELS 1
#define FRAME_SIZE 320 // 20ms at 16kHz
#define MAX_PACKET_SIZE 4000

#define AUDIO_ENCODER_INPUT_SAMPLING_FREQ 16000
#define AUDIO_ENCODER_INPUT_SAMPLE_WIDTH 16
#define AUDIO_ENCODER_INPUT_CHANNELS 1 // mono

/***********************Audio encoder options********************************/
#define AUDIO_ENCODER_FRAME_SIZE 320 // SAMPLES
#define AUDIO_ENCODER_BANDWIDTH OPUS_BANDWIDTH_WIDEBAND
#define AUDIO_ENCODER_BIT_RATE 16000
#define AUDIO_ENCODER_APPLICATION OPUS_APPLICATION_VOIP
#define AUDIO_ENCODER_COMPLEXITY 0            // 0-10 (10 max quality)
#define AUDIO_ENCODER_PACKET_LOSS_RESISTANT 0 // 0-100
#define AUDIO_ENCODER_VARIABLE_DATA_RATE 0    // 0-1
#define AUDIO_ENCODER_SIGNAL_TYPE OPUS_AUTO
// #define AUDIO_ENCODER_INBAND_FER                     0
// #define AUDIO_ENCODER_FRAME_DURATION                 OPUS_FRAMESIZE_20_MS
#define AUDIO_ENCODER_PREDICTION 0
#define AUDIO_ENCODER_FORCE_CHANNEL OPUS_AUTO
#define AUDIO_ENCODER_DTX 0
#define AUDIO_ENCODER_MAX_OUTPUT_PACKETS 500
#define AUDIO_ENCODER_OUTPUT_PACKETS 38

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

int decode_opus_stream_unknown_size(const char *input_file);


int decode_opus_custom_stream_unknown_size(const char *input_file);

int decode_opus_custom2_stream_unknown_size(const char *input_file);

