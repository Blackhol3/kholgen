#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QtConcurrent>
#include <QFileDialog>
#include <QVector>
#include "ExcelExporter.h"

#include <QDesktopServices>

template<typename T>
QSet<T> extract(QVector<T> const &list, QVector<int> const &indexes)
{
	QSet<T> extractedList;
	for (auto index: indexes) {
		extractedList << list[index];
	}

	return extractedList;
}

MainWindow::MainWindow() : QMainWindow(), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->stopButton->hide();

	QVector<Timeslot> timeslots = {
		Timeslot(Day::Monday, 17),
		Timeslot(Day::Monday, 18),
		Timeslot(Day::Tuesday, 12),
		Timeslot(Day::Tuesday, 13),
		Timeslot(Day::Thursday, 12),
		Timeslot(Day::Thursday, 13),
		Timeslot(Day::Thursday, 18),
	};

	subjects
		<< new Subject("Mathématiques", "M", 1, QColor::fromHsv(0, 192, 192))
		<< new Subject("Physique", "φ", 2, QColor::fromHsv(90, 192, 192))
		<< new Subject("Sciences de l'ingénieur", "SI", 2, QColor::fromHsv(180, 192, 192))
		<< new Subject("Anglais", "A", 2, QColor::fromHsv(270, 192, 192))
	;

	teachers
		<< new Teacher("M. Pillet", subjects[0], extract(timeslots, {0, 1, 2, 3, 4, 5, 6}))
		<< new Teacher("M. Biton", subjects[0], extract(timeslots, {2, 3}))
		<< new Teacher("M. Lambour", subjects[0], extract(timeslots, {2, 3}))
		<< new Teacher("Mme Moreau", subjects[1], extract(timeslots, {2, 3, 4, 5, 6}))
		<< new Teacher("M. Berthet", subjects[1], extract(timeslots, {6}))
		<< new Teacher("M. Dalla Monta", subjects[2], extract(timeslots, {3, 4, 5, 6}))
		<< new Teacher("M. Milhaud", subjects[2], extract(timeslots, {0, 1, 4, 5, 6}))
		<< new Teacher("Mme Nouet", subjects[3], extract(timeslots, {0, 1, 2, 3}))
		<< new Teacher("Colleur d'anglais n°1", subjects[3], extract(timeslots, {4}))
		<< new Teacher("Colleur d'anglais n°2", subjects[3], extract(timeslots, {5}))
	;

	ui->subjectsTab->setData(&subjects, &teachers);
	ui->teachersTab->setData(&subjects, &teachers);
	ui->optionsTab->setData(&options);

	connect(&computationWatcher, SIGNAL(finished()), this, SLOT(onComputationFinished()));
}

MainWindow::~MainWindow()
{
	if (solver)
	{
		solver->stopComputation();
		computationWatcher.waitForFinished();
	}

	delete ui;
}

void MainWindow::on_startButton_clicked()
{
	ui->startButton->hide();
	ui->stopButton->show();
	ui->exportButton->setDisabled(true);
	ui->progressBar->setMaximum(0);

	solver = std::make_unique<Solver>(&subjects, &teachers, ui->numberOfGroupsSpinBox->value(), ui->numberOfWeeksSpinBox->value(), &options);
	computationWatcher.setFuture(QtConcurrent::run([&]() { return solver->compute(); }));
}

void MainWindow::on_stopButton_clicked()
{
	solver->stopComputation();
}

void MainWindow::on_exportButton_clicked()
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

	ExcelExporter excelExporter(&subjects, &teachers, solver.get());
	excelExporter.save(filePath);

	QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}

void MainWindow::onComputationFinished()
{
	ui->stopButton->hide();
	ui->startButton->show();
	ui->progressBar->setMaximum(100);

	if (computationWatcher.result()) {
		solver->print(ui->table);
		ui->exportButton->setDisabled(false);
	}
}
