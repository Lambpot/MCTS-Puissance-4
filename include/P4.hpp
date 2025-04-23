#pragma once
#include <string>
#include "projet.hpp"  // pour Resultat

// Classe représentant une position du jeu Puissance 4

// Déclaration anticipée de l'énumération de résultat
enum class Resultat;

class P4
{
public:
    static const int LIGNES = 6;
    static const int COLS = 7;

    // État interne du plateau
    int grille[LIGNES][COLS];   // 0 = vide, 1 = j1, 2 = j0
    int hauteur[COLS];          // hauteur de chaque colonne
    bool j1aletrait;            // true si le joueur 1 doit jouer

    // Constructeur
    P4(bool j1 = true);

    // Retourne le nombre de coups possibles (colonnes non pleines)
    unsigned Nbcoups() const;

    // Évalue la position : gagnant ou partie nulle
    Resultat Eval() const;

    // Applique un coup sur la colonne donnée (0 à 6)
    void EffectuerCoup(unsigned i);

    // Affiche la position sous forme de chaîne
    std::string ToString() const;

    // Compare deux positions (utilisé pour JMCTS2)
    bool operator==(const P4& other) const;
};