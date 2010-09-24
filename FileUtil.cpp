#include "FileUtil.h"
#include <iostream>
#include <fstream>
#include <cassert>
#include <zlib.h>

bool ReadFile(const string fileName,vector<unsigned char>& buffer)
{
	std::ifstream inFile(fileName.c_str(),std::ios::in | std::ios::binary);
	if(!inFile.good())
		return false;
	
	//Get length of file.
	inFile.seekg(0,std::ios::end);
	std::ifstream::pos_type fileLength = inFile.tellg();
	inFile.seekg(0,std::ios::beg);

	//Read entire file into buffer.
	//TODO: Check for super large file sizes.
	buffer.resize(fileLength);
	inFile.read((char*)&buffer[0],fileLength);
	return true;
}

bool WriteFile(const string fileName,const vector<unsigned char>& buffer)
{
	std::ofstream outFile(fileName.c_str(),std::ios::trunc | std::ios::binary);
	if(!outFile.good())
		return false;

	outFile.write((char*)&buffer[0],buffer.size());
	return true;
}

bool UncompressBuffer(const vector<unsigned char>& inputBuffer,vector<unsigned char>& outputBuffer)
{
	z_stream zStream;
	zStream.zalloc = Z_NULL;
	zStream.zfree = Z_NULL;
	zStream.opaque = Z_NULL;
	zStream.avail_in = 0;
	zStream.next_in = Z_NULL;
	if(inflateInit2(&zStream,16+MAX_WBITS) != Z_OK)
	{
		std::cerr << "Failed to initialize zlib inflation stream.\n";
		return false;
	}

	//Adjust buffer to fit entire buffer. If the result is bigger than this, then the file is not supported. (Probably a zip bomb)
	const int BUFFER_SIZE = 256000;
	outputBuffer.resize(BUFFER_SIZE);

	zStream.avail_in = inputBuffer.size();
	zStream.next_in = const_cast<Bytef*>(&inputBuffer[0]);
	zStream.avail_out = outputBuffer.size();
	zStream.next_out = &outputBuffer[0];
	const int ret = inflate(&zStream,Z_FINISH);
	if(ret != Z_STREAM_END)
	{
		std::cerr << "Failed to inflate stream with error " << ret << std::endl;
		inflateEnd(&zStream);
		return false;
	}

	inflateEnd(&zStream);

	outputBuffer.resize(zStream.total_out);
	return true;
}

bool CompressBuffer(const vector<unsigned char>& inputBuffer,vector<unsigned char>& outputBuffer)
{
	z_stream zStream;
	zStream.next_in = 0;
	zStream.zalloc = Z_NULL;
	zStream.zfree = Z_NULL;
	zStream.opaque = Z_NULL;
	if(deflateInit2(&zStream,Z_DEFAULT_COMPRESSION,Z_DEFLATED,31,8,Z_DEFAULT_STRATEGY) != Z_OK)
		return false;

	//We should never need to create a buffer bigger than this. Hopefully.
	const int BUFFER_SIZE = 256000;
	outputBuffer.resize(BUFFER_SIZE);

	zStream.avail_in = inputBuffer.size();
	zStream.next_in = const_cast<Bytef*>(&inputBuffer[0]);
	zStream.avail_out = outputBuffer.size();
	zStream.next_out = &outputBuffer[0];
	const int ret = deflate(&zStream,Z_FINISH);
	if(ret != Z_STREAM_END)
	{
		std::cerr << "Failed to deflate stream with error " << ret << std::endl;
		deflateEnd(&zStream);
		return false;
	}

	deflateEnd(&zStream);

	outputBuffer.resize(zStream.total_out);
	std::cout << "Compressed " << inputBuffer.size() << " down to " << outputBuffer.size() << std::endl;
	return true;
}

