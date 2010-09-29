#include "FilePath.h"
#include <Foundation/Foundation.h>

namespace FilePath
{

string GetMinecraftSavesDirectory()
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

	NSArray* filePathArray = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory,NSUserDomainMask,true);
	if([filePathArray count] == 0)
	{
		[pool release];
		return "";
	}

	NSString* minecraftSavesPath = [filePathArray objectAtIndex:0];
	
	const string result([minecraftSavesPath UTF8String]);
	[pool release];

	return result + "/minecraft/saves/";
}

};

