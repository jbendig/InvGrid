#include "FilePath.h"
#include <QDir>

string FilePath::GetMinecraftSavesDirectory()
{
	return string((QDir::homePath() + "/.minecraft/saves/").toUtf8());
}

