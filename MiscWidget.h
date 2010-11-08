#ifndef MISCWIDGET_H
#define MISCWIDGET_H

#include <QtGui>
#include "NBTTag.h"

class MiscWidget : public QWidget
{
	Q_OBJECT;
	public:
		MiscWidget();

		void LoadFromRootTag(NBT::Tag* rootTag);
		void SaveToRootTag(NBT::Tag* rootTag);
	public slots:
		void SetTimeToSunrise();
		void SetTimeToMidday();
		void SetTimeToSunset();
		void SetTimeToMidnight();
		void SetSpawnToCurrent();
		void SetCurrentToSpawn();
	private:
		QSpinBox* timeSpinBox;
		QSpinBox* spawnXSpinBox;
		QSpinBox* spawnYSpinBox;
		QSpinBox* spawnZSpinBox;
		QLineEdit* posXLineEdit;
		QLineEdit* posYLineEdit;
		QLineEdit* posZLineEdit;
};

#endif

