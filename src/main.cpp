#include <QLocale>
#include <QCoreApplication>
#include <QTranslator>
#include <QWebChannel>
#include <QWebSocketServer>
#include "misc.h"
#include "Communication.h"
#include "JsonImporter.h"
#include "Solver.h"
#include "WebSocketTransport.h"
#include "Objective/MinimalNumberOfSlotsObjective.h"
#include "Objective/NoConsecutiveCollesObjective.h"
#include "Objective/OnlyOneCollePerDayObjective.h"

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

	initStdout();

	QWebSocketServer server(QCoreApplication::tr("Serveur KhôlGen"), QWebSocketServer::NonSecureMode);
	int const port = 4201;
	if (!server.listen(QHostAddress::LocalHost, port)) {
		qStdout() << QCoreApplication::tr("Impossible d'ouvrir le serveur WebSocket sur le port %1.").arg(port) << Qt::endl;
		return a.exec();
	}

	/* @todo Only accepts a single connection */
	QWebChannel channel;
	QObject::connect(&server, &QWebSocketServer::newConnection, [&]() {
		channel.connectTo(new WebSocketTransport(server.nextPendingConnection()));
	});

	const MinimalNumberOfSlotsObjective minimalNumberOfSlotsObjective;
	const NoConsecutiveCollesObjective noConsecutiveCollesObjective;
	const OnlyOneCollePerDayObjective onlyOneCollePerDayObjective;

	std::vector<Objective const *> objectives = {
		&minimalNumberOfSlotsObjective,
		&noConsecutiveCollesObjective,
		&onlyOneCollePerDayObjective,
	};

	Solver solver;
	JsonImporter importer(objectives);

	Communication communication(&importer, &solver);
	channel.registerObject("communication", &communication);

	return a.exec();
}
