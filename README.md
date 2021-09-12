## Introduction
KhôlGen is an application and a library to find the optimal distribution of [colles](https://fr.wikipedia.org/wiki/Colle_(pr%C3%A9pa)) for teachers and students in a [Classe Préparatoire aux Grandes Ecoles (CPGE)](https://fr.wikipedia.org/wiki/Classe_pr%C3%A9paratoire_aux_grandes_%C3%A9coles).

## Usage
### JSON file
Besides the interface, a JSON file can also be used to fill in the data, with the following format:

```json
{
	"subjects": [
		{
			"name": "Mathématiques",
			"shortName": "M",
			"frequency": 1,
			"color": "#c02f2f"
			(color is optional)
		},
		{
			"name": "Physique",
			"shortName": "φ",
			"frequency": 2
		}
	],
	"teachers": [
		{
			"name": "Prof. A.",
			"subject": "Mathématiques",
			"availableTimeslots": ["m12", "M18", "j13"]
			(short form, case sensitive)
		},
		{
			"name": "Prof. B.",
			"subject": "Physique",
			"availableTimeslots": ["mardi 12", "mercredi 18", "jeudi 13"]
			(long form, case insensitive)
		}
	]
}
```

## License
KhôlGen is released to the public for free under the terms of the MIT License. See [LICENSE.md](LICENSE.md) for the full text of the license and the licenses of KhôlGen's third-party dependencies. [LICENSE.md](LICENSE.md) should be distributed alongside any assemblies that use KhôlGen in source or compiled form.
