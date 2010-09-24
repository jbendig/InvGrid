#include "NBTTag.h"
#include <iostream>
#include <exception>
#include <cassert>
#include "Types.h"
using namespace NBT;

//Private functions that do the actual parsing and formatting.
namespace
{

class DataException : public std::exception
{
};

//Deserialize functions.
void DeserializeListTag(unsigned int& index,const vector<unsigned char>& buffer,Tag& tag);
void DeserializeCompoundTag(unsigned int& index,const vector<unsigned char>& buffer,Tag& tag);

void DeserializeTagData(unsigned int& index,const vector<unsigned char>& buffer,Tag& tag)
{
	switch(tag.type)
	{
		case TAG_BYTE:
			tag.byteValue = ReadByte(index,buffer);
			break;
		case TAG_SHORT:
			tag.shortValue = ReadShort(index,buffer);
			break;
		case TAG_INT:
			tag.intValue = ReadInt(index,buffer);
			break;
		case TAG_LONG:
			tag.longValue = ReadLong(index,buffer);
			break;
		case TAG_FLOAT:
			tag.floatValue = ReadFloat(index,buffer);
			break;
		case TAG_DOUBLE:
			tag.doubleValue = ReadDouble(index,buffer);
			break;
		case TAG_BYTE_ARRAY:
			std::cerr << "Byte arrays are not supported.\n";
			throw DataException();
			break;
		case TAG_STRING:
			tag.stringValue = ReadString(index,buffer);
			break;
		case TAG_LIST:
			DeserializeListTag(index,buffer,tag);
			break;
		case TAG_COMPOUND:
			DeserializeCompoundTag(index,buffer,tag);
			break;
		default:
			std::cerr << "Encountered unknown tag type " << (int)tag.type << std::endl;
			throw DataException();
	};
}

void DeserializeListTag(unsigned int& index,const vector<unsigned char>& buffer,Tag& tag)
{
	tag.listType = ReadByte(index,buffer);
	const int tagCount = ReadInt(index,buffer);
	for(int x = 0;x < tagCount;++x)
	{
		Tag childTag;
		childTag.type = tag.listType;
		DeserializeTagData(index,buffer,childTag);

		tag.childTags.push_back(childTag);
	}
}

void DeserializeCompoundTag(unsigned int& index,const vector<unsigned char>& buffer,Tag& tag)
{
	const unsigned int indexMax = buffer.size();
	while(index < indexMax)
	{
		Tag childTag;
		childTag.type = ReadByte(index,buffer);
		if(childTag.type == TAG_END)
			return;
		childTag.name = ReadString(index,buffer);
		DeserializeTagData(index,buffer,childTag);

		tag.childTags.push_back(childTag);
	}
}

//Serialize functions.
void SerializeListTag(vector<unsigned char>& buffer,Tag& tag);
void SerializeCompoundTag(vector<unsigned char>& buffer,Tag& tag);

void SerializeTagData(vector<unsigned char>& buffer,Tag& tag)
{
	switch(tag.type)
	{
		case TAG_BYTE:
			WriteByte(buffer,tag.byteValue);
			break;
		case TAG_SHORT:
			WriteShort(buffer,tag.shortValue);
			break;
		case TAG_INT:
			WriteInt(buffer,tag.intValue);
			break;
		case TAG_LONG:
			WriteLong(buffer,tag.longValue);
			break;
		case TAG_FLOAT:
			WriteFloat(buffer,tag.floatValue);
			break;
		case TAG_DOUBLE:
			WriteDouble(buffer,tag.doubleValue);
			break;
		case TAG_BYTE_ARRAY:
			break;
		case TAG_STRING:
			WriteString(buffer,tag.stringValue);
			break;
		case TAG_LIST:
			SerializeListTag(buffer,tag);
			break;
		case TAG_COMPOUND:
			SerializeCompoundTag(buffer,tag);
			break;
		default:
			std::cerr << "Unable to serialize unknown child tag of type " << (int)tag.type << std::endl;
			throw DataException();
	}
}

void SerializeListTag(vector<unsigned char>& buffer,Tag& tag)
{
	WriteByte(buffer,tag.listType);
	WriteInt(buffer,tag.childTags.size());

	for(unsigned int x = 0;x < tag.childTags.size();++x)
	{
		Tag childTag = tag.childTags[x];
		SerializeTagData(buffer,childTag);
	}
}

void SerializeCompoundTag(vector<unsigned char>& buffer,Tag& tag)
{
	for(unsigned int x = 0;x < tag.childTags.size();++x)
	{
		Tag childTag = tag.childTags[x];
		WriteByte(buffer,childTag.type);
		WriteString(buffer,childTag.name);
		SerializeTagData(buffer,childTag);
	}

	WriteByte(buffer,TAG_END);
}

//Printing functions

void PrintTagRecursive(Tag& tag,int& depth)
{
	for(int x = 0;x < depth;++x)
	{
		std::cout << "\t";
	}

	switch(tag.type)
	{
		case TAG_BYTE:
			std::cout << "TAG_BYTE " << tag.name << " " << (int)tag.byteValue << std::endl;
			break;
		case TAG_SHORT:
			std::cout << "TAG_SHORT " << tag.name << " " << tag.shortValue << std::endl;
			break;
		case TAG_INT:
			std::cout << "TAG_INT " << tag.name << " " << tag.intValue << std::endl;
			break;
		case TAG_LONG:
			std::cout << "TAG_LONG " << tag.name << " " << tag.longValue << std::endl;
			break;
		case TAG_FLOAT:
			std::cout << "TAG_FLOAT " << tag.name << " " << tag.floatValue << std::endl;
			break;
		case TAG_DOUBLE:
			std::cout << "TAG_DOUBLE " << tag.name << " " << tag.doubleValue << std::endl;
			break;
		case TAG_BYTE_ARRAY:
			std::cout << "TAG_BYTE_ARRAY " << tag.name << std::endl;
			break;
		case TAG_STRING:
			std::cout << "TAG_STRING " << tag.name << " " << tag.stringValue << std::endl;
			break;
		case TAG_LIST:
			std::cout << "TAG_LIST " << tag.name << " with " << (int)tag.listType << " type tags\n";
			break;
		case TAG_COMPOUND:
			std::cout << "TAG_COMPOUND " << tag.name << std::endl;
			break;
		default:
			std::cout << "TAG_UNKNOWN\n";
	}

	depth++;
	for(unsigned int x = 0;x < tag.childTags.size();++x)
	{
		PrintTagRecursive(tag.childTags[x],depth);
	}
	depth--;
}

};

