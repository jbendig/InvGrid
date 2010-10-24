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
	slotLabel->setStyleSheet("QLabel {font-weight: bold; font-size: 14pt;}");
	typeSearchEdit = new QLineEdit();
	typeListWidget = new QListWidget();
	typeListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	PopulateTypeListWidget();
	damageSpinBox = new QSpinBox();
	damageSpinBox->setRange(SHRT_MIN,SHRT_MAX);
	countSpinBox = new QSpinBox();
	countSpinBox->setRange(1,255);
	deleteButton = new QPushButton("Delete Item");

	QVBoxLayout* typeLayout = new QVBoxLayout();
	typeLayout->setContentsMargins(0,2,0,0);
	typeLayout->addWidget(typeSearchEdit);
	typeLayout->addWidget(typeListWidget,1);

	QFormLayout* formLayout = new QFormLayout();
	formLayout->addRow(slotLabel);
	formLayout->addRow("Type",typeLayout);
	formLayout->addRow("Damage",damageSpinBox);
	formLayout->addRow("Count",countSpinBox);

	QVBoxLayout* editLayout = new QVBoxLayout();
	editLayout->addLayout(formLayout);
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
	connect(typeSearchEdit,SIGNAL(textEdited(const QString&)),SLOT(FilterTypeList(const QString&)));
	connect(typeListWidget,SIGNAL(currentRowChanged(int)),SIGNAL(UpdateItem()));
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
		const int typeIndex = FindTypeRow(item->id);
		if(typeIndex == -1)
			typeListWidget->setCurrentRow(0);
		else
			typeListWidget->setCurrentRow(typeIndex);
		damageSpinBox->setValue(item->damage);
		countSpinBox->setValue(item->count);

		blockSignals(false);
	}
}

bool EditWidget::GetItemInfo(Item& item)
{
	if(editWidget->isHidden())
		return false;

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

