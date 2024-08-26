*Read this in another language: [English](README.en.md).*
# KhôlGen
## Introduction
<img align="right" width="192" height="192" src="user-interface/src/assets/icon/main.svg">

*KhôlGen* est une application et une bibliothèque pour trouver la distribution optimale des [colles](https://fr.wikipedia.org/wiki/Colle_(pr%C3%A9pa)) pour les enseignants et les étudiants de [Classe Préparatoire aux Grandes Écoles (CPGE)](https://fr.wikipedia.org/wiki/Classe_pr%C3%A9paratoire_aux_grandes_%C3%A9coles). Il gère l'ensemble des situations les plus courantes, et permet notamment :
- d'éviter que les étudiants aient plusieurs colles le même jour ;
- de bien répartir les étudiants entre les différents enseignants disponibles ;
- de s'assurer que les étudiants aient un créneau pour déjeuner ;
- de définir des créneaux de colles même lorsque toute la classe n'est pas disponible, sur des horaires éventuellement changeants d'une semaine à l'autre, comme lors de séances en demi-groupes ;
- de choisir automatiquement les créneaux les plus adaptés si un enseignant a plus de disponibilités que nécessaire ;
- …

## Installation
Pour utiliser *KhôlGen*, [téléchargez la version](../../releases/latest) adaptée à votre système, décompressez l'archive et lancez le programme qu'elle contient. Une interface utilisateur intuitive s'ouvre alors dans votre navigateur. Renseignez toutes les informations nécessaires, dans chacune des pages du menu de gauche, puis lancez le calcul. Suivant la complexité du problème, cette dernière opération peut durer assez longtemps ; vous pouvez interrompre l'optimisation dès que vous avez obtenu une solution qui vous convienne.

## Développement
### Interface utilisateur
L'interface utilisateur utilise Angular, et l'installation globale de l'[Angular CLI](https://angular.dev/tools/cli/setup-local#dependencies) est donc conseillée. Les principales commandes sont les suivantes :
- `ng serve` lance le serveur de développement ; l'interface est alors disponible à l'adresse `http://localhost:4200/` ;
- `ng test` exécute les tests unitaires ;
- `ng build` compile le projet.

### Solveur
Le solveur utilise Qt 6.7, qui doit être préalablement [installé](https://doc.qt.io/qt-6/get-and-install-qt.html). Après compilation, le packaging de l'application peut être préparé à l'aide de la commande `cmake --install`.

### Packaging
Après compilation, le packaging de l'application s'effectue à l'aide du script `package.sh`. L'ensemble des fichiers de l'application sont alors disponibles dans le répertoire `build/package/`.

## License
*KhôlGen* est distribué au public gratuitement sous les termes de la licence MIT. Voir [LICENSE](LICENSE) pour le texte complet de la licence. [LICENSE](LICENSE) doit être distribué avec tout programme qui utilise *KhôlGen* sous la forme source ou compilée.
