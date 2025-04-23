#pragma once
#include "P4.hpp"
#include "projet.hpp"
#include <iostream>

// Classe représentant un joueur humain pour le jeu Puissance 4
class JHP4 : public Joueur<P4>
{
public:
    explicit JHP4() { this->nom = "Humain"; }

    [[nodiscard]] unsigned Jouer(P4 p) override;
};
