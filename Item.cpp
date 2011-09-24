#include "Item.h"
#include <iostream>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include "FilePath.h"
using std::ifstream;

namespace
{

Item TagToItem(NBT::Tag& tag)
{
	using NBT::Tag;

	Tag* idTag = GetChildNamedTag(&tag,"id");
	assert(idTag != NULL);
	Tag* damageTag = GetChildNamedTag(&tag,"Damage");
	assert(damageTag != NULL);
	Tag* countTag = GetChildNamedTag(&tag,"Count");
	assert(countTag != NULL);
	Tag* slotTag = GetChildNamedTag(&tag,"Slot");
	assert(slotTag != NULL);

	Item item;
	if(idTag != NULL)
		item.id = idTag->shortValue;
	else
	{
		std::cerr << "Warning: Item found without item tag.\n";
		PrintTag(tag);
	}
	if(damageTag != NULL)
		item.damage = damageTag->shortValue;
	else
	{
		std::cerr << "Warning: Item found without damage tag.\n";
		PrintTag(tag);
	}
	if(countTag != NULL)
		item.count = countTag->byteValue;
	else
	{
		std::cerr << "Warning: Item found without count tag.\n";
		PrintTag(tag);
	}
	if(slotTag != NULL)
		item.slot = slotTag->byteValue;
	else
	{
		std::cerr << "Warning: Item found without slot tag.\n";
		PrintTag(tag);
	}
	return item;
}

NBT::Tag ItemToTag(const Item& item)
{
	using namespace NBT;

	Tag tag;
	tag.type = TAG_COMPOUND;

	Tag idTag;
	idTag.type = TAG_SHORT;
	idTag.name = "id";
	idTag.shortValue = item.id;
	tag.childTags.push_back(idTag);

	Tag damageTag;
	damageTag.type = TAG_SHORT;
	damageTag.name = "Damage";
	damageTag.shortValue = item.damage;
	tag.childTags.push_back(damageTag);

	Tag countTag;
	countTag.type = TAG_BYTE;
	countTag.name = "Count";
	countTag.byteValue = item.count;
	tag.childTags.push_back(countTag);

	Tag slotTag;
	slotTag.type = TAG_BYTE;
	slotTag.name = "Slot";
	slotTag.byteValue = item.slot;
	tag.childTags.push_back(slotTag);

	return tag;
}

};

bool InventoryTagToItemMap(NBT::Tag* inventoryTag,ItemMap& itemMap)
{
	itemMap.clear();

	if(inventoryTag == NULL)
		return false;

	BOOST_FOREACH(NBT::Tag& childTag,inventoryTag->childTags)
	{
		Item item = TagToItem(childTag);
		itemMap[item.slot] = item;
	}

	return true;
}

bool ItemMapToInventoryTag(const ItemMap& itemMap,NBT::Tag* inventoryTag)
{
	if(inventoryTag == NULL)
		return false;

	inventoryTag->childTags.clear();
	BOOST_FOREACH(const ItemMap::value_type& itemIter,itemMap)
	{
		inventoryTag->childTags.push_back(ItemToTag(itemIter.second));
	}

	return true;
}

