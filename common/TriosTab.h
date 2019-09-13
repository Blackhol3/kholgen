#pragma once

#include "Tab.h"

namespace Ui {
class TriosTab;
}

class Subjects;
class Trios;

class TriosTab : public Tab
{
	Q_OBJECT

	public:
		explicit TriosTab(QWidget *parent = nullptr);
		void setData(Trios* const newTrios, Subjects const* const newSubjects);
		~TriosTab() override;

	protected:
		Trios* trios;
		Subjects const* subjects;

		void reconstruct();
		void updateRow(int row);
		void append();
		void toggleSubject(int row, int column);
		void toggleSelected();
		void deleteSelected();
		bool eventFilter(QObject* object, QEvent* event) override;

	private:
		Ui::TriosTab *ui;
};

