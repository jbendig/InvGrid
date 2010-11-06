#include "MiscWidget.h"

MiscWidget::MiscWidget()
: QWidget()
{
	//Setup widgets.
	timeSpinBox = new QSpinBox();
	timeSpinBox->setRange(0,60000); //TODO: Find the real range.
	QPushButton* sunriseButton = new QPushButton("Sunrise"); //0 //TODO: Test if right.
	QPushButton* middayButton = new QPushButton("Midday"); //6000 //TODO: Test if right.
	QPushButton* sunsetButton = new QPushButton("Sunset"); //12000 //TODO: Test if right.

	spawnXSpinBox = new QSpinBox();
	spawnXSpinBox->setRange(INT_MIN,INT_MAX);
	spawnYSpinBox = new QSpinBox();
	spawnYSpinBox->setRange(INT_MIN,INT_MAX);
	spawnZSpinBox = new QSpinBox();
	spawnZSpinBox->setRange(INT_MIN,INT_MAX);

	QHBoxLayout* timeLayout = new QHBoxLayout();
	timeLayout->addWidget(timeSpinBox);
	timeLayout->addWidget(sunriseButton);
	timeLayout->addWidget(middayButton);
	timeLayout->addWidget(sunsetButton);

	QHBoxLayout* spawnLayout = new QHBoxLayout();
	spawnLayout->addWidget(spawnXSpinBox);
	spawnLayout->addWidget(spawnYSpinBox);
	spawnLayout->addWidget(spawnZSpinBox);

	QFormLayout* formLayout = new QFormLayout();
	formLayout->addRow("Time",timeLayout);
	formLayout->addRow("Spawn",spawnLayout);

	setLayout(formLayout);

	//Setup events.
	connect(sunriseButton,SIGNAL(clicked(bool)),SLOT(SetTimeToSunrise()));
	connect(middayButton,SIGNAL(clicked(bool)),SLOT(SetTimeToMidday()));
	connect(sunsetButton,SIGNAL(clicked(bool)),SLOT(SetTimeToSunset()));
}

void MiscWidget::LoadFromRootTag(NBT::Tag* rootTag)
{
	//Find appropriate data tags.
	NBT::Tag* dataTag = GetChildNamedTag(rootTag,"Data");
	if(dataTag == NULL)
		return;

	NBT::Tag* timeTag = GetChildNamedTag(dataTag,"Time");
	if(timeTag == NULL)
		return;

	NBT::Tag* spawnXTag = GetChildNamedTag(dataTag,"SpawnX");
	NBT::Tag* spawnYTag = GetChildNamedTag(dataTag,"SpawnY");
	NBT::Tag* spawnZTag = GetChildNamedTag(dataTag,"SpawnZ");
	if(spawnXTag == NULL || spawnYTag == NULL || spawnZTag == NULL)
		return;

	//Set control values to those found in tags.
	timeSpinBox->setValue(timeTag->longValue);

	spawnXSpinBox->setValue(spawnXTag->intValue);
	spawnYSpinBox->setValue(spawnYTag->intValue);
	spawnZSpinBox->setValue(spawnZTag->intValue);
}

void MiscWidget::SaveToRootTag(NBT::Tag* rootTag)
{
	//Find appropriate data tags.
	//TODO: Stop duplicating the same code as above.
	NBT::Tag* dataTag = GetChildNamedTag(rootTag,"Data");
	if(dataTag == NULL)
		return;

	NBT::Tag* timeTag = GetChildNamedTag(dataTag,"Time");
	if(timeTag == NULL)
		return;

	NBT::Tag* spawnXTag = GetChildNamedTag(dataTag,"SpawnX");
	NBT::Tag* spawnYTag = GetChildNamedTag(dataTag,"SpawnY");
	NBT::Tag* spawnZTag = GetChildNamedTag(dataTag,"SpawnZ");
	if(spawnXTag == NULL || spawnYTag == NULL || spawnZTag == NULL)
		return;

	//Set tag values to those found in controls.
	timeTag->type = NBT::TAG_LONG;
	timeTag->longValue = timeSpinBox->value();

	spawnXTag->type = NBT::TAG_INT;
	spawnXTag->intValue = spawnXSpinBox->value();

	spawnYTag->type = NBT::TAG_INT;
	spawnYTag->intValue = spawnYSpinBox->value();

	spawnZTag->type = NBT::TAG_INT;
	spawnZTag->intValue = spawnZSpinBox->value();
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

