#include "P4.hpp"
#include <sstream>
#include <cassert>
#include <iostream>  // Ajouté pour std::cout

/**
 * @brief Constructeur de la classe P4
 * Initialise une grille vide et un compteur de hauteur nul pour chaque colonne.
 */
P4::P4(bool j1) : j1aletrait(j1) {
    for (int i = 0; i < LIGNES; ++i)
        for (int j = 0; j < COLS; ++j)
            grille[i][j] = 0;
    for (int j = 0; j < COLS; ++j)
        hauteur[j] = 0;
}

/**
 * @brief Calcule le nombre de coups possibles (colonnes non pleines)
 * @return Nombre de colonnes où un pion peut encore être joué
 */
unsigned P4::Nbcoups() const {
    unsigned n = 0;
    for (int j = 0; j < COLS; ++j)
        if (hauteur[j] < LIGNES)
            ++n;
    return n;
}

/**
 * @brief Applique un coup sur la colonne donnée pour le joueur courant.
 * @param i L'indice de la colonne (0 à 6)
 */
void P4::EffectuerCoup(unsigned i) {
    std::cout << "DEBUG: Tentative de coup dans colonne " << i << std::endl;
    assert(i < COLS && hauteur[i] < LIGNES);
    int ligne = hauteur[i];
    grille[ligne][i] = j1aletrait ? 1 : 2;
    hauteur[i]++;
    j1aletrait = !j1aletrait;
}

/**
 * @brief Évalue l'état actuel de la grille pour déterminer un gagnant, une égalité ou si la partie est en cours.
 * @return Résultat de la partie : j1gagne, j0gagne, partieNulle ou enCours
 */
Resultat P4::Eval() const {
    for (int i = 0; i < LIGNES; ++i) {
        for (int j = 0; j < COLS; ++j) {
            int joueur = grille[i][j];
            if (joueur == 0) continue;

            // Vérification alignement horizontal
            if (j + 3 < COLS &&
                joueur == grille[i][j + 1] &&
                joueur == grille[i][j + 2] &&
                joueur == grille[i][j + 3])
                return joueur == 1 ? Resultat::j1gagne : Resultat::j0gagne;

            // Vérification alignement Vertical
            if (i + 3 < LIGNES &&
                joueur == grille[i + 1][j] &&
                joueur == grille[i + 2][j] &&
                joueur == grille[i + 3][j])
                return joueur == 1 ? Resultat::j1gagne : Resultat::j0gagne;
            
            // Vérification diagonale descendante droite
            if (i + 3 < LIGNES && j + 3 < COLS &&
                joueur == grille[i + 1][j + 1] &&
                joueur == grille[i + 2][j + 2] &&
                joueur == grille[i + 3][j + 3])
                return joueur == 1 ? Resultat::j1gagne : Resultat::j0gagne;

            
            // Vérification diagonale descendante gauche
            if (i + 3 < LIGNES && j - 3 >= 0 &&
                joueur == grille[i + 1][j - 1] &&
                joueur == grille[i + 2][j - 2] &&
                joueur == grille[i + 3][j - 3])
                return joueur == 1 ? Resultat::j1gagne : Resultat::j0gagne;
        }
    }

    // S'il reste des coups possibles, la partie continue
    for (int j = 0; j < COLS; ++j)
        if (hauteur[j] < LIGNES)
            return Resultat::enCours;

    // Sinon, partie nulle (grille pleine sans victoire)
    return Resultat::partieNulle;
}

/**
 * @brief Génère une représentation textuelle de la grille pour affichage.
 * @return Chaîne contenant l'état actuel de la grille et du joueur actif
 */
std::string P4::ToString() const {
    std::ostringstream oss;
    oss << "0 1 2 3 4 5 6\n";
    for (int i = LIGNES - 1; i >= 0; --i) {
        for (int j = 0; j < COLS; ++j) {
            char c = '.';
            if (grille[i][j] == 1) c = 'X';
            else if (grille[i][j] == 2) c = 'O';
            oss << c << ' ';
        }
        oss << '\n';
    }
    oss << (j1aletrait ? "j1 joue (X)" : "j0 joue (O)");
    return oss.str();
}

/**
 * @brief Compare deux positions P4.
 * @param other L'autre position à comparer
 * @return true si les grilles et le joueur actif sont identiques
 */
bool P4::operator==(const P4& other) const {
    if (j1aletrait != other.j1aletrait) return false;
    for (int i = 0; i < LIGNES; ++i)
        for (int j = 0; j < COLS; ++j)
            if (grille[i][j] != other.grille[i][j])
                return false;
    return true;
}
