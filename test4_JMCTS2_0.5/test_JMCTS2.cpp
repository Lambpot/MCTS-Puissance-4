#include "projet.hpp"
#include "p4.hpp"
#include <vector>
#include <iostream>
#include <iomanip>

int main() {
    unsigned temps = 1000;
    Joueur<P4>* ia1 = new JMCTS<P4>(1.4, temps);
    Joueur<P4>* ia2 = new JMCTS2<P4>(1.4, temps);
    std::vector<Joueur<P4>*> joueurs = {ia1, ia2};

    unsigned nbTours = 1;
    std::vector<unsigned> scores(2, 0);
    unsigned n = joueurs.size();
    unsigned total_coups = 0;
    unsigned totalGamesPlayed = 0;

    unsigned totalMatches = nbTours * n * (n - 1) * 2;
    unsigned currentMatch = 0;

    unsigned long long sim1 = dynamic_cast<JMCTS<P4>*>(ia1)->getNbSimulations();
    unsigned long long sim2 = dynamic_cast<JMCTS2<P4>*>(ia2)->getNbSimulations();

    std::cout << "Progression : 0.00% (0/" << totalMatches << ")" << std::endl;
    std::cout << "====== Simulations ======" << std::endl;
    std::cout << ia1->nom << " : " << sim1 << " simulations" << std::endl;
    std::cout << ia2->nom << " : " << sim2 << " simulations" << std::endl;

    for (unsigned tour = 0; tour < nbTours; ++tour) {
        for (unsigned i = 0; i < n; ++i) {
            for (unsigned j = 0; j < n; ++j) {
                if (i == j) continue;
                for (unsigned swap = 0; swap < 2; ++swap) {
                    P4 etat_initial;
                    Joueur<P4>* premier = (swap == 0) ? joueurs[i] : joueurs[j];
                    Joueur<P4>* second = (swap == 0) ? joueurs[j] : joueurs[i];

                    Partie<P4> partie(*premier, *second, etat_initial, false);
                    partie.Commencer();

                    currentMatch++;

                    unsigned long long sim1 = dynamic_cast<JMCTS<P4>*>(ia1)->getNbSimulations();
                    unsigned long long sim2 = dynamic_cast<JMCTS2<P4>*>(ia2)->getNbSimulations();

                    double progress = (100.0 * currentMatch) / totalMatches;
                    std::cout << "\rProgression : " << std::fixed << std::setprecision(2)
                              << progress << "% (" << currentMatch << "/" << totalMatches << ")" << std::endl;

                    std::cout << "====== Simulations ======" << std::endl;
                    std::cout << ia1->nom << " : " << sim1 << " simulations" << std::endl;
                    std::cout << ia2->nom << " : " << sim2 << " simulations" << std::endl;

                    unsigned coups_this_game = 0;
                    P4 temp = etat_initial;
                    while (temp.Nbcoups() > 0) {
                        temp.EffectuerCoup(0);
                        coups_this_game++;
                    }
                    total_coups += coups_this_game;

                    if (partie.r == Resultat::j1gagne) {
                        scores[(swap == 0) ? i : j]++;
                    }
                    else if (partie.r == Resultat::j0gagne) {
                        scores[(swap == 0) ? j : i]++;
                    }
                    totalGamesPlayed++;
                }
            }
        }
    }

    std::cout << std::endl;

    double winrate_jmcts  = (100.0 * scores[0]) / totalGamesPlayed;
    double winrate_jmcts2 = (100.0 * scores[1]) / totalGamesPlayed;
    double moyenne_jmcts = (double)dynamic_cast<JMCTS<P4>*>(ia1)->getNbSimulations() / total_coups;
    double moyenne_jmcts2 = (double)dynamic_cast<JMCTS2<P4>*>(ia2)->getNbSimulations() / total_coups;
    double taux_reuse = (100.0 * dynamic_cast<JMCTS2<P4>*>(ia2)->getNbReuse()) / dynamic_cast<JMCTS2<P4>*>(ia2)->getNbTotalTurns();

    std::cout << "====== Résultats ======" << std::endl;
    std::cout << ia1->nom << " : " << std::fixed << std::setprecision(2) << winrate_jmcts << "% de victoire" << std::endl;
    std::cout << ia2->nom << " : " << std::fixed << std::setprecision(2) << winrate_jmcts2 << "% de victoire" << std::endl;

    // std::cout << "\n====== Reuse ======" << std::endl;
    // std::cout << ia1->nom << " : 0% reuse" << std::endl;
    // std::cout << ia2->nom << " : " << std::fixed << std::setprecision(2) << taux_reuse << "% reuse" << std::endl;

    std::cout << "\n====== Simulations par coup ======" << std::endl;
    std::cout << ia1->nom << " : " << std::fixed << std::setprecision(2) << moyenne_jmcts << " simulations par coup" << std::endl;
    std::cout << ia2->nom << " : " << std::fixed << std::setprecision(2) << moyenne_jmcts2 << " simulations par coup" << std::endl;

    std::cout << "\n====== Simulations totals ======" << std::endl;
    std::cout << ia1->nom << " : " << dynamic_cast<JMCTS<P4>*>(ia1)->getNbSimulations() << " simulations" << std::endl;
    std::cout << ia2->nom << " : " << dynamic_cast<JMCTS2<P4>*>(ia2)->getNbSimulations() << " simulations" << std::endl;

    delete ia1;
    delete ia2;

    return 0;
}