#include "TriosTab.h"
#include "ui_TriosTab.h"

#include <QInputDialog>
#include <QMouseEvent>
#include <QPainter>
#include <QShortcut>
#include "Subject.h"
#include "Subjects.h"
#include "TransparentItemDelegate.h"
#include "Trio.h"
#include "Trios.h"
#include "UndoCommand.h"

TriosTab::TriosTab(QWidget *parent) :
	Tab(parent),
	trios(nullptr),
	subjects(nullptr),
	ui(new Ui::TriosTab)
{
	ui->setupUi(this);
	ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->table->setItemDelegate(new TransparentItemDelegate(20, ui->table));
	ui->table->viewport()->installEventFilter(this);

	connect(ui->addButton, &QPushButton::clicked, this, &TriosTab::append);
	connect(ui->removeButton, &QPushButton::clicked, this, &TriosTab::deleteSelected);
	connect(new QShortcut(QKeySequence(QKeySequence::Delete), this), &QShortcut::activated, this, &TriosTab::deleteSelected);

	connect(ui->table, &QTableWidget::cellDoubleClicked, this, &TriosTab::toggleSubject);
	connect(new QShortcut(QKeySequence(Qt::Key_Enter), this), &QShortcut::activated, this, &TriosTab::toggleSelected);
	connect(new QShortcut(QKeySequence(Qt::Key_Return), this), &QShortcut::activated, this, &TriosTab::toggleSelected);
}

void TriosTab::setData(Trios *const newTrios, Subjects const*const newSubjects)
{
	if (trios != nullptr) {
		trios->disconnect(this);
	}

	if (subjects != nullptr) {
		subjects->disconnect(this);
	}

	trios = newTrios;
	subjects = newSubjects;

	reconstruct();
	connect(trios, &Trios::inserted, this, &TriosTab::reconstruct);
	connect(trios, &Trios::changed, this, &TriosTab::updateRow);
	connect(trios, &Trios::removed, this, &TriosTab::reconstruct);

	connect(subjects, &Subjects::inserted, this, &TriosTab::reconstruct);
	connect(subjects, &Subjects::changed, this, &TriosTab::reconstruct);
	connect(subjects, &Subjects::removed, this, &TriosTab::reconstruct);
}

TriosTab::~TriosTab()
{
	delete ui;
}

void TriosTab::reconstruct()
{
	ui->table->clear();
	ui->table->setRowCount(trios->size());
	ui->table->setColumnCount(subjects->size());

	QStringList horizontalHeaderLabels;
	for (auto const &subject: *subjects) {
		horizontalHeaderLabels << subject->getName();
	}
	ui->table->setHorizontalHeaderLabels(horizontalHeaderLabels);

	for (int row = 0; row < trios->size(); ++row) {
		updateRow(row);
	}
}

void TriosTab::updateRow(int row)
{
	auto trio = trios->at(row);
	for (int column = 0; column < subjects->size(); ++column)
	{
		auto itemAlreadyDefined = ui->table->item(row, column) != nullptr;
		auto subject = subjects->at(column);
		auto item = itemAlreadyDefined ? ui->table->item(row, column) : new QTableWidgetItem();

		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		item->setBackground(trio->hasSubject(subject) ? QBrush(subject->getColor(), Qt::BDiagPattern) : QBrush());

		if (!itemAlreadyDefined) {
			ui->table->setItem(row, column, item);
		}
	}
}

void TriosTab::append()
{
	bool ok;
	int numberOfTriosToAppend = QInputDialog::getInt(
		this,
		tr("Ajouter un trinôme"),
		tr("Veuillez indiquer le nombre de trinômes que vous souhaitez ajouter :"),
		1, 1, 99, 1,
		&ok
	);

	if (!ok) {
		return;
	}

	QSet<Subject const*> allSubjects;
	for (auto const &subject: *subjects) {
		allSubjects << subject;
	}

	beginUndoMacro();
	for (int idNewTrio = 0; idNewTrio < numberOfTriosToAppend; ++idNewTrio)
	{
		auto command = new UndoCommand(
			[=]() { trios->append(new Trio(allSubjects)); },
			[=]() { trios->remove(trios->size() - 1); }
		);
		addUndoCommand(command);
	}
	endUndoMacro();
}

void TriosTab::toggleSubject(int row, int column)
{
	auto subject = subjects->at(column);
	auto command = new UndoCommand(
		[=]() { trios->at(row)->toggleSubject(subject); },
		[=]() { trios->at(row)->toggleSubject(subject); }
	);
	addUndoCommand(command);
}

void TriosTab::toggleSelected()
{
	auto selectedIndexes = ui->table->selectionModel()->selectedIndexes();
	for (auto const &selectedIndex: selectedIndexes) {
		toggleSubject(selectedIndex.row(), selectedIndex.column());
	}
}

void TriosTab::deleteSelected()
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
		auto trio = trios->at(rowToDelete);
		auto command = new UndoCommand(
			[=]() { trios->remove(rowToDelete); },
			[=]() { trios->insert(rowToDelete, trio); }
		);
		addUndoCommand(command);
	}
	endUndoMacro();
}

bool TriosTab::eventFilter(QObject* object, QEvent* event)
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
