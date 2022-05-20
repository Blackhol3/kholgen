#pragma once

#include <QWebChannelAbstractTransport>

class QWebSocket;

class WebSocketTransport : public QWebChannelAbstractTransport
{
	Q_OBJECT
	public:
		explicit WebSocketTransport(QWebSocket *socket);
		virtual ~WebSocketTransport();
		void sendMessage(const QJsonObject &message) override;

	protected slots:
		void textMessageReceived(const QString &message);

	protected:
		QWebSocket *socket;
};

