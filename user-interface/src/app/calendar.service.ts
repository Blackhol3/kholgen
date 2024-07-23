import { Injectable } from '@angular/core';

import { DateTime, Interval } from 'luxon';

import { getFirstValidDate } from './calendar';

const localStoragePrefix = 'calendarService';
const schoolHolidayAPI = 'https://data.education.gouv.fr/api/explore/v2.1/catalog/datasets/fr-en-calendrier-scolaire/records';
const publicHolidayAPI = 'https://calendrier.api.gouv.fr/jours-feries';

/* @todo Handle network errors */
async function fetchData(url: URL) {
	const response = await fetch(url);
	return response.json();
}

function getLocalStorageDataKey(item: string) {
	return `${localStoragePrefix}.${item}`;
}

function getLocalStorageDateTimeKey(item: string) {
	return `${getLocalStorageDataKey(item)}.dateTime`;
}

function getFromLocalStorage<T>(item: string): T | null {
	const storedData = localStorage.getItem(getLocalStorageDataKey(item));
	const storedDateTime = localStorage.getItem(getLocalStorageDateTimeKey(item));

	if (storedData === null || storedDateTime === null) {
		return null;
	}

	const dateTime = DateTime.fromISO(storedDateTime);
	if (dateTime.diffNow().negate().as('months') > 6) {
		return null;
	}

	return JSON.parse(storedData) as T;
}

function setToLocalStorage(item: string, value: unknown) {
	try {
		localStorage.setItem(getLocalStorageDataKey(item), JSON.stringify(value));
		localStorage.setItem(getLocalStorageDateTimeKey(item), DateTime.now().toISO());
	} catch { /* Empty */ }
}

function academieToZone(academie: string) {
	switch (academie) {
		/* @todo Include parts of the Académie of Nancy-Metz as well */
		case 'Strasbourg':
			return 'alsace-moselle';
		
		case 'Réunion':
			return 'la-reunion';
		
		case 'Polynésie':
			return 'polynesie-francaise';

		case 'Guadeloupe':
		case 'Guyane':
		case 'Martinique':
		case 'Mayotte':
		case 'Nouvelle Calédonie':
		case 'Wallis et Futuna':
			return academie.normalize('NFKD').replace(/\p{Diacritic}/gu, '').toLocaleLowerCase().replace(' ', '-');

		default:
			return 'metropole';
	}
}

@Injectable({
	providedIn: 'root'
})
export class CalendarService {
	async getAcademies() {
		let academies = getFromLocalStorage<string[]>('academies');
		if (academies !== null) {
			return academies;
		}

		const params = new URLSearchParams({
			select: 'location',
			where: `end_date >= "${getFirstValidDate().toSQLDate()}"`,
			group_by: 'location',
		});
		const url = new URL(`${schoolHolidayAPI}?${params.toString()}`);
		const data = await fetchData(url) as {results: {location: string}[]};
		academies = data.results.map(x => x.location).sort();

		setToLocalStorage('academies', academies);
		return academies;
	}

	async getHolidays(academie: string) {
		if (!(await this.getAcademies()).includes(academie)) {
			throw new Error(`« ${academie} » n'est pas une académie reconnue.`);
		}

		const stringHolidays = getFromLocalStorage<string[]>(`holidays.${academie}`);
		if (stringHolidays !== null) {
			return stringHolidays.map(x => Interval.fromISO(x));
		}

		const firstValidDate = getFirstValidDate();
		const params = new URLSearchParams({
			select: 'start_date, end_date',
			where: `end_date >= "${firstValidDate.toSQLDate()}" and location = "${academie}" and (population = "-" or population = "Élèves")`,
			order_by: 'start_date',
		});
		const schoolUrl = new URL(`${schoolHolidayAPI}?${params.toString()}`);
		const schoolData = await fetchData(schoolUrl) as {results: {start_date: string, end_date: string}[]};
		const holidays = schoolData.results.map(x => Interval.fromISO(`${x.start_date}/${x.end_date}`));
		
		const publicUrl = new URL(`${publicHolidayAPI}/${academieToZone(academie)}.json`);
		const publicData = Object.keys(await fetchData(publicUrl) as Record<string, string>);
		publicData
			.map(x => DateTime.fromISO(x))
			.filter(x => x >= firstValidDate)
			.map(x => Interval.after(x, {day: 1}))
			.forEach(x => holidays.push(x))
		;

		setToLocalStorage(`holidays.${academie}`, holidays.map(x => x.toISO()));
		return holidays;
	}
}
