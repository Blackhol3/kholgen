import { type Draft, castDraft } from 'immer';

import { State } from './state';
import { Subject } from './subject';
import { Teacher } from './teacher';

describe('State', () => {
	it('should create an instance', () => {
		expect(new State()).toBeTruthy();
	});

	describe('should remove a subject', () => {
		const subjects = [
			new Subject('A', '', 1, ''),
			new Subject('B', '', 1, ''),
			new Subject('C', '', 1, ''),
		];
		const teachers = castDraft([
			new Teacher('X', subjects[0].id, []),
			new Teacher('Y', subjects[1].id, []),
			new Teacher('Z', subjects[1].id, []),
		]);
		const forbiddenSubjectIdsCombination = new Set([subjects[1].id, subjects[2].id]);
		let state: Draft<State>;

		beforeEach(() => {
			state = castDraft(new State());
			state.subjects = [...subjects];
			state.teachers = [...teachers];
		});

		it('if the subject does not have associated teachers', () => {
			const result = state.removeSubject(subjects[2]);

			expect(state.subjects).toEqual([subjects[0], subjects[1]]);
			expect(state.teachers).toEqual(teachers);
			expect(result.hadAssociatedTeachers).toBeFalse();
		});

		it('if the subject have associated teachers', () => {
			const result = state.removeSubject(subjects[1]);

			expect(state.subjects).toEqual([subjects[0], subjects[2]]);
			expect(state.teachers).toEqual([teachers[0]]);
			expect(result.hadAssociatedTeachers).toBeTrue();
		});

		it('if the subject does not exist', () => {
			const result = state.removeSubject(new Subject('D', '', 1, ''));

			expect(state.subjects).toEqual(subjects);
			expect(state.teachers).toEqual(teachers);
			expect(result.hadAssociatedTeachers).toBeFalse();
		});

		it('if the subject was in the forbidden combination', () => {
			state.forbiddenSubjectIdsCombination = forbiddenSubjectIdsCombination
			const result = state.removeSubject(subjects[1]);

			expect(state.forbiddenSubjectIdsCombination).toEqual(new Set());
			expect(result.wasInForbiddenCombination).toBeTrue();
		});

		it('if the subject was not in the forbidden combination', () => {
			state.forbiddenSubjectIdsCombination = forbiddenSubjectIdsCombination
			const result = state.removeSubject(subjects[0]);

			expect(state.forbiddenSubjectIdsCombination).toEqual(forbiddenSubjectIdsCombination);
			expect(result.wasInForbiddenCombination).toBeFalse();
		});
	});
});
