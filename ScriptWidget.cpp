#include "ScriptWidget.h"
#include <string>
#include <sstream>
#include <boost/foreach.hpp>
#include "ScriptRunner.h"
#include "Item.h"
#include "FilePath.h"
using std::string;

ScriptWidget::ScriptWidget()
: QWidget(),
  inventoryTag(NULL)
{
	//Setup widgets.
	QPushButton* runButton = new QPushButton("Run");
	scriptTextEdit = new QPlainTextEdit();
	consoleTextEdit = new QTextEdit();
	consoleTextEdit->setReadOnly(true);
	QPalette textPalette = palette();
	textPalette.setColor(QPalette::Active,QPalette::Base,QColor(0,0,0));
	textPalette.setColor(QPalette::Disabled,QPalette::Base,QColor(0,0,0));
	textPalette.setColor(QPalette::Active,QPalette::Text,QColor(212,212,212));
	textPalette.setColor(QPalette::Disabled,QPalette::Text,QColor(212,212,212));
	consoleTextEdit->setPalette(textPalette);
	QSplitter* splitter = new QSplitter(Qt::Vertical);
	splitter->addWidget(scriptTextEdit);
	splitter->addWidget(consoleTextEdit);
	splitter->setCollapsible(0,false);

	//Layout widgets.
	QVBoxLayout* topLayout = new QVBoxLayout();
	topLayout->addWidget(runButton);
	topLayout->addWidget(splitter,1);

	setLayout(topLayout);
	setEnabled(false);

	//Connect events.
	connect(runButton,SIGNAL(pressed()),SLOT(RunScript()));

	//Load previous lua script if one is available.
	const QString settingsDir = FilePath::GetInvGridSettingsDirectory().c_str();
	LoadScript(settingsDir + "auto.lua");
}

ScriptWidget::~ScriptWidget()
{
	//Save current script so it can be reloaded on start.
	const QString settingsDir = FilePath::GetInvGridSettingsDirectory().c_str();
	QDir("").mkpath(settingsDir);

	SaveScript(settingsDir + "auto.lua");
}

void ScriptWidget::SetInventoryTag(NBT::Tag* toInventoryTag)
{
	inventoryTag = toInventoryTag;

	setEnabled(inventoryTag != NULL);
}

void ScriptWidget::RunScript()
{
	//Get the current state of the inventory.
	emit SaveToInventoryTag();

	//Run script on inventory.
	consoleTextEdit->clear();
	const string scriptString = scriptTextEdit->toPlainText().toStdString();
	
	ScriptRunner scriptRunner;
	if(!scriptRunner.Run(inventoryTag,scriptString))
	{
		consoleTextEdit->append(scriptRunner.Output().c_str());
		consoleTextEdit->append(scriptRunner.Error().c_str());
		return;
	}
	consoleTextEdit->append(scriptRunner.Output().c_str());
	consoleTextEdit->append("<b>Done.</b>");

	//Sync changes to rest of program.
	emit ReloadFromInventoryTag();
}

void ScriptWidget::GenerateScriptFromInventory()
{
	//Get the current state of the inventory.
	emit SaveToInventoryTag();

	//Make the inventory easier to work with.
	ItemMap itemMap;
	InventoryTagToItemMap(inventoryTag,itemMap);

	//Generate script that clears the current inventory and sets the items whatever the inventory is when this is run.
	std::stringstream scriptStream;
	scriptStream << "ClearInventory()\n\n";

	BOOST_FOREACH(ItemMap::const_reference itemIter,itemMap)
	{
		const Item& item = itemIter.second;
		scriptStream << "SetItem(" <<
			(int)item.slot<< "," <<
			(int)item.id << "," <<
			(int)item.damage << "," <<
			(int)item.count << ")\n";

	}

	scriptTextEdit->setPlainText(scriptStream.str().c_str());
}

void ScriptWidget::SaveScript(const QString fileName)
{
	QFile file(fileName);
	if(!file.open(QFile::WriteOnly | QFile::Truncate))
		return;

	QTextStream textStream(&file);
	textStream << scriptTextEdit->toPlainText();
}

void ScriptWidget::LoadScript(const QString fileName)
{
	QFile file(fileName);
	if(!file.open(QFile::ReadOnly))
		return;

	QTextStream textStream(&file);
	scriptTextEdit->setPlainText(textStream.readAll());
}

