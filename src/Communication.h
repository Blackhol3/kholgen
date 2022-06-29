#pragma once

#include <QObject>
#include <QJsonObject>
#include <vector>
#include "Colle.h"
#include "Subject.h"
#include "Teacher.h"
#include "Trio.h"

class Communication : public QObject
{
	Q_OBJECT

	public:
		Communication(QObject *parent = nullptr);
		void sendColles(std::vector<Colle> const &colles) const;
		void sendSettings(std::vector<Subject> const &subjects, std::vector<Teacher> const &teachers, std::vector<Trio> const &trios, int nbWeeks) const;

	signals:
		void sendJsonColles(const QJsonArray &colles) const;
		void sendJsonSettings(const QJsonObject &settings) const;
};
