# Projet MCTS – Structure Professionnelle

Ce projet implémente l’algorithme Monte Carlo Tree Search (MCTS) pour des jeux à information parfaite à deux joueurs, conformément aux exigences du fichier `projet.pdf` (sections 10.1 et 10.2).

---

## 📁 Structure du projet


```text
projet/
│
├── include/                  # Tous les fichiers d’en-tête (.hpp)
│   ├── projet.hpp            # Classes génériques : Joueur, JMCTS, Partie, PA
│   ├── P4.hpp                # Jeu Puissance 4 (représentation des positions)
│   ├── JMCTS2.hpp            # Variante de MCTS avec réutilisation d'arbre
│   └── JHP4.hpp              # Joueur humain via entrée utilisateur
│
├── src/                      # Fichiers source (.cpp)
│   ├── main.cpp              # Démo principale (ex: JMCTS<PA> vs JMCTS<PA>)
│   ├── P4.cpp                # Implémentation de P4 (non-template)
│   ├── JHP4.cpp              # Lecture de l'entrée utilisateur pour P4
│   └── test_jmcts2.cpp       # Expériences pour ajuster le paramètre 'a'
│
├── tests/                    # Tests de jeux, interface, ou stratégie
│   └── test_p4.cpp           # Test d'une partie JHP4 vs JMCTS<P4>
│
├── build/                    # Dossier pour la compilation via CMake (optionnel)
│
├── projet.pdf                # Sujet de projet fourni
├── CMakeLists.txt            # Script de compilation (si CMake utilisé)
└── README.md                 # Présentation du projet
```

---

## ✅ Conformité avec `projet.pdf`

- Toutes les classes requises en 10.1 sont dans `projet.hpp`
- Tous les travaux de 10.2 sont répartis dans des fichiers/test dédiés
- Le projet est facilement extensible, modulaire et maintenable

---

## 🔧 Compilation

### Commande simple (avec g++) :
```bash
g++ -std=c++11 src/main.cpp src/P4.cpp src/JHP4.cpp -I include -o projet
```

### Ou avec CMake :
```bash
mkdir build && cd build
cmake ..
make
```

---

## 🚀 Objectif

- Comparer différentes variantes de MCTS (`JMCTS`, `JMCTS2`)
- Expérimenter avec le jeu Puissance 4
- Préparer un rapport complet avec résultats reproductibles

