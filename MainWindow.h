#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <map>
#include <boost/shared_ptr.hpp>
#include <QtGui>
#include "NBTTag.h"
#include "Item.h"
using std::map;

class EditWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT;
	public:
		MainWindow();
		~MainWindow();
	public slots:
		void NewItem();
		void DeleteItem();
		void UpdateItem();
		void CopyItem();
		void PasteItem();
	private slots:
		void Open();
		void OpenWorld1();
		void OpenWorld2();
		void OpenWorld3();
		void OpenWorld4();
		void OpenWorld5();
		void Save();
		void SaveAs();
		void ImportInventory();
		void ExportInventory();
		void SelectedItem(const QItemSelection& selected,const QItemSelection& deselected);
	private:
		//Widgets
		QMenu* fileMenu;
		QMenu* editMenu;
		QMenu* inventoryMenu;
		QTableView* inventoryTableView;
		EditWidget* editWidget;
		
		//Data
		QString openFileName;
		NBT::Tag rootTag;
		ItemMap itemMap;
		QStandardItemModel model;
		boost::shared_ptr<Item> copiedItem;

		MainWindow(const MainWindow&); //No definition.
		MainWindow& operator=(const MainWindow&); //No definition.
		void LoadWorld(const char* worldName);
		bool Load(const char* filePath);
		bool Save(const char* filePath);
		void ImportInventory(const char* filePath);
		void ExportInventory(const char* filePath);
		void ReloadTableModel();
		void SelectItem(const unsigned int* slot);
		bool GetSelectedItem(int& selectedRow,unsigned char& slot);
		NBT::Tag* GetInventoryTag();
		static Item TagToItem(NBT::Tag& tag);
		static NBT::Tag ItemToTag(const Item& item);
};

#endif

