#pragma once

#ifndef __WAVFILE_H_
#define __WAVFILE_H_

#include <fstream>
#include <vector>
#include <memory>

/*

This class read and writes wave files. It is limited to uncompressed PCM formats, with 16 bits per sample.

The canonical WAVE format starts with the RIFF header:

0         4   ChunkID          Contains the letters "RIFF" in ASCII form
                               (0x52494646 big-endian form).
4         4   ChunkSize        36 + SubChunk2Size, or more precisely:
                               4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
                               This is the size of the rest of the chunk 
                               following this number.  This is the size of the 
                               entire file in bytes minus 8 bytes for the
                               two fields not included in this count:
                               ChunkID and ChunkSize.
8         4   Format           Contains the letters "WAVE"
                               (0x57415645 big-endian form).

The "WAVE" format consists of two subchunks: "fmt " and "data":
The "fmt " subchunk describes the sound data's format:

12        4   Subchunk1ID      Contains the letters "fmt "
                               (0x666d7420 big-endian form).
16        4   Subchunk1Size    16 for PCM.  This is the size of the
                               rest of the Subchunk which follows this number.
20        2   AudioFormat      PCM = 1 (i.e. Linear quantization)
                               Values other than 1 indicate some 
                               form of compression.
22        2   NumChannels      Mono = 1, Stereo = 2, etc.
24        4   SampleRate       8000, 44100, etc.
28        4   ByteRate         == SampleRate * NumChannels * BitsPerSample/8
32        2   BlockAlign       == NumChannels * BitsPerSample/8
                               The number of bytes for one sample including
                               all channels. I wonder what happens when
                               this number isn't an integer?
34        2   BitsPerSample    8 bits = 8, 16 bits = 16, etc.
          2   ExtraParamSize   if PCM, then doesn't exist
          X   ExtraParams      space for extra parameters

The "data" subchunk contains the size of the data and the actual sound:

36        4   Subchunk2ID      Contains the letters "data"
                               (0x64617461 big-endian form).
40        4   Subchunk2Size    == NumSamples * NumChannels * BitsPerSample/8
                               This is the number of bytes in the data.
                               You can also think of this as the size
                               of the read of the subchunk following this 
                               number.
44        *   Data             The actual sound data. */

namespace WavTools
{
	class WavFile;
}

class WavTools::WavFile
{

	public:	

		struct WavFileHeader {

			int16_t num_channels;
			uint32_t sample_rate;
			int32_t  bits_per_sample;
		};

		static std::shared_ptr<WavFile> LoadFromFile(std::string filename);
		static std::shared_ptr<WavFile> CreateFromBuffer(std::vector<int16_t> buffer, int num_channels);
		bool WriteToFile(std::string filename);
		const WavFileHeader& GetHeader(void) { return m_header; }

	private:
		WavFile(void);	
		WavFile(std::string filename);
		WavFile(std::vector<int16_t> buffer, int num_channels);

		int ReadInt(unsigned char* bytes , int start) { return (bytes[start+3] << 24) + (bytes[start+2] << 16) + (bytes[start+1] << 8) + bytes[start]; }
		int16_t ReadShort(unsigned char* bytes , int start) { return (bytes[start+1] << 8) + bytes[start]; }

	private:
		std::vector<int16_t> m_pcm_data;
		WavFileHeader m_header;
};

#endif