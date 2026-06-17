#include"decoder.h"
int decode_opus_stream_unknown_size(const char *input_file)
{
    char full_fname[100] = "Decoded_audios/";
    char full_fnameOut[100] = "Decoded_audios/";
    strcat(full_fname,input_file);
    strcat(full_fname,".opus");
    
    strcat(full_fnameOut,input_file);
    strcat(full_fnameOut,"Dec.wav");
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

    uint32_t sample_rate = SAMPLE_RATE;
    uint32_t channels = CHANNELS;

    // Create Opus decoder
    int error;
    int lookahead;
/*
    OpusCustomMode *opusmode = NULL;

    opusmode = opus_custom_mode_create(AUDIO_ENCODER_INPUT_SAMPLING_FREQ, AUDIO_ENCODER_FRAME_SIZE, &error);

    OpusCustomDecoder *decoder = opus_custom_decoder_create(opusmode, channels, &error);
*/
OpusDecoder *decoder = opus_decoder_create(SAMPLE_RATE,CHANNELS,&error);
    if (error != OPUS_OK)
    {
        printf("Error creating decoder: %s\n", opus_strerror(error));
        fclose(fin);
        fclose(fout);
        return -1;
    }
    
    opus_decoder_ctl(decoder,OPUS_SET_BANDWIDTH(AUDIO_ENCODER_BANDWIDTH));
    opus_decoder_ctl(decoder, OPUS_SET_BITRATE((16000)));
    //opus_custom_decoder_ctl(decoder, OPUS_SET_BANDWIDTH(AUDIO_ENCODER_BANDWIDTH));
    //opus_decoder_ctl(decoder, OPUS_SET_VBR(AUDIO_ENCODER_VARIABLE_DATA_RATE));
    
    opus_decoder_ctl(decoder, OPUS_SET_DTX(0));
    opus_decoder_ctl(decoder, OPUS_SET_COMPLEXITY(0));
    opus_decoder_ctl(decoder, OPUS_SET_VBR_CONSTRAINT(1));
    //opus_decoder_ctl(decoder, OPUS_SET_GAIN(1000));
    //opus_decoder_ctl(decoder, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE));
    //opus_decoder_ctl(decoder,OPUS_GET_LOOKAHEAD_REQUEST(&lookahead));
    
    printf("Lookahead: %d \n", lookahead);
    //opus_decoder_ctl(decoder, OPUS_SET_DTX(AUDIO_ENCODER_DTX));
    //opus_decoder_ctl(decoder, OPUS_SET_VBR_CONSTRAINT(1));
    //opus_decoder_ctl(decoder,OPUS_SET_PREDICTION_DISABLED(1));
    //opus_custom_decoder_ctl(decoder, OPUS_SET_COMPLEXITY(AUDIO_ENCODER_COMPLEXITY));
    //opus_custom_decoder_ctl(decoder, OPUS_SET_FORCE_CHANNELS(AUDIO_ENCODER_FORCE_CHANNEL));
   //opus_custom_decoder_ctl(decoder, OPUS_SET_DTX(AUDIO_ENCODER_DTX));
    //opus_custom_decoder_ctl(decoder, OPUS_SET_PACKET_LOSS_PERC(AUDIO_ENCODER_PACKET_LOSS_RESISTANT));
    //opus_custom_decoder_ctl(decoder, OPUS_SET_LSB_DEPTH(AUDIO_ENCODER_INPUT_SAMPLE_WIDTH));
    WavHeader wav_header;
    memset(&wav_header, 0, sizeof(WavHeader));
    memcpy(wav_header.chunk_id, "RIFF", 4);
    // chunk_size = 0
    memcpy(wav_header.format, "WAVE", 4);
    memcpy(wav_header.subchunk1_id, "fmt ", 4);
    wav_header.subchunk1_size = 16;
    wav_header.audio_format = 1;
    wav_header.num_channels = channels;
    wav_header.sample_rate = sample_rate;
    wav_header.byte_rate = sample_rate * channels * sizeof(int16_t);
    wav_header.block_align = channels * sizeof(int16_t);
    wav_header.bits_per_sample = 16;
    memcpy(wav_header.subchunk2_id, "data", 4);
    // subchunk2_size = 0

    // header position
    long header_pos = ftell(fout);
    fwrite(&wav_header, sizeof(WavHeader), 1, fout);

    // Decode frames and write PCM data
    uint16_t *pcm_buffer = (uint16_t *)malloc(FRAME_SIZE * sizeof(uint16_t));
    unsigned char opus_data[AUDIO_ENCODER_MAX_OUTPUT_PACKETS];
    uint32_t total_pcm_bytes = 0;
    int frames_decoded = 0;
    
    while (!feof(fin))
    {
        int enc_frame_size = 0;
        fread(&enc_frame_size,1,1,fin); //Read encoded frame size byte

        printf("Enc frame size: %d \n",enc_frame_size);

        // Read encoded data
        if (fread(opus_data, 1, enc_frame_size, fin) != (size_t)enc_frame_size)
        {
            printf("Error reading frame data. Frame %d \n", frames_decoded);
            break;
        }
        //fread(opus_data, 1, frame_size, fin);

        // Decode frame
        int samples_decoded = opus_decode(decoder, opus_data, enc_frame_size,
                                          pcm_buffer, FRAME_SIZE,0);

        printf("Samples decoded: %d \n",samples_decoded);

        if (samples_decoded < 0)
        {
            printf("Decoding error: %s\n", opus_strerror(samples_decoded));
            break;
        }

        // Write PCM data
        size_t bytes_written = samples_decoded * channels * sizeof(int16_t);
        fwrite(pcm_buffer, 1, bytes_written, fout);
        total_pcm_bytes += bytes_written;
        frames_decoded++;
    }

    // Calculate final sizes
    uint32_t riff_size = 36 + total_pcm_bytes; // 36 = 44 - 8

    // Go back and fix the header
    fseek(fout, header_pos, SEEK_SET);

    wav_header.chunk_size = riff_size;
    wav_header.subchunk2_size = total_pcm_bytes;

    fwrite(&wav_header, sizeof(WavHeader), 1, fout);

    printf("Decoded %d frames, %u PCM bytes\n", frames_decoded, total_pcm_bytes);

    // Cleanup
    fclose(fin);
    fclose(fout);
    opus_decoder_destroy(decoder);

    return 0;
}

