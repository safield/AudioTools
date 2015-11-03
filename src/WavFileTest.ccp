#include "WavFile.h"
#include "LogStream.h"
#include "assert.h"
#include <fstream>

using namespace WavTools;

static const int HEADER_SIZE = 44;

WavFile::WavFile(std::string filename)
{
	std::ifstream ifs;		
	ifs.open( filename, std::ios::binary | std::ios::in | std::ios::ate);	

	if(ifs.fail())
		throw std::invalid_argument("WavFile::WavFile : Invalid argument - unable to open wavfile "+filename);


	unsigned char hbytes[HEADER_SIZE];

	ifs.read(hbytes , HEADER_SIZE);

	for (int i = 0; i < HEADER_SIZE; ++i)
		LogStream(LOG_DEBUG) << hbytes[i];

	// this will be enough to convince me that it is a real wave file
	bool valid_wave;
 	int chunk_size = (hbytes[4] << 24) + (hbytes[5] << 16) + (hbytes[6] << 8) + hbytes[7];
 	LogStream(LOG_DEBUG) << "CHUNK_SIZE = "<<chunk_size;
	valid_wave =  hbytes[0]=='R' && hbytes[1]=='I' && hbytes[2]=='F' && hbytes[3]=='F';
	valid_wave = valid_wave && (hbytes[12]=='f' && hbytes[13]=='m' && hbytes[14]=='t' && hbytes[15]==' ');
	valid_wave = valid_wave && (hbytes[36]=='d' && hbytes[37]=='a' && hbytes[38]=='t' && hbytes[39]=='a');
	valid_wave = valid_wave && (ifs.tellg() == chunk_size - 8);
}

std::shared_ptr<WavFile> WavFile::LoadWavFromFile(std::string filename)
{
	return std::shared_ptr<WavFile>(new WavFile(filename));
}
