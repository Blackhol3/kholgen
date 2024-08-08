import { Ajv, type JTDSchemaType } from 'ajv/dist/jtd';

import { type HumanJson } from './json';
import { type State } from './state';

const schema: JTDSchemaType<HumanJson<State>> = {
	properties: {
		groups: {
			elements: {
				properties: {
					name: { type: 'string' },
					availableTimeslots: { elements: { type: 'string' } },
					trios: { elements: { type: 'uint8' } },
				},
				optionalProperties: { duration: { type: 'uint8' }, nextGroup: { type: 'string' } },
			},
		},
		subjects: {
			elements: {
				properties: {
					name: { type: 'string' },
					shortName: { type: 'string' },
					frequency: { type: 'uint8' },
					color: { type: 'string' },
				},
			},
		},
		teachers: {
			elements: {
				properties: {
					name: { type: 'string' },
					subject: { type: 'string' },
					availableTimeslots: { elements: { type: 'string' } },
				},
				optionalProperties: { weeklyAvailabilityFrequency: { type: 'uint8' } },
			},
		},
		objectives: { elements: { type: 'string' } },
		calendar: {
			properties: {
				interval: { type: 'string' },
			},
			optionalProperties: {
				academie: { type: 'string' },
				firstWeekNumber: { type: 'uint8' },
				interruptions: {
					elements: {
						properties: {
							name: { type: 'string' },
							interval: { type: 'string' },
						},
						optionalProperties: {
							weeksNumbering: { type: 'boolean' },
							groupsRotation: { type: 'boolean' }
						},
					},
				},
			}
		}
	},
	optionalProperties: {
		lunchTimeRange: { elements: { type: 'uint8' } },
		forbiddenSubjectsCombination: { elements: { type: 'string' } },
	},
};

const ajv = new Ajv({});

export const validateHumanJson = ajv.compile(schema);