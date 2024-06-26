import pluginJs from '@eslint/js';
import stylistic from '@stylistic/eslint-plugin'
import tseslint from 'typescript-eslint';

export default [
	pluginJs.configs.recommended,
	...tseslint.configs.recommendedTypeChecked,
	{
		languageOptions: {
			parserOptions: {
				project: true,
				tsconfigRootDir: import.meta.dirname,
			},
		},
	},
	{
		ignores: [
			'**/*.js',
		],
	},
	{
		plugins: {
			'@stylistic': stylistic,
		},
		rules: {
			'@stylistic/indent': ['error', 'tab', {flatTernaryExpressions: true}],
			'@stylistic/linebreak-style': ['error', 'unix'],
			'@typescript-eslint/no-floating-promises': 'error',
			'@typescript-eslint/no-misused-promises': 'error',
			'@typescript-eslint/no-unused-expressions': ['error', {allowTernary: true}],
			'@typescript-eslint/no-unused-vars': ["error", {argsIgnorePattern: '^_'}],
			'@typescript-eslint/unbound-method': 'off',
			'no-promise-executor-return': 'error',
			'no-irregular-whitespace': 'off',
		},
	},
];