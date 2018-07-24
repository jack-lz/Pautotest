#include <stdio.h>
#include <string>
#include <stdint.h>

#pragma once
class WavFile
{
public:
	WavFile();
	~WavFile();

	bool openW(const std::string &filename, uint32_t sampleRate);
	bool openR(const std::string &filename, uint32_t sampleRate);

	size_t read(void *buffer, size_t len);

	size_t write(void *pData, const size_t iLen);

	void close();

private:
	// Disable copy/assign constructor
	WavFile(const WavFile& o);
	WavFile& operator= (const WavFile& o);

	void writeHeader(uint32_t sampleRate);
	void writeHeaderLen();

public:
	FILE *m_pFile;
	size_t m_iDataPos;
	size_t m_iDataLen;
};