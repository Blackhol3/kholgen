#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QMessageBox>
#include <QVector>
#include "Subject.h"
#include "Teacher.h"

template<typename T>
QSet<T> extract(QVector<T> const &list, QVector<int> const &indexes)
{
	QSet<T> extractedList;
	for (auto index: indexes) {
		extractedList << list[index];
	}

	return extractedList;
}

MainWindow::MainWindow() : QMainWindow(), solver(&subjects, &teachers, &groups, &options), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	//setDefaultData();

	ui->subjectsTab->setUndoStack(&undoStack);
	ui->teachersTab->setUndoStack(&undoStack);
	ui->groupsTab->setUndoStack(&undoStack);
	ui->optionsTab->setUndoStack(&undoStack);
	ui->computationTab->setUndoStack(&undoStack);

	ui->subjectsTab->setData(&groups, &subjects, &teachers);
	ui->teachersTab->setData(&subjects, &teachers);
	ui->groupsTab->setData(&groups, &subjects);
	ui->optionsTab->setData(&options);
	ui->computationTab->setData(&groups, &options, &solver, &subjects, &teachers);

	auto undoAction = undoStack.createUndoAction(this, tr("&Annuler"));
	undoAction->setShortcuts(QKeySequence::Undo);

	auto redoAction = undoStack.createRedoAction(this, tr("&Rétablir"));
	redoAction->setShortcuts(QKeySequence::Redo);

	auto editMenu = menuBar()->addMenu(tr("&Édition"));
	editMenu->addAction(undoAction);
	editMenu->addAction(redoAction);

	auto aboutAction = new QAction(tr("À &propos de %1").arg(QApplication::applicationName()), this);
	connect(aboutAction, &QAction::triggered, this, [&]() {
		QMessageBox::about(this, tr("À propos de %1").arg(QApplication::applicationName()), tr(
			"<h1>%1</h1>"
			"<h3>v%2</h3>"
			"Ce logiciel est développée par %3 et distribué sous la licence MIT. "
			"Il utilise ou inclut les composants listés ci-dessous, chacun d'eux étant distribué "
			"sous sa licence propre. Pour plus d'informations, vous êtes invités à consulter le "
			"fichier <code>LICENCE.md</code> distribué avec ce logiciel."
			"<blockquote>%4</blockquote>"
		).arg(
			QApplication::applicationName(),
			QApplication::applicationVersion(),
			"Amaury Dalla Monta",
			"OR-Tools, Qt, Eye of Ra, small-n-flat, xlnt, Google Test"
		));
	});

	auto helpMenu = menuBar()->addMenu(tr("&?"));
	helpMenu->addAction(aboutAction);
}

MainWindow::~MainWindow()
{
	delete ui->tabWidget;
	delete ui;
}

void MainWindow::setDefaultData()
{
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
}
