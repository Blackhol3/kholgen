import { Injectable } from '@angular/core';

import Color from 'color';
import { type Cell, Workbook, type Worksheet } from 'exceljs';

import { type Colle } from './colle';
import { type State } from './state';
import { dayNames } from './timeslot';
import { type WorkingWeek } from './week';

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
	protected state!: State;
	protected workingWeeks!: WorkingWeek[];

	protected createWorkbook() {
		const workbook = new Workbook();
		workbook.creator = 'KhôlGen';
		workbook.lastModifiedBy = workbook.creator;

		return workbook;
	}

	protected setTeachersSection(worksheet: Worksheet, firstRow = 1) {
		const dateRow = firstRow;
		const weekRow = dateRow + 1;
		const firstColleRow = weekRow + 1;
		const bottomBorderedRows = [weekRow];

		worksheet.pageSetup = {
			paperSize: 9,
			orientation: 'landscape',
			horizontalCentered: true,

			printTitlesRow: `${dateRow}:${weekRow}`,
			printTitlesColumn: `${worksheet.getColumn(TeachersSectionColumn.Subject).letter}:${worksheet.getColumn(TeachersSectionColumn.Timeslot).letter}`,
		};

		this.setStandardColumnStyle(worksheet, TeachersSectionColumn.Subject, TeachersSectionColumn.FirstColle + this.workingWeeks.length - 1);

		let row = firstColleRow;
		for (const subject of this.state.subjects) {
			const subjectStartingRow = row;
			const teachersOfSubject = this.state.teachers.filter(teacher => teacher.subjectId === subject.id);
			const teachersIdsOfSubject = teachersOfSubject.map(teacher => teacher.id);
			const collesWithSubject = this.state.colles.filter(colle => teachersIdsOfSubject.includes(colle.teacherId));

			for (const teacher of teachersOfSubject) {
				const teacherStartingRow = row;
				const collesWithTeacher = collesWithSubject.filter(colle => colle.teacherId === teacher.id);

				const groups = [...Map.groupBy(collesWithTeacher, colle => colle.timeslot.toString()).values()].sort((a, b) => a[0].timeslot.compare(b[0].timeslot));
				for (const collesWithTeacherAtTimeslot of groups) {
					worksheet.getCell(row, TeachersSectionColumn.Timeslot).value = collesWithTeacherAtTimeslot[0].timeslot.toReadableString();

					for (const colle of collesWithTeacherAtTimeslot) {
						const week = this.workingWeeks.find(week => week.id === colle.weekId)!;
						const weekColumnIndex = this.workingWeeks.indexOf(week);

						const cell = worksheet.getCell(row, TeachersSectionColumn.FirstColle + weekColumnIndex);
						cell.value = colle.trioId;
						this.fillIfNotWorkingDayCell(cell, colle);
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

		worksheet.getColumn(TeachersSectionColumn.Subject).width = this.state.subjects.map(x => x.name.length).sort((a, b) => b - a)[0];
		worksheet.getColumn(TeachersSectionColumn.TeacherCode).width = 5.5;
		worksheet.getColumn(TeachersSectionColumn.TeacherName).width = this.state.teachers.map(x => x.name.length).sort((a, b) => b - a)[0];
		worksheet.getColumn(TeachersSectionColumn.Timeslot).width = dayNames.map(x => x.length).sort((a, b) => b - a)[0] + 5;

		this.setDatesHeader(worksheet, TeachersSectionColumn.FirstColle, dateRow, 90);
		this.setWeeksHeader(worksheet, TeachersSectionColumn.FirstColle, weekRow, 3.5);
		this.setHolidaysBorder(worksheet, TeachersSectionColumn.FirstColle, dateRow);

		for (let columnIndex: number = TeachersSectionColumn.Subject; columnIndex < TeachersSectionColumn.FirstColle + this.workingWeeks.length; ++columnIndex) {
			for (const row of bottomBorderedRows) {
				const cell = worksheet.getCell(row, columnIndex);
				cell.style.border = { ...cell.style.border, bottom: { style: 'medium' } };
			}
		}
	}

	protected setStudentsSection(worksheet: Worksheet, firstRow = 1) {
		const dateRow = firstRow;
		const weekRow = dateRow + 1;
		const firstColleRow = weekRow + 1;
		const maximalNumberOfCollesByWeek = this.getMaximalNumberOfCollesByWeek();

		worksheet.pageSetup = {
			paperSize: 9,
			orientation: 'landscape',
			horizontalCentered: true,

			printTitlesRow: `${dateRow}:${weekRow}`,
			printTitlesColumn: `${worksheet.getColumn(StudentsSectionColumn.Trio).letter}:${worksheet.getColumn(StudentsSectionColumn.Trio).letter}`,
		};

		this.setStandardColumnStyle(worksheet, StudentsSectionColumn.Trio, StudentsSectionColumn.FirstColle + this.workingWeeks.length - 1);

		let row = firstColleRow;
		for (const trio of this.state.trios.toSorted((a, b) => a.id - b.id)) {
			const trioStartingRow = row;
			const collesOfTrio = this.state.colles.filter(colle => colle.trioId === trio.id);

			for (const [i, week] of this.workingWeeks.entries()) {
				const collesOfTrioInWeek = collesOfTrio.filter(colle => colle.weekId === week.id);

				for (const [j, colle] of collesOfTrioInWeek.entries()) {
					const teacher = this.state.findId('teachers', colle.teacherId)!;
					const teachersOfSubject = this.state.teachers.filter(x => x.subjectId === teacher.subjectId);
					const subject = this.state.findId('subjects', teacher.subjectId)!;
					
					const cell = worksheet.getCell(trioStartingRow + j, StudentsSectionColumn.FirstColle + i);
					cell.value = `${subject.shortName}${teachersOfSubject.indexOf(teacher) + 1} ${colle.timeslot.toShortString()}`;

					if (colle.isDuringWorkingDay(this.state)) {
						cell.style.font = { size: 10, color: {argb: Color(subject.color).isDark() ? 'FFFFFFFF' : 'FF000000'} };
						cell.style.fill = { type: 'pattern', pattern: 'solid', fgColor: {argb: `FF${subject.color.slice(1)}`} };
					}
					else {
						this.fillIfNotWorkingDayCell(cell, colle);
					}
				}
			}

			row += maximalNumberOfCollesByWeek

			worksheet.mergeCells(trioStartingRow, StudentsSectionColumn.Trio, row - 1, StudentsSectionColumn.Trio);
			worksheet.getCell(trioStartingRow, StudentsSectionColumn.Trio).value = `G${trio.id}`;
		}

		this.setDatesHeader(worksheet, StudentsSectionColumn.FirstColle, dateRow);
		this.setWeeksHeader(worksheet, StudentsSectionColumn.FirstColle, weekRow, 7);
		this.setHolidaysBorder(worksheet, StudentsSectionColumn.FirstColle, dateRow);

		for (let columnIndex: number = StudentsSectionColumn.Trio; columnIndex < StudentsSectionColumn.FirstColle + this.workingWeeks.length; ++columnIndex) {
			for (let row = weekRow; row <= worksheet.rowCount; row += maximalNumberOfCollesByWeek) {
				const cell = worksheet.getCell(row, columnIndex);
				cell.style.border = { ...cell.style.border, bottom: { style: 'medium' } };
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

	protected setDatesHeader(worksheet: Worksheet, firstColumn: number, row: number, textRotation: number = 0) {
		for (const [i, week] of this.workingWeeks.entries()) {
			const column = firstColumn + i;
			const cell = worksheet.getCell(row, column);
			cell.value = week.start.setZone('utc', {keepLocalTime: true}).toJSDate();
			cell.numFmt = `[$-C]d mmm`;
			cell.alignment = {...cell.alignment, textRotation: textRotation};
		}
	}

	protected setWeeksHeader(worksheet: Worksheet, firstColumn: number, row: number, columnWidth?: number) {
		for (const [i, week] of this.workingWeeks.entries()) {
			const column = firstColumn + i;
			if (columnWidth !== undefined) {
				worksheet.getColumn(column).width = columnWidth;
			}
			
			const cell = worksheet.getCell(row, column);
			cell.value = `S${week.number}`
			cell.style.font = { size: 10, italic: true };
			cell.style.fill = { type: 'pattern', pattern: 'solid', fgColor: {argb: 'FFF2F2F2'} };
		}
	}

	protected setHolidaysBorder(worksheet: Worksheet, firstColumn: number, firstRow: number) {
		const leftBorderedColumns = [];
		for (const [i, week] of this.workingWeeks.entries()) {
			if (i === 0 || week.start.diff(this.workingWeeks[i - 1].start).as('weeks') > 1) {
				leftBorderedColumns.push(firstColumn + i);
			}
		}

		const borderStyle = { style: 'medium' } as const;
		for (let row = firstRow; row <= worksheet.rowCount; ++row) {
			for (const column of leftBorderedColumns) {
				const cell = worksheet.getCell(row, column);
				cell.style.border = { left: borderStyle };
			}

			const cell = worksheet.getCell(row, firstColumn + this.workingWeeks.length - 1);
			cell.style.border = { right: borderStyle };
		}
	}

	protected fillIfNotWorkingDayCell(cell: Cell, colle: Colle) {
		if (!colle.isDuringWorkingDay(this.state)) {
			const teacher = this.state.findId('teachers', colle.teacherId)!;
			const subject = this.state.findId('subjects', teacher.subjectId)!;

			cell.style.fill = { type: 'pattern', pattern: Color(subject.color).isDark() ? 'lightUp' : 'darkUp', fgColor: {argb: `FF${subject.color.slice(1)}`} };
		}
	}

	protected getMaximalNumberOfCollesByWeek() {
		let maximalNumberOfCollesByWeek = 0;
		for (const collesOfTrio of Map.groupBy(this.state.colles, colle => colle.trioId).values()) {
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
		this.state = state;
		this.workingWeeks = this.state.calendar.getWorkingWeeks();

		const workbook = this.createWorkbook();

		this.setTeachersSection(workbook.addWorksheet('Professeurs'));
		this.setStudentsSection(workbook.addWorksheet('Étudiants'));

		return new Blob(
			[await workbook.xlsx.writeBuffer()],
			{type: 'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet'},
		);
	}

	async asCsv(state: State) {
		this.state = state;
		this.workingWeeks = this.state.calendar.getWorkingWeeks();

		const workbook = this.createWorkbook();
		const worksheet = workbook.addWorksheet();

		this.setTeachersSection(worksheet);
		this.setStudentsSection(worksheet, worksheet.rowCount + 3);
		worksheet.unMergeCells(1, 1, worksheet.rowCount, worksheet.columnCount);

		return new Blob(
			['sep=,\n', await workbook.csv.writeBuffer({dateFormat: 'DD/MM/YYYY'})],
			{type: 'text/csv'},
		);
	}
}