SlotNameBimap CreateSlotNameBimap()
{
	/* Inventory Slot Numbers
	 * 0-8 quickbar
	 * 9-35 inventory
	 * 100 feet
	 * 101 pants
	 * 102 chest
	 * 103 helmet
	 */

	SlotNameBimap slotNameBimap;
	slotNameBimap.insert(SlotNameBimap::value_type(0,"Quickbar 1"));
	slotNameBimap.insert(SlotNameBimap::value_type(1,"Quickbar 2"));
	slotNameBimap.insert(SlotNameBimap::value_type(2,"Quickbar 3"));
	slotNameBimap.insert(SlotNameBimap::value_type(3,"Quickbar 4"));
	slotNameBimap.insert(SlotNameBimap::value_type(4,"Quickbar 5"));
	slotNameBimap.insert(SlotNameBimap::value_type(5,"Quickbar 6"));
	slotNameBimap.insert(SlotNameBimap::value_type(6,"Quickbar 7"));
	slotNameBimap.insert(SlotNameBimap::value_type(7,"Quickbar 8"));
	slotNameBimap.insert(SlotNameBimap::value_type(8,"Quickbar 9"));

	slotNameBimap.insert(SlotNameBimap::value_type(9,"Inventory 1"));
	slotNameBimap.insert(SlotNameBimap::value_type(10,"Inventory 2"));
	slotNameBimap.insert(SlotNameBimap::value_type(11,"Inventory 3"));
	slotNameBimap.insert(SlotNameBimap::value_type(12,"Inventory 4"));
	slotNameBimap.insert(SlotNameBimap::value_type(13,"Inventory 5"));
	slotNameBimap.insert(SlotNameBimap::value_type(14,"Inventory 6"));
	slotNameBimap.insert(SlotNameBimap::value_type(15,"Inventory 7"));
	slotNameBimap.insert(SlotNameBimap::value_type(16,"Inventory 8"));
	slotNameBimap.insert(SlotNameBimap::value_type(17,"Inventory 9"));
	slotNameBimap.insert(SlotNameBimap::value_type(18,"Inventory 10"));
	slotNameBimap.insert(SlotNameBimap::value_type(19,"Inventory 11"));
	slotNameBimap.insert(SlotNameBimap::value_type(20,"Inventory 12"));
	slotNameBimap.insert(SlotNameBimap::value_type(21,"Inventory 13"));
	slotNameBimap.insert(SlotNameBimap::value_type(22,"Inventory 14"));
	slotNameBimap.insert(SlotNameBimap::value_type(23,"Inventory 15"));
	slotNameBimap.insert(SlotNameBimap::value_type(24,"Inventory 16"));
	slotNameBimap.insert(SlotNameBimap::value_type(25,"Inventory 17"));
	slotNameBimap.insert(SlotNameBimap::value_type(26,"Inventory 18"));
	slotNameBimap.insert(SlotNameBimap::value_type(27,"Inventory 19"));
	slotNameBimap.insert(SlotNameBimap::value_type(28,"Inventory 20"));
	slotNameBimap.insert(SlotNameBimap::value_type(29,"Inventory 21"));
	slotNameBimap.insert(SlotNameBimap::value_type(30,"Inventory 22"));
	slotNameBimap.insert(SlotNameBimap::value_type(31,"Inventory 23"));
	slotNameBimap.insert(SlotNameBimap::value_type(32,"Inventory 24"));
	slotNameBimap.insert(SlotNameBimap::value_type(33,"Inventory 25"));
	slotNameBimap.insert(SlotNameBimap::value_type(34,"Inventory 26"));
	slotNameBimap.insert(SlotNameBimap::value_type(35,"Inventory 27"));

	slotNameBimap.insert(SlotNameBimap::value_type(100,"Feet"));
	slotNameBimap.insert(SlotNameBimap::value_type(101,"Pants"));
	slotNameBimap.insert(SlotNameBimap::value_type(102,"Chest"));
	slotNameBimap.insert(SlotNameBimap::value_type(103,"Helmet"));

	return slotNameBimap;
}

string SlotName(const unsigned char slot)
{
	static SlotNameBimap slotNameBimap = CreateSlotNameBimap();

	SlotNameBimap::left_const_iterator slotNameIter = slotNameBimap.left.find(slot);
	assert(slotNameIter != slotNameBimap.left.end());
	return (*slotNameIter).second;
}

