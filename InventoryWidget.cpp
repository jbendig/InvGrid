#include "InventoryWidget.h"
#include <iostream>
#include <fstream>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include "EditWidget.h"

InventoryWidget::InventoryWidget()
: QWidget()
{
	//Setup widgets.
	inventoryTableView = new QTableView();
	inventoryTableView->verticalHeader()->hide();
	inventoryTableView->horizontalHeader()->setStretchLastSection(true);
	inventoryTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	inventoryTableView->setSelectionMode(QAbstractItemView::SingleSelection);
	inventoryTableView->setEditTriggers(QTableView::NoEditTriggers);
	inventoryTableView->setModel(&model);
	inventoryTableView->setTabKeyNavigation(false);
	connect(inventoryTableView->selectionModel(),
			SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
			SLOT(SelectedItem(const QItemSelection&,const QItemSelection&)));
	connect(inventoryTableView,SIGNAL(doubleClicked(const QModelIndex&)),SLOT(NewItem()));

	editWidget = new EditWidget();

	QHBoxLayout* topLayout = new QHBoxLayout();
	topLayout->addWidget(inventoryTableView,1);
	topLayout->addWidget(editWidget);

	setLayout(topLayout);

	//Setup events.	connect(editWidget,SIGNAL(NewItem()),SLOT(NewItem()));
	connect(editWidget,SIGNAL(NewStoneItem()),SLOT(NewStoneItem()));
	connect(editWidget,SIGNAL(NewWoodItem()),SLOT(NewWoodItem()));
	connect(editWidget,SIGNAL(NewGlassItem()),SLOT(NewGlassItem()));
	connect(editWidget,SIGNAL(NewTorchesItem()),SLOT(NewTorchesItem()));
	connect(editWidget,SIGNAL(DeleteItem()),SLOT(DeleteItem()));
	connect(editWidget,SIGNAL(UpdateItem()),SLOT(UpdateItem()));
}

void InventoryWidget::SetInventoryTag(NBT::Tag* toInventoryTag)
{
	inventoryTag = toInventoryTag;
	itemMap.clear();

	if(inventoryTag != NULL)
	{
		for(unsigned int x = 0;x < inventoryTag->childTags.size();++x)
		{
			Item item = TagToItem(inventoryTag->childTags[x]);
			itemMap[item.slot] = item;
		}
	}

	ReloadTableModel();
}

void InventoryWidget::SaveChangesToInventoryTag()
{
	if(inventoryTag == NULL)
		return;

	inventoryTag->childTags.clear();
	BOOST_FOREACH(const ItemMap::value_type& itemIter,itemMap)
	{
		inventoryTag->childTags.push_back(ItemToTag(itemIter.second));
	}
}

void InventoryWidget::ImportInventory(const char* filePath)
{
	std::ifstream inFile(filePath);
	if(!inFile.good())
	{
		QMessageBox::critical(NULL,"Unable to import inventory","Unable to read file. Do you have proper permissions?");
		return;
	}

	//Clear existing items.
	itemMap.clear();

	//Read each line of csv file and split column groups of 4 into items. Ordered by slot, id, damage, and count.
	while(inFile.good())
	{
		//Read a line.
		string line;
		std::getline(inFile,line);

		//Split line by commas into 4 fields that make up the item.
		vector<string> columns;
		boost::split(columns,line,boost::algorithm::is_any_of(","),boost::token_compress_on);
		if(columns.size() != 4)
			continue;

		//Try to turn text columns into item.
		try
		{
			Item newItem;
			newItem.slot = boost::numeric_cast<unsigned char>(boost::lexical_cast<short>(columns[0]));
			newItem.id = boost::lexical_cast<short>(columns[1]);
			newItem.damage = boost::lexical_cast<short>(columns[2]);
			newItem.count = boost::numeric_cast<unsigned char>(boost::lexical_cast<short>(columns[3]));

			itemMap[newItem.slot] = newItem;
		}
		catch(...)
		{
			//Ignore invalid item.
		}
	}

	//Update table.
	ReloadTableModel();
}

void InventoryWidget::ExportInventory(const char* filePath)
{
	std::ofstream outFile(filePath);
	if(!outFile.good())
	{
		QMessageBox::critical(NULL,"Unable to export inventory","Unable to write file. Do you have proper permissions?");
		return;
	}

	BOOST_FOREACH(const ItemMap::value_type& itemIter,itemMap)
	{
		const Item& item = itemIter.second;
		outFile << (int)item.slot << "," << (int)item.id << "," << (int)item.damage << "," << (int)item.count << std::endl;
	}
}

void InventoryWidget::NewItem()
{
	CreateNewItem(1,0,1); //Stone, x1
}

