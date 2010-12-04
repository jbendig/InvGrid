#include "ScriptRunner.h"
#include <sstream>
#include <iostream>
#include <exception>
#include <unistd.h>
#include <fcntl.h>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include "NBTTag.h"
#include "Item.h"

static ItemMap itemMap;

namespace
{

string StringAsLuaConstName(string str)
{
	str = boost::ireplace_all_copy(str," ","_");
	str = boost::ireplace_all_copy(str,"\"","_");
	str = boost::ireplace_all_copy(str,"-","_");
	str = boost::ireplace_all_copy(str,"(","_");
	str = boost::ireplace_all_copy(str,")","_");

	return boost::to_upper_copy(str);
}

class RedirectStdOut
{
	public:
		RedirectStdOut()
		: redirecting(false)
		{
			if(pipe(pipePair) != 0)
				return;

			redirecting = true;
			dup2(pipePair[1],STDOUT_FILENO);
			::close(pipePair[1]);
		}
		~RedirectStdOut()
		{
			Done();
		}
		string Done()
		{
			if(!redirecting)
				return "";

			//Turn off blocking so we can safely read all text on pipe.
			int fdFlags = fcntl(pipePair[0],F_GETFL);
			long nonBlockFlags = fdFlags | O_NONBLOCK;
			fcntl(pipePair[0],F_SETFL,nonBlockFlags);

			//Read all available text on pipe.
			vector<char> buffer;
			char readBuffer[256];
			while(1)
			{
				const int readCount = read(pipePair[0],readBuffer,256);
				if(readCount <= 0)
					break;

				const int copyStart = buffer.size();
				buffer.resize(buffer.size() + readCount);
				memcpy(&buffer[copyStart],readBuffer,readCount);
			}
			const string text(buffer.begin(),buffer.end());

			//Close pipe and return stdout to the way it was.
			dup2(oldStdOut,STDOUT_FILENO);
			::close(pipePair[0]);

			return text;
		}
	private:
		bool redirecting;
		int pipePair[2];
		int oldStdOut;
};

class LuaException : public std::exception
{
};

int ScriptRunner_onpanic(lua_State* luaState)
{
	throw LuaException();

	return 0;
}

void ScriptRunner_openlib(lua_State* luaState,const char* libName,lua_CFunction function)
{
	lua_pushcfunction(luaState,function);
	lua_pushstring(luaState,libName);
	lua_call(luaState,1,0);
}

int ScriptRunner_GetItem(lua_State* luaState)
{
	const int argc = lua_gettop(luaState);
	if(argc != 1 || !lua_isnumber(luaState,1))
	{
		lua_pushstring(luaState,"Invalid argument, expected GetItem(integer slot)");
		lua_error(luaState);
	}

	lua_Integer inventorySlot = lua_tointeger(luaState,1);

	ItemMap::const_iterator itemMapIter = itemMap.find(inventorySlot);
	if(itemMapIter == itemMap.end())
	{
		lua_pushnil(luaState);
		return 1;
	}

	const Item item = itemMapIter->second;
	lua_pushinteger(luaState,item.slot);
	lua_pushinteger(luaState,item.id);
	lua_pushinteger(luaState,item.damage);
	lua_pushinteger(luaState,item.count);
	return 4;
}

int ScriptRunner_SetItem(lua_State* luaState)
{
	const int argc = lua_gettop(luaState);
	if(argc != 4 ||
	   !lua_isnumber(luaState,1) ||
	   !lua_isnumber(luaState,2) ||
	   !lua_isnumber(luaState,3) ||
	   !lua_isnumber(luaState,4))
	{
		lua_pushstring(luaState,"Invalid argument, expected SetItem(integer slot,integer type,integer damage,integer count)");
		lua_error(luaState);
	}

	//Just load numbers and cast as necessary. As long as the slot is valid, who cares?
	Item item;
	item.slot = lua_tointeger(luaState,1);
	item.id = lua_tointeger(luaState,2);
	item.damage = lua_tointeger(luaState,3);
	item.count = lua_tointeger(luaState,4);
	
	//Is slot is invalid, return with false.
	static SlotNameBimap slotNameBimap = CreateSlotNameBimap();
	if(slotNameBimap.left.find(item.slot) == slotNameBimap.left.end())
	{
		lua_pushboolean(luaState,false);
		return 1;
	}

	//Item is valid, save and return true.
	itemMap[item.slot] = item;

	lua_pushboolean(luaState,true);
	return 1;
}

int ScriptRunner_HaveItem(lua_State* luaState)
{
	const int argc = lua_gettop(luaState);
	if(argc != 1 || !lua_isnumber(luaState,1))
	{
		lua_pushstring(luaState,"Invalid argument, expected HaveItem(integer slot)");
		lua_error(luaState);
	}

	lua_Integer inventorySlot = lua_tointeger(luaState,1);

	ItemMap::const_iterator itemMapIter = itemMap.find(inventorySlot);
	const bool haveItem = (itemMapIter == itemMap.end());

	lua_pushboolean(luaState,haveItem);
	return 1;
}

int ScriptRunner_RemoveItem(lua_State* luaState)
{
	const int argc = lua_gettop(luaState);
	if(argc != 1 || !lua_isnumber(luaState,1))
	{
		lua_pushstring(luaState,"Invalid argument, expected RemoveItem(integer slot)");
		lua_error(luaState);
	}

	lua_Integer inventorySlot = lua_tointeger(luaState,1);

	ItemMap::iterator itemMapIter = itemMap.find(inventorySlot);
	if(itemMapIter == itemMap.end())
	{
		lua_pushboolean(luaState,false);
		return 1;
	}

	itemMap.erase(itemMapIter);
	lua_pushboolean(luaState,true);
	return 1;
}

int ScriptRunner_ClearInventory(lua_State* luaState)
{
	//Ignore passed in parameters.

	//Remove all items from inventory.
	itemMap.clear();
	
	//Nothing to return.
	return 0;
}

};