ItemTypeBimap CreateItemTypeBimap()
{
	ItemTypeBimap itemTypeBimap;
#define ADD_ITEM_TYPE(value,name) itemTypeBimap.insert(ItemTypeBimap::value_type(value,name))
	//ADD_ITEM_TYPE(0,"Air"); //Commented out because it's useless and causes crashes. Left in for completion.
	ADD_ITEM_TYPE(1,"Stone");
	ADD_ITEM_TYPE(2,"Grass");
	ADD_ITEM_TYPE(3,"Dirt");
	ADD_ITEM_TYPE(4,"Cobblestone");
	ADD_ITEM_TYPE(5,"Wooden Plank");
	ADD_ITEM_TYPE(6,"Sapling");
	ADD_ITEM_TYPE(7,"Bedrock");
	ADD_ITEM_TYPE(8,"Water");
	ADD_ITEM_TYPE(9,"Stationary water");
	ADD_ITEM_TYPE(10,"Lava");
	ADD_ITEM_TYPE(11,"Stationary lava");
	ADD_ITEM_TYPE(12,"Sand");
	ADD_ITEM_TYPE(13,"Gravel");
	ADD_ITEM_TYPE(14,"Gold Ore");
	ADD_ITEM_TYPE(15,"Iron Ore");
	ADD_ITEM_TYPE(16,"Coal Ore");
	ADD_ITEM_TYPE(17,"Wood");
	ADD_ITEM_TYPE(18,"Leaves");
	ADD_ITEM_TYPE(19,"Sponge");
	ADD_ITEM_TYPE(20,"Glass");
	ADD_ITEM_TYPE(21,"Lapis Lazuli Ore");
	ADD_ITEM_TYPE(22,"Lapis Lazuli Block");
	ADD_ITEM_TYPE(23,"Dispenser");
	ADD_ITEM_TYPE(24,"Sandstone");
	ADD_ITEM_TYPE(25,"Note Block");
	//ADD_ITEM_TYPE(26,"Bed"); //World block, not for inventory.
	ADD_ITEM_TYPE(27,"Powered Rail");
	ADD_ITEM_TYPE(28,"Detector Rail");
	ADD_ITEM_TYPE(29,"Sticky Piston");
	ADD_ITEM_TYPE(30,"Cobweb");
	ADD_ITEM_TYPE(31,"Tall Grass");
	ADD_ITEM_TYPE(32,"Dead Shrubs");
	ADD_ITEM_TYPE(33,"Piston");
	ADD_ITEM_TYPE(34,"Piston Extension");
	ADD_ITEM_TYPE(35,"Wool");
	ADD_ITEM_TYPE(36,"Block 36");
	ADD_ITEM_TYPE(37,"Dandelion");
	ADD_ITEM_TYPE(38,"Rose");
	ADD_ITEM_TYPE(39,"Brown Mushroom");
	ADD_ITEM_TYPE(40,"Red Mushroom");
	ADD_ITEM_TYPE(41,"Gold Block");
	ADD_ITEM_TYPE(42,"Iron Block");
	ADD_ITEM_TYPE(43,"Double Slabs");
	ADD_ITEM_TYPE(44,"Slabs");
	ADD_ITEM_TYPE(45,"Brick Block");
	ADD_ITEM_TYPE(46,"TNT");
	ADD_ITEM_TYPE(47,"Bookshelf");
	ADD_ITEM_TYPE(48,"Moss Stone");
	ADD_ITEM_TYPE(49,"Obsidian");
	ADD_ITEM_TYPE(50,"Torch");
	ADD_ITEM_TYPE(51,"Fire");
	ADD_ITEM_TYPE(52,"Monster Spawner");
	ADD_ITEM_TYPE(53,"Wooden Stairs");
	ADD_ITEM_TYPE(54,"Chest");
	ADD_ITEM_TYPE(55,"Redstone Wire");
	ADD_ITEM_TYPE(56,"Diamond Ore");
	ADD_ITEM_TYPE(57,"Diamond Block");
	ADD_ITEM_TYPE(58,"Crafting Table");
	ADD_ITEM_TYPE(59,"Seeds");
	ADD_ITEM_TYPE(60,"Farmland");
	ADD_ITEM_TYPE(61,"Furnace");
	ADD_ITEM_TYPE(62,"Burning Furnace");
	ADD_ITEM_TYPE(63,"Sign Post");
	//ADD_ITEM_TYPE(64,"Wooden Door"); //Creates a rather useless half door that might cause Minecraft to crash.
	ADD_ITEM_TYPE(65,"Ladders");
	ADD_ITEM_TYPE(66,"Rails");
	ADD_ITEM_TYPE(67,"Cobblestone Stairs");
	ADD_ITEM_TYPE(68,"Wall Sign");
	ADD_ITEM_TYPE(69,"Lever");
	ADD_ITEM_TYPE(70,"Stone Pressure Plate");
	ADD_ITEM_TYPE(71,"Iron Door"); //Creates a rather useless half door that might cause Minecraft to crash.
	ADD_ITEM_TYPE(72,"Wooden Pressure Plate");
	ADD_ITEM_TYPE(73,"Redstone Ore");
	ADD_ITEM_TYPE(74,"Glowing Redstone Ore");
	ADD_ITEM_TYPE(75,"Redstone Torch (\"off\" state)");
	ADD_ITEM_TYPE(76,"Redstone Torch (\"on\" state)");
	ADD_ITEM_TYPE(77,"Stone Button");
	ADD_ITEM_TYPE(78,"Snow");
	ADD_ITEM_TYPE(79,"Ice");
	ADD_ITEM_TYPE(80,"Snow Block");
	ADD_ITEM_TYPE(81,"Cactus");
	ADD_ITEM_TYPE(82,"Clay Block");
	ADD_ITEM_TYPE(83,"Sugar Cane");
	ADD_ITEM_TYPE(84,"Jukebox");
	ADD_ITEM_TYPE(85,"Fence");
	ADD_ITEM_TYPE(86,"Pumpkin");
	ADD_ITEM_TYPE(87,"Netherrack");
	ADD_ITEM_TYPE(88,"Soul Sand");
	ADD_ITEM_TYPE(89,"Glowstone Block");
	ADD_ITEM_TYPE(90,"Portal");
	ADD_ITEM_TYPE(91,"Jack-O-Lantern");
	ADD_ITEM_TYPE(92,"Cake Block");
	//ADD_ITEM_TYPE(93,"Redstone Repeater (\"off\" state)"); //Same as "Redstone Repeater" below.
	//ADD_ITEM_TYPE(94,"Redstone Repeater (\"on\" state)"); //Same as "Redstone Repeater" below.
	ADD_ITEM_TYPE(95,"Locked Chest");
	ADD_ITEM_TYPE(96,"Trapdoor");
	ADD_ITEM_TYPE(97,"Hidden Silverfish");
	ADD_ITEM_TYPE(98,"Stone Brick");
	ADD_ITEM_TYPE(99,"Brown Mushroom");
	ADD_ITEM_TYPE(100,"Red Mushroom");
	ADD_ITEM_TYPE(101,"Iron Bars");
	ADD_ITEM_TYPE(102,"Glass Pane");
	ADD_ITEM_TYPE(103,"Melon");
	ADD_ITEM_TYPE(104,"Pumpkin Stem");
	ADD_ITEM_TYPE(105,"Melon Stem");
	ADD_ITEM_TYPE(106,"Vines");
	ADD_ITEM_TYPE(107,"Fence Gate");
	ADD_ITEM_TYPE(108,"Brick Stairs");
	ADD_ITEM_TYPE(109,"Stone Brick Stairs");
	ADD_ITEM_TYPE(110,"Mycelium");
	ADD_ITEM_TYPE(111,"Lily Pad");
	ADD_ITEM_TYPE(112,"Nether Brick");
	ADD_ITEM_TYPE(113,"Nether Brick Fence");
	ADD_ITEM_TYPE(114,"Nether Brick Stairs");
	ADD_ITEM_TYPE(115,"Nether Wart");
	ADD_ITEM_TYPE(256,"Iron Shovel");
	ADD_ITEM_TYPE(257,"Iron Pickaxe");
	ADD_ITEM_TYPE(258,"Iron Axe");
	ADD_ITEM_TYPE(259,"Flint and Steel");
	ADD_ITEM_TYPE(260,"Apple");
	ADD_ITEM_TYPE(261,"Bow");
	ADD_ITEM_TYPE(262,"Arrow");
	ADD_ITEM_TYPE(263,"Coal");
	ADD_ITEM_TYPE(264,"Diamond");
	ADD_ITEM_TYPE(265,"Iron Ingot");
	ADD_ITEM_TYPE(266,"Gold Ingot");
	ADD_ITEM_TYPE(267,"Iron Sword");
	ADD_ITEM_TYPE(268,"Wooden Sword");
	ADD_ITEM_TYPE(269,"Wooden Shovel");
	ADD_ITEM_TYPE(270,"Wooden Pickaxe");
	ADD_ITEM_TYPE(271,"Wooden Axe");
	ADD_ITEM_TYPE(272,"Stone Sword");
	ADD_ITEM_TYPE(273,"Stone Shovel");
	ADD_ITEM_TYPE(274,"Stone Pickaxe");
	ADD_ITEM_TYPE(275,"Stone Axe");
	ADD_ITEM_TYPE(276,"Diamond Sword");
	ADD_ITEM_TYPE(277,"Diamond Shovel");
	ADD_ITEM_TYPE(278,"Diamond Pickaxe");
	ADD_ITEM_TYPE(279,"Diamond Axe");
	ADD_ITEM_TYPE(280,"Stick");
	ADD_ITEM_TYPE(281,"Bowl");
	ADD_ITEM_TYPE(282,"Mushroom Soup");
	ADD_ITEM_TYPE(283,"Gold Sword");
	ADD_ITEM_TYPE(284,"Gold Shovel");
	ADD_ITEM_TYPE(285,"Gold Pickaxe");
	ADD_ITEM_TYPE(286,"Gold Axe");
	ADD_ITEM_TYPE(287,"String");
	ADD_ITEM_TYPE(288,"Feather");
	ADD_ITEM_TYPE(289,"Gunpowder");
	ADD_ITEM_TYPE(290,"Wooden Hoe");
	ADD_ITEM_TYPE(291,"Stone Hoe");
	ADD_ITEM_TYPE(292,"Iron Hoe");
	ADD_ITEM_TYPE(293,"Diamond Hoe");
	ADD_ITEM_TYPE(294,"Gold Hoe");
	ADD_ITEM_TYPE(295,"Seeds");
	ADD_ITEM_TYPE(296,"Wheat");
	ADD_ITEM_TYPE(297,"Bread");
	ADD_ITEM_TYPE(298,"Leather Cap");
	ADD_ITEM_TYPE(299,"Leather Tunic");
	ADD_ITEM_TYPE(300,"Leather Pants");
	ADD_ITEM_TYPE(301,"Leather Boots");
	ADD_ITEM_TYPE(302,"Chain Helmet");
	ADD_ITEM_TYPE(303,"Chain Chestplate");
	ADD_ITEM_TYPE(304,"Chain Leggings");
	ADD_ITEM_TYPE(305,"Chain Boots");
	ADD_ITEM_TYPE(306,"Iron Helmet");
	ADD_ITEM_TYPE(307,"Iron Chestplate");
	ADD_ITEM_TYPE(308,"Iron Leggings");
	ADD_ITEM_TYPE(309,"Iron Boots");
	ADD_ITEM_TYPE(310,"Diamond Helmet");
	ADD_ITEM_TYPE(311,"Diamond Chestplate");
	ADD_ITEM_TYPE(312,"Diamond Leggings");
	ADD_ITEM_TYPE(313,"Diamond Boots");
	ADD_ITEM_TYPE(314,"Gold Helmet");
	ADD_ITEM_TYPE(315,"Gold Chestplate");
	ADD_ITEM_TYPE(316,"Gold Leggings");
	ADD_ITEM_TYPE(317,"Gold Boots");
	ADD_ITEM_TYPE(318,"Flint");
	ADD_ITEM_TYPE(319,"Raw Porkchop");
	ADD_ITEM_TYPE(320,"Cooked Porkchop");
	ADD_ITEM_TYPE(321,"Paintings");
	ADD_ITEM_TYPE(322,"Golden Apple");
	ADD_ITEM_TYPE(323,"Sign");
	ADD_ITEM_TYPE(324,"Wooden door");
	ADD_ITEM_TYPE(325,"Bucket");
	ADD_ITEM_TYPE(326,"Water bucket");
	ADD_ITEM_TYPE(327,"Lava bucket");
	ADD_ITEM_TYPE(328,"Minecart");
	ADD_ITEM_TYPE(329,"Saddle");
	ADD_ITEM_TYPE(330,"Iron door");
	ADD_ITEM_TYPE(331,"Redstone");
	ADD_ITEM_TYPE(332,"Snowball");
	ADD_ITEM_TYPE(333,"Boat");
	ADD_ITEM_TYPE(334,"Leather");
	ADD_ITEM_TYPE(335,"Milk");
	ADD_ITEM_TYPE(336,"Clay Brick");
	ADD_ITEM_TYPE(337,"Clay");
	ADD_ITEM_TYPE(338,"Sugar Cane");
	ADD_ITEM_TYPE(339,"Paper");
	ADD_ITEM_TYPE(340,"Book");
	ADD_ITEM_TYPE(341,"Slimeball");
	ADD_ITEM_TYPE(342,"Storage Minecart");
	ADD_ITEM_TYPE(343,"Powered Minecart");
	ADD_ITEM_TYPE(344,"Egg");
	ADD_ITEM_TYPE(345,"Compass");
	ADD_ITEM_TYPE(346,"Fishing Rod");
	ADD_ITEM_TYPE(347,"Clock");
	ADD_ITEM_TYPE(348,"Glowstone Dust");
	ADD_ITEM_TYPE(349,"Raw Fish");
	ADD_ITEM_TYPE(350,"Cooked Fish");
	ADD_ITEM_TYPE(351,"Dye");
	ADD_ITEM_TYPE(352,"Bone");
	ADD_ITEM_TYPE(353,"Sugar");
	ADD_ITEM_TYPE(354,"Cake");
	ADD_ITEM_TYPE(355,"Bed");
	ADD_ITEM_TYPE(356,"Redstone Repeater");
	ADD_ITEM_TYPE(357,"Cookie");
	ADD_ITEM_TYPE(358,"Map");
	ADD_ITEM_TYPE(359,"Shears");
	ADD_ITEM_TYPE(360,"Melon Slice");
	ADD_ITEM_TYPE(361,"Pumpkin Seeds");
	ADD_ITEM_TYPE(362,"Melon Seeds");
	ADD_ITEM_TYPE(363,"Raw Beef");
	ADD_ITEM_TYPE(364,"Steak");
	ADD_ITEM_TYPE(365,"Raw Chicken");
	ADD_ITEM_TYPE(366,"Cooked Chicken");
	ADD_ITEM_TYPE(367,"Rotten Flesh");
	ADD_ITEM_TYPE(368,"Ender Pearl");
	ADD_ITEM_TYPE(369,"Blaze Rod");
	ADD_ITEM_TYPE(370,"Ghast Tear");
	ADD_ITEM_TYPE(371,"Gold Nugget");
	ADD_ITEM_TYPE(372,"Nether Wart");
	ADD_ITEM_TYPE(2256,"Gold Music Disc");
	ADD_ITEM_TYPE(2257,"Green Music Disc");
#undef ADD_ITEM_TYPE

	//Try and load a custom comma delimitered file that can overwrite default items.
	//File has two columns in the order of: value,name
	//Value must be a number and name should only use ASCII characters.
	std::ifstream inFile((FilePath::GetInvGridSettingsDirectory() + "/items.csv").c_str());
	while(inFile.good())
	{
		//Read a line.
		string line;
		std::getline(inFile,line);

		//Split line by commas into 4 fields that make up the item.
		vector<string> columns;
		boost::split(columns,line,boost::algorithm::is_any_of(","),boost::token_compress_on);
		if(columns.size() != 2)
			continue;

		try
		{
			const short value = boost::lexical_cast<short>(columns[0]);
			const string name = columns[1];

			//Delete any items that match value OR name.
			itemTypeBimap.left.erase(value);
			itemTypeBimap.right.erase(name);
			
			//Add item or overwrite if one already exists with that value.
			itemTypeBimap.insert(ItemTypeBimap::value_type(value,name));
		}
		catch(...)
		{
			//Invalid row, skip.
		}
	}

	return itemTypeBimap;
}

