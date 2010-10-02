#include "EditWidget.h"
#include <boost/foreach.hpp>

EditWidget::EditWidget()
: QWidget()
{
	//Setup new widget.
	newButton = new QPushButton("New Item");

	QVBoxLayout* newLayout = new QVBoxLayout();
	newLayout->addWidget(newButton);

	newWidget = new QWidget();
	newWidget->setLayout(newLayout);

	//Setup edit widget.
	slotLabel = new QLabel("Slot");
	typeLabel = new QLabel("Type");
	typeCombo = new QComboBox();
	PopulateTypeComboBox();
	//typeEdit = new QLineEdit();
	damageLabel = new QLabel("Damage");
	damageSpinBox = new QSpinBox();
	damageSpinBox->setRange(SHRT_MIN,SHRT_MAX);
	countLabel = new QLabel("Count");
	countSpinBox = new QSpinBox();
	countSpinBox->setRange(1,255);
	deleteButton = new QPushButton("Delete Item");

	QHBoxLayout* typeLayout = new QHBoxLayout();
	typeLayout->addWidget(typeLabel);
	typeLayout->addWidget(typeCombo,1);
	//typeLayout->addWidget(typeEdit,1);

	QHBoxLayout* damageLayout = new QHBoxLayout();
	damageLayout->addWidget(damageLabel);
	damageLayout->addWidget(damageSpinBox,1);

	QHBoxLayout* countLayout = new QHBoxLayout();
	countLayout->addWidget(countLabel);
	countLayout->addWidget(countSpinBox,1);

	QVBoxLayout* editLayout = new QVBoxLayout();
	editLayout->addWidget(slotLabel);
	editLayout->addLayout(typeLayout);
	editLayout->addLayout(damageLayout);
	editLayout->addLayout(countLayout);
	editLayout->addStretch(1);
	editLayout->addWidget(deleteButton);

	editWidget = new QWidget();
	editWidget->setLayout(editLayout);

	//Setup initial view.
	setLayout(new QVBoxLayout());
	layout()->addWidget(newWidget);
	layout()->addWidget(editWidget);
	editWidget->hide();

	//Setup events.
	connect(typeCombo,SIGNAL(currentIndexChanged(int)),SIGNAL(UpdateItem()));
	connect(damageSpinBox,SIGNAL(valueChanged(int)),SIGNAL(UpdateItem()));
	connect(countSpinBox,SIGNAL(valueChanged(int)),SIGNAL(UpdateItem()));
	connect(newButton,SIGNAL(clicked(bool)),SIGNAL(NewItem()));
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
		const int typeIndex = typeCombo->findData(QVariant(item->id));
		if(typeIndex == -1)
			typeCombo->setCurrentIndex(0);
		else
			typeCombo->setCurrentIndex(typeIndex);
		damageSpinBox->setValue(item->damage);
		countSpinBox->setValue(item->count);

		//When an item is newly created, automatically focus the type combobox.
		if(editIsHidden)
			typeCombo->setFocus();
		blockSignals(false);
	}
}

bool EditWidget::GetItemInfo(Item& item)
{
	if(editWidget->isHidden())
		return false;

	item.id = typeCombo->itemData(typeCombo->currentIndex()).toUInt();
	item.count = countSpinBox->value();
	item.damage = damageSpinBox->value();
	return true;
}

void EditWidget::PopulateTypeComboBox()
{
	ItemTypeBimap itemTypeBimap = CreateItemTypeBimap();
	
	BOOST_FOREACH(ItemTypeBimap::right_const_reference itemType,itemTypeBimap.right)
	{
		typeCombo->addItem(itemType.first.c_str(),QVariant(itemType.second));
	}
}

