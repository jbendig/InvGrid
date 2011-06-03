#include "MainWindow.h"
#include <iostream>
#include <cassert>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include "FileUtil.h"
#include "FilePath.h"
#include "ScriptWidget.h"

MainWindow::MainWindow()
: QMainWindow()
{
	//Setup widgets.
	inventoryWidget = new InventoryWidget();
	scriptWidget = new ScriptWidget();
	miscWidget = new MiscWidget();
	miscWidget->setEnabled(false);
	tabWidget = new QTabWidget();
	tabWidget->addTab(inventoryWidget,"Inventory");
	tabWidget->addTab(scriptWidget,"Script");
	tabWidget->addTab(miscWidget,"Misc.");

	//Reflect changes made in inventory editor, in scripts, and vice versa.
	connect(scriptWidget,SIGNAL(SaveToInventoryTag()),inventoryWidget,SLOT(SaveChangesToInventoryTag()));
	connect(scriptWidget,SIGNAL(ReloadFromInventoryTag()),inventoryWidget,SLOT(ReloadFromInventoryTag()));

	//Setup menu.
	QAction* openAction = new QAction(tr("&Open"),NULL);
	openAction->setShortcuts(QKeySequence::Open);
	connect(openAction,SIGNAL(triggered()),this,SLOT(Open()));

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

	QAction* runAction = new QAction("&Run",NULL);
	runAction->setShortcut(QKeySequence("Ctrl+R"));
	connect(runAction,SIGNAL(triggered()),scriptWidget,SLOT(RunScript()));

	QAction* generateScriptFromInventoryAction = new QAction("Generate Script From Inventory",NULL);
	connect(generateScriptFromInventoryAction,SIGNAL(triggered()),scriptWidget,SLOT(GenerateScriptFromInventory()));

	scriptMenu = menuBar()->addMenu("&Script");
	scriptMenu->addAction(runAction);
	scriptMenu->addAction(generateScriptFromInventoryAction);

	//Setup main window.
	setCentralWidget(tabWidget);
	resize(780,500);
	setWindowTitle("InvGrid 0.9 | Simple Minecraft Inventory Editor");
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	//If a world isn't open, nothing to save so program can exit immediately.
	NBT::Tag* inventoryTag = GetInventoryTag();
	if(inventoryTag == NULL)
	{
		event->accept();
		return;
	}

	//Otherwise, ask the user if they want to save...even if they JUST saved.
	QMessageBox messageBox;
	messageBox.setText("Do you want to save your changes?");
	messageBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	messageBox.setDefaultButton(QMessageBox::Save);
	const int result = messageBox.exec();
	if(result == QMessageBox::Cancel)
	{
		event->ignore();
		return;
	}

	if(result == QMessageBox::Save)
		Save();

	event->accept();
}

void MainWindow::Open()
{
	const string savesDirectory = FilePath::GetMinecraftSavesDirectory();
	QString fileName = QFileDialog::getOpenFileName(this,tr("Open NBT (Look for level.dat)"),savesDirectory.c_str());
	if(fileName.isNull())
		return;

	Load(fileName.toUtf8().data());
}

void MainWindow::Save()
{
	if(openFileName.isEmpty())
		QMessageBox::critical(NULL,"Unable to save file","You must open a file first.");

	Save(openFileName.toUtf8().data());
}

void MainWindow::SaveAs()
{
	QString fileName = QFileDialog::getSaveFileName(this,tr("Save NBT"),"",tr("Dat files (*.dat)"));
	if(fileName.isNull())
		return;

	if(Save(fileName.toUtf8().data()))
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
	inventoryWidget->ImportInventory(fileName.toUtf8().data());
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
	inventoryWidget->ExportInventory(fileName.toUtf8().data());
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
	scriptWidget->SetInventoryTag(NULL);
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
	scriptWidget->SetInventoryTag(inventoryTag);
	miscWidget->LoadFromRootTag(&rootTag);

	openFileName = QString::fromUtf8(filePath,strlen(filePath));
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

