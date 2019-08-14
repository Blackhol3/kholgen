#pragma once

#include <QWidget>

namespace Ui {
class OptionsTab;
}

class Options;
class QUndoStack;

class OptionsTab : public QWidget
{
	Q_OBJECT

	public:
		explicit OptionsTab(QWidget *parent = nullptr);
		void setData(Options* const newOptions, QUndoStack* const newUndoStack);
		~OptionsTab();

	signals:
		void actionned();

	protected:
		Options* options;
		QUndoStack* undoStack;

		void reconstruct();
		void move(int from, int to);

	private:
		Ui::OptionsTab *ui;
};

