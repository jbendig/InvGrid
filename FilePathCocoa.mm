#include "FilePath.h"
#include <Foundation/Foundation.h>

namespace
{
	string GetSupportDirectory()
	{
		NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

		NSArray* filePathArray = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory,NSUserDomainMask,true);
		if([filePathArray count] == 0)
		{
			[pool release];
			return "";
		}

		NSString* pathString = [filePathArray objectAtIndex:0];
		
		const string result([pathString UTF8String]);
		[pool release];

		return result;
	}
}

namespace FilePath
{

string GetMinecraftSavesDirectory()
{
	return GetSupportDirectory() + "/minecraft/saves/";
}

string GetInvGridSettingsDirectory()
{
	return GetSupportDirectory() + "/invgrid/";
}

};

