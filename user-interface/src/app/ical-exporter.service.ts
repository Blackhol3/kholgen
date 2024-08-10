import { Injectable } from '@angular/core';

import { downloadZip } from 'client-zip';
import { type ICalAlarmData, ICalAlarmType, ICalCalendar, ICalCalendarMethod, ICalEventBusyStatus, ICalEventStatus, ICalEventTransparency } from 'ical-generator';
import { Settings } from 'luxon';
import sanitize from 'sanitize-filename';

import { type Computation } from './computation';
import { type Teacher } from './teacher';
import { type Trio } from './trio';

@Injectable({
	providedIn: 'root'
})
export class ICalExporterService {
	protected computation!: Computation;

	async asZip(computation: Computation) {
		this.computation = computation;
		
		const files = [
			...this.computation.teachers.map(teacher => this.createFileForTeacher(teacher)).filter((x): x is File => x !== null),
			...this.computation.trios.map(trio => this.createFileForTrio(trio)).filter((x): x is File => x !== null),
		];
		return downloadZip(files, {metadata: files, buffersAreUTF8: true}).blob();
	}
	
	protected createFileForTeacher(teacher: Teacher) {
		const collesOfTeacher = this.computation.colles.filter(colle => colle.teacherId === teacher.id);
		if (collesOfTeacher.length === 0) {
			return null;
		}

		const calendar = new ICalCalendar({
			name: `Colles de ${teacher.name}`,
			method: ICalCalendarMethod.PUBLISH,
			prodId: `-//KholGen//NONSGML Teacher//FR`,
			source: undefined,
			timezone: Settings.defaultZone.name,
		});

		for (const colle of collesOfTeacher) {
			const startDate = colle.getStartDate(this.computation.calendar);

			const event = {
				start: startDate,
				end: startDate.plus({hour: 1}),

				status: ICalEventStatus.CONFIRMED,
				busystatus: ICalEventBusyStatus.BUSY,
				transparency: ICalEventTransparency.OPAQUE,

				summary: 'Colle',
				description: `Trinôme ${colle.trioId}`,
				alarms: [] as ICalAlarmData[],
			};

			if (!colle.isDuringWorkingDay(this.computation.calendar)) {
				event.busystatus = ICalEventBusyStatus.FREE;
				event.transparency = ICalEventTransparency.TRANSPARENT;
				event.summary += ' (à reporter)';
				event.description += '<br><b>Cette colle doit être reportée.</b>';
				event.alarms.push({
					type: ICalAlarmType.display,
					trigger: startDate.minus({week: 1}),
					description: 'Cette colle doit être reportée.',
				});
			}

			calendar.createEvent(event);
		}

		const subject = this.computation.findId('subjects', teacher.subjectId)!;
		return new File(
			[calendar.toString()],
			`Enseignant/${sanitize(subject.name)}/${sanitize(teacher.name)}.ics`,
			{type: 'text/calendar'},
		);
	}

	protected createFileForTrio(trio: Trio) {
		const collesOfTrio = this.computation.colles.filter(colle => colle.trioId === trio.id);
		if (collesOfTrio.length === 0) {
			return null;
		}

		const calendar = new ICalCalendar({
			name: `Colles du trinôme ${trio.id}`,
			method: ICalCalendarMethod.PUBLISH,
			prodId: `-//KholGen//NONSGML Trio//FR`,
			source: undefined,
			timezone: Settings.defaultZone.name,
		});

		for (const colle of collesOfTrio) {
			const startDate = colle.getStartDate(this.computation.calendar);
			const teacher = this.computation.findId('teachers', colle.teacherId)!;
			const subject = this.computation.findId('subjects', teacher.subjectId)!;

			const event = {
				start: startDate,
				end: startDate.plus({hour: 1}),

				status: ICalEventStatus.CONFIRMED,
				busystatus: ICalEventBusyStatus.BUSY,
				transparency: ICalEventTransparency.OPAQUE,

				summary: `Colle – ${subject.name}`,
				description: `Enseignant : ${teacher.name}`,
			};

			if (!colle.isDuringWorkingDay(this.computation.calendar)) {
				event.busystatus = ICalEventBusyStatus.FREE;
				event.transparency = ICalEventTransparency.TRANSPARENT;
				event.summary += ' (à reporter)';
				event.description += '<br><b>Cette colle doit être reportée.</b>';
			}

			calendar.createEvent(event);
		}

		return new File(
			[calendar.toString()],
			`Trinôme/${trio.id}.ics`,
			{type: 'text/calendar'},
		);
	}
}