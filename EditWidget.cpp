#include "EditWidget.h"
#include <iostream>
#include <boost/foreach.hpp>

EditWidget::EditWidget()
: QWidget()
{
	//Setup new widget.
	newButton = new QPushButton("New Item");
	QLabel* presetItemLabel = new QLabel("Preset Items");
	QPushButton* stoneButton = new QPushButton("Stone");
	QPushButton* woodButton = new QPushButton("Wood");
	QPushButton* glassButton = new QPushButton("Glass");
	QPushButton* torchesButton = new QPushButton("Torches");

	QVBoxLayout* newLayout = new QVBoxLayout();
	newLayout->addWidget(newButton);
	newLayout->addSpacing(45);
	newLayout->addWidget(presetItemLabel);
	newLayout->addWidget(stoneButton);
	newLayout->addWidget(woodButton);
	newLayout->addWidget(glassButton);
	newLayout->addWidget(torchesButton);
	newLayout->addStretch();

	newWidget = new QWidget();
	newWidget->setLayout(newLayout);

	//Setup edit widget.
	slotLabel = new QLabel("Slot");
	slotLabel->setStyleSheet("QLabel {font-weight: bold; font-size: 14pt;}");
	QLabel* typeLabel = new QLabel("Type");
	typeSearchEdit = new QLineEdit();
	typeListWidget = new QListWidget();
	typeListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	PopulateTypeListWidget();
	QLabel* damageLabel = new QLabel("Damage");
	damageSpinBox = new QSpinBox();
	damageSpinBox->setRange(SHRT_MIN,SHRT_MAX);
	QPushButton* damageZeroButton = new QPushButton("0");
	QLabel* countLabel = new QLabel("Count");
	countSpinBox = new QSpinBox();
	countSpinBox->setRange(1,255);
	QPushButton* countSixtyFourButton = new QPushButton("64");
	deleteButton = new QPushButton("Delete Item");

	QVBoxLayout* typeLayout = new QVBoxLayout();
	typeLayout->addWidget(typeSearchEdit);
	typeLayout->addWidget(typeListWidget,1);

	QHBoxLayout* damageLayout = new QHBoxLayout();
	damageLayout->addWidget(damageSpinBox);
	damageLayout->addWidget(damageZeroButton);

	QHBoxLayout* countLayout = new QHBoxLayout();
	countLayout->addWidget(countSpinBox);
	countLayout->addWidget(countSixtyFourButton);

	const Qt::Alignment labelAlignment = Qt::AlignRight | Qt::AlignTop;
	QGridLayout* formLayout = new QGridLayout();
	formLayout->addWidget(slotLabel,0,0,1,2);
	formLayout->addWidget(typeLabel,1,0,labelAlignment);
	formLayout->addLayout(typeLayout,1,1);
	formLayout->addWidget(damageLabel,2,0,labelAlignment);
	formLayout->addLayout(damageLayout,2,1);
	formLayout->addWidget(countLabel,3,0,labelAlignment);
	formLayout->addLayout(countLayout,3,1);

	QVBoxLayout* editLayout = new QVBoxLayout();
	editLayout->addLayout(formLayout);
	editLayout->addWidget(deleteButton);

	editWidget = new QWidget();
	editWidget->setLayout(editLayout);

	//Setup initial view.
	setLayout(new QVBoxLayout());
	layout()->addWidget(newWidget);
	layout()->addWidget(editWidget);
	editWidget->hide();

	//Setup events.
	connect(typeSearchEdit,SIGNAL(textEdited(const QString&)),SLOT(FilterTypeList(const QString&)));
	connect(typeListWidget,SIGNAL(currentRowChanged(int)),SIGNAL(UpdateItem()));
	connect(damageSpinBox,SIGNAL(valueChanged(int)),SIGNAL(UpdateItem()));
	connect(damageZeroButton,SIGNAL(pressed()),SLOT(SetDamageToZero()));
	connect(countSpinBox,SIGNAL(valueChanged(int)),SIGNAL(UpdateItem()));
	connect(countSixtyFourButton,SIGNAL(pressed()),SLOT(SetCountToSixtyFour()));
	connect(newButton,SIGNAL(clicked(bool)),SIGNAL(NewItem()));
	connect(stoneButton,SIGNAL(clicked(bool)),SIGNAL(NewStoneItem()));
	connect(woodButton,SIGNAL(clicked(bool)),SIGNAL(NewWoodItem()));
	connect(glassButton,SIGNAL(clicked(bool)),SIGNAL(NewGlassItem()));
	connect(torchesButton,SIGNAL(clicked(bool)),SIGNAL(NewTorchesItem()));
	connect(deleteButton,SIGNAL(clicked(bool)),SIGNAL(DeleteItem()));
}

void EditWidget::SetItem(const Item* item)
{
	if(item == NULL)
	{
		editWidget->hide();
		newWidget->show();
	}
	else
	{
		const bool editIsHidden = editWidget->isHidden();
		editWidget->show();
		newWidget->hide();

		blockSignals(true);

		slotLabel->setText(SlotName(item->slot).c_str());
		const int typeIndex = FindTypeRow(item->id);
		if(typeIndex == -1)
		{
			typeListWidget->setEnabled(false);
			typeListWidget->setCurrentRow(0);
		}
		else
		{
			typeListWidget->setEnabled(true);
			typeListWidget->setCurrentRow(typeIndex);
		}
		damageSpinBox->setValue(item->damage);
		countSpinBox->setValue(item->count);

		blockSignals(false);
	}
}

bool EditWidget::GetItemInfo(Item& item)
{
	if(editWidget->isHidden())
		return false;

	if(typeListWidget->isEnabled())
		item.id = typeListWidget->currentItem()->data(Qt::UserRole).toUInt();
	item.count = countSpinBox->value();
	item.damage = damageSpinBox->value();
	return true;
}

void EditWidget::FilterTypeList(const QString& filterText)
{
	//Only show item types with names that contains filterText.
	//If filter text is empty, all items are shown.
	const int typeRows = typeListWidget->count();
	for(int x = 0;x < typeRows;x++)
	{
		QListWidgetItem* item = typeListWidget->item(x);
		if(filterText.isEmpty() || item->text().contains(filterText,Qt::CaseInsensitive))
		{
			item->setHidden(false);
			item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		}
		else
		{
			item->setHidden(true);
			item->setFlags(Qt::NoItemFlags);
		}
	}

	//Make sure currently selected row is scrolled into view when ever visible.
	typeListWidget->scrollToItem(typeListWidget->currentItem());
}

void EditWidget::SetDamageToZero()
{
	damageSpinBox->setValue(0);
}

void EditWidget::SetCountToSixtyFour()
{
	countSpinBox->setValue(64);
}

void EditWidget::PopulateTypeListWidget()
{
	ItemTypeBimap itemTypeBimap = CreateItemTypeBimap();
	
	BOOST_FOREACH(ItemTypeBimap::right_const_reference itemType,itemTypeBimap.right)
	{
		QListWidgetItem* listWidgetItem = new QListWidgetItem(itemType.first.c_str());
		listWidgetItem->setData(Qt::UserRole,QVariant(itemType.second));
		typeListWidget->addItem(listWidgetItem);
	}
}

int EditWidget::FindTypeRow(const int type) const
{
	const QVariant typeVariant(type);

	const int rowCount = typeListWidget->count();
	for(int x = 0;x < rowCount;x++)
	{
		QListWidgetItem* item = typeListWidget->item(x);
		if(item->data(Qt::UserRole) == typeVariant)
			return x;
	}

	return -1;
}

