#ifndef SCRIPTWIDGET_H
#define SCRIPTWIDGET_H

#include <QtGui>

namespace NBT
{
	class Tag;
};

class ScriptWidget : public QWidget
{
	Q_OBJECT;
	public:
		ScriptWidget();
		~ScriptWidget();

		void SetInventoryTag(NBT::Tag* inventoryTag);
	signals:
		void SaveToInventoryTag(); //Emitted before script is run so script can see the current state of the inventory.
		void ReloadFromInventoryTag(); //Emitted after script is run so other widgets can stay in sync.
	private slots:
		void RunScript();
		void ValidateScript();
	private:
		QPlainTextEdit* scriptTextEdit;
		QTextEdit* consoleTextEdit;
		NBT::Tag* inventoryTag;
};

#endif

