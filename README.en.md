*Read this in another language: [français](README.md).*
# KhôlGen
## Introduction
<img align="right" width="192" height="192" src="user-interface/src/assets/icon/main.svg">

*KhôlGen* is an application and a library to find the optimal distribution of [colles](https://fr.wikipedia.org/wiki/Colle_(pr%C3%A9pa)) for teachers and students in a [Classe Préparatoire aux Grandes Écoles (CPGE)](https://fr.wikipedia.org/wiki/Classe_pr%C3%A9paratoire_aux_grandes_%C3%A9coles). It handles the most common situations and allows for example:
- avoiding students having multiple colles on the same day;
- properly distributing students among the available teachers;
- ensuring students have a lunch break;
- scheduling oral exams even when the entire class is not available, with potentially changing schedules from week to week, such as during half-group class;
- automatically choosing the most suitable timeslots if a teacher has more availabilities than needed;
- …

## Installation
To use *KhôlGen*, [download the version](../../releases/latest) suitable to your system, unzip the archive et run the executable inside. An intuitive user interface will then open in your browser. Fill in all the necessary information on each page of the left menu, and then start the calculation. Depending on the complexity of the problem, this last operation may take quite a while; you can stop the optimization as soon as you have obtained a solution that suits you.

## Development
### User interface
The user interface uses Angular, so it is recommended to install the [Angular CLI](https://angular.dev/tools/cli/setup-local#dependencies) globally. The main commands are:
- `ng serve` starts the development server; the interface is then available at `http://localhost:4200/`;
- `ng test` executes the unit tests;
- `ng build` builds the project.

### Solver
The solver uses Qt 6.7, which must be [installed](https://doc.qt.io/qt-6/get-and-install-qt.html) beforehand. After compilation, the application packaging can be prepared using the command `cmake --install`.

### Packaging
After compilation, the application packaging is done using the script `package.sh`. All the application files are then available in the `build/package/` directory.

## License
*KhôlGen* is released to the public for free under the terms of the MIT License. See [LICENSE.](LICENSE) for the full text of the license. [LICENSE](LICENSE) should be distributed alongside any assemblies that use *KhôlGen* in source or compiled form.
