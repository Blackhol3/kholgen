#pragma once

#include <QWidget>

namespace Ui {
class OptionsTab;
}

class Options;

class OptionsTab : public QWidget
{
		Q_OBJECT

	public:
		explicit OptionsTab(QWidget *parent = nullptr);
		void setData(Options* const newOptions);
		~OptionsTab();

	protected:
		Options* options;

		void reconstruct();
		void move(int from, int to);

	private:
		Ui::OptionsTab *ui;
};

