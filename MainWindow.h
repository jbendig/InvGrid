#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <map>
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
	private slots:
		void Open();
		void Save();
		void SelectedItem(const QItemSelection& selected,const QItemSelection& deselected);
	private:
		//Widgets
		QMenu* fileMenu;
		QTableView* inventoryTableView;
		EditWidget* editWidget;
		
		//Data
		NBT::Tag rootTag;
		ItemMap itemMap;
		QStandardItemModel model;

		MainWindow(const MainWindow&); //No definition.
		MainWindow& operator=(const MainWindow&); //No definition.
		void Load(const char* filePath);
		void Save(const char* filePath);
		void SelectItem(const unsigned int* slot);
		NBT::Tag* GetInventoryTag();
		static Item TagToItem(NBT::Tag& tag);
		static NBT::Tag ItemToTag(const Item& item);
};

#endif

