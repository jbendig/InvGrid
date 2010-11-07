#include "MiscWidget.h"
#include <iostream>
#include <boost/lexical_cast.hpp>

namespace
{

struct MiscTags
{
	NBT::Tag* timeTag;
	NBT::Tag* spawnXTag;
	NBT::Tag* spawnYTag;
	NBT::Tag* spawnZTag;
	NBT::Tag* posXTag;
	NBT::Tag* posYTag;
	NBT::Tag* posZTag;
};

bool GetMiscTagsFromRootTag(NBT::Tag* rootTag,MiscTags& miscTags)
{
	if(rootTag == NULL)
		return false;

	//Data
	NBT::Tag* dataTag = GetChildNamedTag(rootTag,"Data");
	if(dataTag == NULL)
		return false;

	//Time
	miscTags.timeTag = GetChildNamedTag(dataTag,"Time");
	if(miscTags.timeTag == NULL)
		return false;

	//Spawn
	miscTags.spawnXTag = GetChildNamedTag(dataTag,"SpawnX");
	miscTags.spawnYTag = GetChildNamedTag(dataTag,"SpawnY");
	miscTags.spawnZTag = GetChildNamedTag(dataTag,"SpawnZ");
	if(miscTags.spawnXTag == NULL || miscTags.spawnYTag == NULL || miscTags.spawnZTag == NULL)
		return false;

	//Player
	NBT::Tag* playerTag = GetChildNamedTag(dataTag,"Player");
	if(playerTag == NULL)
		return false;

	//Pos collection
	NBT::Tag* posTag = GetChildNamedTag(playerTag,"Pos");
	if(posTag == NULL || posTag->childTags.size() != 3)
		return false;

	//Pos
	miscTags.posXTag = &posTag->childTags[0];
	miscTags.posYTag = &posTag->childTags[1];
	miscTags.posZTag = &posTag->childTags[2];

	return true;
}

};

MiscWidget::MiscWidget()
: QWidget()
{
	//Setup widgets.
	QLabel* warningLabel = new QLabel("Warning: Changing some of these values might cause your character to get stuck or worse!");
	warningLabel->setStyleSheet("QLabel {font-weight: bold; font-size: 10pt; color: #ff0000;}");
	warningLabel->setWordWrap(true);
	timeSpinBox = new QSpinBox();
	timeSpinBox->setRange(0,24000);
	QPushButton* sunriseButton = new QPushButton("Sunrise"); //0
	QPushButton* middayButton = new QPushButton("Midday"); //6000
	QPushButton* sunsetButton = new QPushButton("Sunset"); //12000
	QPushButton* midnightButton = new QPushButton("Midnight"); //18000

	spawnXSpinBox = new QSpinBox();
	spawnXSpinBox->setRange(INT_MIN,INT_MAX);
	spawnYSpinBox = new QSpinBox();
	spawnYSpinBox->setRange(INT_MIN,INT_MAX);
	spawnZSpinBox = new QSpinBox();
	spawnZSpinBox->setRange(INT_MIN,INT_MAX);

	posXLineEdit = new QLineEdit();
	posYLineEdit = new QLineEdit();
	posZLineEdit = new QLineEdit();

	QHBoxLayout* timeLayout = new QHBoxLayout();
	timeLayout->addWidget(timeSpinBox);
	timeLayout->addWidget(sunriseButton);
	timeLayout->addWidget(middayButton);
	timeLayout->addWidget(sunsetButton);
	timeLayout->addWidget(midnightButton);

	QHBoxLayout* spawnLayout = new QHBoxLayout();
	spawnLayout->addWidget(spawnXSpinBox);
	spawnLayout->addWidget(spawnYSpinBox);
	spawnLayout->addWidget(spawnZSpinBox);

	QHBoxLayout* posLayout = new QHBoxLayout();
	posLayout->addWidget(posXLineEdit);
	posLayout->addWidget(posYLineEdit);
	posLayout->addWidget(posZLineEdit);

	QFormLayout* formLayout = new QFormLayout();
	formLayout->addRow(warningLabel);
	formLayout->addRow("Time",timeLayout);
	formLayout->addRow("Spawn Position",spawnLayout);
	formLayout->addRow("Current Position",posLayout);

	setLayout(formLayout);

	//Setup events.
	connect(sunriseButton,SIGNAL(clicked(bool)),SLOT(SetTimeToSunrise()));
	connect(middayButton,SIGNAL(clicked(bool)),SLOT(SetTimeToMidday()));
	connect(sunsetButton,SIGNAL(clicked(bool)),SLOT(SetTimeToSunset()));
	connect(midnightButton,SIGNAL(clicked(bool)),SLOT(SetTimeToMidnight()));
}

void MiscWidget::LoadFromRootTag(NBT::Tag* rootTag)
{
	//Find appropriate data tags.
	MiscTags miscTags;
	if(!GetMiscTagsFromRootTag(rootTag,miscTags))
		return;

	//Set control values to those found in tags.
	timeSpinBox->setValue(miscTags.timeTag->longValue % 24000);

	spawnXSpinBox->setValue(miscTags.spawnXTag->intValue);
	spawnYSpinBox->setValue(miscTags.spawnYTag->intValue);
	spawnZSpinBox->setValue(miscTags.spawnZTag->intValue);

	posXLineEdit->setText(boost::lexical_cast<string>(miscTags.posXTag->doubleValue).c_str());
	posYLineEdit->setText(boost::lexical_cast<string>(miscTags.posYTag->doubleValue).c_str());
	posZLineEdit->setText(boost::lexical_cast<string>(miscTags.posZTag->doubleValue).c_str());

	setEnabled(true);
}

void MiscWidget::SaveToRootTag(NBT::Tag* rootTag)
{
	//Find appropriate data tags.
	MiscTags miscTags;
	if(!GetMiscTagsFromRootTag(rootTag,miscTags))
		return;

	//Set tag values to those found in controls.
	miscTags.timeTag->type = NBT::TAG_LONG;
	miscTags.timeTag->longValue = timeSpinBox->value();

	miscTags.spawnXTag->type = NBT::TAG_INT;
	miscTags.spawnXTag->intValue = spawnXSpinBox->value();

	miscTags.spawnYTag->type = NBT::TAG_INT;
	miscTags.spawnYTag->intValue = spawnYSpinBox->value();

	miscTags.spawnZTag->type = NBT::TAG_INT;
	miscTags.spawnZTag->intValue = spawnZSpinBox->value();

	//Try to set the player's position. If any dimension is invalid, the position is not touched at all.
	try
	{
		const double posX = boost::lexical_cast<double>(posXLineEdit->text().toAscii().data());
		const double posY = boost::lexical_cast<double>(posYLineEdit->text().toAscii().data());
		const double posZ = boost::lexical_cast<double>(posZLineEdit->text().toAscii().data());

		miscTags.posXTag->doubleValue = posX;
		miscTags.posYTag->doubleValue = posY;
		miscTags.posZTag->doubleValue = posZ;
	}
	catch(boost::bad_lexical_cast&)
	{
		//Unable to set player position.
		//I could tell the user with a messagebox ... or I could just put a big warning label. :)
		std::cerr << "Warning: Unable to save changes to player's position.\n";
	}
}

void MiscWidget::SetTimeToSunrise()
{
	timeSpinBox->setValue(0);
}

void MiscWidget::SetTimeToMidday()
{
	timeSpinBox->setValue(6000);
}

void MiscWidget::SetTimeToSunset()
{
	timeSpinBox->setValue(12000);
}

void MiscWidget::SetTimeToMidnight()
{
	timeSpinBox->setValue(18000);
}

