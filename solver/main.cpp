#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QHttpServer>
#include <QLocale>
#include <QLocalServer>
#include <QLocalSocket>
#include <QMimeDatabase>
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
	#include <shellapi.h>
#endif

void createLocalServer() {
	QLocalSocket socket;
	socket.connectToServer(QCoreApplication::applicationName());
	if (socket.waitForConnected()) {
		qStdout() << QCoreApplication::tr("Une autre instance de l'application est déjà en train de s'exécuter.") << Qt::endl;
		std::exit(EXIT_FAILURE);
	}

	auto const server = new QLocalServer(QCoreApplication::instance());
	server->listen(QCoreApplication::applicationName());
	QObject::connect(server, &QLocalServer::newConnection, [=]() {
		#ifdef Q_OS_WIN
			::SetForegroundWindow(::GetConsoleWindow());
		#endif

		auto const socket = server->nextPendingConnection();
		socket->disconnectFromServer();
		socket->deleteLater();
	});
}

QFile getFileFromUrl(QUrl const &url = QUrl()) {
	auto const &path = url.fileName().contains('.') ? url.path() : "index.html";
	return QFile(QCoreApplication::applicationDirPath() + "/../user-interface/" + path);
}

void createHttpServer(int port) {
	auto const indexFile = getFileFromUrl();
	if (!indexFile.exists()) {
		qStdout() << QCoreApplication::tr("Les fichiers de l'interface graphique n'ont pas été trouvés.") << Qt::endl;
		return;
	}

	auto const server = new QHttpServer(QCoreApplication::instance());
	server->setMissingHandler([] (const QHttpServerRequest &request, QHttpServerResponder &&responder) {
		auto file = getFileFromUrl(request.url());
		if (!file.open(QIODevice::ReadOnly)) {
			return responder.write(QHttpServerResponse::StatusCode::NotFound);
		}

		QFileInfo fileInfo(file);
		QMimeDatabase mimeDatabase;
		QMimeType mimeType = mimeDatabase.mimeTypeForFile(fileInfo);
		return responder.write(file.readAll(), mimeType.name().toUtf8());
	});

	if (!server->listen(QHostAddress::LocalHost, port)) {
		qStdout() << QCoreApplication::tr("Impossible d'ouvrir le serveur HTTP sur le port %1.").arg(port) << Qt::endl;
		server->deleteLater();
		return;
	}

	#ifdef Q_OS_WIN
		ShellExecute(NULL, L"open", QString("http://localhost:%1").arg(port).toStdWString().c_str(), NULL, NULL, SW_SHOW);
	#endif
}

QWebChannel* createWebSocketServer(int port) {
	auto server = new QWebSocketServer(
		QCoreApplication::tr("Serveur %1").arg(QCoreApplication::applicationName()),
		QWebSocketServer::NonSecureMode,
		QCoreApplication::instance()
	);
	if (!server->listen(QHostAddress::LocalHost, port)) {
		qStdout() << QCoreApplication::tr("Impossible d'ouvrir le serveur WebSocket sur le port %1.").arg(port) << Qt::endl;
		std::exit(EXIT_FAILURE);
	}

	/* @todo Only accepts a single connection */
	auto const channel = new QWebChannel(QCoreApplication::instance());
	QObject::connect(server, &QWebSocketServer::newConnection, [=]() {
		channel->connectTo(new WebSocketTransport(server->nextPendingConnection()));
	});

	return channel;
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

	createLocalServer();
	createHttpServer(4200);
	auto const channel = createWebSocketServer(4201);

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
	channel->registerObject("communication", &communication);
	preventSleepMode(true);

	return a.exec();
}