string ItemTypeName(const short value)
{
	static ItemTypeBimap itemTypeBimap = CreateItemTypeBimap();

	ItemTypeBimap::left_const_iterator itemTypeIter = itemTypeBimap.left.find(value);
	if(itemTypeIter == itemTypeBimap.left.end())
		return boost::lexical_cast<string>(value);
	else
		return (*itemTypeIter).second;
}

ItemDetailMap CreateItemDetailMap()
{
	ItemDetailMap itemDetailMap;
/* TODO: Maybe InvGrid will support changing an item based on damage/detail someday.
#define ADD_ITEM_DETAIL(v,damage,name) v.push_back(ItemDetail(damage,name));
	//Wool
	vector<ItemDetail> woolDetails;
	ADD_ITEM_DETAIL(woolDetails,0,"Plain");
	ADD_ITEM_DETAIL(woolDetails,1,"Orange");
	ADD_ITEM_DETAIL(woolDetails,2,"Magenta");
	ADD_ITEM_DETAIL(woolDetails,3,"Light Blue");
	ADD_ITEM_DETAIL(woolDetails,4,"Yellow");
	ADD_ITEM_DETAIL(woolDetails,5,"Light Green");
	ADD_ITEM_DETAIL(woolDetails,6,"Pink");
	ADD_ITEM_DETAIL(woolDetails,7,"Black");
	ADD_ITEM_DETAIL(woolDetails,8,"Grey");
	ADD_ITEM_DETAIL(woolDetails,9,"Cyan");
	ADD_ITEM_DETAIL(woolDetails,10,"Purple");
	ADD_ITEM_DETAIL(woolDetails,11,"Blue");
	ADD_ITEM_DETAIL(woolDetails,12,"Brown");
	ADD_ITEM_DETAIL(woolDetails,13,"Dark Green");
	ADD_ITEM_DETAIL(woolDetails,14,"Red");
	ADD_ITEM_DETAIL(woolDetails,15,"Black");
	itemDetailMap[35] = woolDetails;

	//Dye
	vector<ItemDetail> dyeDetails;
	ADD_ITEM_DETAIL(dyeDetails,0,"Ink Sac");
	ADD_ITEM_DETAIL(dyeDetails,1,"Rose Red Dye");
	ADD_ITEM_DETAIL(dyeDetails,2,"Cactus Green Dye");
	ADD_ITEM_DETAIL(dyeDetails,3,"Cocoa Beans Dye");
	ADD_ITEM_DETAIL(dyeDetails,4,"Lapis Lazuli Dye");
	ADD_ITEM_DETAIL(dyeDetails,5,"Purple Dye");
	ADD_ITEM_DETAIL(dyeDetails,6,"Cyan Dye");
	ADD_ITEM_DETAIL(dyeDetails,7,"Light Gray Dye");
	ADD_ITEM_DETAIL(dyeDetails,8,"Gray Dye");
	ADD_ITEM_DETAIL(dyeDetails,9,"Pink Dye");
	ADD_ITEM_DETAIL(dyeDetails,10,"Lime Dye");
	ADD_ITEM_DETAIL(dyeDetails,11,"Dandelion Yellow Dye");
	ADD_ITEM_DETAIL(dyeDetails,12,"Light Blue Dye");
	ADD_ITEM_DETAIL(dyeDetails,13,"Magenta Dye");
	ADD_ITEM_DETAIL(dyeDetails,14,"Orange Dye");
	ADD_ITEM_DETAIL(dyeDetails,15,"Bone Meal Dye");
	itemDetailMap[351] = dyeDetails;
#undef ADD_ITEM_DETAIL
*/

	return itemDetailMap;
}

