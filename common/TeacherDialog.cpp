#include "TeacherDialog.h"
#include "ui_TeacherDialog.h"

#include <QMessageBox>
#include <QShortcut>
#include "Subject.h"
#include "Subjects.h"
#include "Teacher.h"
#include "Teachers.h"
#include "UndoCommand.h"

TeacherDialog::TeacherDialog(Subjects const* const subjects, Teachers const* const teachers, QWidget *parent) :
	QDialog(parent),
	subjects(subjects),
	teachers(teachers),
	teacher(nullptr),
	idSubject(0),
	ui(new Ui::TeacherDialog)
{
	ui->setupUi(this);
	ui->availableTimeslots->setUndoStack(&undoStack);

	for (auto const &subject: *subjects)
	{
		QPixmap pixmap(100, 100);
		pixmap.fill(subject->getColor());

		ui->subject->addItem(pixmap, subject->getName());
	}

	connect(ui->name, &QLineEdit::editingFinished, this, &TeacherDialog::editName);
	connect(ui->subject, qOverload<int>(&QComboBox::currentIndexChanged), this, &TeacherDialog::editSubject);

	connect(new QShortcut(QKeySequence::Undo, this), &QShortcut::activated, &undoStack, &QUndoStack::undo);
	connect(new QShortcut(QKeySequence::Redo, this), &QShortcut::activated, &undoStack, &QUndoStack::redo);
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

	name = teacher->getName();
	idSubject = subjects->indexOf(teacher->getSubject());

	ui->name->setText(name);
	ui->subject->blockSignals(true);
	ui->subject->setCurrentIndex(idSubject);
	ui->subject->blockSignals(false);
	ui->availableTimeslots->setAvailableTimeslots(teacher->getAvailableTimeslots());
}

Teacher* TeacherDialog::createTeacher()
{
	return new Teacher(name, subjects->at(idSubject), ui->availableTimeslots->getAvailableTimeslots());
}

void TeacherDialog::updateTeacher()
{
	teacher->setName(name);
	teacher->setSubject(subjects->at(idSubject));
	teacher->setAvailableTimeslots(ui->availableTimeslots->getAvailableTimeslots());
}

TeacherDialog::~TeacherDialog()
{
	delete ui;
}

void TeacherDialog::editName()
{
	auto newName = ui->name->text().trimmed();
	auto oldName = name;

	if (newName == oldName) {
		ui->name->setText(name);
		return;
	}

	auto command = new UndoCommand(
		[=]() {
			name = newName;
			ui->name->setText(name);
		},
		[=]() {
			name = oldName;
			ui->name->setText(name);
		}
	);
	undoStack.push(command);
}

void TeacherDialog::editSubject()
{
	auto newIdSubject = ui->subject->currentIndex();
	auto oldIdSubject = idSubject;

	auto command = new UndoCommand(
		[=]() {
			idSubject = newIdSubject;

			ui->subject->blockSignals(true);
			ui->subject->setCurrentIndex(idSubject);
			ui->subject->blockSignals(false);
		},
		[=]() {
			idSubject = oldIdSubject;

			ui->subject->blockSignals(true);
			ui->subject->setCurrentIndex(idSubject);
			ui->subject->blockSignals(false);
		}
	);
	undoStack.push(command);
}
