#include "ScriptWidget.h"
#include <string>
#include "ScriptRunner.h"
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
}

ScriptWidget::~ScriptWidget()
{
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
		consoleTextEdit->append(scriptRunner.Error().c_str());
		return;
	}
	consoleTextEdit->append(scriptRunner.Output().c_str());
	consoleTextEdit->append("<b>Done.</b>");

	//Sync changes to rest of program.
	emit ReloadFromInventoryTag();
}

void ScriptWidget::ValidateScript()
{
}

