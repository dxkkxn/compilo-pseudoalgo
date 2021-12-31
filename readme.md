# I53 Compilateur

## Utilisation
```bash
    ./arc <file.algo> -o <file.ram>
```
## Description 
Si la flag -o n'est pas specifie' le fichier executable s'appelera out.ram

L'archive contient les fichiers suivants que nous allons détailler :

- parser.y : L'analyseur syntaxique qui transforme la chaine en "postfixée"
- lexer.c  : L'analyseur lexical
- asa.c/.h : Implementation des arbres syntaxique et generation du code ram
- ts.c/.h  : Implementation d'un tableau des symboles

### **Caracteristiques**
1. Les declarations de variables multiples (ex: VAR x y z);
2. L’initialisation des variables pendant la d´eclaration (ex: VAR x <- 0;
3. La d´eclaration de variables de type tableau de taille statique (ex: VAR tab[5]);
4. Les instructions sp´ecifiques de la machine RAM pour produire un code plus simple dans le cas d’une operation arithmmetique dont une operande est une constante (ex: x+5 );
5. idem pour les instructions de type incrementation ou decrementation (ex x <- x+1);
6. idem pour les operations de comparaisons de type x < y 

### **parser.y**
La grammaire est extense donc on va pas detailler chaque production
Quelques ajouts que nous avons fait sont vous pouvez enchainer les declarations
des tableaux et des variables. Example:
```
VAR tab[20] x arr[30] y z t tmp
```
Vous pouvez attendre en entrée un tableau. Example:
```
ALGO example
ENTRE tab[10]
...

```
Par contre le code pour la lecture pourrait etre optimise', pour l'instant
il se limite a generer un code avec N lectures et N stockages a la suite. 

On a du creer une list chaines simple pour gerer toutes les instructions 
lorsque elles sont imbriques dans un structure conditionelle

Une petite optimisation a ete fait aussi lors l'evaluation d'expressions logiques
avec un ET ou un OU pour s'assimiler aux reste des langages de programation.
Ex:
```
exp_1 ET exp_2
exp_1 OU exp_2
```
Pour l'ET Lors que exp_1 est fausse on evalue pas exp2 et inversement pour l'OU

## Exemples
Nous fournisons une serie de examples dans le fichier il suffit d'executer
```bash
make -B tests 
```
et vous aurez quelques examples a executer sur la machiner ram
### BENJELLOUN Youssef / LEAL André / Groupe 1.A
