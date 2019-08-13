#include "TeachersTab.h"
#include "ui_TeachersTab.h"

#include <QShortcut>
#include "Subject.h"
#include "Subjects.h"
#include "Teacher.h"
#include "TeacherDialog.h"
#include "Teachers.h"

TeachersTab::TeachersTab(QWidget *parent) :
	QWidget(parent),
	subjects(nullptr),
	teachers(nullptr),
	ui(new Ui::TeachersTab)
{
	ui->setupUi(this);
	ui->tree->header()->setSectionResizeMode(QHeaderView::Stretch);

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
		ui->addButton->disconnect(this);
	}

	subjects = newSubjects;
	teachers = newTeachers;

	reconstruct();
	connect(subjects, &Subjects::changed, this, &TeachersTab::updateSubject);
	connect(subjects, &Subjects::appended, this, &TeachersTab::updateSubject);
	connect(subjects, &Subjects::removed, this, &TeachersTab::reconstruct);

	connect(teachers, &Teachers::changed, this, &TeachersTab::reconstruct);
	connect(teachers, &Teachers::appended, this, [&](int i) { appendTeacher(teachers->at(i)); });
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
	teachers->append(teacher);
}

void TeachersTab::editTeacher(QTreeWidgetItem* item)
{
	auto teacher = teachers->at(item->data(0, Qt::UserRole).toInt());

	TeacherDialog teacherDialog(subjects, teachers, this);
	teacherDialog.setTeacher(teacher);
	auto status = teacherDialog.exec();

	if (status == QDialog::Accepted) {
		teacherDialog.updateTeacher();
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
