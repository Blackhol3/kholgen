#include "OptionsTab.h"
#include "ui_OptionsTab.h"

#include <QMessageBox>
#include "Options.h"

OptionsTab::OptionsTab(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::OptionsTab)
{
	ui->setupUi(this);

	connect(ui->list->model(), &QAbstractItemModel::rowsMoved, [&](auto, int source, auto, auto, int destination) {
		move(source, destination);
	});
}

void OptionsTab::setData(Options *const newOptions)
{
	options = newOptions;
	reconstruct();
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
		item->setText(Options::optionNames[option]);
	}

	/** @link https://stackoverflow.com/a/27830068 **/
	ui->list->setMinimumWidth(ui->list->sizeHintForColumn(0) + 2 * ui->list->frameWidth());
}

void OptionsTab::move(int from, int to)
{
	bool isMoveSuccessful = options->move(from, to > from ? to - 1 : to);
	if (!isMoveSuccessful)
	{
		auto const errorPair = options->getLastErrorPair();
		auto const message = tr("La contrainte <em>« %1 »</em> ne peut logiquement pas être plus importante que la contrainte <em>« %2 »</em>").arg(Options::optionNames[errorPair.first], Options::optionNames[errorPair.second]);

		QMessageBox messageBox;
		messageBox.setIcon(QMessageBox::Critical);
		messageBox.setText(message);
		messageBox.setStandardButtons(QMessageBox::Ok);
		messageBox.setDefaultButton(QMessageBox::Ok);
		messageBox.exec();

		reconstruct();
	}
}
