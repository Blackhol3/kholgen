#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QVector>
#include "JsonImporter.h"
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

MainWindow::MainWindow() : QMainWindow(), solver(&subjects, &teachers, &trios, &options), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	ui->subjectsTab->setUndoStack(&undoStack);
	ui->teachersTab->setUndoStack(&undoStack);
	ui->triosTab->setUndoStack(&undoStack);
	ui->optionsTab->setUndoStack(&undoStack);
	ui->computationTab->setUndoStack(&undoStack);

	ui->subjectsTab->setData(&trios, &subjects, &teachers);
	ui->teachersTab->setData(&subjects, &teachers);
	ui->triosTab->setData(&trios, &subjects);
	ui->optionsTab->setData(&options);
	ui->computationTab->setData(&trios, &options, &solver, &subjects, &teachers);

	auto openAction = new QAction(tr("&Ouvrir…"), this);
	openAction->setShortcut(QKeySequence::Open);
	connect(openAction, &QAction::triggered, this, &MainWindow::openFile);

	auto fileMenu = menuBar()->addMenu(tr("&Fichier"));
	fileMenu->addAction(openAction);

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

void MainWindow::openFile()
{
	QString filePath = QFileDialog::getOpenFileName(
		this,
		tr("Ouvrir"),
		QString(),
		tr("Fichier JSON (*.json)")
	);

	if (filePath.isEmpty()) {
		return;
	}

	if (subjects.size() > 0 || teachers.size() > 0 || trios.size() > 0) {
		QMessageBox messageBox;
		messageBox.setIcon(QMessageBox::Question);
		messageBox.setTextFormat(Qt::RichText);
		messageBox.setText(tr("Une ou plusieurs matières, enseignant·es et groupes ont été définies."));
		messageBox.setInformativeText(tr("Ils seront définitivement supprimés. Voulez-vous continuer malgré tout ?"));
		messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		messageBox.setDefaultButton(QMessageBox::No);
		auto clickedButton = messageBox.exec();

		if (clickedButton == QMessageBox::No) {
			return;
		}

		for (int i = trios.size() - 1; i >= 0; --i) {
			trios.remove(i);
		}

		for (int i = teachers.size() - 1; i >= 0; --i) {
			teachers.remove(i);
		}

		for (int i = subjects.size() - 1; i >= 0; --i) {
			subjects.remove(i);
		}

		/** @todo Undo **/
		undoStack.clear();
	}

	auto importer = JsonImporter(subjects, teachers);
	if (importer.open(filePath)) {
		return;
	}

	QMessageBox messageBox;
	messageBox.setIcon(QMessageBox::Critical);
	messageBox.setText(tr("Le fichier n'a pas pu être lu correctement."));
	messageBox.setInformativeText(importer.getErrorString());
	messageBox.setStandardButtons(QMessageBox::Ok);
	messageBox.setDefaultButton(QMessageBox::Ok);
	messageBox.exec();
}
