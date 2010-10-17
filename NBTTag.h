#ifndef NBTTAG_H
#define NBTTAG_H

#include <string>
#include <vector>
#include <stdint.h>
#include <cstring>
using std::string;
using std::vector;

namespace NBT
{

struct Tag
{
	unsigned char type;
	string name;

	unsigned char byteValue;
	short shortValue;
	int intValue;
	int64_t longValue;
	float floatValue;
	double doubleValue;
	string stringValue;
	unsigned char listType;

	vector<Tag> childTags;
};

enum
{
	TAG_END = 0,
	TAG_BYTE = 1, //signed byte
	TAG_SHORT = 2, //signed short (16 bits, big endian)
	TAG_INT = 3, //signed int (32 bits, big endian)
	TAG_LONG = 4, //signed long (64 bits, big endian)
	TAG_FLOAT = 5, //float (32 bits, big endian, IEEE 754-2008, binary32)
	TAG_DOUBLE = 6, //double (64 bits, big endian, IEEE 754-2008, binary64)
	TAG_BYTE_ARRAY = 7, //TAG_INT length, and length bytes.
	TAG_STRING = 8, //TAG_SHORT length, and length bytes in UTF-8 format.
	TAG_LIST = 9, //TAG_BYTE typeId, TAG_INT length, length unnamed tags of typeId.
	TAG_COMPOUND = 10 //List of named tags until TAG_END is found. Can be nested.
};

//Deserialized (uncompressed) buffer into tag tree.
bool DeserializeTag(vector<unsigned char>& buffer,Tag& tag);
//Serializes tag tree into (uncompressed) buffer.
bool SerializeTag(vector<unsigned char>& buffer,Tag& tag);

//Print tag recursively in a tab tree style to standard out.
void PrintTag(Tag& tag);
//Searches children of parentTag for a tag named name. Returns NULL when not found.
Tag* GetChildNamedTag(NBT::Tag* parentTag,const string name);

};

#endif

