#include "projet.hpp"
#include <iostream>

// Fonction principale : deux joueurs JMCTS s'affrontent sur le jeu des allumettes
int main()
{
  std::cout << "Début de la partie : JMCTS<PA> vs JMCTS<PA>\n";

  JMCTS<PA> j1(1.1, 1); // Joueur j1
  JMCTS<PA> j2(1.1, 1); // Joueur j0

  Partie<PA> part(j1, j2, PA(500, true));
  part.Commencer();
}