#include "SubjectsTab.h"
#include "ui_SubjectsTab.h"

#include <QColorDialog>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QShortcut>
#include <QUndoStack>
#include "Group.h"
#include "Groups.h"
#include "Subject.h"
#include "Subjects.h"
#include "Teacher.h"
#include "Teachers.h"
#include "UndoCommand.h"

SubjectsTab::SubjectsTab(QWidget *parent) :
	Tab(parent),
	groups(nullptr),
	subjects(nullptr),
	teachers(nullptr),
	ui(new Ui::SubjectsTab)
{
	ui->setupUi(this);
	ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	connect(ui->table, &QTableWidget::cellDoubleClicked, this, [&](int row, int column) { if (column == ColumnColor) { edit(row, column); } });
	connect(ui->table, &QTableWidget::cellChanged, this, &SubjectsTab::edit);

	connect(ui->addButton, &QPushButton::clicked, this, &SubjectsTab::append);
	connect(ui->removeButton, &QPushButton::clicked, this, &SubjectsTab::deleteSelected);
	connect(new QShortcut(QKeySequence(QKeySequence::Delete), this), &QShortcut::activated, this, &SubjectsTab::deleteSelected);
}

void SubjectsTab::setData(Groups* const newGroups, Subjects* const newSubjects, Teachers* const newTeachers)
{
	if (subjects != nullptr) {
		subjects->disconnect(this);
	}

	groups = newGroups;
	subjects = newSubjects;
	teachers = newTeachers;

	reconstruct();
	connect(subjects, &Subjects::inserted, this, &SubjectsTab::insert);
	connect(subjects, &Subjects::changed, this, &SubjectsTab::updateRow);
	connect(subjects, &Subjects::removed, this, &SubjectsTab::reconstruct);
}

SubjectsTab::~SubjectsTab()
{
	delete ui;
}

void SubjectsTab::reconstruct()
{
	ui->table->blockSignals(true);
	ui->table->clearContents();
	ui->table->setRowCount(subjects->size());
	for (int row = 0; row < subjects->size(); ++row) {
		updateRow(row);
	}
	ui->table->blockSignals(false);
}

void SubjectsTab::append()
{
	QString name(tr("Matière %1"));
	int i = 1;

	while (std::any_of(subjects->cbegin(), subjects->cend(), [&](auto const &subject) { return subject->getName() == name.arg(i) || subject->getShortName() == name.arg(i); })) {
		++i;
	}

	auto subject = new Subject(name.arg(i), name.arg(i), 1, QColor::fromHsv(QRandomGenerator::global()->bounded(0, 360), 192, 192));
	for (auto const &group: *groups) {
		group->addSubject(subject);
	}

	auto command = new UndoCommand(
		[=]() { subjects->append(subject); },
		[=]() { subjects->remove(subjects->size() - 1); }
	);
	addUndoCommand(command);
}

void SubjectsTab::insert(int row)
{
	ui->table->blockSignals(true);
	ui->table->insertRow(row);
	updateRow(row);
	ui->table->blockSignals(false);
}

void SubjectsTab::deleteSelected()
{
	auto selectedIndexes = ui->table->selectionModel()->selectedIndexes();

	QVector<int> rowsToDelete;
	for (auto const &selectedIndex: selectedIndexes)
	{
		if (!rowsToDelete.contains(selectedIndex.row())) {
			rowsToDelete << selectedIndex.row();
		}
	}

	beginUndoMacro();
	std::sort(rowsToDelete.rbegin(), rowsToDelete.rend());
	for (int rowToDelete: rowsToDelete)
	{
		auto subject = subjects->at(rowToDelete);
		auto teachersOfSubject = teachers->ofSubject(subject);

		if (!teachersOfSubject.empty())
		{
			QMessageBox messageBox;
			messageBox.setIcon(QMessageBox::Question);
			messageBox.setTextFormat(Qt::RichText);
			messageBox.setText(tr("Un ou plusieurs enseignants collant en <em>%1</em> ont été définis.").arg(subject->getName()));
			messageBox.setInformativeText(tr("Ils seront supprimés en même temps que cette matière. Voulez-vous continuer malgré tout ?"));
			messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			messageBox.setDefaultButton(QMessageBox::No);
			auto clickedButton = messageBox.exec();

			if (clickedButton == QMessageBox::No) {
				continue;
			}
		}

		QVector<int> idTeachersOfSubject;
		for (auto const &teacher: teachersOfSubject) {
			idTeachersOfSubject << teachers->indexOf(teacher);
		}

		auto groupsWithSubject = groups->withSubject(subject);
		auto command = new UndoCommand(
			[=]() {
				for (int i = teachersOfSubject.size() - 1; i >= 0; --i) {
					teachers->remove(idTeachersOfSubject[i]);
				}
				for (auto const &group: groupsWithSubject) {
					group->removeSubject(subject);
				}
				subjects->remove(rowToDelete);
			},
			[=]() {
				for (int i = 0; i < teachersOfSubject.size(); ++i) {
					teachers->insert(idTeachersOfSubject[i], teachersOfSubject[i]);
				}
				for (auto const &group: groupsWithSubject) {
					group->addSubject(subject);
				}
				subjects->insert(rowToDelete, subject);
			}
		);
		addUndoCommand(command);
	}
	endUndoMacro();
}

