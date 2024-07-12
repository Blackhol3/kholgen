#include <QLocale>
#include <QCoreApplication>
#include <QTranslator>
#include <QWebChannel>
#include <QWebSocketServer>
#include "misc.h"
#include "Communication.h"
#include "Solver.h"
#include "State.h"
#include "WebSocketTransport.h"
#include "Objective/EvenDistributionBetweenTeachersObjective.h"
#include "Objective/MinimalNumberOfSlotsObjective.h"
#include "Objective/NoConsecutiveCollesObjective.h"
#include "Objective/OnlyOneCollePerDayObjective.h"
#include "Objective/SameSlotOnlyOnceInCycleObjective.h"

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

	const EvenDistributionBetweenTeachersObjective evenDistributionBetweenTeachersObjective;
	const MinimalNumberOfSlotsObjective minimalNumberOfSlotsObjective;
	const NoConsecutiveCollesObjective noConsecutiveCollesObjective;
	const OnlyOneCollePerDayObjective onlyOneCollePerDayObjective;
	const SameSlotOnlyOnceInCycleObjective sameSlotOnlyOnceInCycleObjective;

	std::vector<Objective const *> objectives = {
		&evenDistributionBetweenTeachersObjective,
		&minimalNumberOfSlotsObjective,
		&noConsecutiveCollesObjective,
		&onlyOneCollePerDayObjective,
		&sameSlotOnlyOnceInCycleObjective,
	};

	State state(objectives);
	Solver solver(state);

    Communication communication(state, solver);
	channel.registerObject("communication", &communication);
	preventSleepMode(true);

	return a.exec();
}
