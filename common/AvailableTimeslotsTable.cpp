#include "AvailableTimeslotsTable.h"

#include <QHeaderView>
#include <QShortcut>
#include <QUndoStack>
#include "TransparentItemDelegate.h"
#include "UndoCommand.h"

AvailableTimeslotsTable::AvailableTimeslotsTable(QWidget* parent): QTableWidget(parent)
{
	setItemDelegate(new TransparentItemDelegate(20, this));
	verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	setRowCount(Timeslot::lastHour - Timeslot::firstHour + 1);
	setColumnCount(Timeslot::dayNames.size());

	QStringList verticalHeaderLabels;
	for (int hour = Timeslot::firstHour; hour <= Timeslot::lastHour; ++hour) {
		verticalHeaderLabels << tr("%1h").arg(hour);
	}
	setVerticalHeaderLabels(verticalHeaderLabels);

	QStringList horizontalHeaderLabels;
	for (auto const &dayName: Timeslot::dayNames) {
		horizontalHeaderLabels << dayName;
	}
	setHorizontalHeaderLabels(horizontalHeaderLabels);

	for (int row = 0; row < rowCount(); ++row) {
		for (int column = 0; column < columnCount(); ++column) {
			auto item = new QTableWidgetItem();
			item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			setItem(row, column, item);
		}
	}

	connect(this, &QTableWidget::cellDoubleClicked, this, &AvailableTimeslotsTable::toggleAvailabilityUndoable);
	connect(new QShortcut(QKeySequence(Qt::Key_Enter), this, nullptr, nullptr, Qt::WidgetWithChildrenShortcut), &QShortcut::activated, this, &AvailableTimeslotsTable::toggleSelected);
	connect(new QShortcut(QKeySequence(Qt::Key_Return), this, nullptr, nullptr, Qt::WidgetWithChildrenShortcut), &QShortcut::activated, this, &AvailableTimeslotsTable::toggleSelected);
}

QSet<Timeslot> AvailableTimeslotsTable::getAvailableTimeslots() const
{
	QSet<Timeslot> availableTimeslots;
	for (int row = 0; row < rowCount(); ++row) {
		for (int column = 0; column < columnCount(); ++column) {
			auto currentItem = item(row, column);
			bool isAvailable = currentItem->data(Qt::UserRole).toBool();

			if (isAvailable) {
				availableTimeslots << Timeslot(static_cast<Day>(column), row + Timeslot::firstHour);
			}
		}
	}

	return availableTimeslots;
}

void AvailableTimeslotsTable::setAvailableTimeslots(const QSet<Timeslot>& availableTimeslots)
{
	for (int row = 0; row < rowCount(); ++row) {
		for (int column = 0; column < columnCount(); ++column) {
			setAvailability(row, column, false);
		}
	}

	for (auto const &timeslot: availableTimeslots) {
		setAvailability(timeslot.getHour() - Timeslot::firstHour, static_cast<int>(timeslot.getDay()), true);
	}
}

void AvailableTimeslotsTable::setUndoStack(QUndoStack* const newUndoStack)
{
	undoStack = newUndoStack;
}

void AvailableTimeslotsTable::setAvailability(int row, int column, bool isAvailable)
{
	auto currentItem = item(row, column);
	currentItem->setBackground(isAvailable ? Qt::BDiagPattern : Qt::NoBrush);
	currentItem->setData(Qt::UserRole, isAvailable);
}

void AvailableTimeslotsTable::toggleAvailability(int row, int column)
{
	setAvailability(row, column, !item(row, column)->data(Qt::UserRole).toBool());
}

void AvailableTimeslotsTable::toggleAvailabilityUndoable(int row, int column)
{
	auto command = new UndoCommand(
		[=]() { toggleAvailability(row, column); },
		[=]() { toggleAvailability(row, column); }
	);
	undoStack->push(command);
}

void AvailableTimeslotsTable::toggleSelected()
{
	undoStack->beginMacro("");
	auto selectedIndexes = selectionModel()->selectedIndexes();
	for (auto const &selectedIndex: selectedIndexes) {
		toggleAvailabilityUndoable(selectedIndex.row(), selectedIndex.column());
	}
	undoStack->endMacro();
}
