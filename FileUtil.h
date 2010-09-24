#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <string>
#include <vector>
using std::string;
using std::vector;

bool ReadFile(const string fileName,vector<unsigned char>& buffer);
bool WriteFile(const string fileName,const vector<unsigned char>& buffer);

bool UncompressBuffer(const vector<unsigned char>& inputBuffer,vector<unsigned char>& outputBuffer);
bool CompressBuffer(const vector<unsigned char>& inputBuffer,vector<unsigned char>& outputBuffer);

#endif

