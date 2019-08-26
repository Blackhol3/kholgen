#include "TeachersTab.h"
#include "ui_TeachersTab.h"

#include <QMouseEvent>
#include <QShortcut>
#include "Subject.h"
#include "Subjects.h"
#include "Teacher.h"
#include "TeacherDialog.h"
#include "Teachers.h"
#include "UndoCommand.h"

TeachersTab::TeachersTab(QWidget *parent) :
	Tab(parent),
	subjects(nullptr),
	teachers(nullptr),
	ui(new Ui::TeachersTab)
{
	ui->setupUi(this);
	ui->tree->header()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tree->viewport()->installEventFilter(this);

	connect(ui->tree, &QTreeWidget::itemDoubleClicked, this, [&](QTreeWidgetItem *item) {
		int indexSubject = ui->tree->indexOfTopLevelItem(item);
		if (indexSubject == -1) {
			editTeacher(item);
		}
	});

	connect(ui->addButton, &QPushButton::clicked, this, &TeachersTab::editNewTeacher);
	connect(ui->removeButton, &QPushButton::clicked, this, &TeachersTab::deleteSelectedTeacher);
	connect(new QShortcut(QKeySequence(QKeySequence::Delete), this), &QShortcut::activated, this, &TeachersTab::deleteSelectedTeacher);
}

void TeachersTab::setData(Subjects const* const newSubjects, Teachers* const newTeachers)
{
	if (subjects != nullptr) {
		subjects->disconnect(this);
		teachers->disconnect(this);
	}

	subjects = newSubjects;
	teachers = newTeachers;

	reconstruct();
	connect(subjects, &Subjects::inserted, this, &TeachersTab::reconstruct);
	connect(subjects, &Subjects::changed, this, &TeachersTab::updateSubject);
	connect(subjects, &Subjects::removed, this, &TeachersTab::reconstruct);

	connect(teachers, &Teachers::inserted, this, &TeachersTab::reconstruct);
	connect(teachers, &Teachers::changed, this, &TeachersTab::reconstruct);
	connect(teachers, &Teachers::removed, this, &TeachersTab::reconstruct);
}

TeachersTab::~TeachersTab()
{
	delete ui;
}

void TeachersTab::reconstruct()
{
	ui->tree->clear();
	for (int idSubject = 0; idSubject < subjects->size(); ++idSubject)
	{
		updateSubject(idSubject);

		for (auto const &teacher: teachers->ofSubject(subjects->at(idSubject))) {
			appendTeacher(teacher);
		}
	}
}


void TeachersTab::appendTeacher(const Teacher *const teacher) const
{
	auto subjectItem = ui->tree->topLevelItem(subjects->indexOf(teacher->getSubject()));
	auto teacherItem = new QTreeWidgetItem(subjectItem);
	teacherItem->setData(0, Qt::DisplayRole, teacher->getName());
	teacherItem->setData(0, Qt::UserRole, teachers->indexOf(teacher));
}

void TeachersTab::deleteSelectedTeacher()
{
	QVector<int> idTeachersToDelete;
	for (auto const &selectedItem: ui->tree->selectedItems()) {
		idTeachersToDelete << selectedItem->data(0, Qt::UserRole).toInt();
	}

	std::sort(idTeachersToDelete.rbegin(), idTeachersToDelete.rend());
	for (auto const &idTeacherToDelete: idTeachersToDelete) {
		teachers->remove(idTeacherToDelete);
	}
}

void TeachersTab::editNewTeacher()
{
	TeacherDialog teacherDialog(subjects, teachers, this);
	auto status = teacherDialog.exec();

	if (status == QDialog::Rejected) {
		return;
	}

	auto teacher = teacherDialog.createTeacher();
	auto command = new UndoCommand(
		[=]() { teachers->append(teacher); },
		[=]() { teachers->remove(teachers->size() - 1); }
	);
	addUndoCommand(command);
}

void TeachersTab::editTeacher(QTreeWidgetItem* item)
{
	auto idTeacher = item->data(0, Qt::UserRole).toInt();
	auto teacher = teachers->at(idTeacher);

	TeacherDialog teacherDialog(subjects, teachers, this);
	teacherDialog.setTeacher(teacher);
	auto status = teacherDialog.exec();

	if (status == QDialog::Accepted)
	{
		auto const currentName = teacher->getName();
		auto const currentSubject = teacher->getSubject();
		auto const currentAvailableTimeslots = teacher->getAvailableTimeslots();

		teacherDialog.updateTeacher();

		auto const newName = teacher->getName();
		auto const newSubject = teacher->getSubject();
		auto const newAvailableTimeslots = teacher->getAvailableTimeslots();

		auto command = new UndoCommand(
			[=]() {
				teachers->at(idTeacher)->setName(newName);
				teachers->at(idTeacher)->setSubject(newSubject);
				teachers->at(idTeacher)->setAvailableTimeslots(newAvailableTimeslots);
			},
			[=]() {
				teachers->at(idTeacher)->setName(currentName);
				teachers->at(idTeacher)->setSubject(currentSubject);
				teachers->at(idTeacher)->setAvailableTimeslots(currentAvailableTimeslots);
			}
		);
		addUndoCommand(command);
	}
}

void TeachersTab::updateSubject(int row) const
{
	if (row > ui->tree->topLevelItemCount()) {
		updateSubject(row - 1);
	}

	auto subject = subjects->at(row);
	auto item = row < ui->tree->topLevelItemCount() ? ui->tree->topLevelItem(row) : new QTreeWidgetItem(ui->tree);

	item->setData(0, Qt::DisplayRole, subject->getName());
	item->setData(0, Qt::BackgroundRole, subject->getColor());
	item->setData(0, Qt::ForegroundRole, qGray(subject->getColor().rgb()) < 128 ? QColor(Qt::white) : QColor(Qt::black));
	item->setFlags(Qt::ItemIsEnabled);
	item->setExpanded(true);
}

bool TeachersTab::eventFilter(QObject* object, QEvent* event)
{
	if (object != ui->tree->viewport() || event->type() != QEvent::MouseButtonDblClick) {
		return false;
	}

	auto doubleClickEvent = static_cast<QMouseEvent*>(event);
	if (doubleClickEvent->button() != Qt::LeftButton) {
		return false;
	}

	if (ui->tree->itemAt(doubleClickEvent->pos()) == nullptr) {
		editNewTeacher();
	}

	return false;
}