bool NBT::DeserializeTag(vector<unsigned char>& buffer,Tag& tag)
{
	//First tag must be a TAG_COMPOUND or else the format is unsupported.
	assert(buffer[0] == TAG_COMPOUND);

	unsigned int index = 0;
	tag.type = ReadByte(index,buffer);
	tag.name = ReadString(index,buffer);

	try
	{
		DeserializeCompoundTag(index,buffer,tag);
	}
	catch(DataException&)
	{
		return false;
	}

	return true;
}

bool NBT::SerializeTag(vector<unsigned char>& buffer,Tag& tag)
{
	//Top tag must be a compound tag or else the format is unsupported.
	assert(tag.type == TAG_COMPOUND);

	//Write header of first compound tag. Child compound tags will have their headers written by parent compound tag.
	WriteByte(buffer,TAG_COMPOUND);
	WriteString(buffer,tag.name);

	try
	{
		SerializeCompoundTag(buffer,tag);
	}
	catch(DataException&)
	{
		return false;
	}

	return true;
}

void NBT::PrintTag(Tag& tag)
{
	int depth = 0;
	PrintTagRecursive(tag,depth);
}

NBT::Tag* NBT::GetChildNamedTag(NBT::Tag* parentTag,const string name)
{
	for(unsigned int x = 0;x < parentTag->childTags.size();++x)
	{
		if(parentTag->childTags[x].name == name)
			return &parentTag->childTags[x];
	}

	return NULL;
}

