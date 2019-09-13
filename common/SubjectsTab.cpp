#include "SubjectsTab.h"
#include "ui_SubjectsTab.h"

#include <QColorDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QRandomGenerator>
#include <QShortcut>
#include <QStandardItemModel>
#include <QUndoStack>
#include "Trio.h"
#include "Trios.h"
#include "Subject.h"
#include "Subjects.h"
#include "Teacher.h"
#include "Teachers.h"
#include "TransparentItemDelegate.h"
#include "UndoCommand.h"

SubjectsTab::SubjectsTab(QWidget *parent) :
	Tab(parent),
	trios(nullptr),
	subjects(nullptr),
	teachers(nullptr),
	ui(new Ui::SubjectsTab)
{
	ui->setupUi(this);
	ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->table->setItemDelegateForColumn(ColumnColor, new TransparentItemDelegate(0, ui->table));
	ui->table->viewport()->installEventFilter(this);

	connect(ui->table, &QTableWidget::cellDoubleClicked, this, [&](int row, int column) { if (column == ColumnColor) { edit(row, column); } });
	connect(ui->table, &QTableWidget::cellChanged, this, &SubjectsTab::edit);

	connect(ui->defaultButton, &QPushButton::clicked, this, &SubjectsTab::useClassSubject);
	connect(ui->addButton, &QPushButton::clicked, this, &SubjectsTab::append);
	connect(ui->removeButton, &QPushButton::clicked, this, &SubjectsTab::deleteSelected);
	connect(new QShortcut(QKeySequence(QKeySequence::Delete), this), &QShortcut::activated, this, &SubjectsTab::deleteSelected);

	setClassesSubjects();
}

void SubjectsTab::setData(Trios* const newTrios, Subjects* const newSubjects, Teachers* const newTeachers)
{
	if (subjects != nullptr) {
		subjects->disconnect(this);
	}

	trios = newTrios;
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
	for (auto const &trio: *trios) {
		trio->addSubject(subject);
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
			messageBox.setText(tr("Un ou plusieurs enseignant·es collant en <em>%1</em> ont été défini·es.").arg(subject->getName()));
			messageBox.setInformativeText(tr("Ils ou elles seront supprimé·es en même temps que cette matière. Voulez-vous continuer malgré tout ?"));
			messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			messageBox.setDefaultButton(QMessageBox::No);
			auto clickedButton = messageBox.exec();

			if (clickedButton == QMessageBox::No) {
				continue;
			}
		}

		deleteSubject(rowToDelete);
	}
	endUndoMacro();
}

