#include "TeacherDialog.h"
#include "ui_TeacherDialog.h"

#include <QMessageBox>
#include <QShortcut>
#include "Subject.h"
#include "Subjects.h"
#include "Teacher.h"
#include "Teachers.h"
#include "TransparentItemDelegate.h"

TeacherDialog::TeacherDialog(Subjects const* const subjects, Teachers const* const teachers, QWidget *parent) :
	QDialog(parent),
	subjects(subjects),
	teachers(teachers),
	teacher(nullptr),
	ui(new Ui::TeacherDialog)
{
	ui->setupUi(this);

	connect(ui->name, &QLineEdit::editingFinished, this, [&]() { ui->name->setText(ui->name->text().trimmed()); });

	for (auto const &subject: *subjects)
	{
		QPixmap pixmap(100, 100);
		pixmap.fill(subject->getColor());

		ui->subject->addItem(pixmap, subject->getName());
	}

	ui->availableTimeslots->setItemDelegate(new TransparentItemDelegate(20, ui->availableTimeslots));
	ui->availableTimeslots->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->availableTimeslots->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	ui->availableTimeslots->setRowCount(Timeslot::lastHour - Timeslot::firstHour + 1);
	ui->availableTimeslots->setColumnCount(Timeslot::dayNames.size());

	QStringList verticalHeaderLabels;
	for (int hour = Timeslot::firstHour; hour <= Timeslot::lastHour; ++hour) {
		verticalHeaderLabels << tr("%1h").arg(hour);
	}
	ui->availableTimeslots->setVerticalHeaderLabels(verticalHeaderLabels);

	QStringList horizontalHeaderLabels;
	for (auto const &dayName: Timeslot::dayNames) {
		horizontalHeaderLabels << dayName;
	}
	ui->availableTimeslots->setHorizontalHeaderLabels(horizontalHeaderLabels);

	for (int row = 0; row < ui->availableTimeslots->rowCount(); ++row) {
		for (int column = 0; column < ui->availableTimeslots->columnCount(); ++column) {
			auto item = new QTableWidgetItem();
			item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			ui->availableTimeslots->setItem(row, column, item);
		}
	}

	connect(ui->availableTimeslots, &QTableWidget::cellDoubleClicked, this, &TeacherDialog::toggleAvailability);
	connect(new QShortcut(QKeySequence(Qt::Key_Enter), ui->availableTimeslots, nullptr, nullptr, Qt::WidgetWithChildrenShortcut), &QShortcut::activated, this, &TeacherDialog::toggleSelected);
	connect(new QShortcut(QKeySequence(Qt::Key_Return), ui->availableTimeslots, nullptr, nullptr, Qt::WidgetWithChildrenShortcut), &QShortcut::activated, this, &TeacherDialog::toggleSelected);
}

void TeacherDialog::accept()
{
	if (ui->name->text().isEmpty())
	{
		QMessageBox messageBox;
		messageBox.setIcon(QMessageBox::Critical);
		messageBox.setText(tr("Vous n'avez indiqué aucun nom pour cet·te enseignant·e."));
		messageBox.setInformativeText(tr("Veuillez indiquer un nom."));
		messageBox.setStandardButtons(QMessageBox::Ok);
		messageBox.setDefaultButton(QMessageBox::Ok);
		messageBox.exec();

		return;
	}

	auto teachersOfSubject = teachers->ofSubject(subjects->at(ui->subject->currentIndex()));
	if (std::any_of(teachersOfSubject.cbegin(), teachersOfSubject.cend(), [&] (auto const &checkedTeacher) { return checkedTeacher != teacher && checkedTeacher->getName() == ui->name->text(); }))
	{
		QMessageBox messageBox;
		messageBox.setIcon(QMessageBox::Critical);
		messageBox.setText(tr("Un·e autre enseignant·e de cette matière portant ce nom existe déjà."));
		messageBox.setInformativeText(tr("Veuillez indiquer un autre nom."));
		messageBox.setStandardButtons(QMessageBox::Ok);
		messageBox.setDefaultButton(QMessageBox::Ok);
		messageBox.exec();

		return;
	}

	QDialog::accept();
}

void TeacherDialog::setTeacher(Teacher* const newTeacher)
{
	teacher = newTeacher;

	ui->name->setText(teacher->getName());
	ui->subject->setCurrentIndex(subjects->indexOf(teacher->getSubject()));

	for (auto const &timeslot: teacher->getAvailableTimeslots()) {
		toggleAvailability(timeslot.getHour() - Timeslot::firstHour, static_cast<int>(timeslot.getDay()));
	}
}

Teacher* TeacherDialog::createTeacher()
{
	auto name = ui->name->text();
	auto subject = subjects->at(ui->subject->currentIndex());

	return new Teacher(name, subject, getAvailableTimeslots());
}

void TeacherDialog::updateTeacher()
{
	teacher->setName(ui->name->text());
	teacher->setSubject(subjects->at(ui->subject->currentIndex()));
	teacher->setAvailableTimeslots(getAvailableTimeslots());
}

TeacherDialog::~TeacherDialog()
{
	delete ui;
}

void TeacherDialog::toggleAvailability(int row, int column)
{
	auto item = ui->availableTimeslots->item(row, column);
	auto isAvailable = !item->data(Qt::UserRole).toBool();
	item->setBackground(isAvailable ? Qt::BDiagPattern : Qt::NoBrush);
	item->setData(Qt::UserRole, isAvailable);
}

void TeacherDialog::toggleSelected()
{
	auto selectedIndexes = ui->availableTimeslots->selectionModel()->selectedIndexes();
	for (auto const &selectedIndex: selectedIndexes) {
		toggleAvailability(selectedIndex.row(), selectedIndex.column());
	}
}

QSet<Timeslot> TeacherDialog::getAvailableTimeslots() const
{
	QSet<Timeslot> availableTimeslots;
	for (int row = 0; row < ui->availableTimeslots->rowCount(); ++row) {
		for (int column = 0; column < ui->availableTimeslots->columnCount(); ++column) {
			auto item = ui->availableTimeslots->item(row, column);
			bool isAvailable = item->data(Qt::UserRole).toBool();

			if (isAvailable) {
				availableTimeslots << Timeslot(static_cast<Day>(column), row + Timeslot::firstHour);
			}
		}
	}

	return availableTimeslots;
}
