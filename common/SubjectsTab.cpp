#include "SubjectsTab.h"
#include "ui_SubjectsTab.h"

#include <QColorDialog>
#include <QDebug>
#include <QMessageBox>
#include <QShortcut>
#include "Subject.h"
#include "Subjects.h"
#include "Teacher.h"
#include "Teachers.h"

SubjectsTab::SubjectsTab(QWidget *parent) :
	QWidget(parent),
	subjects(nullptr),
	teachers(nullptr),
	isModificationInProgress(false),
	ui(new Ui::SubjectsTab)
{
	ui->setupUi(this);
	ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	connect(ui->table, &QTableWidget::cellDoubleClicked, this, [&](int row, int column) { if (column == ColumnColor) { edit(row, column); } });
	connect(ui->table, &QTableWidget::cellChanged, this, &SubjectsTab::edit);

	connect(ui->removeButton, &QPushButton::clicked, this, &SubjectsTab::deleteSelected);
	connect(new QShortcut(QKeySequence(QKeySequence::Delete), this), &QShortcut::activated, this, &SubjectsTab::deleteSelected);
}

void SubjectsTab::setData(Subjects* const newSubjects, Teachers* const newTeachers)
{
	if (subjects != nullptr) {
		subjects->disconnect(this);
		ui->addButton->disconnect(this);
	}

	subjects = newSubjects;
	teachers = newTeachers;

	reconstruct();
	connect(subjects, &Subjects::changed, this, &SubjectsTab::updateRow);
	connect(subjects, &Subjects::appended, this, &SubjectsTab::append);
	connect(subjects, &Subjects::removed, this, &SubjectsTab::reconstruct);
	connect(ui->addButton, &QPushButton::clicked, this, [&]() { subjects->append(); });
}

SubjectsTab::~SubjectsTab()
{
	delete ui;
}

void SubjectsTab::reconstruct()
{
	isModificationInProgress = true;

	ui->table->clearContents();
	ui->table->setRowCount(subjects->size());
	for (int row = 0; row < subjects->size(); ++row) {
		updateRow(row);
	}

	isModificationInProgress = false;
}

void SubjectsTab::append()
{
	isModificationInProgress = true;
	ui->table->setRowCount(subjects->size());
	updateRow(subjects->size() - 1);
	isModificationInProgress = false;
}

void SubjectsTab::deleteSelected() const
{
	auto selectedIndexes = ui->table->selectionModel()->selectedIndexes();

	QVector<int> rowsToDelete;
	for (auto const &selectedIndex: selectedIndexes)
	{
		if (!rowsToDelete.contains(selectedIndex.row())) {
			rowsToDelete << selectedIndex.row();
		}
	}

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

			for (auto const &teacher: teachersOfSubject) {
				teachers->remove(teachers->indexOf(teacher));
			}
		}

		subjects->remove(rowToDelete);
	}
}

void SubjectsTab::edit(int row, int column)
{
	if (isModificationInProgress) {
		return;
	}

	isModificationInProgress = true;
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
	isModificationInProgress = false;
}

void SubjectsTab::editColor(int row) const
{
	auto subject = subjects->at(row);
	auto color = QColorDialog::getColor(subject->getColor(), ui->table);
	if (color.isValid()) {
		subject->setColor(color);
	}
}

void SubjectsTab::editName(int row) const
{
	auto item = ui->table->item(row, ColumnName);
	QString name = item->data(Qt::DisplayRole).toString();

	if (name == subjects->at(row)->getName()) {
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

		item->setData(Qt::DisplayRole, subjects->at(row)->getName());
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

		item->setData(Qt::DisplayRole, subjects->at(row)->getName());
		return;
	}

	subjects->at(row)->setName(name);
}

void SubjectsTab::editShortName(int row) const
{
	auto item = ui->table->item(row, ColumnShortName);
	QString shortName = item->data(Qt::DisplayRole).toString();

	if (shortName == subjects->at(row)->getShortName()) {
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

		item->setData(Qt::DisplayRole, subjects->at(row)->getShortName());
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

		item->setData(Qt::DisplayRole, subjects->at(row)->getShortName());
		return;
	}

	subjects->at(row)->setShortName(shortName);
}

void SubjectsTab::editFrequency(int row) const
{
	auto item = ui->table->item(row, ColumnFrequency);
	int frequency = item->data(Qt::DisplayRole).toInt();

	if (frequency <= 0)
	{
		item->setData(Qt::DisplayRole, subjects->at(row)->getFrequency());
		return;
	}

	subjects->at(row)->setFrequency(frequency);
}

void SubjectsTab::updateRow(int row) const
{
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
}
