#include "MainWindow.h"
#include <iostream>
#include <cassert>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include "FileUtil.h"
#include "FilePath.h"

MainWindow::MainWindow()
: QMainWindow()
{
	//Setup widgets.
	inventoryWidget = new InventoryWidget();
	miscWidget = new MiscWidget();
	miscWidget->setEnabled(false);
	tabWidget = new QTabWidget();
	tabWidget->addTab(inventoryWidget,"Inventory");
	tabWidget->addTab(miscWidget,"Misc.");

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

	QAction* importAction = new QAction(tr("Import Inventory (.csv)"),NULL);
	connect(importAction,SIGNAL(triggered()),this,SLOT(ImportInventory()));

	QAction* exportAction = new QAction(tr("Export Inventory (.csv)"),NULL);
	connect(exportAction,SIGNAL(triggered()),this,SLOT(ExportInventory()));

	QAction* quitAction = new QAction("Quit",NULL);
	quitAction->setShortcut(QString("Ctrl+Q"));
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
	fileMenu->addSeparator();
	fileMenu->addAction(importAction);
	fileMenu->addAction(exportAction);
#ifdef __linux__
	fileMenu->addSeparator();
	fileMenu->addAction(quitAction);
#endif

	QAction* copyItemAction = new QAction("&Copy Item",NULL);
	copyItemAction->setShortcut(QString("Ctrl+C"));
	connect(copyItemAction,SIGNAL(triggered()),inventoryWidget,SLOT(CopyItem()));

	QAction* pasteItemAction = new QAction("Paste Item",NULL);
	pasteItemAction->setShortcut(QString("Ctrl+V"));
	connect(pasteItemAction,SIGNAL(triggered()),inventoryWidget,SLOT(PasteItem()));

	editMenu = menuBar()->addMenu(tr("&Edit"));
	editMenu->addAction(copyItemAction);
	editMenu->addAction(pasteItemAction);

	QAction* newItemAction = new QAction("&New Item",NULL);
	newItemAction->setShortcut(QString("Ctrl+N"));
	connect(newItemAction,SIGNAL(triggered()),inventoryWidget,SLOT(NewItem()));

	QAction* deleteItemAction = new QAction("&Delete Item",NULL);
	QList<QKeySequence> deleteKeySequences;
	deleteKeySequences.push_back(QKeySequence("Ctrl+D"));
	deleteKeySequences.push_back(QKeySequence::Delete);
	deleteItemAction->setShortcuts(deleteKeySequences);
	connect(deleteItemAction,SIGNAL(triggered()),inventoryWidget,SLOT(DeleteItem()));

	inventoryMenu = menuBar()->addMenu(tr("&Inventory"));
	inventoryMenu->addAction(newItemAction);
	inventoryMenu->addAction(deleteItemAction);

	//Setup main window.
	setCentralWidget(tabWidget);
	resize(780,500);
	setWindowTitle("InvGrid 0.7-rc1 | Simple Minecraft Inventory Editor");
}

MainWindow::~MainWindow()
{
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

void MainWindow::ImportInventory()
{
	//Make sure a world has been opened.
	NBT::Tag* inventoryTag = GetInventoryTag();
	if(inventoryTag == NULL)
	{
		QMessageBox::critical(NULL,"Unable to import inventory","You must open a world file first.");
		return;
	}

	//Ask user where import file is.
	QString fileName = QFileDialog::getOpenFileName(this,tr("Import Inventory"));
	if(fileName.isNull())
		return;

	//Import.
	inventoryWidget->ImportInventory(fileName.toAscii().data());
}

void MainWindow::ExportInventory()
{
	//Make sure a world has been opened.
	NBT::Tag* inventoryTag = GetInventoryTag();
	if(inventoryTag == NULL)
	{
		QMessageBox::critical(NULL,"Unable to export inventory","You must open a world file first.");
		return;
	}

	//Ask user where to save csv export file to.
	QString fileName = QFileDialog::getSaveFileName(this,tr("Export Inventory"),"",tr("csv files (*.csv)"));
	if(fileName.isNull())
		return;

	//Export.
	inventoryWidget->ExportInventory(fileName.toAscii().data());
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
	inventoryWidget->SetInventoryTag(NULL);
	miscWidget->setEnabled(false);
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
	inventoryWidget->SetInventoryTag(inventoryTag);
	miscWidget->LoadFromRootTag(&rootTag);

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

	//Save changes to NBT tag tree.
	inventoryWidget->SaveChangesToInventoryTag();
	miscWidget->SaveToRootTag(&rootTag);

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

