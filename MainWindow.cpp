#include "MainWindow.h"
#include <iostream>
#include <cassert>
#include <boost/foreach.hpp>
#include "EditWidget.h"
#include "FileUtil.h"
#include "FilePath.h"

MainWindow::MainWindow()
: QMainWindow()
{
	//Setup menu.
	QAction* openAction = new QAction(tr("&Open"),NULL);
	openAction->setShortcuts(QKeySequence::Open);
	connect(openAction,SIGNAL(triggered()),this,SLOT(Open()));

	QAction* openWorld1Action = new QAction("Open World &1",NULL);
	openWorld1Action->setShortcut(QString("Ctrl+1"));
	connect(openWorld1Action,SIGNAL(triggered()),this,SLOT(OpenWorld1()));

	QAction* openWorld2Action = new QAction("Open World &2",NULL);
	openWorld2Action->setShortcut(QString("Ctrl+2"));
	connect(openWorld2Action,SIGNAL(triggered()),this,SLOT(OpenWorld2()));

	QAction* openWorld3Action = new QAction("Open World &3",NULL);
	openWorld3Action->setShortcut(QString("Ctrl+3"));
	connect(openWorld3Action,SIGNAL(triggered()),this,SLOT(OpenWorld3()));

	QAction* openWorld4Action = new QAction("Open World &4",NULL);
	openWorld4Action->setShortcut(QString("Ctrl+4"));
	connect(openWorld4Action,SIGNAL(triggered()),this,SLOT(OpenWorld4()));

	QAction* openWorld5Action = new QAction("Open World &5",NULL);
	openWorld5Action->setShortcut(QString("Ctrl+5"));
	connect(openWorld5Action,SIGNAL(triggered()),this,SLOT(OpenWorld5()));

	QAction* saveAction = new QAction(tr("&Save"),NULL);
	saveAction->setShortcuts(QKeySequence::Save);
	connect(saveAction,SIGNAL(triggered()),this,SLOT(Save()));

	QAction* saveAsAction = new QAction(tr("Save As"),NULL);
	saveAsAction->setShortcuts(QKeySequence::SaveAs);
	connect(saveAsAction,SIGNAL(triggered()),this,SLOT(SaveAs()));

	QAction* quitAction = new QAction("Quit",NULL);
	quitAction->setShortcuts(QKeySequence::Quit);
	connect(quitAction,SIGNAL(triggered()),this,SLOT(close()));

	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(openAction);
	fileMenu->addAction(openWorld1Action);
	fileMenu->addAction(openWorld2Action);
	fileMenu->addAction(openWorld3Action);
	fileMenu->addAction(openWorld4Action);
	fileMenu->addAction(openWorld5Action);
	fileMenu->addSeparator();
	fileMenu->addAction(saveAction);
	fileMenu->addAction(saveAsAction);
#ifdef __linux__
	fileMenu->addSeparator();
	fileMenu->addAction(quitAction);
#endif

	QAction* copyItemAction = new QAction("&Copy Item",NULL);
	copyItemAction->setShortcut(QString("Ctrl+C"));
	connect(copyItemAction,SIGNAL(triggered()),this,SLOT(CopyItem()));

	QAction* pasteItemAction = new QAction("Paste Item",NULL);
	pasteItemAction->setShortcut(QString("Ctrl+V"));
	connect(pasteItemAction,SIGNAL(triggered()),this,SLOT(PasteItem()));

	editMenu = menuBar()->addMenu(tr("&Edit"));
	editMenu->addAction(copyItemAction);
	editMenu->addAction(pasteItemAction);

	QAction* newItemAction = new QAction("&New Item",NULL);
	newItemAction->setShortcut(QString("Ctrl+N"));
	connect(newItemAction,SIGNAL(triggered()),this,SLOT(NewItem()));

	QAction* deleteItemAction = new QAction("&Delete Item",NULL);
	QList<QKeySequence> deleteKeySequences;
	deleteKeySequences.push_back(QKeySequence("Ctrl+D"));
	deleteKeySequences.push_back(QKeySequence::Delete);
	deleteItemAction->setShortcuts(deleteKeySequences);
	connect(deleteItemAction,SIGNAL(triggered()),this,SLOT(DeleteItem()));

	inventoryMenu = menuBar()->addMenu(tr("&Inventory"));
	inventoryMenu->addAction(newItemAction);
	inventoryMenu->addAction(deleteItemAction);

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

	editWidget = new EditWidget();

	QHBoxLayout* topLayout = new QHBoxLayout();
	topLayout->addWidget(inventoryTableView,1);
	topLayout->addWidget(editWidget);

	QWidget* centralWidget = new QWidget();
	centralWidget->setLayout(topLayout);

	setCentralWidget(centralWidget);
	resize(780,500);
	setWindowTitle("InvGrid 0.4 | Simple Minecraft Inventory Editor");

	//Setup events.
	connect(editWidget,SIGNAL(NewItem()),SLOT(NewItem()));
	connect(editWidget,SIGNAL(DeleteItem()),SLOT(DeleteItem()));
	connect(editWidget,SIGNAL(UpdateItem()),SLOT(UpdateItem()));
}

MainWindow::~MainWindow()
{
}

void MainWindow::NewItem()
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
	newItem.id = 0;
	newItem.damage = 0;
	newItem.count = 1;
	itemMap[slot] = newItem;

	//Update table.
	model.setItem(selectedRow,1,new QStandardItem(ItemTypeName(newItem.id).c_str()));
	model.setItem(selectedRow,2,new QStandardItem(QString::number(newItem.damage)));
	model.setItem(selectedRow,3,new QStandardItem(QString::number(newItem.count)));
	
	//Switch from "new" to "edit" mode.
	editWidget->SetItem(&newItem);
}

