 I53 Compilateur

## Utilisation
```bash
    ./arc <file.algo> -o <file.ram>
```
## Description
Si le flag -o n'est pas spécifié le fichier exécutable s'appelera out.ram

L'archive contient les fichiers suivants que nous allons détailler :

- lexer.c  : L'analyseur lexical
- parser.y : L'analyseur syntaxique 
- asa.c/.h : Implémentation des arbres syntaxiques et génération du code ram
- ts.c/.h  : Implémentation d'un tableau de symboles

### **Caractéristiques**
1. Les déclarations de variables multiples (ex: VAR x y z);
2. L’initialisation des variables pendant la déclaration (ex: VAR x <- 0);
3. La déclaration de variables de type tableau de taille statique (ex: VAR tab[5]);
4. Les instructions spécifiques de la machine RAM pour produire un code plus simple dans le cas d’une opération arithmétique dont une opérande est une constante (ex: x+5);
5. Idem pour les instructions de type incrémentation ou décrémentation (ex x <- x+1);
6. Idem pour les opérations de comparaison de type x < y

### **parser.y**
La grammaire est extensible de ce fait chaque production ne sera pas détaillée.

Nous avons ajouté des options supplémentaires:

- Possibilité d'enchaîner les déclarations de tableaux et de variables. Exemple:
```
VAR tab[20] x arr[30] y z t tmp
```
- Un tableau peut être attendu en entrée. Exemple:
```
ALGO exemple
ENTRE tab[10]
...

```
Cependant, le code de la lecture pourrait être optimisé. Il se limite pour l'instant à générer un code avec N lectures et N stockages à la suite.

Nous avons dû créer une liste chaînée simple pour gérer toutes les instructions lorsqu'elles sont imbriquées dans une structure conditionnelle.

D'autre part, une petite optimisation a été faite lors  de l'évaluation d'expressions logiques avec un ET ou un OU pour s'assimiler au reste des langages de programmation.
Exemple:
```
exp_1 ET exp_2
exp_1 OU exp_2
```
Pour le ET lorsque exp_1 est fausse exp2 n'est pas évaluée et inversement pour le OU.

## Exemples
Nous fournissons une série d'exemples dans le fichier.  Pour tester quelques exemples sur la machine ram il suffit d'exécuter la commande suivante:
```bash
make -B tests
```
### BENJELLOUN Youssef / LEAL André / L3 Info Groupe 1.A
