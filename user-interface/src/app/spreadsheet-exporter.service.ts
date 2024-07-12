import { Injectable } from '@angular/core';

import Color from 'color';
import { Workbook, type Worksheet } from 'exceljs';

import { type State } from './state';
import { dayNames } from './timeslot';

enum TeachersSectionColumn {
	Subject = 1,
	TeacherCode,
	TeacherName,
	Timeslot,
	FirstColle,
}

enum StudentsSectionColumn {
	Trio = 1,
	FirstColle,
}

@Injectable({
	providedIn: 'root'
})
export class SpreadsheetExporterService {
	protected createWorkbook() {
		const workbook = new Workbook();
		workbook.creator = 'KhôlGen';
		workbook.lastModifiedBy = workbook.creator;

		return workbook;
	}

	protected setTeachersSection(state: State, worksheet: Worksheet, firstRow = 1) {
		const weekRow = firstRow;
		const firstColleRow = weekRow + 1;
		const bottomBorderedRows = [weekRow];

		this.setStandardColumnStyle(worksheet, TeachersSectionColumn.Subject, TeachersSectionColumn.FirstColle + state.weeks.length - 1);

		let row = firstColleRow;
		for (const subject of state.subjects) {
			const subjectStartingRow = row;
			const teachersOfSubject = state.teachers.filter(teacher => teacher.subjectId === subject.id);
			const teachersIdsOfSubject = teachersOfSubject.map(teacher => teacher.id);
			const collesWithSubject = state.colles.filter(colle => teachersIdsOfSubject.includes(colle.teacherId));

			for (const teacher of teachersOfSubject) {
				const teacherStartingRow = row;
				const collesWithTeacher = collesWithSubject.filter(colle => colle.teacherId === teacher.id);

				const groups = [...Map.groupBy(collesWithTeacher, colle => colle.timeslot.toString()).values()].sort((a, b) => a[0].timeslot.compare(b[0].timeslot));
				for (const collesWithTeacherAtTimeslot of groups) {
					worksheet.getCell(row, TeachersSectionColumn.Timeslot).value = collesWithTeacherAtTimeslot[0].timeslot.toReadableString();

					for (const colle of collesWithTeacherAtTimeslot) {
						worksheet.getCell(row, TeachersSectionColumn.FirstColle + colle.weekId).value = colle.trioId + 1;
					}

					++row;
				}

				if (teacherStartingRow === row) { continue; }

				worksheet.mergeCells(teacherStartingRow, TeachersSectionColumn.TeacherCode, row - 1, TeachersSectionColumn.TeacherCode);
				worksheet.getCell(teacherStartingRow, TeachersSectionColumn.TeacherCode).value = `${subject.shortName}${teachersOfSubject.indexOf(teacher) + 1}`;
				
				worksheet.mergeCells(teacherStartingRow, TeachersSectionColumn.TeacherName, row - 1, TeachersSectionColumn.TeacherName);
				worksheet.getCell(teacherStartingRow, TeachersSectionColumn.TeacherName).value = teacher.name;
			}

			if (subjectStartingRow === row) { continue; }

			worksheet.mergeCells(subjectStartingRow, TeachersSectionColumn.Subject, row - 1, TeachersSectionColumn.Subject);
			const cell = worksheet.getCell(subjectStartingRow, TeachersSectionColumn.Subject);
			cell.value = subject.name;
			cell.style.font = { size: 10, color: {argb: Color(subject.color).isDark() ? 'FFFFFFFF' : 'FF000000'} };
			cell.style.fill = { type: 'pattern', pattern: 'solid', fgColor: {argb: `FF${subject.color.slice(1)}`} };

			bottomBorderedRows.push(row - 1);
		}

		worksheet.getColumn(TeachersSectionColumn.Subject).width = state.subjects.map(x => x.name.length).sort((a, b) => b - a)[0];
		worksheet.getColumn(TeachersSectionColumn.TeacherCode).width = 5.5;
		worksheet.getColumn(TeachersSectionColumn.TeacherName).width = state.teachers.map(x => x.name.length).sort((a, b) => b - a)[0];
		worksheet.getColumn(TeachersSectionColumn.Timeslot).width = dayNames.map(x => x.length).sort((a, b) => b - a)[0] + 5;

		this.setWeeksHeader(state, worksheet, TeachersSectionColumn.FirstColle, weekRow, 3.5);

		for (let columnIndex: number = TeachersSectionColumn.Subject; columnIndex < TeachersSectionColumn.FirstColle + state.weeks.length; ++columnIndex) {
			for (const row of bottomBorderedRows) {
				const cell = worksheet.getCell(row, columnIndex);
				cell.style.border = { bottom: { style: 'medium' } };
			}
		}
	}

