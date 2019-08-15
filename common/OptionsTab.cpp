#include "OptionsTab.h"
#include "ui_OptionsTab.h"

#include <QMessageBox>
#include "Options.h"
#include "UndoCommand.h"

OptionsTab::OptionsTab(QWidget *parent) :
	Tab(parent),
	options(nullptr),
	ui(new Ui::OptionsTab)
{
	ui->setupUi(this);

	connect(ui->list->model(), &QAbstractItemModel::rowsMoved, [&](auto, int source, auto, auto, int destination) {
		ui->list->blockSignals(true);
		move(source, destination > source ? destination - 1 : destination);
		ui->list->blockSignals(false);
	});
}

void OptionsTab::setData(Options *const newOptions)
{
	if (options != nullptr) {
		options->disconnect(this);
	}

	options = newOptions;

	reconstruct();
	connect(options, &Options::moved, this, &OptionsTab::reconstruct);
}

OptionsTab::~OptionsTab()
{
	delete ui;
}

void OptionsTab::reconstruct()
{
	ui->list->clear();
	for (auto const &option: *options)
	{
		auto item = new QListWidgetItem(ui->list);
		item->setText(option.getName());
	}

	/** @link https://stackoverflow.com/a/27830068 **/
	ui->list->setMinimumWidth(ui->list->sizeHintForColumn(0) + 2 * ui->list->frameWidth());
}

void OptionsTab::move(int from, int to)
{
	bool isMoveSuccessful = options->move(from, to);
	if (!isMoveSuccessful)
	{
		auto const errorPair = options->getLastErrorPair();
		auto const message = tr("La contrainte <em>« %1 »</em> ne peut logiquement pas être plus importante que la contrainte <em>« %2 »</em>").arg(errorPair.first.getName(), errorPair.second.getName());

		QMessageBox messageBox;
		messageBox.setIcon(QMessageBox::Critical);
		messageBox.setText(message);
		messageBox.setStandardButtons(QMessageBox::Ok);
		messageBox.setDefaultButton(QMessageBox::Ok);
		messageBox.exec();

		reconstruct();
		return;
	}

	options->move(to, from);
	auto command = new UndoCommand(
		[=]() { options->move(from, to); },
		[=]() { options->move(to, from); }
	);
	addUndoCommand(command);
}
