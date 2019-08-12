#include "ComputationTab.h"
#include "ui_ComputationTab.h"

#include <QFileDialog>
#include <QDesktopServices>
#include <QtConcurrent>
#include "ExcelExporter.h"
#include "Options.h"
#include "OptionsVariations.h"
#include "Solver.h"
#include "Subject.h"
#include "Subjects.h"
#include "Teachers.h"

QIcon const ComputationTab::inProgressIcon("../../image/cogs.svg");
QIcon const ComputationTab::successIcon("../../image/check.svg");
QIcon const ComputationTab::errorIcon("../../image/error.svg");

ComputationTab::ComputationTab(QWidget *parent) :
	QWidget(parent),
	options(nullptr),
	solver(nullptr),
	subjects(nullptr),
	teachers(nullptr),
	ui(new Ui::ComputationTab)
{
	ui->setupUi(this);
	ui->stopButton->hide();

	connect(ui->startButton, &QPushButton::clicked, this, &ComputationTab::start);
	connect(ui->exportButton, &QPushButton::clicked, this, &ComputationTab::exportResult);
}

void ComputationTab::setData(Options *const newOptions, Solver *const newSolver, Subjects *const newSubjects, Teachers *const newTeachers)
{
	if (options != nullptr) {
		options->disconnect(this);
	}

	if (solver != nullptr) {
		ui->stopButton->disconnect(solver);
		solver->disconnect(this);
	}

	options = newOptions;
	solver = newSolver;
	subjects = newSubjects;
	teachers = newTeachers;

	reconstruct();
	connect(options, &Options::moved, this, [&](int from, int to) {
		auto item = ui->optionsTree->takeTopLevelItem(from);
		ui->optionsTree->insertTopLevelItem(to, item);
		item->setExpanded(true);
	});
	connect(ui->stopButton, &QPushButton::clicked, solver, &Solver::stopComputation);
	connect(solver, &Solver::optionFreezed, this, &ComputationTab::updateIcons);
	connect(solver, &Solver::finished, this, &ComputationTab::onFinished);
}

ComputationTab::~ComputationTab()
{
	solver->stopComputation();
	computationWatcher.waitForFinished();

	delete ui;
}

void ComputationTab::reconstruct()
{
	ui->optionsTree->clear();
	for (auto const &option: *options)
	{
		auto item = new QTreeWidgetItem(ui->optionsTree);
		item->setText(0, Options::optionNames[option]);
		item->setExpanded(true);

		if (option == Option::SameTeacherOnlyOnceInCycle || option == Option::SameTeacherAndTimeslotOnlyOnceInCycle)
		{
			for (auto const &subject: *subjects)
			{
				auto subItem = new QTreeWidgetItem(item);
				subItem->setText(0, subject->getName());
			}
		}
	}
}

void ComputationTab::setInProgressIcons()
{
	auto blankIcon = QPixmap(100, 100);
	blankIcon.fill();

	for (int idOption = 0; idOption < options->size(); ++idOption)
	{
		auto item = ui->optionsTree->topLevelItem(idOption);
		if (!item->childCount()) {
			item->setIcon(0, inProgressIcon);
			continue;
		}

		item->setIcon(0, blankIcon);

		for (int subOption = 0; subOption < item->childCount(); ++subOption) {
			item->child(subOption)->setIcon(0, inProgressIcon);
		}
	}
}

void ComputationTab::updateIcons()
{
	auto optionsVariations = solver->getOptionsVariations();
	auto freezedOption = optionsVariations->getFreezedOption();
	for (int idOption = 0; idOption <= options->indexOf(freezedOption.option); ++idOption)
	{
		auto item = ui->optionsTree->topLevelItem(idOption);
		auto option = options->at(idOption);

		if (!item->childCount()) {
			item->setIcon(0, optionsVariations->shouldEnforce(option) ? successIcon : errorIcon);
			continue;
		}

		int firstSubOption = option == freezedOption.option ? freezedOption.subOption : 0;
		for (int subOption = firstSubOption; subOption < item->childCount(); ++subOption) {
			item->child(subOption)->setIcon(0, optionsVariations->shouldEnforce(option, subOption) ? successIcon : errorIcon);
		}
	}
}

void ComputationTab::start()
{
	setInProgressIcons();
	ui->startButton->hide();
	ui->stopButton->show();
	ui->exportButton->setDisabled(true);
	ui->progressBar->setMaximum(0);

	computationWatcher.setFuture(QtConcurrent::run([&]() { solver->compute(ui->numberOfGroupsSpinBox->value(), ui->numberOfWeeksSpinBox->value()); }));
}

void ComputationTab::onFinished(bool success)
{
	updateIcons();
	ui->stopButton->hide();
	ui->startButton->show();
	ui->progressBar->setMaximum(100);

	if (success) {
		solver->print(ui->table);
		ui->exportButton->setDisabled(false);
	}
}

void ComputationTab::exportResult()
{
	QString filePath = QFileDialog::getSaveFileName(
		this,
		tr("Exporter"),
		"",
		tr("Classeur Microsoft Excel (*.xlsx)")
	);

	if (filePath.isEmpty()) {
		return;
	}

	ExcelExporter excelExporter(subjects, teachers, solver);
	excelExporter.save(filePath);

	QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}
