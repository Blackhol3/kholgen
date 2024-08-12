import { immerable } from 'immer';

export const enum Day {
	Monday, Tuesday, Wednesday, Thursday, Friday
}

export const firstHour = 8;
export const lastHour = 18;

export const nbDaysInWeek = 5;
type DayNames<T extends readonly string[] = []> = T['length'] extends typeof nbDaysInWeek ? T : DayNames<readonly [string, ...T]>;

export const dayNames: DayNames = [
	'Lundi', 'Mardi', 'Mercredi', 'Jeudi', 'Vendredi'
];

export const dayShortNames: DayNames = [
	'l', 'm', 'w', 'j', 'v'
];

export class Timeslot {
	[immerable] = true;
	
	constructor(
		readonly day: Day,
		readonly hour: number,
	) {}
	
	isEqual(timeslot: Timeslot): boolean {
		return this.day === timeslot.day && this.hour === timeslot.hour;
	}

	compare(timeslot: Timeslot): number {
		const compareDay = this.day - timeslot.day;
		return compareDay === 0 ? (this.hour - timeslot.hour) : compareDay;
	}
	
	toReadableString(): string {
		return `${dayNames[this.day]} à ${this.hour}h`;
	}
	
	toString(): string {
		return `${dayNames[this.day]} ${this.hour}`;
	}

	toShortString(): string {
		return `${dayShortNames[this.day]}${this.hour}`;
	}
	
	static fromString(string: string): Timeslot {
		let day: Day | undefined;
		let hourString: string | undefined;
		
		string = string.toLowerCase();
		for (let i = 0; i < dayNames.length; ++i) {
			if (string.startsWith(dayNames[i].toLowerCase())) {
				day = i;
				hourString = string.substring(dayNames[i].length);
			}
		}
		
		if (day === undefined) {
			for (let i = 0; i < dayShortNames.length; ++i) {
				if (string.startsWith(dayShortNames[i])) {
					day = i;
					hourString = string.substring(dayShortNames[i].length);
				}
			}
		}
		
		if (day === undefined) {
			throw new SyntaxError(`Le jour associé au créneau « ${string} » est mal défini.`);
		}
		
		const hour = parseInt(hourString!.trim());
		if (Number.isNaN(hour) || hour < firstHour || hour > lastHour) {
			throw new SyntaxError(`L'horaire associé au créneau « ${string} » est mal défini.`);
		}

		return new Timeslot(day, hour);
	}
}