ScriptRunner::ScriptRunner()
{
	//Setup LUA.
	luaState = lua_open();
	lua_atpanic(luaState,ScriptRunner_onpanic);
	luaopen_debug(luaState);
	ScriptRunner_openlib(luaState,"",luaopen_base);
	ScriptRunner_openlib(luaState,LUA_TABLIBNAME,luaopen_table);
	ScriptRunner_openlib(luaState,LUA_STRLIBNAME,luaopen_string);

	lua_register(luaState,"GetItem",ScriptRunner_GetItem);
	lua_register(luaState,"SetItem",ScriptRunner_SetItem);
	lua_register(luaState,"HaveItem",ScriptRunner_HaveItem);
	lua_register(luaState,"ClearInventory",ScriptRunner_ClearInventory);

	//Setup constants.
	std::stringstream constSStream;
	std::stringstream slotTableSStream;
	slotTableSStream << "SLOT_TABLE = {";
	std::stringstream itemTableSStream;
	itemTableSStream << "ITEM_TABLE = {";

	SlotNameBimap slotNameBimap = CreateSlotNameBimap();
	BOOST_FOREACH(SlotNameBimap::left_const_reference slot,slotNameBimap.left)
	{
		const string luaConstName = StringAsLuaConstName(slot.second);
		constSStream << luaConstName << " = " << (int)slot.first << std::endl;
		slotTableSStream << luaConstName << " = " << (int)slot.first << ",";
	}

	ItemTypeBimap itemTypeBimap = CreateItemTypeBimap();
	BOOST_FOREACH(ItemTypeBimap::left_const_reference item,itemTypeBimap.left)
	{
		const string luaConstName = StringAsLuaConstName(item.second);
		constSStream << luaConstName << " = " << (int)item.first << std::endl;
		itemTableSStream << luaConstName << " = " << (int)item.first << ",";
	}

	luaL_dostring(luaState,constSStream.str().c_str());
	slotTableSStream << "}";
	luaL_dostring(luaState,slotTableSStream.str().c_str());
	itemTableSStream << "}";
	luaL_dostring(luaState,itemTableSStream.str().c_str());
}

ScriptRunner::~ScriptRunner()
{
	if(luaState != NULL)
		lua_close(luaState);
}

bool ScriptRunner::Run(NBT::Tag* inventoryTag,const string script)
{
	//Prepare inventory to be modified by script.
	InventoryTagToItemMap(inventoryTag,itemMap);

	//Run script, redirecting anything outputted by script from stdout to output.
	RedirectStdOut redirect;
	if(luaL_loadstring(luaState,script.c_str()) != 0)
	{
		//PROBABLY a syntax error, but could be a memory error....
		errorString = "Syntax error.\n";
		errorString += lua_tostring(luaState,-1);
		return false;
	}
	if(lua_pcall(luaState,0,LUA_MULTRET,0) != 0)
	{
		outputString = redirect.Done();
		errorString = "An error has occurred.\n";
		errorString += lua_tostring(luaState,-1);
		return false;
	}
	outputString = redirect.Done();

	//Save changes made by script.
	ItemMapToInventoryTag(itemMap,inventoryTag);

	return true;
}

string ScriptRunner::Output() const
{
	return outputString;
}

string ScriptRunner::Error() const
{
	return errorString;
}

