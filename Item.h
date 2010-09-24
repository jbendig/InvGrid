#ifndef ITEM_H
#define ITEM_H

#include <map>
#include <string>
#include <boost/bimap.hpp>
using std::map;
using std::string;

struct Item
{
	short id; //Type
	short damage; //0 is undamaged. Higher is worse.
	unsigned char count; //1-255
	unsigned char slot; //Inventory slot.
};
typedef map<unsigned char,Item> ItemMap;

typedef boost::bimap<unsigned char,string> SlotNameBimap;
SlotNameBimap CreateSlotNameBimap();
string SlotName(const unsigned char slot);

typedef boost::bimap<short,string> ItemTypeBimap;
ItemTypeBimap CreateItemTypeBimap();
string ItemTypeName(const short value);

#endif

