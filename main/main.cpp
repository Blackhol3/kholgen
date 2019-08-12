#include <QApplication>
#include <QLibraryInfo>
#include <QTranslator>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setApplicationName("KhôlGen");
	QCoreApplication::setApplicationVersion("1.0.0alpha");

	QApplication a(argc, argv);
	a.setWindowIcon(QIcon("../../image/icon.svg"));

	QTranslator translator;
	if (translator.load(QLocale::system(), "qt", "_", QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
		a.installTranslator(&translator);
	}

	QTranslator baseTranslator;
	if (baseTranslator.load("qtbase_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
		a.installTranslator(&baseTranslator);
	}

	MainWindow mainWindow;
	mainWindow.show();

	return a.exec();
}
