#pragma once

#include <QTableWidget>
#include <QSet>
#include "Timeslot.h"

class QUndoStack;

class AvailableTimeslotsTable : public QTableWidget
{
	public:
		AvailableTimeslotsTable(QWidget* parent = nullptr);
		QSet<Timeslot> getAvailableTimeslots() const;
		void setAvailableTimeslots(QSet<Timeslot> const &availableTimeslots);
		void setUndoStack(QUndoStack* const newUndoStack);

	protected:
		QUndoStack* undoStack;

		void setAvailability(int row, int column, bool isAvailable);
		void toggleAvailability(int row, int column);
		void toggleAvailabilityUndoable(int row, int column);
		void toggleSelected();
};

