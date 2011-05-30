#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <map>
#include <boost/shared_ptr.hpp>
#include <QtGui>
#include "NBTTag.h"
#include "Item.h"
#include "InventoryWidget.h"
#include "ScriptWidget.h"
#include "MiscWidget.h"
using std::map;

class MainWindow : public QMainWindow
{
	Q_OBJECT;
	public:
		MainWindow();
		~MainWindow();

		void closeEvent(QCloseEvent* event);
	private slots:
		void Open();
		void Save();
		void SaveAs();
		void ImportInventory();
		void ExportInventory();
	private:
		//Widgets
		QMenu* fileMenu;
		QMenu* editMenu;
		QMenu* inventoryMenu;
		QMenu* scriptMenu;
		QTabWidget* tabWidget;
		InventoryWidget* inventoryWidget;
		ScriptWidget* scriptWidget;
		MiscWidget* miscWidget;
		
		//Data
		QString openFileName;
		NBT::Tag rootTag;

		MainWindow(const MainWindow&); //No definition.
		MainWindow& operator=(const MainWindow&); //No definition.
		void LoadWorld(const char* worldName);
		bool Load(const char* filePath);
		bool Save(const char* filePath);
		void ImportInventory(const char* filePath);
		void ExportInventory(const char* filePath);
		NBT::Tag* GetInventoryTag();
};

#endif