void InventoryWidget::NewStoneItem()
{
	CreateNewItem(1,0,64); //Stone, x64
}

void InventoryWidget::NewWoodItem()
{
	CreateNewItem(5,0,64); //Wood, x64
}

void InventoryWidget::NewGlassItem()
{
	CreateNewItem(20,0,64); //Glass, x64
}

void InventoryWidget::NewTorchesItem()
{
	CreateNewItem(50,0,64); //Torch, x64
}

void InventoryWidget::DeleteItem()
{
	//Find the selected row.
	int selectedRow = 0;
	unsigned char slot = 0;
	if(!GetSelectedItem(selectedRow,slot))
		return;

	//Find the item that belongs on that row.
	ItemMap::iterator itemIter = itemMap.find(slot);
	if(itemIter == itemMap.end())
		return;

	//Delete item.
	itemMap.erase(itemIter);

	//Update table.
	delete model.takeItem(selectedRow,1);
	delete model.takeItem(selectedRow,2);
	delete model.takeItem(selectedRow,3);

	//Keep the row selected even after deleting the items.
	inventoryTableView->selectRow(selectedRow);
}

void InventoryWidget::UpdateItem()
{
	//Find the selected row.
	int selectedRow = 0;
	unsigned char slot = 0;
	if(!GetSelectedItem(selectedRow,slot))
		return;

	//Find the item that belongs on that row.
	ItemMap::iterator itemIter = itemMap.find(slot);
	if(itemIter == itemMap.end())
		return;
	Item& item = itemIter->second;

	//Update item.
	if(!editWidget->GetItemInfo(item))
		return;

	//Update table.
	QStandardItem* typeItem = model.item(selectedRow,1);
	typeItem->setText(QString(ItemTypeName(item.id).c_str()));
	QStandardItem* damageItem = model.item(selectedRow,2);
	damageItem->setText(QString::number(item.damage));
	QStandardItem* countItem = model.item(selectedRow,3);
	countItem->setText(QString::number(item.count));
}

void InventoryWidget::CopyItem()
{
	//Find the selected row.
	int selectedRow = 0;
	unsigned char slot = 0;
	if(!GetSelectedItem(selectedRow,slot))
		return;

	//Find the item that belongs on that row.
	ItemMap::iterator itemIter = itemMap.find(slot);
	if(itemIter == itemMap.end())
		return;
	copiedItem = boost::shared_ptr<Item>(new Item(itemIter->second));
}

void InventoryWidget::PasteItem()
{
	//An item must have been previously copied.
	if(!copiedItem)
		return;
	
	//Find the selected row.
	int selectedRow = 0;
	unsigned char slot = 0;
	if(!GetSelectedItem(selectedRow,slot))
		return;

	//Update item. If it doesn't exist, create it.
	Item newItem = *copiedItem;
	newItem.slot = slot;
	itemMap[slot] = newItem;

	//Update table.
	model.setItem(selectedRow,1,new QStandardItem(ItemTypeName(newItem.id).c_str()));
	model.setItem(selectedRow,2,new QStandardItem(QString::number(newItem.damage)));
	model.setItem(selectedRow,3,new QStandardItem(QString::number(newItem.count)));

	//Prevent row from deselecting after model change.
	inventoryTableView->selectRow(selectedRow);
	
	//Update edit widget to show the copied item.
	editWidget->SetItem(&newItem);
}

void InventoryWidget::SelectedItem(const QItemSelection& selected,const QItemSelection&)
{
	QList<QModelIndex> indexes = selected.indexes();
	if(indexes.isEmpty())
	{
		SelectItem(NULL);
		return;
	}

	QStandardItem* selectedSlotItem = model.item(indexes.first().row());
	if(selectedSlotItem == NULL)
		SelectItem(NULL);
	else
	{
		const unsigned int slot = selectedSlotItem->data().toUInt();
		SelectItem(&slot);
	}
}

void InventoryWidget::ReloadTableModel()
{
	//Remove any existing rows.
	model.removeRows(0,model.rowCount());

	//Insert each item slot into table and fill out columns for row if item exists.
	SlotNameBimap slotNameBimap = CreateSlotNameBimap();
	model.setHorizontalHeaderLabels(
		QStringList() << "Slot" << "Type" << "Damage" << "Count");
	for(SlotNameBimap::const_iterator slotIter = slotNameBimap.begin();
	   slotIter != slotNameBimap.end();
	   ++slotIter)
	{
		QList<QStandardItem*> rowItems;
		QStandardItem* slotItem = new QStandardItem(slotIter->right.c_str());
		slotItem->setData(QVariant((unsigned int)slotIter->left));
		rowItems.append(slotItem);

		//If the item exists, fill out the remaining cells.
		map<unsigned char,Item>::const_iterator itemIter = itemMap.find(slotIter->left);
		if(itemIter != itemMap.end())
		{
			const Item& item = (*itemIter).second;

			rowItems.append(new QStandardItem(ItemTypeName(item.id).c_str()));
			rowItems.append(new QStandardItem(QString::number(item.damage)));
			rowItems.append(new QStandardItem(QString::number(item.count)));
		}
		model.appendRow(rowItems);
	}
}

