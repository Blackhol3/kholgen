/* eslint-disable @stylistic/indent */
declare module 'qwebchannel' {
	type Template = {
		slots: Record<string, (...args: never[]) => Promise<unknown>>,
		signals: Record<string, (...args: never[]) => void>,
	}

	type Signal<F extends (...args: never[]) => void> = {connect: (callback: F) => void}

	export type ChannelObject<T extends Template> = {
		[K in keyof T['slots'] | keyof T['signals']]:
			K extends keyof T['slots'] ? T['slots'][K] :
			K extends keyof T['signals'] ? Signal<T['signals'][K]> :
			never
	}

	export class QWebChannel<R extends Record<string, Template>> {
		constructor(
			transport: {send: (data: never) => void},
			initCallback: (channel: {objects: {[K in keyof R]: ChannelObject<R[K]>}}) => unknown,
		)
	}
}
