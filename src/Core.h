#pragma once

#include <QObject>
#include "misc.h"

class Core : public QObject
{
	Q_OBJECT

	public:
		Core(QObject *parent = nullptr)
			: QObject(parent)
		{

		}

	signals:
		/*
			This signal is emitted from the C++ side and the text displayed on the HTML client side.
		*/
		void sendText(const QString &text);

	public slots:
		/*
			This slot is invoked from the HTML client side and the text displayed on the server side.
		*/
		void receiveText(const QString &text)
		{
			qStdout() << text << Qt::endl;
		}
};
