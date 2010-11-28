#ifndef SCRIPTRUNNER_H
#define SCRIPTRUNNER_H

#include <string>
#include <lua.hpp>
using std::string;

namespace NBT
{
	class Tag;
};

class ScriptRunner
{
	public:
		ScriptRunner();
		~ScriptRunner();

		bool Run(NBT::Tag* inventoryTag,const string script);
		string Output() const; //Text outputted by script.
		string Error() const; //Error text resulted from running script.
	private:
		lua_State* luaState;
		string outputString;
		string errorString;
};

#endif

