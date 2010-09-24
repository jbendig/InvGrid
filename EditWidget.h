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
		void DeleteItem();
		void UpdateItem();
	private:
		QWidget* newWidget;
		QPushButton* newButton;

		QWidget* editWidget;
		QLabel* slotLabel;
		QLabel* typeLabel;
		QComboBox* typeCombo;
		//QLineEdit* typeEdit;
		QLabel* damageLabel;
		QSpinBox* damageSpinBox;
		QLabel* countLabel;
		QSpinBox* countSpinBox;
		QPushButton* deleteButton;

		void PopulateTypeComboBox();
};

#endif

