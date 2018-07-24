#include "WavFile.h"
#include "Audio_Recog.h"
// #include "RecordButton_Define.h"
#include <cstring>
#include<stdio.h>
#include "LOG.h"

#ifndef MIN
#   define MIN(a, b)    (((a)<(b))?(a):(b))
#endif

// header of wav file
struct WAV_HDR
{
	char rID[4];                // 'RIFF'
	uint32_t rLen;              // member description for rLen
	char wID[4];                // 'WAVE'
	char fId[4];                // 'fmt '
	uint32_t pcm_header_len;    // varies...
	uint16_t wFormatTag;       // member description for wFormatTag
	uint16_t nChannels;        // 1, 2 for stereo data is (l, r) pairs
	uint32_t nSamplesPerSec;    // member description for nSamplesPerSec
	uint32_t nAvgBytesPerSec;   // member description for nAvgBytesPerSec
	uint16_t nBlockAlign;      // member description for nBlockAlign
	uint16_t nBitsPerSample;   // member description for nBitsPerSample
};

bool CheckWavHeader(const WAV_HDR& header)
{
	if ((memcmp(&header.rID, "RIFF", 4) != 0)
		&& (memcmp(&header.wID, "WAVE", 4) != 0)
		&& (memcmp(&header.fId, "fmt ", 4) != 0)) {
        LOG("rID=[%c %c%c %c](RIFF) \n", header.rID[0], header.rID[1], header.rID[2], header.rID[3]);
        LOG("wID=[%c %c%c %c](WAVE) \n", header.wID[0], header.wID[1], header.wID[2], header.wID[3]);
        LOG("fId=[%c %c%c %c](fmt ) \n", header.fId[0], header.fId[1], header.fId[2], header.fId[3]);
		return false;
	}
	return true;
}

// header of chunk
struct CHUNK_HDR
{
	char dId[4];            // 'data' or 'fact'
	uint32_t dLen;          // data length
	//  unsigned char *data;
};


bool CheckChunkHeader(const CHUNK_HDR& header, const char* chunk_tag)
{
	if (0 != memcmp(&header.dId, chunk_tag, 4)) {
       // LOG("dId=[%c %c%c %c](data) \n", header.dId[0], header.dId[1], header.dId[2], header.dId[3]);
		return false;
	}
	return true;
}

WavFile::WavFile()
	: m_pFile(NULL)
	, m_iDataPos(0)
	, m_iDataLen(0)
{
}


WavFile::~WavFile()
{
}

bool WavFile::openW(const std::string &filename, uint32_t sampleRate)
{
	if (NULL != m_pFile) {
		fclose(m_pFile);
		m_pFile = NULL;
	}

    m_pFile = fopen(filename.c_str(), "wb");
	writeHeader(sampleRate);
	m_iDataPos = 0;
	m_iDataLen = 0;
	return true;
}

bool WavFile::openR(const std::string &filename, uint32_t sampleRate)
{
	if (NULL != m_pFile) {
		fclose(m_pFile);
		m_pFile = NULL;
	}
    m_pFile = fopen(filename.c_str(), "rb");
    if (!m_pFile)
    {
        return false;
    }
	fseek(m_pFile, 0, SEEK_END);
	int32_t size = ftell(m_pFile);
	fseek(m_pFile, 0, SEEK_SET);

	WAV_HDR hdr;
	size_t count = fread(&hdr, sizeof(hdr), 1, m_pFile);
	size -= sizeof(hdr);

	if ((count < 1) || (!CheckWavHeader(hdr))
		|| sampleRate != hdr.nSamplesPerSec) {
        LOG("sampleRate req/real=%d/%d \n", sampleRate, hdr.nSamplesPerSec);
		fclose(m_pFile);
		m_pFile = NULL;
		return false;
	}

	CHUNK_HDR chdr;
	count = fread(&chdr, sizeof(chdr), 1, m_pFile);
	size -= sizeof(chdr);
	if ((1 == count) && CheckChunkHeader(chdr, "fact")) {
		fseek(m_pFile, chdr.dLen, SEEK_CUR);
		size -= (chdr.dLen + sizeof(chdr));

		size_t len = fread(&chdr, sizeof(chdr), 1, m_pFile);
		if (sizeof(chdr) != len) {
            LOG("Read chunk header fact failed \n");
		}
	}

	if (!CheckChunkHeader(chdr, "data")) {
        LOG("chunk has no data section \n");
		return false;
	}

	if (size != (int32_t)chdr.dLen) {
        LOG("data chunk size is incorrect, data.size=%d, file size=%d \n", chdr.dLen, size);
	}

	m_iDataLen = size;
	m_iDataPos = 0;
//    LOG("Open wav file ok, data len=%d \n", size);
	return true;
}

