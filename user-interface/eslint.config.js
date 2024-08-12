import pluginJs from '@eslint/js';
import stylistic from '@stylistic/eslint-plugin'
import tseslint from 'typescript-eslint';
import angular from 'angular-eslint';

export default tseslint.config(
	{
		languageOptions: {
			parserOptions: {
				projectService: true,
				tsconfigRootDir: import.meta.dirname,
			},
		},
	},
	{
		files: ['**/*.ts'],
		extends: [
			pluginJs.configs.recommended,
			...tseslint.configs.recommendedTypeChecked,
			...angular.configs.tsRecommended,
		],
		processor: angular.processInlineTemplates,
		plugins: {
			'@stylistic': stylistic,
		},
		rules: {
			'@angular-eslint/directive-selector': ['error', {
				type: 'attribute',
				prefix: 'app',
				style: 'camelCase',
			}],
			'@angular-eslint/component-selector': ['error', {
				type: 'element',
				prefix: 'app',
				style: 'kebab-case',
			}],
			'@angular-eslint/no-input-rename': 'off',
			'@angular-eslint/prefer-standalone': 'error',
			'@angular-eslint/prefer-standalone-component': 'error',
			'@stylistic/indent': ['error', 'tab', {flatTernaryExpressions: true, SwitchCase: 1}],
			'@stylistic/linebreak-style': ['error', 'unix'],
			'@typescript-eslint/no-floating-promises': 'error',
			'@typescript-eslint/no-misused-promises': 'error',
			'@typescript-eslint/no-unused-expressions': ['error', {allowTernary: true}],
			'@typescript-eslint/no-unused-vars': ['error', {argsIgnorePattern: '^_'}],
			'@typescript-eslint/unbound-method': 'off',
			'no-promise-executor-return': 'error',
			'no-irregular-whitespace': 'off',
		},
	},
	{
		files: ['**/*.html'],
		extends: [
			...angular.configs.templateRecommended,
			...angular.configs.templateAccessibility,
		],
		rules: {
			'@angular-eslint/template/mouse-events-have-key-events': 'off',
		},
	}
);
