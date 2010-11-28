#ifndef INVENTORYWIDGET_H
#define INVENTORYWIDGET_H

#include <boost/shared_ptr.hpp>
#include <QtGui>
#include "NBTTag.h"
#include "Item.h"

class EditWidget;

class InventoryWidget : public QWidget
{
	Q_OBJECT;
	public:
		InventoryWidget();

		void SetInventoryTag(NBT::Tag* inventoryTag); //Automatically reloads displayed inventory with data from node.
		void ImportInventory(const char* filePath);
		void ExportInventory(const char* filePath);
	public slots:
		void SaveChangesToInventoryTag();
		void ReloadFromInventoryTag();
		void NewItem();
		void NewStoneItem();
		void NewWoodItem();
		void NewGlassItem();
		void NewTorchesItem();
		void DeleteItem();
		void UpdateItem();
		void CopyItem();
		void PasteItem();
	private slots:
		void SelectedItem(const QItemSelection& selected,const QItemSelection& deselected);
	private:
		//Widgets
		QTableView* inventoryTableView;
		EditWidget* editWidget;

		//Data
		NBT::Tag* inventoryTag;
		ItemMap itemMap;
		QStandardItemModel model;
		boost::shared_ptr<Item> copiedItem;

		InventoryWidget(const InventoryWidget&); //No definition.
		InventoryWidget& operator=(const InventoryWidget&); //No definition.
		void ReloadTableModel();
		void SelectItem(const unsigned int* slot);
		bool GetSelectedItem(int& selectedRow,unsigned char& slot);
		void CreateNewItem(const short type,const short damage,const unsigned char count);
};

#endif