void SubjectsTab::edit(int row, int column)
{
	ui->table->blockSignals(true);
	if (column == ColumnName || column == ColumnShortName) {
		auto item = ui->table->item(row, column);
		item->setData(Qt::DisplayRole, item->data(Qt::DisplayRole).toString().trimmed());
	}

	switch (column) {
		case ColumnColor: editColor(row); break;
		case ColumnName: editName(row); break;
		case ColumnShortName: editShortName(row); break;
		case ColumnFrequency: editFrequency(row); break;
	}
	ui->table->blockSignals(false);
}

void SubjectsTab::editColor(int row)
{
	auto currentColor = subjects->at(row)->getColor();
	auto color = QColorDialog::getColor(currentColor, ui->table);
	if (color.isValid())
	{
		auto command = new UndoCommand(
			[=]() { subjects->at(row)->setColor(color); },
			[=]() { subjects->at(row)->setColor(currentColor); }
		);
		addUndoCommand(command);
	}
}

void SubjectsTab::editName(int row)
{
	auto item = ui->table->item(row, ColumnName);
	auto name = item->data(Qt::DisplayRole).toString();
	auto currentName = subjects->at(row)->getName();

	if (name == currentName) {
		return;
	}

	if (name.isEmpty())
	{
		QMessageBox messageBox;
		messageBox.setIcon(QMessageBox::Critical);
		messageBox.setText(tr("Vous n'avez indiqué aucun nom pour cette matière."));
		messageBox.setInformativeText(tr("Veuillez indiquer un nom."));
		messageBox.setStandardButtons(QMessageBox::Ok);
		messageBox.setDefaultButton(QMessageBox::Ok);
		messageBox.exec();

		item->setData(Qt::DisplayRole, currentName);
		return;
	}

	if (std::any_of(subjects->cbegin(), subjects->cend(), [&](auto const &subject) { return subject->getName() == name; }))
	{
		QMessageBox messageBox;
		messageBox.setIcon(QMessageBox::Critical);
		messageBox.setText(tr("Une matière portant ce nom existe déjà."));
		messageBox.setInformativeText(tr("Veuillez indiquer un autre nom."));
		messageBox.setStandardButtons(QMessageBox::Ok);
		messageBox.setDefaultButton(QMessageBox::Ok);
		messageBox.exec();

		item->setData(Qt::DisplayRole, currentName);
		return;
	}

	auto command = new UndoCommand(
		[=]() { subjects->at(row)->setName(name); },
		[=]() { subjects->at(row)->setName(currentName); }
	);
	addUndoCommand(command);
}

void SubjectsTab::editShortName(int row)
{
	auto item = ui->table->item(row, ColumnShortName);
	auto shortName = item->data(Qt::DisplayRole).toString();
	auto currentShortName = subjects->at(row)->getShortName();

	if (shortName == currentShortName) {
		return;
	}

	if (shortName.isEmpty())
	{
		QMessageBox messageBox;
		messageBox.setIcon(QMessageBox::Critical);
		messageBox.setText(tr("Vous n'avez indiqué aucune abréviation pour cette matière."));
		messageBox.setInformativeText(tr("Veuillez indiquer une abréviation."));
		messageBox.setStandardButtons(QMessageBox::Ok);
		messageBox.setDefaultButton(QMessageBox::Ok);
		messageBox.exec();

		item->setData(Qt::DisplayRole, currentShortName);
		return;
	}

	if (std::any_of(subjects->cbegin(), subjects->cend(), [&](auto const &subject) { return subject->getShortName() == shortName; }))
	{
		QMessageBox messageBox;
		messageBox.setIcon(QMessageBox::Critical);
		messageBox.setText(tr("Une matière portant cette abréviation existe déjà."));
		messageBox.setInformativeText(tr("Veuillez indiquer une autre abréviation."));
		messageBox.setStandardButtons(QMessageBox::Ok);
		messageBox.setDefaultButton(QMessageBox::Ok);
		messageBox.exec();

		item->setData(Qt::DisplayRole, currentShortName);
		return;
	}

	auto command = new UndoCommand(
		[=]() { subjects->at(row)->setShortName(shortName); },
		[=]() { subjects->at(row)->setShortName(currentShortName); }
	);
	addUndoCommand(command);
}

void SubjectsTab::editFrequency(int row)
{
	auto item = ui->table->item(row, ColumnFrequency);
	auto frequency = item->data(Qt::DisplayRole).toInt();
	auto currentFrequency = subjects->at(row)->getFrequency();

	if (frequency <= 0)
	{
		item->setData(Qt::DisplayRole, currentFrequency);
		return;
	}

	auto command = new UndoCommand(
		[=]() { subjects->at(row)->setFrequency(frequency); },
		[=]() { subjects->at(row)->setFrequency(currentFrequency); }
	);
	addUndoCommand(command);
}

void SubjectsTab::updateRow(int row) const
{
	ui->table->blockSignals(true);
	auto subject = subjects->at(row);

	auto color = new QTableWidgetItem();
	color->setBackground(subject->getColor());
	color->setFlags(Qt::ItemIsEnabled);
	ui->table->setItem(row, ColumnColor, color);

	auto name = new QTableWidgetItem();
	name->setData(Qt::DisplayRole, subject->getName());
	name->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
	ui->table->setItem(row, ColumnName, name);

	auto shortName = new QTableWidgetItem();
	shortName->setData(Qt::DisplayRole, subject->getShortName());
	shortName->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
	ui->table->setItem(row, ColumnShortName, shortName);

	auto frequency = new QTableWidgetItem();
	frequency->setData(Qt::DisplayRole, subject->getFrequency());
	frequency->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
	ui->table->setItem(row, ColumnFrequency, frequency);

	ui->table->blockSignals(false);
}
