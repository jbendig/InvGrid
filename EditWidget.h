#ifndef EDITWIDGET_H
#define EDITWIDGET_H

#include <QtGui>
#include "Item.h"

class EditWidget : public QWidget
{
	Q_OBJECT;
	public:
		EditWidget();

		void SetItem(const Item* item); //Set item to edit or NULL to prompt creation of a new item.
		bool GetItemInfo(Item& item); //Fills out item info using edited values in widgets.
	signals:
		void NewItem();
		void NewStoneItem();
		void NewWoodItem();
		void NewGlassItem();
		void NewTorchesItem();
		void DeleteItem();
		void UpdateItem();
	private slots:
		void FilterTypeList(const QString& filterText);
		void SetDamageToZero();
		void SetCountToSixtyFour();
	private:
		QWidget* newWidget;
		QPushButton* newButton;

		QWidget* editWidget;
		QLabel* slotLabel;
		QLineEdit* typeSearchEdit;
		QListWidget* typeListWidget;
		QSpinBox* damageSpinBox;
		QSpinBox* countSpinBox;
		QPushButton* deleteButton;

		void PopulateTypeListWidget();
		int FindTypeRow(const int type) const;
};

#endif