void MainWindow::DeleteItem()
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

void MainWindow::UpdateItem()
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

void MainWindow::CopyItem()
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

void MainWindow::PasteItem()
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

void MainWindow::Open()
{
	QString fileName = QFileDialog::getOpenFileName(this,tr("Open NBT"));
	if(fileName.isNull())
		return;

	Load(fileName.toAscii().data());
}

void MainWindow::OpenWorld1()
{
	LoadWorld("World1");
}

void MainWindow::OpenWorld2()
{
	LoadWorld("World2");
}

void MainWindow::OpenWorld3()
{
	LoadWorld("World3");
}

void MainWindow::OpenWorld4()
{
	LoadWorld("World4");
}

void MainWindow::OpenWorld5()
{
	LoadWorld("World5");
}

void MainWindow::Save()
{
	if(openFileName.isEmpty())
		QMessageBox::critical(NULL,"Unable to save file","You must open a file first.");

	Save(openFileName.toAscii().data());
}

void MainWindow::SaveAs()
{
	QString fileName = QFileDialog::getSaveFileName(this,tr("Save NBT"),"",tr("Dat files (*.dat)"));
	if(fileName.isNull())
		return;

	if(Save(fileName.toAscii().data()))
		openFileName = fileName;
}

void MainWindow::SelectedItem(const QItemSelection& selected,const QItemSelection&)
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

void MainWindow::LoadWorld(const char* worldName)
{
	const string worldFilePath = FilePath::GetMinecraftSavesDirectory() + worldName + "/level.dat";
	Load(worldFilePath.c_str());
}

bool MainWindow::Load(const char* filePath)
{
	const QString failToLoadTitle = "Unable to load file";

	//Reset item and table data so existing data doesn't collide with loaded data.
	itemMap.clear();
	model.removeRows(0,model.rowCount());
	rootTag = NBT::Tag();
	openFileName = QString();

	//Read file.
	vector<unsigned char> compressedBuffer;
	if(!ReadFile(filePath,compressedBuffer))
	{
		QMessageBox::critical(NULL,failToLoadTitle,"Unable to open file. Does file exist and do you have permission to access it?");
		return false;
	}
	std::cout << "Read " << compressedBuffer.size() << " bytes from file.\n";

	//Decompress.
	vector<unsigned char> buffer;
	if(!UncompressBuffer(compressedBuffer,buffer))
	{
		QMessageBox::critical(NULL,failToLoadTitle,"Unable to decompress file. Unsupported file type.");
		std::cerr << "Unable to uncompress file.\n";
		return false;
	}

	//Deserialize.
	if(!NBT::DeserializeTag(buffer,rootTag))
	{
		QMessageBox::critical(NULL,failToLoadTitle,"Unable to parse file. Unsupported file type.");
		return false;
	}

	NBT::Tag* inventoryTag = GetInventoryTag();
	if(inventoryTag == NULL)
	{
		QMessageBox::critical(NULL,failToLoadTitle,"Unable to use file. Not a level.dat or version is unsupported.");
		return false;
	}
	for(unsigned int x = 0;x < inventoryTag->childTags.size();++x)
	{
		Item item = TagToItem(inventoryTag->childTags[x]);
		itemMap[item.slot] = item;
	}

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

	openFileName = filePath;
	return true;
}

bool MainWindow::Save(const char* filePath)
{
	const QString failToSaveTitle = "Unable to save file";
	const QString backupSuffix = ".invgridbackup";

	NBT::Tag* inventoryTag = GetInventoryTag();
	if(inventoryTag == NULL)
	{
		QMessageBox::critical(NULL,failToSaveTitle,"You must open a file first.");
		return false;
	}

	inventoryTag->childTags.clear();
	BOOST_FOREACH(const ItemMap::value_type& itemIter,itemMap)
	{
		inventoryTag->childTags.push_back(ItemToTag(itemIter.second));
	}

	//Serialize.
	vector<unsigned char> buffer;
	if(!NBT::SerializeTag(buffer,rootTag))
	{
		QMessageBox::critical(NULL,failToSaveTitle,"Internal data error.");
		return false;
	}

	//Compress.
	vector<unsigned char> compressedBuffer;
	if(!CompressBuffer(buffer,compressedBuffer))
	{
		QMessageBox::critical(NULL,failToSaveTitle,"Internal data error. Unable to compress.");
		return false;
	}

	//Backup original.
	const QString backupFile = filePath + backupSuffix;
	QFile::remove(backupFile);
	QFile::rename(filePath,backupFile);

	//Save.
	if(!WriteFile(filePath,compressedBuffer))
	{
		QMessageBox::critical(NULL,failToSaveTitle,"Unable to write file. Do you have proper permissions?");
		return false;
	}

	return true;
}

void MainWindow::SelectItem(const unsigned int* slot)
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

bool MainWindow::GetSelectedItem(int& selectedRow,unsigned char& slot)
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

NBT::Tag* MainWindow::GetInventoryTag()
{
	using NBT::Tag;
	
	Tag* dataTag = GetChildNamedTag(&rootTag,"Data");
	if(dataTag == NULL)
		return NULL;
	Tag* playerTag = GetChildNamedTag(dataTag,"Player");
	if(playerTag == NULL)
		return NULL;
	Tag* inventoryTag = GetChildNamedTag(playerTag,"Inventory");

	//Hack work around for an issue where empty inventories have the wrong list type.
	if(inventoryTag->listType != NBT::TAG_COMPOUND)
	{
		inventoryTag->listType = NBT::TAG_COMPOUND;
		inventoryTag->childTags.clear();
	}

	return inventoryTag;
}

Item MainWindow::TagToItem(NBT::Tag& tag)
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

NBT::Tag MainWindow::ItemToTag(const Item& item)
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

