#include "GroupsTab.h"
#include "ui_GroupsTab.h"

#include <QInputDialog>
#include <QPainter>
#include <QShortcut>
#include <QStyledItemDelegate>
#include "Group.h"
#include "Groups.h"
#include "Subject.h"
#include "Subjects.h"
#include "UndoCommand.h"

class StyledItemDelegate: public QStyledItemDelegate
{
	public:
		StyledItemDelegate(QObject *parent = nullptr): QStyledItemDelegate(parent) {}

		void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
		{
			auto unselectedOption = option;
			unselectedOption.state.setFlag(QStyle::State_Selected, false);

			if (option.state.testFlag(QStyle::State_Selected)) {
				QColor color(Qt::darkBlue);
				color.setAlpha(20);
				painter->fillRect(option.rect, color);
			}

			QStyledItemDelegate::paint(painter, unselectedOption, index);
		}
};

GroupsTab::GroupsTab(QWidget *parent) :
	Tab(parent),
	groups(nullptr),
	subjects(nullptr),
	ui(new Ui::GroupsTab)
{
	ui->setupUi(this);
	ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->table->setItemDelegate(new StyledItemDelegate(ui->table));

	connect(ui->addButton, &QPushButton::clicked, this, &GroupsTab::append);
	connect(ui->removeButton, &QPushButton::clicked, this, &GroupsTab::deleteSelected);
	connect(new QShortcut(QKeySequence(QKeySequence::Delete), this), &QShortcut::activated, this, &GroupsTab::deleteSelected);

	connect(ui->table, &QTableWidget::cellDoubleClicked, this, &GroupsTab::toggleSubject);
	connect(new QShortcut(QKeySequence(Qt::Key_Enter), this), &QShortcut::activated, this, &GroupsTab::toggleSelected);
	connect(new QShortcut(QKeySequence(Qt::Key_Return), this), &QShortcut::activated, this, &GroupsTab::toggleSelected);
}

void GroupsTab::setData(Groups *const newGroups, Subjects const*const newSubjects)
{
	if (groups != nullptr) {
		groups->disconnect(this);
	}

	if (subjects != nullptr) {
		subjects->disconnect(this);
	}

	groups = newGroups;
	subjects = newSubjects;

	reconstruct();
	connect(groups, &Groups::inserted, this, &GroupsTab::reconstruct);
	connect(groups, &Groups::changed, this, &GroupsTab::updateRow);
	connect(groups, &Groups::removed, this, &GroupsTab::reconstruct);

	connect(subjects, &Subjects::inserted, this, &GroupsTab::reconstruct);
	connect(subjects, &Subjects::changed, this, &GroupsTab::reconstruct);
	connect(subjects, &Subjects::removed, this, &GroupsTab::reconstruct);
}

GroupsTab::~GroupsTab()
{
	delete ui;
}

void GroupsTab::reconstruct()
{
	ui->table->clear();
	ui->table->setRowCount(groups->size());
	ui->table->setColumnCount(subjects->size());

	QStringList horizontalHeaderLabels;
	for (auto const &subject: *subjects) {
		horizontalHeaderLabels << subject->getName();
	}
	ui->table->setHorizontalHeaderLabels(horizontalHeaderLabels);

	for (int row = 0; row < groups->size(); ++row) {
		updateRow(row);
	}
}

void GroupsTab::updateRow(int row)
{
	auto group = groups->at(row);
	for (int column = 0; column < subjects->size(); ++column)
	{
		auto itemAlreadyDefined = ui->table->item(row, column) != nullptr;
		auto subject = subjects->at(column);
		auto item = itemAlreadyDefined ? ui->table->item(row, column) : new QTableWidgetItem();

		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		item->setBackground(group->hasSubject(subject) ? QBrush(subject->getColor(), Qt::BDiagPattern) : QBrush());

		if (!itemAlreadyDefined) {
			ui->table->setItem(row, column, item);
		}
	}
}

void GroupsTab::append()
{
	bool ok;
	int numberOfGroupsToAppend = QInputDialog::getInt(
		this,
		tr("Ajouter un groupe"),
		tr("Veuillez indiquer le nombre de groupes que vous souhaitez ajouter :"),
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
	for (int idNewGroup = 0; idNewGroup < numberOfGroupsToAppend; ++idNewGroup)
	{
		auto command = new UndoCommand(
			[=]() { groups->append(new Group(allSubjects)); },
			[=]() { groups->remove(groups->size() - 1); }
		);
		addUndoCommand(command);
	}
	endUndoMacro();
}

void GroupsTab::toggleSubject(int row, int column)
{
	auto subject = subjects->at(column);
	auto command = new UndoCommand(
		[=]() { groups->at(row)->toggleSubject(subject); },
		[=]() { groups->at(row)->toggleSubject(subject); }
	);
	addUndoCommand(command);
}

void GroupsTab::toggleSelected()
{
	auto selectedIndexes = ui->table->selectionModel()->selectedIndexes();
	for (auto const &selectedIndex: selectedIndexes) {
		toggleSubject(selectedIndex.row(), selectedIndex.column());
	}
}

void GroupsTab::deleteSelected()
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
		auto group = groups->at(rowToDelete);
		auto command = new UndoCommand(
			[=]() { groups->remove(rowToDelete); },
			[=]() { groups->insert(rowToDelete, group); }
		);
		addUndoCommand(command);
	}
	endUndoMacro();
}