void InventoryWidget::SelectItem(const unsigned int* slot)
{
	if(slot == NULL)
		editWidget->SetItem(NULL);
	else
	{
		ItemMap::const_iterator itemIter = itemMap.find(*slot);
		if(itemIter != itemMap.end())
			editWidget->SetItem(&(*itemIter).second);
		else
			editWidget->SetItem(NULL);
	}
}

bool InventoryWidget::GetSelectedItem(int& selectedRow,unsigned char& slot)
{
	//Find the selected row in table.
	QModelIndexList rowsList = inventoryTableView->selectionModel()->selectedRows();
	if(rowsList.empty())
		return false;
	selectedRow = rowsList[0].row();

	//Find slot of item that belongs on that row.
	QStandardItem* slotItem = model.item(selectedRow,0);
	slot = slotItem->data().toUInt();

	return true;
}

void InventoryWidget::CreateNewItem(const short type,const short damage,const unsigned char count)
{
	//Find the selected row.
	QModelIndexList rowsList = inventoryTableView->selectionModel()->selectedRows();
	if(rowsList.empty())
		return;
	const int selectedRow = rowsList[0].row();

	//Make sure item doesn't already exist.
	QStandardItem* slotItem = model.item(selectedRow,0);
	const unsigned char slot = slotItem->data().toUInt();
	ItemMap::iterator itemIter = itemMap.find(slot);
	if(itemIter != itemMap.end())
		return;

	//Create item.
	Item newItem;
	newItem.slot = slot;
	newItem.id = type;
	newItem.damage = damage;
	newItem.count = count;
	itemMap[slot] = newItem;

	//Update table.
	model.setItem(selectedRow,1,new QStandardItem(ItemTypeName(newItem.id).c_str()));
	model.setItem(selectedRow,2,new QStandardItem(QString::number(newItem.damage)));
	model.setItem(selectedRow,3,new QStandardItem(QString::number(newItem.count)));
	
	//Switch from "new" to "edit" mode.
	editWidget->SetItem(&newItem);
}

Item InventoryWidget::TagToItem(NBT::Tag& tag)
{
	using NBT::Tag;

	Tag* idTag = GetChildNamedTag(&tag,"id");
	assert(idTag != NULL);
	Tag* damageTag = GetChildNamedTag(&tag,"Damage");
	assert(damageTag != NULL);
	Tag* countTag = GetChildNamedTag(&tag,"Count");
	assert(countTag != NULL);
	Tag* slotTag = GetChildNamedTag(&tag,"Slot");
	assert(slotTag != NULL);

	Item item;
	if(idTag != NULL)
		item.id = idTag->shortValue;
	else
	{
		std::cerr << "Warning: Item found without item tag.\n";
		PrintTag(tag);
	}
	if(damageTag != NULL)
		item.damage = damageTag->shortValue;
	else
	{
		std::cerr << "Warning: Item found without damage tag.\n";
		PrintTag(tag);
	}
	if(countTag != NULL)
		item.count = countTag->byteValue;
	else
	{
		std::cerr << "Warning: Item found without count tag.\n";
		PrintTag(tag);
	}
	if(slotTag != NULL)
		item.slot = slotTag->byteValue;
	else
	{
		std::cerr << "Warning: Item found without slot tag.\n";
		PrintTag(tag);
	}
	return item;
}

NBT::Tag InventoryWidget::ItemToTag(const Item& item)
{
	using namespace NBT;

	Tag tag;
	tag.type = TAG_COMPOUND;

	Tag idTag;
	idTag.type = TAG_SHORT;
	idTag.name = "id";
	idTag.shortValue = item.id;
	tag.childTags.push_back(idTag);

	Tag damageTag;
	damageTag.type = TAG_SHORT;
	damageTag.name = "Damage";
	damageTag.shortValue = item.damage;
	tag.childTags.push_back(damageTag);

	Tag countTag;
	countTag.type = TAG_BYTE;
	countTag.name = "Count";
	countTag.byteValue = item.count;
	tag.childTags.push_back(countTag);

	Tag slotTag;
	slotTag.type = TAG_BYTE;
	slotTag.name = "Slot";
	slotTag.byteValue = item.slot;
	tag.childTags.push_back(slotTag);

	return tag;
}

