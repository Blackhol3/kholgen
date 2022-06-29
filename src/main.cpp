#include <QLocale>
#include <QCoreApplication>
#include <QDebug>
#include <QFutureWatcher>
#include <QTimer>
#include <QTranslator>
#include <QWebChannel>
#include <QWebSocketServer>
#include <QtConcurrent>
#include <vector>
#include "misc.h"
#include "Communication.h"
#include "JsonImporter.h"
#include "Solver.h"
#include "Subject.h"
#include "Teacher.h"
#include "Timeslot.h"
#include "Trio.h"
#include "WebSocketTransport.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	QCoreApplication::setApplicationName("KhôlGen");
	QCoreApplication::setApplicationVersion("2.0.0-dev");

	QTranslator translator;
	const QStringList uiLanguages = QLocale::system().uiLanguages();
	for (const QString &locale : uiLanguages) {
		const QString baseName = "KholGen_" + QLocale(locale).name();
		if (translator.load(":/i18n/" + baseName)) {
			a.installTranslator(&translator);
			break;
		}
	}

	QWebSocketServer server(QCoreApplication::tr("Serveur KhôlGen"), QWebSocketServer::NonSecureMode);
	int const port = 4201;
	if (!server.listen(QHostAddress::LocalHost, port)) {
		qStdout() << QCoreApplication::tr("Impossible d'ouvrir le serveur WebSocket sur le port %1.").arg(port) << Qt::endl;
		return a.exec();
	}

	QWebChannel channel;
	QObject::connect(&server, &QWebSocketServer::newConnection, [&]() {
		channel.connectTo(new WebSocketTransport(server.nextPendingConnection()));
	});

	Communication communication;
	channel.registerObject("communication", &communication);

	QEventLoop loop;
	QTimer::singleShot(3000, &loop, &QEventLoop::quit);
	loop.exec();

	initStdout();

	JsonImporter importer;
	if (!importer.open("../test0.json")) {
		qStdout() << importer.getErrorString() << Qt::endl;
		return a.exec();
	}

	communication.sendSettings(importer.getSubjects(), importer.getTeachers(), importer.getTrios(), 20);

	Solver solver(importer.getSubjects(), importer.getTeachers(), importer.getTrios(), 20);
	QObject::connect(&solver, &Solver::solutionFound, &communication, &Communication::sendColles);

	QFutureWatcher<void> watcher;
	watcher.setFuture(QtConcurrent::run([&]() { solver.compute(); }));

	return a.exec();
}
