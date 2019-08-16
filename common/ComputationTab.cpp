#include "ComputationTab.h"
#include "ui_ComputationTab.h"

#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QtConcurrent>
#include "CsvExporter.h"
#include "ExcelExporter.h"
#include "Groups.h"
#include "Options.h"
#include "OptionsVariations.h"
#include "Solver.h"
#include "Subject.h"
#include "Subjects.h"
#include "Teacher.h"
#include "Teachers.h"
#include "UndoCommand.h"

ComputationTab::ComputationTab(QWidget *parent) :
	Tab(parent),

	groups(nullptr),
	options(nullptr),
	solver(nullptr),
	subjects(nullptr),
	teachers(nullptr),

	inProgressIcon(":/image/cogs.svg"),
	successIcon(":/image/check.svg"),
	errorIcon(":/image/error.svg"),

	ui(new Ui::ComputationTab)
{
	ui->setupUi(this);
	ui->stopButton->hide();

	connect(ui->startButton, &QPushButton::clicked, this, &ComputationTab::start);
	connect(ui->exportButton, &QPushButton::clicked, this, &ComputationTab::exportResult);
	connect(ui->numberOfWeeksSpinBox, &QSpinBox::editingFinished, this, &ComputationTab::updateNumberOfWeeks);

	numberOfWeeks = ui->numberOfWeeksSpinBox->value();
}

void ComputationTab::setData(
	Groups const* const newGroups,
	const Options *const newOptions,
	Solver* const newSolver,
	Subjects const* const newSubjects,
	Teachers const* const newTeachers
)
{
	if (options != nullptr) {
		options->disconnect(this);
	}

	if (solver != nullptr) {
		ui->stopButton->disconnect(solver);
		solver->disconnect(this);
	}

	if (subjects != nullptr) {
		subjects->disconnect(this);
	}

	groups = newGroups;
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
	connect(solver, &Solver::started, this, &ComputationTab::updateIcons);
	connect(solver, &Solver::optionFreezed, this, &ComputationTab::updateIcons);
	connect(solver, &Solver::finished, this, &ComputationTab::onFinished);

	connect(subjects, &Subjects::inserted, this, &ComputationTab::reconstruct);
	connect(subjects, &Subjects::changed, this, &ComputationTab::reconstruct);
	connect(subjects, &Subjects::removed, this, &ComputationTab::reconstruct);
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
		item->setText(0, option.getName());
		item->setExpanded(true);

		if (option.isDefinedBySubject())
		{
			for (auto const &subject: *subjects)
			{
				auto subItem = new QTreeWidgetItem(item);
				subItem->setText(0, subject->getName());
			}
		}
	}
}

void ComputationTab::updateNumberOfWeeks()
{
	auto const currentNumberOfWeeks = numberOfWeeks;
	auto const newNumberOfWeeks = ui->numberOfWeeksSpinBox->value();

	if (currentNumberOfWeeks == newNumberOfWeeks) {
		return;
	}

	auto command = new UndoCommand(
		[=]() {
			numberOfWeeks = newNumberOfWeeks;
			ui->numberOfWeeksSpinBox->setValue(numberOfWeeks);
		},
		[=]() {
			numberOfWeeks = currentNumberOfWeeks;
			ui->numberOfWeeksSpinBox->setValue(numberOfWeeks);
		}
	);
	addUndoCommand(command);
}

void ComputationTab::updateIcons()
{
	for (int idOption = 0; idOption < options->size(); ++idOption)
	{
		auto item = ui->optionsTree->topLevelItem(idOption);
		auto option = options->at(idOption);

		if (!item->childCount()) {
			item->setIcon(0, getIcon(option));
			continue;
		}

		for (int subOption = 0; subOption < item->childCount(); ++subOption) {
			item->child(subOption)->setIcon(0, getIcon(option, subOption));
		}
	}
}

const QIcon &ComputationTab::getIcon(Option option, int subOption) const
{
	auto optionsVariations = solver->getOptionsVariations();
	if (optionsVariations->isOptionFreezed(option, subOption)) {
		return optionsVariations->shouldEnforce(option, subOption) ? successIcon : errorIcon;
	}

	return computationWatcher.isRunning() ? inProgressIcon : errorIcon;
}

void ComputationTab::start()
{
	ui->startButton->hide();
	ui->stopButton->show();
	ui->progressBar->setMaximum(0);

	computationWatcher.setFuture(QtConcurrent::run([&]() { solver->compute(numberOfWeeks); }));
}