int decode_opus_custom2_stream_unknown_size(const char *input_file)
{
    char full_fname[100] = "Decoded_audios/";
    char full_fnameOut[100] = "Decoded_audios/";
    strcat(full_fname,input_file);
    strcat(full_fname,".opus");
    
    strcat(full_fnameOut,input_file);
    strcat(full_fnameOut,"Dec.wav");
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

    uint32_t sample_rate = SAMPLE_RATE;
    uint32_t channels = CHANNELS;

    // Create Opus decoder
    int error;
    int lookahead;

    OpusCustomMode *opusmode = NULL;

    opusmode = opus_custom_mode_create(AUDIO_ENCODER_INPUT_SAMPLING_FREQ, AUDIO_ENCODER_FRAME_SIZE, &error);

    OpusCustomDecoder *decoder = opus_custom_decoder_create(opusmode, channels, &error);

//OpusDecoder *decoder = opus_decoder_create(SAMPLE_RATE,CHANNELS,&error);
    if (error != OPUS_OK)
    {
        printf("Error creating decoder: %s\n", opus_strerror(error));
        fclose(fin);
        fclose(fout);
        return -1;
    }
    
    opus_custom_decoder_ctl(decoder,OPUS_SET_BANDWIDTH(AUDIO_ENCODER_BANDWIDTH));
    opus_custom_decoder_ctl(decoder, OPUS_SET_BITRATE((16000)));
    //opus_custom_decoder_ctl(decoder, OPUS_SET_BANDWIDTH(AUDIO_ENCODER_BANDWIDTH));
    //opus_decoder_ctl(decoder, OPUS_SET_VBR(AUDIO_ENCODER_VARIABLE_DATA_RATE));
    
    opus_custom_decoder_ctl(decoder, OPUS_SET_DTX(0));
    opus_custom_decoder_ctl(decoder, OPUS_SET_COMPLEXITY(0));
    opus_custom_decoder_ctl(decoder, OPUS_SET_VBR_CONSTRAINT(1));
    //opus_decoder_ctl(decoder, OPUS_SET_GAIN(1000));
    //opus_decoder_ctl(decoder, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE));
    //opus_decoder_ctl(decoder,OPUS_GET_LOOKAHEAD_REQUEST(&lookahead));
    
    printf("Lookahead: %d \n", lookahead);
    //opus_decoder_ctl(decoder, OPUS_SET_DTX(AUDIO_ENCODER_DTX));
    //opus_decoder_ctl(decoder, OPUS_SET_VBR_CONSTRAINT(1));
    //opus_decoder_ctl(decoder,OPUS_SET_PREDICTION_DISABLED(1));
    //opus_custom_decoder_ctl(decoder, OPUS_SET_COMPLEXITY(AUDIO_ENCODER_COMPLEXITY));
    //opus_custom_decoder_ctl(decoder, OPUS_SET_FORCE_CHANNELS(AUDIO_ENCODER_FORCE_CHANNEL));
   //opus_custom_decoder_ctl(decoder, OPUS_SET_DTX(AUDIO_ENCODER_DTX));
    //opus_custom_decoder_ctl(decoder, OPUS_SET_PACKET_LOSS_PERC(AUDIO_ENCODER_PACKET_LOSS_RESISTANT));
    //opus_custom_decoder_ctl(decoder, OPUS_SET_LSB_DEPTH(AUDIO_ENCODER_INPUT_SAMPLE_WIDTH));
    WavHeader wav_header;
    memset(&wav_header, 0, sizeof(WavHeader));
    memcpy(wav_header.chunk_id, "RIFF", 4);
    // chunk_size = 0
    memcpy(wav_header.format, "WAVE", 4);
    memcpy(wav_header.subchunk1_id, "fmt ", 4);
    wav_header.subchunk1_size = 16;
    wav_header.audio_format = 1;
    wav_header.num_channels = channels;
    wav_header.sample_rate = sample_rate;
    wav_header.byte_rate = sample_rate * channels * sizeof(int16_t);
    wav_header.block_align = channels * sizeof(int16_t);
    wav_header.bits_per_sample = 16;
    memcpy(wav_header.subchunk2_id, "data", 4);
    // subchunk2_size = 0

    // header position
    long header_pos = ftell(fout);
    fwrite(&wav_header, sizeof(WavHeader), 1, fout);

    // Decode frames and write PCM data
    uint16_t *pcm_buffer = (uint16_t *)malloc(FRAME_SIZE * sizeof(uint16_t));
    unsigned char opus_data[AUDIO_ENCODER_MAX_OUTPUT_PACKETS];
    uint32_t total_pcm_bytes = 0;
    int frames_decoded = 0;
    
    while (!feof(fin))
    {
        int enc_frame_size = 0;
        fread(&enc_frame_size,1,1,fin); //Read encoded frame size byte

        printf("Enc frame size: %d \n",enc_frame_size);

        // Read encoded data
        if (fread(opus_data, 1, enc_frame_size, fin) != (size_t)enc_frame_size)
        {
            printf("Error reading frame data. Frame %d \n", frames_decoded);
            break;
        }
        //fread(opus_data, 1, frame_size, fin);

        // Decode frame
        int samples_decoded = opus_custom_decode(decoder,opus_data,enc_frame_size,pcm_buffer,FRAME_SIZE);

        printf("Samples decoded: %d \n",samples_decoded);

        if (samples_decoded < 0)
        {
            printf("Decoding error: %s\n", opus_strerror(samples_decoded));
            break;
        }

        // Write PCM data
        size_t bytes_written = samples_decoded * channels * sizeof(int16_t);
        fwrite(pcm_buffer, 1, bytes_written, fout);
        total_pcm_bytes += bytes_written;
        frames_decoded++;
    }

    // Calculate final sizes
    uint32_t riff_size = 36 + total_pcm_bytes; // 36 = 44 - 8

    // Go back and fix the header
    fseek(fout, header_pos, SEEK_SET);

    wav_header.chunk_size = riff_size;
    wav_header.subchunk2_size = total_pcm_bytes;

    fwrite(&wav_header, sizeof(WavHeader), 1, fout);

    printf("Decoded %d frames, %u PCM bytes\n", frames_decoded, total_pcm_bytes);

    // Cleanup
    fclose(fin);
    fclose(fout);
    opus_custom_decoder_destroy(decoder);

    return 0;
}

