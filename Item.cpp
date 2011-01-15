#include "Item.h"
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

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
	ADD_ITEM_TYPE(9,"Stationary Water");
	ADD_ITEM_TYPE(10,"Lava");
	ADD_ITEM_TYPE(11,"Stationary Lava");
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
	ADD_ITEM_TYPE(26,"Aqua Green Cloth");
	ADD_ITEM_TYPE(27,"Cyan Cloth");
	ADD_ITEM_TYPE(28,"Blue Cloth");
	ADD_ITEM_TYPE(29,"Purple Cloth");
	ADD_ITEM_TYPE(30,"Indigo Cloth");
	ADD_ITEM_TYPE(31,"Violet Cloth");
	ADD_ITEM_TYPE(32,"Magenta Cloth");
	ADD_ITEM_TYPE(33,"Pink Cloth");
	ADD_ITEM_TYPE(34,"Black Cloth");
	ADD_ITEM_TYPE(35,"Gray Cloth");
	ADD_ITEM_TYPE(36,"Cloth");
	ADD_ITEM_TYPE(37,"Yellow flower");
	ADD_ITEM_TYPE(38,"Red Rose");
	ADD_ITEM_TYPE(39,"Brown Mushroom");
	ADD_ITEM_TYPE(40,"Red Mushroom");
	ADD_ITEM_TYPE(41,"Gold Block");
	ADD_ITEM_TYPE(42,"Iron Block");
	ADD_ITEM_TYPE(43,"Double Stone Slab");
	ADD_ITEM_TYPE(44,"Stone Slab");
	ADD_ITEM_TYPE(45,"Brick");
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
	ADD_ITEM_TYPE(58,"Workbench");
	ADD_ITEM_TYPE(59,"Crops");
	ADD_ITEM_TYPE(60,"Soil");
	ADD_ITEM_TYPE(61,"Furnace");
	ADD_ITEM_TYPE(62,"Burning Furnace");
	ADD_ITEM_TYPE(63,"Sign Post");
	//ADD_ITEM_TYPE(64,"Wooden Door"); //Creates a rather useless half door that might cause Minecraft to crash.
	ADD_ITEM_TYPE(65,"Ladder");
	ADD_ITEM_TYPE(66,"Minecart Tracks");
	ADD_ITEM_TYPE(67,"Cobblestone Stairs");
	ADD_ITEM_TYPE(68,"Wall Sign");
	ADD_ITEM_TYPE(69,"Lever");
	ADD_ITEM_TYPE(70,"Stone Pressure Plate");
	//ADD_ITEM_TYPE(71,"Iron Door"); //Creates a rather useless half door that might cause Minecraft to crash.
	ADD_ITEM_TYPE(72,"Wooden Pressure Plate");
	ADD_ITEM_TYPE(73,"Redstone Ore");
	ADD_ITEM_TYPE(74,"Glowing Redstone Ore");
	ADD_ITEM_TYPE(75,"Redstone torch (\"off\" state)");
	ADD_ITEM_TYPE(76,"Redstone torch (\"on\" state)");
	ADD_ITEM_TYPE(77,"Stone Button");
	ADD_ITEM_TYPE(78,"Snow");
	ADD_ITEM_TYPE(79,"Ice");
	ADD_ITEM_TYPE(80,"Snow Block");
	ADD_ITEM_TYPE(81,"Cactus");
	ADD_ITEM_TYPE(82,"Clay");
	ADD_ITEM_TYPE(83,"Sugar Cane");
	ADD_ITEM_TYPE(84,"Jukebox");
	ADD_ITEM_TYPE(85,"Fence");
	ADD_ITEM_TYPE(86,"Pumpkin");
	ADD_ITEM_TYPE(87,"Netherrack");
	ADD_ITEM_TYPE(88,"Soul Sand");
	ADD_ITEM_TYPE(89,"Glowstone");
	ADD_ITEM_TYPE(90,"Portal");
	ADD_ITEM_TYPE(91,"Jack-O-Lantern");
	ADD_ITEM_TYPE(92,"Cake Block");

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
	ADD_ITEM_TYPE(289,"Sulphur");
	ADD_ITEM_TYPE(290,"Wooden Hoe");
	ADD_ITEM_TYPE(291,"Stone Hoe");
	ADD_ITEM_TYPE(292,"Iron Hoe");
	ADD_ITEM_TYPE(293,"Diamond Hoe");
	ADD_ITEM_TYPE(294,"Gold Hoe");
	ADD_ITEM_TYPE(295,"Seeds");
	ADD_ITEM_TYPE(296,"Wheat");
	ADD_ITEM_TYPE(297,"Bread");
	ADD_ITEM_TYPE(298,"Leather Helmet");
	ADD_ITEM_TYPE(299,"Leather Chestplate");
	ADD_ITEM_TYPE(300,"Leather Leggings");
	ADD_ITEM_TYPE(301,"Leather Boots");
	ADD_ITEM_TYPE(302,"Chainmail Helmet");
	ADD_ITEM_TYPE(303,"Chainmail Chestplate");
	ADD_ITEM_TYPE(304,"Chainmail Leggings");
	ADD_ITEM_TYPE(305,"Chainmail Boots");
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
	ADD_ITEM_TYPE(322,"Golden apple");
	ADD_ITEM_TYPE(323,"Sign");
	ADD_ITEM_TYPE(324,"Wooden Door");
	ADD_ITEM_TYPE(325,"Bucket");
	ADD_ITEM_TYPE(326,"Water Bucket");
	ADD_ITEM_TYPE(327,"Lava Bucket");
	ADD_ITEM_TYPE(328,"Mine Cart");
	ADD_ITEM_TYPE(329,"Saddle");
	ADD_ITEM_TYPE(330,"Iron Door");
	ADD_ITEM_TYPE(331,"Redstone");
	ADD_ITEM_TYPE(332,"Snowball");
	ADD_ITEM_TYPE(333,"Boat");
	ADD_ITEM_TYPE(334,"Leather");
	ADD_ITEM_TYPE(335,"Milk");
	ADD_ITEM_TYPE(336,"Clay Brick");
	ADD_ITEM_TYPE(337,"Clay Balls");
	ADD_ITEM_TYPE(338,"Sugar Canes");
	ADD_ITEM_TYPE(339,"Paper");
	ADD_ITEM_TYPE(340,"Book");
	ADD_ITEM_TYPE(341,"Slime Ball");
	ADD_ITEM_TYPE(342,"Storage Minecart");
	ADD_ITEM_TYPE(343,"Powered Minecart");
	ADD_ITEM_TYPE(344,"Egg");
	ADD_ITEM_TYPE(345,"Compass");
	ADD_ITEM_TYPE(346,"Fishing Rod");
	ADD_ITEM_TYPE(347,"Clock");
	ADD_ITEM_TYPE(348,"Glowstone Dust");
	ADD_ITEM_TYPE(349,"Raw Fish");
	ADD_ITEM_TYPE(350,"Cooked Fish");
	ADD_ITEM_TYPE(351,"Sack");
	ADD_ITEM_TYPE(352,"Bone");
	ADD_ITEM_TYPE(353,"Sugar");
	ADD_ITEM_TYPE(354,"Cake");
	ADD_ITEM_TYPE(2256,"Gold Music Disc");
	ADD_ITEM_TYPE(2257,"Green Music Disc");
#undef ADD_ITEM_TYPE

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

