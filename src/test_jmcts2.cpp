#include "../include/projet.hpp"
#include "../include/JMCTS2.hpp"
#include <vector>
#include <iostream>

/**
 * @brief Organise un championnat entre plusieurs joueurs JMCTS2<PA>
 * Chaque joueur affronte tous les autres deux fois (aller-retour) pendant plusieurs tours.
 */
void Championnat(std::vector<Joueur<PA>*>& joueurs, unsigned nbTours) {
    size_t n = joueurs.size();
    std::vector<unsigned> scores(n, 0);

    for (unsigned tour = 0; tour < nbTours; ++tour) {
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                if (i == j) continue;

                // Partie 1 : joueur i commence
                Partie<PA> p1(*joueurs[i], *joueurs[j], PA(20, true), false);
                p1.Commencer();
                if (p1.r == Resultat::j1gagne) scores[i]++;
                else if (p1.r == Resultat::j0gagne) scores[j]++;

                // Partie 2 : joueur j commence
                Partie<PA> p2(*joueurs[j], *joueurs[i], PA(20, true), false);
                p2.Commencer();
                if (p2.r == Resultat::j1gagne) scores[j]++;
                else if (p2.r == Resultat::j0gagne) scores[i]++;
            }
        }
    }

    std::cout << "\nRésultats du championnat après " << nbTours << " tours :\n";
    for (size_t i = 0; i < n; ++i) {
        std::cout << joueurs[i]->nom << " : " << scores[i] << " points\n";
    }
}

/**
 * @brief Point d'entrée du test : compare plusieurs JMCTS2 avec différents paramètres 'a'
 */
int main() {
    std::vector<Joueur<PA>*> joueurs = {
        new JMCTS2<PA>(1, 10),
        new JMCTS2<PA>(2, 10),
        new JMCTS2<PA>(3, 10),
        new JMCTS2<PA>(5, 10)
    };

    Championnat(joueurs, 3); // Chaque joueur joue 2x contre chaque autre, pendant 3 tours

    for (auto j : joueurs) delete j;
}