int decode_opus_custom_stream_unknown_size(const char *input_file)
{
    char full_fname[100] = "Decoded_audios/";
    char full_fnameOut[100] = "Decoded_audios/";
    strcat(full_fname,input_file);
    strcat(full_fname,".opus");
    
    strcat(full_fnameOut,input_file);
    strcat(full_fnameOut,"Dec.wav");
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

    uint32_t sample_rate = SAMPLE_RATE;
    uint32_t channels = CHANNELS;

    // Create Opus decoder
    int error;

    OpusCustomMode *opusmode = NULL;

    opusmode = opus_custom_mode_create(AUDIO_ENCODER_INPUT_SAMPLING_FREQ, AUDIO_ENCODER_FRAME_SIZE, &error);

    OpusCustomDecoder *decoder = opus_custom_decoder_create(opusmode, channels, &error);

//OpusDecoder *decoder = opus_decoder_create(SAMPLE_RATE,CHANNELS,&error);
    if (error != OPUS_OK)
    {
        printf("Error creating decoder: %s\n", opus_strerror(error));
        fclose(fin);
        fclose(fout);
        return -1;
    }
    //opus_decoder_ctl(decoder,OPUS_SET_BANDWIDTH(AUDIO_ENCODER_BANDWIDTH));
    //opus_decoder_ctl(decoder, OPUS_SET_BITRATE((AUDIO_ENCODER_BIT_RATE)));
    opus_custom_decoder_ctl(decoder, OPUS_SET_BANDWIDTH(OPUS_BANDWIDTH_MEDIUMBAND));
    opus_custom_decoder_ctl(decoder, OPUS_SET_BITRATE(15000));
    opus_custom_decoder_ctl(decoder, OPUS_SET_VBR(AUDIO_ENCODER_VARIABLE_DATA_RATE));
    opus_custom_decoder_ctl(decoder, OPUS_SET_COMPLEXITY(AUDIO_ENCODER_COMPLEXITY));
    opus_custom_decoder_ctl(decoder, OPUS_SET_FORCE_CHANNELS(AUDIO_ENCODER_FORCE_CHANNEL));
    opus_custom_decoder_ctl(decoder, OPUS_SET_DTX(AUDIO_ENCODER_DTX));
    opus_custom_decoder_ctl(decoder, OPUS_SET_PACKET_LOSS_PERC(AUDIO_ENCODER_PACKET_LOSS_RESISTANT));
    opus_custom_decoder_ctl(decoder, OPUS_SET_LSB_DEPTH(AUDIO_ENCODER_INPUT_SAMPLE_WIDTH));
    WavHeader wav_header;
    memset(&wav_header, 0, sizeof(WavHeader));
    memcpy(wav_header.chunk_id, "RIFF", 4);
    // chunk_size = 0
    memcpy(wav_header.format, "WAVE", 4);
    memcpy(wav_header.subchunk1_id, "fmt ", 4);
    wav_header.subchunk1_size = 16;
    wav_header.audio_format = 1;
    wav_header.num_channels = channels;
    wav_header.sample_rate = sample_rate;
    wav_header.byte_rate = sample_rate * channels * sizeof(int16_t);
    wav_header.block_align = channels * sizeof(int16_t);
    wav_header.bits_per_sample = 16;
    memcpy(wav_header.subchunk2_id, "data", 4);
    // subchunk2_size = 0

    // header position
    long header_pos = ftell(fout);
    fwrite(&wav_header, sizeof(WavHeader), 1, fout);

    // Decode frames and write PCM data
    uint16_t *pcm_buffer = (uint16_t *)malloc(FRAME_SIZE * sizeof(uint16_t));
    unsigned char opus_data[AUDIO_ENCODER_MAX_OUTPUT_PACKETS];
    uint32_t total_pcm_bytes = 0;
    int frames_decoded = 0;
    char byte;
    while (!feof(fin))
    {
        int enc_frame_size = 38;
        //fread(&enc_frame_size,1,1,fin);

        printf("Enc frame size: %d \n",enc_frame_size);
        
        // Read encoded data
        if (fread(opus_data, 1, enc_frame_size, fin) != (size_t)enc_frame_size)
        {
            printf("Error reading frame data. Frame %d \n", frames_decoded);
            break;
        }
        //fread(opus_data, 1, frame_size, fin);

        // Decode frame
        int samples_decoded = opus_custom_decode(decoder, opus_data, enc_frame_size,
                                          pcm_buffer, FRAME_SIZE);

        if (samples_decoded < 0)
        {
            printf("Decoding error: %s\n", opus_strerror(samples_decoded));
            break;
        }

        // Write PCM data
        size_t bytes_written = samples_decoded * channels * sizeof(int16_t);
        fwrite(pcm_buffer, 1, bytes_written, fout);
        total_pcm_bytes += bytes_written;
        frames_decoded++;
    }

    // Calculate final sizes
    uint32_t riff_size = 36 + total_pcm_bytes; // 36 = 44 - 8

    // Go back and fix the header
    fseek(fout, header_pos, SEEK_SET);

    wav_header.chunk_size = riff_size;
    wav_header.subchunk2_size = total_pcm_bytes;

    fwrite(&wav_header, sizeof(WavHeader), 1, fout);

    printf("Decoded %d frames, %u PCM bytes\n", frames_decoded, total_pcm_bytes);

    // Cleanup
    fclose(fin);
    fclose(fout);
    opus_custom_decoder_destroy(decoder);

    return 0;
}

