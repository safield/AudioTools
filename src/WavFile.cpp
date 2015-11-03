#include "WavFile.h"
#include "LogStream.h"
#include "assert.h"
#include <fstream>
#include "Util.h"

using namespace WavTools;

static const int HEADER_SIZE = 44;

/**
* Static factory methods
*/
std::shared_ptr<WavFile> WavFile::CreateFromBuffer(std::vector<int16_t> buffer, int num_channels)
{
	return std::shared_ptr<WavFile>(new WavFile(buffer , num_channels));
}

std::shared_ptr<WavFile> WavFile::LoadFromFile(std::string filename)
{
	return std::shared_ptr<WavFile>(new WavFile(filename));
}

WavFile::WavFile(std::string filename)
{
	std::ifstream ifs;		
	ifs.open( filename, std::ios::binary | std::ios::in );

	LogStream(LOG_DEBUG) << "WavFile::WavFile - Opening wave "<<filename;

	if(ifs.fail())
		throw std::invalid_argument("WavFile::WavFile : Failed to open wavFile "+filename);
		
	unsigned char hbytes[HEADER_SIZE];

	ifs.read((char*)hbytes , HEADER_SIZE);

	// check that this is actually a wave file
    bool valid_riff = hbytes[0]=='R' && hbytes[1]=='I' && hbytes[2]=='F' && hbytes[3]=='F';
    bool valid_wave = hbytes[8]=='W' && hbytes[9]=='A' && hbytes[10]=='V' && hbytes[11]=='E';
	bool valid_ftm = (hbytes[12]=='f' && hbytes[13]=='m' && hbytes[14]=='t' && hbytes[15]==' ');
	bool valid_data = (hbytes[36]=='d' && hbytes[37]=='a' && hbytes[38]=='t' && hbytes[39]=='a');

	if(!(valid_data && valid_ftm && valid_riff && valid_data))
		throw std::invalid_argument("WavFile::WavFile : Invalid argument - unable to open wavfile "+filename+" unknown format "+filename);

	int audioFormat = ReadShort(hbytes , 20);
	int SubChunk1Size = ReadInt(hbytes , 16);

	if(audioFormat != 1 || SubChunk1Size != 16)
		throw std::invalid_argument("WavFile::WavFile : Only uncompressed PCM wave format supported."+filename);	

	int subChunk2size = ReadInt(hbytes , 40);
	m_header.num_channels = ReadShort(hbytes , 22);
	m_header.sample_rate = ReadInt(hbytes , 24);
	m_header.bits_per_sample = ReadShort(hbytes , 34);

	if(m_header.bits_per_sample != 16)
		throw std::invalid_argument("WavFile::WavFile : Only 16 bit wave files are supported."+filename);			

	LogStream(LOG_DEBUG) << "WavFile::WavFile num_channels="<<m_header.num_channels << " sample_rate="<<m_header.sample_rate<<" bits_per_sample="<<m_header.bits_per_sample;

	m_pcm_data.resize( subChunk2size / sizeof(int16_t) );
	ifs.read((char*)m_pcm_data.data() , subChunk2size);

	ifs.close();

	if(ifs.fail() || ifs.bad())
		throw std::runtime_error("WavFile::WavFile : runtime_error - reading wavfile "+filename+" has failed.");	
}

WavFile::WavFile(std::vector<int16_t> buffer, int num_channels)
{

}

bool WavFile::WriteToFile(std::string filename)
{

	LogStream(LOG_DEBUG) << "WavFile::WriteToFile - Writing wav to file "<<filename;

	std::ofstream ofs (filename , std::ios::binary | std::ios::trunc);

	// 0 write "RIFF"
	ofs << "RIFF";

	// 4 write the chunk size which is 36 + data
	int chunksize = m_pcm_data.size() + 36;
	assert(ofs.tellp()==4);
	ofs.write((char*)&chunksize , 4);

	// 8 write format "WAVE"
	assert(ofs.tellp()==8);
	ofs << "WAVE";

	// 12 write subchunk1ID "fmt "
	assert(ofs.tellp()==12);
	ofs << "fmt ";

	// 16 write Subchunk1Size = 16 since we only support PCM data
	int subchunk1size = 16;
	assert(ofs.tellp()==16);
	ofs.write((char*)&subchunk1size , 4);

	// 20 write audioFormat = 1 since we only support PCM
	int16_t audioformat = 1;
	assert(ofs.tellp()==20);
	ofs.write((char*)&audioformat , 2);

	// 22 write numchannels
	assert(ofs.tellp()==22);
	ofs.write((char*)&m_header.num_channels , 2);

	// 24 write samplerate
	assert(ofs.tellp()==24);
	ofs.write((char*)&m_header.sample_rate , 4);

	// 28 write byte rate
	int byterate = m_header.sample_rate * m_header.num_channels * (m_header.bits_per_sample / 8);
	assert(ofs.tellp()==28);
	ofs.write((char*)&byterate , 4);

	// 32 write block align
	int16_t blockalign = m_header.num_channels * (m_header.bits_per_sample / 8);
	assert(ofs.tellp()==32);
	ofs.write((char*)&blockalign , 2);

	// 34 write bits per sample
	assert(ofs.tellp()==34);
	ofs.write((char*)&m_header.bits_per_sample , 2);

	// 36 write subchunk2id "data"
	ofs << "data";

	// 40 write subchunk2size - size in bytes of the PCM data
	int subchunk2size = m_pcm_data.size() * (m_header.bits_per_sample / 8);
	assert(ofs.tellp()==40);
	ofs.write((char*)&subchunk2size , 4);

	// write the data
	ofs.write((char*)m_pcm_data.data() , subchunk2size);

	ofs.close();

	if(ofs.fail() || ofs.bad())
		throw std::runtime_error("WavFile::WavFile : writing of wavfile "+filename+" has failed.");
}