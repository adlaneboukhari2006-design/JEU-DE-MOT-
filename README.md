# JEU-DE-MOT-
# Wordle Master & Solver en C

Un jeu Wordle complet développé en C, incluant un mode de jeu classique et un **solveur intelligent** capable de déduire le mot mystère en filtrant dynamiquement un dictionnaire.

## 📋 Fonctionnalités

* **Mode Jeu** : Devinez le mot caché en 6 essais avec un retour visuel coloré (Vert, Jaune, Gris).
* **Mode Solveur** : L'ordinateur suggère des mots et élimine les impossibilités en fonction de vos retours (feedback).
* **Nettoyage rigoureux** : Gestion des espaces, de la casse (minuscules/majuscules) et des caractères spéciaux (BOM UTF-8).
* **Filtrage Dynamique** : Le solveur réduit la liste des mots possibles après chaque tentative.

## 🛠️ Installation et Compilation

### Prérequis
* Un compilateur C (GCC, Clang, MinGW, etc.)
* Un terminal supportant les codes couleurs ANSI (la plupart des terminaux Linux/macOS et les terminaux modernes sous Windows comme PowerShell ou Windows Terminal).

### Compilation
Ouvrez votre terminal dans le dossier du projet et exécutez :

```bash
gcc main.c -o wordle