	protected setStudentsSection(state: State, worksheet: Worksheet, firstRow = 1) {
		const weekRow = firstRow;
		const firstColleRow = weekRow + 1;
		const maximalNumberOfCollesByWeek = this.getMaximalNumberOfCollesByWeek(state);

		this.setStandardColumnStyle(worksheet, TeachersSectionColumn.Subject, TeachersSectionColumn.FirstColle + state.weeks.length - 1);

		let row = firstColleRow;
		for (const trio of state.trios.toSorted((a, b) => a.id - b.id)) {
			const trioStartingRow = row;
			const collesOfTrio = state.colles.filter(colle => colle.trioId === trio.id);

			for (const week of state.weeks) {
				const collesOfTrioInWeek = collesOfTrio.filter(colle => colle.weekId === week.id);

				for (const [j, colle] of collesOfTrioInWeek.entries()) {
					const teacher = state.findId('teachers', colle.teacherId)!;
					const teachersOfSubject = state.teachers.filter(x => x.subjectId === teacher.subjectId);
					const subject = state.findId('subjects', teacher.subjectId)!;
					
					const cell = worksheet.getCell(trioStartingRow + j, StudentsSectionColumn.FirstColle + week.id);
					cell.value = `${subject.shortName}${teachersOfSubject.indexOf(teacher) + 1} ${colle.timeslot.toShortString()}`;
					cell.style.font = { size: 10, color: {argb: Color(subject.color).isDark() ? 'FFFFFFFF' : 'FF000000'} };
					cell.style.fill = { type: 'pattern', pattern: 'solid', fgColor: {argb: `FF${subject.color.slice(1)}`} };
				}
			}

			row += maximalNumberOfCollesByWeek

			worksheet.mergeCells(trioStartingRow, StudentsSectionColumn.Trio, row - 1, StudentsSectionColumn.Trio);
			worksheet.getCell(trioStartingRow, StudentsSectionColumn.Trio).value = `G${trio.id + 1}`;
		}

		this.setWeeksHeader(state, worksheet, StudentsSectionColumn.FirstColle, weekRow, 7);

		for (let columnIndex: number = StudentsSectionColumn.Trio; columnIndex < StudentsSectionColumn.FirstColle + state.weeks.length; ++columnIndex) {
			for (let row = weekRow; row <= worksheet.rowCount; row += maximalNumberOfCollesByWeek) {
				const cell = worksheet.getCell(row, columnIndex);
				cell.style.border = { bottom: { style: 'medium' } };
			}
		}
	}

	protected setStandardColumnStyle(worksheet: Worksheet, firstColumn: number, lastColumn: number) {
		for (let columnIndex = firstColumn; columnIndex <= lastColumn; ++columnIndex) {
			const column = worksheet.getColumn(columnIndex);
			column.style.alignment = { horizontal: 'center', vertical: 'middle'};
			column.style.font = { size: 10 };
		}
	}

	protected setWeeksHeader(state: State, worksheet: Worksheet, firstColumn: number, row: number, columnWidth?: number) {
		for (const week of state.weeks) {
			const column = firstColumn + week.id;
			if (columnWidth !== undefined) {
				worksheet.getColumn(column).width = columnWidth;
			}

			/** @todo Permettre de modifier le numéro de la première semaine */
			const cell = worksheet.getCell(row, column);
			cell.value = `S${week.id + 1}`
			cell.style.font = { size: 10, italic: true };
			cell.style.fill = { type: 'pattern', pattern: 'solid', fgColor: {argb: 'FFF2F2F2'} };
		}
	}

	protected getMaximalNumberOfCollesByWeek(state: State) {
		let maximalNumberOfCollesByWeek = 0;
		for (const collesOfTrio of Map.groupBy(state.colles, colle => colle.trioId).values()) {
			for (const collesOfTrioInWeek of Map.groupBy(collesOfTrio, colle => colle.weekId).values()) {
				maximalNumberOfCollesByWeek = Math.max(
					maximalNumberOfCollesByWeek,
					collesOfTrioInWeek.length,
				);
			}
		}

		return maximalNumberOfCollesByWeek;
	}

	async asExcel(state: State) {
		const workbook = this.createWorkbook();

		this.setTeachersSection(state, workbook.addWorksheet('Professeurs'));
		this.setStudentsSection(state, workbook.addWorksheet('Étudiants'));

		return new Blob(
			[await workbook.xlsx.writeBuffer()],
			{type: 'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet'},
		);
	}

	async asCsv(state: State) {
		const workbook = this.createWorkbook();
		const worksheet = workbook.addWorksheet();

		this.setTeachersSection(state, worksheet);
		this.setStudentsSection(state, worksheet, worksheet.rowCount + 3);
		worksheet.unMergeCells(1, 1, worksheet.rowCount, worksheet.columnCount);

		return new Blob(
			['sep=,\n', await workbook.csv.writeBuffer()],
			{type: 'text/csv'},
		);
	}
}