void SubjectsTab::deleteSubject(int row)
{
	auto subject = subjects->at(row);
	auto teachersOfSubject = teachers->ofSubject(subject);

	QVector<int> idTeachersOfSubject;
	for (auto const &teacher: teachersOfSubject) {
		idTeachersOfSubject << teachers->indexOf(teacher);
	}

	auto triosWithSubject = trios->withSubject(subject);
	auto command = new UndoCommand(
		[=]() {
			for (int i = teachersOfSubject.size() - 1; i >= 0; --i) {
				teachers->remove(idTeachersOfSubject[i]);
			}
			for (auto const &Trio: triosWithSubject) {
				Trio->removeSubject(subject);
			}
			subjects->remove(row);
		},
		[=]() {
			for (int i = 0; i < teachersOfSubject.size(); ++i) {
				teachers->insert(idTeachersOfSubject[i], teachersOfSubject[i]);
			}
			for (auto const &Trio: triosWithSubject) {
				Trio->addSubject(subject);
			}
			subjects->insert(row, subject);
		}
	);
	addUndoCommand(command);
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
	color->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
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

void SubjectsTab::useClassSubject()
{
	if (subjects->size() > 0)
	{
		QMessageBox messageBox;
		messageBox.setIcon(QMessageBox::Question);
		messageBox.setTextFormat(Qt::RichText);
		messageBox.setText(tr("Une ou plusieurs matières et enseignant·es ont été définies."));
		messageBox.setInformativeText(tr("Ils ou elles seront supprimé·es. Voulez-vous continuer malgré tout ?"));
		messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		messageBox.setDefaultButton(QMessageBox::No);
		auto clickedButton = messageBox.exec();

		if (clickedButton == QMessageBox::No) {
			return;
		}

		beginUndoMacro();
		for (int row = subjects->size() - 1; row >= 0; --row) {
			deleteSubject(row);
		}
	}
	else {
		beginUndoMacro();
	}

	auto selectedYear = ui->classComboBox->currentData(Qt::UserRole).toInt();
	auto selectedClass = ui->classComboBox->currentText();

	QVector<Subject*> classSubjects = (selectedYear == 1 ? firstYearClassesSubjects : secondYearClassesSubjects)[selectedClass];
	auto command = new UndoCommand(
		[=]() {
			for (auto const &subject: classSubjects) {
				subjects->append(subject);
			}
		},
		[=]() {
			for (int row = subjects->size() - 1; row >= 0; --row) {
				subjects->remove(row);
			}
		}
	);
	addUndoCommand(command);
	endUndoMacro();
}

bool SubjectsTab::eventFilter(QObject* object, QEvent* event)
{
	if (object != ui->table->viewport() || event->type() != QEvent::MouseButtonDblClick) {
		return false;
	}

	auto doubleClickEvent = static_cast<QMouseEvent*>(event);
	if (doubleClickEvent->button() != Qt::LeftButton) {
		return false;
	}

	if (ui->table->itemAt(doubleClickEvent->pos()) == nullptr) {
		append();
	}

	return false;
}

void SubjectsTab::setClassesSubjects()
{
	Subject
		mathematics(tr("Mathématiques"), tr("M"), 1, QColor("#B82035")),
		physics(tr("Physique"), tr("φ"), 1, QColor("#E9D844")),
		biology(tr("Biologie"), tr("B"), 1, QColor("#62AC49")),
		engineering(tr("Sciences de l'ingénieur"), tr("SI"), 1, QColor("#95D7E4")),
		english(tr("Anglais"), tr("A"), 1, QColor("#6F0989")),
		geography(tr("Géographie"), tr("G"), 1, QColor("#2B3918")),
		historyAndGeography(tr("Histoire-Géographie"), tr("HG"), 1, QColor("#2B3918")),
		informatics(tr("Informatique"), tr("I"), 1, QColor("#2955A9"));

	firstYearClassesSubjects["MPSI"] << new Subject(mathematics, 1) << new Subject(physics, 2) << new Subject(english, 2);
	firstYearClassesSubjects["PCSI (commun)"] << new Subject(mathematics, 2) << new Subject(physics, 2) << new Subject(engineering, 4) << new Subject(english, 2);
	firstYearClassesSubjects["PCSI (option PC)"] << new Subject(mathematics, 2) << new Subject(physics, 1) << new Subject(english, 2);
	firstYearClassesSubjects["PCSI (option PSI)"] << new Subject(mathematics, 2) << new Subject(physics, 2) << new Subject(engineering, 4) << new Subject(english, 2);
	firstYearClassesSubjects["PTSI"] << new Subject(mathematics, 2) << new Subject(physics, 2) << new Subject(engineering, 2) << new Subject(english, 2);
	firstYearClassesSubjects["TSI"] << new Subject(mathematics, 1) << new Subject(physics, 2) << new Subject(engineering, 2) << new Subject(english, 2);
	firstYearClassesSubjects["TPC"] << new Subject(mathematics, 1) << new Subject(physics, 2) << new Subject(engineering, 2) << new Subject(english, 2);
	firstYearClassesSubjects["BCPST"] << new Subject(mathematics, 2) << new Subject(physics, 2) << new Subject(biology, 2) << new Subject(english, 4) << new Subject(informatics, 4);
	firstYearClassesSubjects["MT"] << new Subject(mathematics, 1) << new Subject(physics, 2) << new Subject(english, 2);

	secondYearClassesSubjects["MP"] << new Subject(mathematics, 1) << new Subject(physics, 2) << new Subject(english, 2);
	secondYearClassesSubjects["PC"] << new Subject(mathematics, 2) << new Subject(physics, 1) << new Subject(english, 2);
	secondYearClassesSubjects["PSI"] << new Subject(mathematics, 2) << new Subject(physics, 2) << new Subject(engineering, 4) << new Subject(english, 2);
	secondYearClassesSubjects["PT"] << new Subject(mathematics, 2) << new Subject(physics, 2) << new Subject(engineering, 2) << new Subject(english, 2);
	secondYearClassesSubjects["TSI"] << new Subject(mathematics, 1) << new Subject(physics, 2) << new Subject(engineering, 2) << new Subject(english, 2);
	secondYearClassesSubjects["TPC"] << new Subject(mathematics, 2) << new Subject(physics, 1) << new Subject(engineering, 2) << new Subject(english, 2);
	secondYearClassesSubjects["BCPST"] << new Subject(mathematics, 2) << new Subject(physics, 2) << new Subject(biology, 2) << new Subject(english, 4) << new Subject(geography, 4) << new Subject(informatics, 4);
	secondYearClassesSubjects["MT"] << new Subject(mathematics, 1) << new Subject(physics, 2) << new Subject(engineering, 4) << new Subject(english, 2);
	secondYearClassesSubjects["TB"] << new Subject(mathematics, 2) << new Subject(physics, 2) << new Subject(biology, 2) << new Subject(engineering, 2) << new Subject(english, 4) << new Subject(historyAndGeography, 4);

	auto classModel = qobject_cast<QStandardItemModel*>(ui->classComboBox->model());

	auto firstYearItem = new QStandardItem(tr("Première année"));
	firstYearItem->setEnabled(false);
	classModel->appendRow(firstYearItem);

	for (auto it = firstYearClassesSubjects.cbegin(); it != firstYearClassesSubjects.cend(); ++it) {
		auto classItem = new QStandardItem(it.key());
		classItem->setData(1, Qt::UserRole);
		classModel->appendRow(classItem);
	}

	auto secondYearItem = new QStandardItem(tr("Deuxième année"));
	secondYearItem->setEnabled(false);
	classModel->appendRow(secondYearItem);

	for (auto it = secondYearClassesSubjects.cbegin(); it != secondYearClassesSubjects.cend(); ++it) {
		auto classItem = new QStandardItem(it.key());
		classItem->setData(2, Qt::UserRole);
		classModel->appendRow(classItem);
	}

	ui->classComboBox->setCurrentIndex(1);
}