size_t WavFile::write(void *pData, const size_t iLen)
{
	if (NULL == m_pFile || NULL == pData || iLen < 1) {
		if (NULL == m_pFile) {
            LOG("m_pFile is NULL\n");
		}
        LOG("invalid param, m_pFile=[%p], pData=[%p], len=[%d] \n", m_pFile, pData, iLen);
		return 0;
	}
    size_t write_len = fwrite(pData, 1, iLen, m_pFile);
    if (iLen == write_len) {
		m_iDataPos += iLen;
	}

	return write_len;
}

void WavFile::close()
{
	if (NULL != m_pFile) {
		if (0 == m_iDataLen) { // for write file
			writeHeaderLen();
		}
		fclose(m_pFile);
		m_pFile = NULL;

		m_iDataPos = 0;
		m_iDataLen = 0;
	}
}

void WavFile::writeHeader(uint32_t sampleRate)
{
	if (NULL == m_pFile) {
		return;
	}
	WAV_HDR hdr;
	memcpy(&hdr.rID, "RIFF", 4);
	hdr.rLen = sizeof(WAV_HDR) - sizeof(char) * 4 - sizeof(uint32_t) + sizeof(CHUNK_HDR);
	memcpy(&hdr.wID, "WAVE", 4);
	memcpy(&hdr.fId, "fmt ", 4);
	hdr.pcm_header_len = 16;
	hdr.wFormatTag = 1;
	hdr.nChannels = 1;
	hdr.nSamplesPerSec = sampleRate;
	hdr.nBitsPerSample = 16;
	hdr.nAvgBytesPerSec = hdr.nChannels*sampleRate*(hdr.nBitsPerSample >> 3);
	hdr.nBlockAlign = hdr.nChannels*(hdr.nBitsPerSample >> 3);
	fwrite(&hdr, 1, sizeof(hdr), m_pFile);

	CHUNK_HDR chdr;
	memcpy(&chdr.dId, "data", 4);
	chdr.dLen = 0;
	fwrite(&chdr, 1, sizeof(chdr), m_pFile);
}

void WavFile::writeHeaderLen()
{
	if (NULL == m_pFile) {
		return;
	}

	uint32_t total = m_iDataPos + sizeof(WAV_HDR) - sizeof(char) * 4 - sizeof(uint32_t) + sizeof(CHUNK_HDR);
	fseek(m_pFile, 4, SEEK_SET);
	fwrite(&total, 1, 4, m_pFile);

	fseek(m_pFile, 40, SEEK_SET);
	fwrite(&m_iDataPos, 1, 4, m_pFile);
}

size_t WavFile::read(void *buffer, size_t len)
{
	if ((NULL == buffer) || (0 == len)) {
        LOG("buffer=%p, len=%d \n", buffer, len);
		return 0;
	}

	if (m_iDataPos >= m_iDataLen) { // No more data
		return 0;
	}

	len = fread(buffer, 1, MIN(len, m_iDataLen - m_iDataPos), m_pFile);
	if (len > 0) {
		m_iDataPos += len;
	}

	return len;
}
