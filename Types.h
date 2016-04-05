#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <string>
#include <cstring>
#include <boost/static_assert.hpp>
using std::vector;
using std::string;

BOOST_STATIC_ASSERT(sizeof(unsigned char) == 1);
BOOST_STATIC_ASSERT(sizeof(short) == 2);
BOOST_STATIC_ASSERT(sizeof(int) == 4);
BOOST_STATIC_ASSERT(sizeof(int64_t) == 8);
BOOST_STATIC_ASSERT(sizeof(float) == 4);
BOOST_STATIC_ASSERT(sizeof(double) == 8);

//Generic byte switching function.
template <class T>
T FlipByteOrder(T input)
{
	T output;
	unsigned char* inputBytes = (unsigned char*)&input;
	unsigned char* outputBytes = (unsigned char*)&output;

	for(unsigned int x = 0;x < sizeof(T);++x)
	{
		outputBytes[x] = inputBytes[(sizeof(T) - x) - 1];
	}

	return output;
}

template <unsigned char>
unsigned char FlipByteOrder(unsigned char input)
{
	return input;
}

//Defined to flip on little-endian and to do nothing on big-endian.
#define NativeToBigEndian FlipByteOrder
#define BigEndianToNative FlipByteOrder

template <class T>
T ReadValue(unsigned int& index,const vector<unsigned char>& buffer)
{
	T value;
	memcpy(&value,&buffer[index],sizeof(value));
	value = BigEndianToNative(value);
	index += sizeof(value);

	return value;
}

template <class T>
void WriteValue(vector<unsigned char>& buffer,T value)
{
	value = NativeToBigEndian(value);
	const size_t writeIndex = buffer.size();
	buffer.resize(buffer.size() + sizeof(T));
	memcpy(&buffer[writeIndex],&value,sizeof(T));
}

//All of these functions are UNSAFE and do not check the buffer's bounds.
//Values are assumed to be stored in Big-Endian order. They will automatically be flipped to host order.
#define ReadByte ReadValue<unsigned char>
#define ReadShort ReadValue<short>
#define ReadInt ReadValue<int>
#define ReadLong ReadValue<int64_t>
#define ReadFloat ReadValue<float>
#define ReadDouble ReadValue<double>

string ReadString(unsigned int& index,const vector<unsigned char>& buffer);

#define WriteByte WriteValue<unsigned char>
#define WriteShort WriteValue<short>
#define WriteInt WriteValue<int>
#define WriteLong WriteValue<int64_t>
#define WriteFloat WriteValue<float>
#define WriteDouble WriteValue<double>

void WriteString(vector<unsigned char>& buffer,string value);

#endif