void ComputationTab::onFinished(bool success)
{
	ui->stopButton->hide();
	ui->startButton->show();
	ui->progressBar->setMaximum(100);

	if (success)
	{
		updateIcons();
		printTable();
		ui->exportButton->setDisabled(solver->getColles().empty());
	}
	else
	{
		reconstruct();

		QMessageBox messageBox;
		messageBox.setIcon(QMessageBox::Critical);
		messageBox.setText(tr("Aucune solution n'a été trouvée."));
		messageBox.setInformativeText(tr("Si vous avez interrompu volontairement le calcul, relancez-le puis laissez-le se terminer. Sinon, ajouter des enseignant·es ou modifier leurs disponibilités, puis relancer un calcul."));
		messageBox.setStandardButtons(QMessageBox::Ok);
		messageBox.setDefaultButton(QMessageBox::Ok);
		messageBox.exec();
	}
}

void ComputationTab::printTable()
{
	auto colles = solver->getColles();
	int nbWeeks = 0;

	QVector<Slot> creneaux;
	for (auto const &colle: colles)
	{
		Slot slot(colle.getTeacher(), colle.getTimeslot());
		if (!creneaux.contains(slot)) {
			creneaux << slot;
		}

		if (nbWeeks < colle.getWeek().getId() + 1) {
			nbWeeks = colle.getWeek().getId() + 1;
		}
	}

	std::sort(creneaux.begin(), creneaux.end(), [&](auto const &a, auto const &b) {
		if (a.getTeacher()->getSubject() != b.getTeacher()->getSubject()) { return subjects->indexOf(a.getTeacher()->getSubject()) < subjects->indexOf(b.getTeacher()->getSubject()); }
		if (a.getTeacher() != b.getTeacher()) { return teachers->indexOf(a.getTeacher()) < teachers->indexOf(b.getTeacher()); }
		return a.getTimeslot() < b.getTimeslot();
	});

	ui->table->clear();
	ui->table->setRowCount(creneaux.size());
	ui->table->setColumnCount(nbWeeks);

	for (int idWeek = 0; idWeek < nbWeeks; ++idWeek)
	{
		auto headerItem = new QTableWidgetItem();
		headerItem->setData(Qt::DisplayRole, idWeek + 1);

		ui->table->setHorizontalHeaderItem(idWeek, headerItem);
		ui->table->setColumnWidth(idWeek, 5);
	}

	for (int idCreneau = 0; idCreneau < creneaux.size(); ++idCreneau)
	{
		auto &creneau = creneaux[idCreneau];

		auto pixmap = QPixmap(100, 100);
		pixmap.fill(creneau.getTeacher()->getSubject()->getColor());

		auto headerItem = new QTableWidgetItem();
		headerItem->setText(QObject::tr("%1, %2 à %3h00").arg(creneau.getTeacher()->getName(), creneau.getTimeslot().getDayName().toLower(), QString::number(creneau.getTimeslot().getHour())));
		headerItem->setIcon(pixmap);
		ui->table->setVerticalHeaderItem(idCreneau, headerItem);
	}

	for (auto const &colle: colles)
	{
		auto idGroup = groups->indexOf(colle.getGroup());
		auto item = new QTableWidgetItem(QString::number(idGroup + 1));
		item->setBackground(QColor::fromHsv(360 * idGroup / groups->size(), 70, 255));
		ui->table->setItem(creneaux.indexOf(Slot(colle.getTeacher(), colle.getTimeslot())), colle.getWeek().getId(), item);
	}
}

void ComputationTab::exportResult()
{
	QStringList filters{
		tr("Classeur Microsoft Excel (*.xlsx)"),
		tr("Fichier CSV (*.csv)"),
	};

	QString filePath = QFileDialog::getSaveFileName(
		this,
		tr("Exporter"),
		QString(),
		filters.join(";;")
	);

	if (filePath.isEmpty()) {
		return;
	}

	Exporter *exporter;
	if (filePath.endsWith(".xlsx")) {
		exporter = new ExcelExporter(subjects, teachers, groups, solver);
	}
	else if (filePath.endsWith(".csv")) {
		exporter = new CsvExporter(subjects, teachers, groups, solver);
	}
	else {
		throw std::runtime_error("File extension not supported.");
	}

	if (exporter->save(filePath))
	{
		QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
		delete exporter;
		return;
	}

	QMessageBox messageBox;
	messageBox.setIcon(QMessageBox::Critical);
	messageBox.setText(tr("Le fichier n'a pas pu être ouvert en écriture."));
	messageBox.setInformativeText(tr("Vérifiez que le fichier n'est pas ouvert dans une autre application, puis réessayez l'exportation."));
	messageBox.setStandardButtons(QMessageBox::Ok);
	messageBox.setDefaultButton(QMessageBox::Ok);
	messageBox.exec();

	exportResult();
}
