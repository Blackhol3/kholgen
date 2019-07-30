#pragma once

#include <QDialog>
#include <QSet>
#include "Timeslot.h"

namespace Ui {
class TeacherDialog;
}

class Subject;
class Subjects;
class Teacher;
class Teachers;

class TeacherDialog : public QDialog
{
	Q_OBJECT

	public:
		TeacherDialog(Subjects const* const subjects, Teachers const* const teachers, QWidget *parent = nullptr);
		void accept();
		void setTeacher(Teacher* const newTeacher);
		Teacher* createTeacher();
		void updateTeacher();
		~TeacherDialog();

	protected:
		Subjects const* subjects;
		Teachers const* teachers;
		Teacher* teacher;

		void toggleAvailability(int row, int column);
		QSet<Timeslot> getAvailableTimeslots() const;

	private:
		Ui::TeacherDialog *ui;
};

