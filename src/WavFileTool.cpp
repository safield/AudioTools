#include "WavFile.h"
#include "LogStream.h"
#include "assert.h"

using namespace WavTools;

int main(int argc, char const *argv[])
{

	std::shared_ptr<WavFile> curr_wav = WavFile::LoadFromFile("test.wav");
    curr_wav->WriteToFile("new.wav");
    WavFile::LoadFromFile("new.wav");

	return 0;
}
