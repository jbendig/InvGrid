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
	NBT::Tag* rotYaw;
	NBT::Tag* rotPitch;
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

	//Pos list
	NBT::Tag* posTag = GetChildNamedTag(playerTag,"Pos");
	if(posTag == NULL || posTag->childTags.size() != 3)
		return false;

	//Pos
	miscTags.posXTag = &posTag->childTags[0];
	miscTags.posYTag = &posTag->childTags[1];
	miscTags.posZTag = &posTag->childTags[2];

	//Rot list
	NBT::Tag* rotTag = GetChildNamedTag(playerTag,"Rotation");
	if(rotTag == NULL || rotTag->childTags.size() != 2)
		return false;

	//Rot
	miscTags.rotYaw = &rotTag->childTags[0];
	miscTags.rotPitch = &rotTag->childTags[1];

	return true;
}

};

MiscWidget::MiscWidget()
: QWidget()
{
	//Setup widgets.
	QLabel* warningLabel = new QLabel("Warning: Changing some of these values might cause your character to get stuck or worse!<br>");
	warningLabel->setStyleSheet("QLabel {font-weight: bold; font-size: 10pt; color: #ff0000;}");
	warningLabel->setWordWrap(true);
	timeSpinBox = new QSpinBox();
	timeSpinBox->setRange(0,24000);
	QLabel* positionExplanationLabel = new QLabel("<br>A position is determined by X, Y, and Z coordiantes, with units measured in blocks.<br><br>* To move <b>north</b>, subtract from the X value. To move <b>south</b>, add to X.<br>* To move <b>down</b>, subtract from the Y value. To move <b>up</b>, add to Y.<br>* To move <b>east</b>, subtract from the Z value. To move <b>west</b>, add to Z.<br><br>In the above descriptions, \"east\" refers to the direction of sunrise, while \"north\" is the direction clouds travel.<br>");
	directionLabel = new QLabel("");
	positionExplanationLabel->setWordWrap(true);
	QPushButton* sunriseButton = new QPushButton("Sunrise"); //0
	QPushButton* middayButton = new QPushButton("Midday"); //6000
	QPushButton* sunsetButton = new QPushButton("Sunset"); //12000
	QPushButton* midnightButton = new QPushButton("Midnight"); //18000

	QGroupBox* spawnGroupBox = new QGroupBox("Spawn Position");
	spawnXSpinBox = new QSpinBox();
	spawnXSpinBox->setRange(INT_MIN,INT_MAX);
	spawnYSpinBox = new QSpinBox();
	spawnYSpinBox->setRange(INT_MIN,INT_MAX);
	spawnZSpinBox = new QSpinBox();
	spawnZSpinBox->setRange(INT_MIN,INT_MAX);
	QPushButton* setToCurrentPositionButton = new QPushButton("Set To Current Position");

	QGroupBox* posGroupBox = new QGroupBox("Current Position");
	posXLineEdit = new QLineEdit();
	posYLineEdit = new QLineEdit();
	posZLineEdit = new QLineEdit();
	QPushButton* setToSpawnPositionButton = new QPushButton("Set To Spawn Position");

	QHBoxLayout* timeLayout = new QHBoxLayout();
	timeLayout->addWidget(timeSpinBox);
	timeLayout->addWidget(sunriseButton);
	timeLayout->addWidget(middayButton);
	timeLayout->addWidget(sunsetButton);
	timeLayout->addWidget(midnightButton);

	QFormLayout* spawnLayout = new QFormLayout();
	spawnLayout->addRow("X",spawnXSpinBox);
	spawnLayout->addRow("Y",spawnYSpinBox);
	spawnLayout->addRow("Z",spawnZSpinBox);
	spawnLayout->addRow(setToCurrentPositionButton);
	spawnGroupBox->setLayout(spawnLayout);

	QFormLayout* posLayout = new QFormLayout();
	posLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
	posLayout->addRow("X",posXLineEdit);
	posLayout->addRow("Y",posYLineEdit);
	posLayout->addRow("Z",posZLineEdit);
	posLayout->addRow(setToSpawnPositionButton);
	posGroupBox->setLayout(posLayout);

	QFormLayout* formLayout = new QFormLayout();
	formLayout->addRow(warningLabel);
	formLayout->addRow("Time",timeLayout);
	formLayout->addRow(positionExplanationLabel);
	formLayout->addRow(directionLabel);
	formLayout->addRow(spawnGroupBox);
	formLayout->addRow(posGroupBox);

	setLayout(formLayout);

	//Setup events.
	connect(sunriseButton,SIGNAL(clicked(bool)),SLOT(SetTimeToSunrise()));
	connect(middayButton,SIGNAL(clicked(bool)),SLOT(SetTimeToMidday()));
	connect(sunsetButton,SIGNAL(clicked(bool)),SLOT(SetTimeToSunset()));
	connect(midnightButton,SIGNAL(clicked(bool)),SLOT(SetTimeToMidnight()));
	connect(setToCurrentPositionButton,SIGNAL(clicked(bool)),SLOT(SetSpawnToCurrent()));
	connect(setToSpawnPositionButton,SIGNAL(clicked(bool)),SLOT(SetCurrentToSpawn()));
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

	//Figure out the character's current direction.
	const float rot = fmodf(miscTags.rotYaw->floatValue,360.0f);
	QString directionString = "Your character is facing <b>";
	if(rot < 45.0f || rot > 315.0f)
		directionString += "west";
	else if(rot < 135)
		directionString += "north";
	else if(rot < 225)
		directionString += "east";
	else
		directionString += "south";
	directionString += "</b> as of when this file was loaded.";
	directionLabel->setText(directionString);

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

void MiscWidget::SetSpawnToCurrent()
{
	try
	{
		const double posX = boost::lexical_cast<double>(posXLineEdit->text().toAscii().data());
		const double posY = boost::lexical_cast<double>(posYLineEdit->text().toAscii().data());
		const double posZ = boost::lexical_cast<double>(posZLineEdit->text().toAscii().data());

		spawnXSpinBox->setValue((int)posX);
		spawnYSpinBox->setValue((int)posY);
		spawnZSpinBox->setValue((int)posZ);
	}
	catch(boost::bad_lexical_cast&)
	{
		//User edited the current position with text that doesn't make up a double.
		//Ignoring because the big red warning label lets me.
	}
}

void MiscWidget::SetCurrentToSpawn()
{
	posXLineEdit->setText(QString::number(spawnXSpinBox->value()));
	posYLineEdit->setText(QString::number(spawnYSpinBox->value() + 2)); //Adjust +2 so player doesn't get stuck in the ground.
	posZLineEdit->setText(QString::number(spawnZSpinBox->value()));
}

