#include <QCoreApplication>
#include <QLocale>
#include <QLocalServer>
#include <QLocalSocket>
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

#ifdef Q_OS_WIN
	#include <Windows.h>
#endif

bool shouldQuitAlreadyRunningApplication(QCoreApplication *a) {
	auto localSocket = new QLocalSocket(a);
	localSocket->connectToServer(a->applicationName());
	if (localSocket->waitForConnected()) {
		localSocket->disconnectFromServer();
		return true;
	}

	auto localServer = new QLocalServer(a);
	localServer->listen(a->applicationName());
	QObject::connect(localServer, &QLocalServer::newConnection, [=]() {
		#ifdef Q_OS_WIN
			::SetForegroundWindow(::GetConsoleWindow());
		#endif

		localServer->nextPendingConnection()->deleteLater();
	});

	return false;
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	QCoreApplication::setApplicationName(PROJECT_HUMAN_NAME);
	QCoreApplication::setApplicationVersion(PROJECT_VERSION);

	QTranslator translator;
	if (translator.load(QLocale(), PROJECT_NAME, "_", ":/i18n")) {
		a.installTranslator(&translator);
	}

	initStdout();

	if (shouldQuitAlreadyRunningApplication(&a)) {
		qStdout() << QCoreApplication::tr("Une autre instance de l'application est déjà en train de s'exécuter.") << Qt::endl;
		return EXIT_FAILURE;
	}

	QWebSocketServer server(QCoreApplication::tr("Serveur %1").arg(QCoreApplication::applicationName()), QWebSocketServer::NonSecureMode);
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
