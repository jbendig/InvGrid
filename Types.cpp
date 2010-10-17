#include "Types.h"
#include <cstring>

string ReadString(unsigned int& index,const vector<unsigned char>& buffer)
{
	const short stringLength = ReadShort(index,buffer);

	string text;
	text.resize(stringLength);
	memcpy((void*)text.data(),(void*)&buffer[index],stringLength);
	index += stringLength;

	return text;
}

void WriteString(vector<unsigned char>& buffer,string value)
{
	WriteShort(buffer,value.length());

	const size_t writeIndex = buffer.size();
	buffer.resize(buffer.size() + value.length());
	memcpy(&buffer[writeIndex],value.c_str(),value.length());
}

