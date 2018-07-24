#include "wav.h"
#include <string.h>
#include "LOG.h"

int strncasecmp(char *s1, char *s2, register int n) {
    while (--n >= 0 && toupper((unsigned char)*s1) == toupper((unsigned char)*s2++))
    if (*s1++ == ' ') return 0;
    return(n < 0 ? 0 : toupper((unsigned char)*s1) - toupper((unsigned char)*--s2));
}

TWav::TWav(const char *filename){
    wav_info.fp = fopen(filename, "rb");
    assert(wav_info.fp != NULL);

    char buffer[256];
    int read_len = 0;
    int offset = 0;

    //handle RIFF WAVE chunk
    read_len = fread(buffer, 1, 12, wav_info.fp);
    assert(read_len == 12);
    assert(0 == strncasecmp("RIFF", buffer, 4));
    memcpy(wav_info.riff.id, buffer, 4);
    wav_info.riff.size = *(int *)(buffer + 4);
    assert(0 == strncasecmp("WAVE", buffer + 8, 4));
    memcpy(wav_info.riff.type, buffer + 8, 4);
    wav_info.file_size = wav_info.riff.size + 8;

    offset += 12;

    while(1){
        char id_buffer[5] = {0};
        int tmp_size = 0;

        read_len = fread(buffer, 1, 8, wav_info.fp);
        assert(read_len == 8);
        memcpy(id_buffer, buffer, 4);
        tmp_size = *(int *)(buffer + 4);


        if(0 == strncasecmp("FMT", id_buffer, 3)){
            memcpy(wav_info.format.id, id_buffer, 3);
            wav_info.format.size = tmp_size;
            read_len = fread(buffer, 1, tmp_size, wav_info.fp);
            assert(read_len == tmp_size);
            wav_info.format.compression_code = *(short *)buffer;
            wav_info.format.channels = *(short *)(buffer + 2);
            wav_info.format.samples_per_sec = *(int *)(buffer + 4);
            wav_info.format.avg_bytes_per_sec = *(int *)(buffer + 8);
            wav_info.format.block_align = *(short *)(buffer + 12);
            wav_info.format.bits_per_sample = *(short *)(buffer + 14);
        }
        else if(0 == strncasecmp("DATA", id_buffer, 4)){
            memcpy(wav_info.data.id, id_buffer, 4);
            wav_info.data.size = tmp_size;
            offset += 8;
            wav_info.data_offset = offset;
            wav_info.data_size = wav_info.data.size;
            break;
        }
        else{
//            LOG("unhandled chunk: %s, size: %d\n", id_buffer, tmp_size);
            fseek(wav_info.fp, tmp_size, SEEK_CUR);
        }
        offset += 8 + tmp_size;
    }
}

TWav::~TWav(){
    fclose(wav_info.fp);
}

int16_t TWav::GetSample(){
	int16_t s1;
    fread(&s1, 2, 1, wav_info.fp);
	//FIXME check file end
	return s1;
}

void TWav::StartFromOffset(int offset){
    fseek(wav_info.fp, wav_info.data_offset + offset, SEEK_SET);
}
