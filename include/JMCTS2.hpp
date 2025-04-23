#pragma once
#include "projet.hpp"
#include <ctime>
#include <cstdlib>

// Joueur MCTS amélioré avec réutilisation de l'arbre de recherche
template <typename P>
class JMCTS2 : public Joueur<P>
{
    double a;
    unsigned temps;
    noeud<P>* racine = nullptr;

public:
    JMCTS2(double a, unsigned temps) : a(a), temps(temps) {
        this->nom = "JMCTS2(a=" + std::to_string(a) + ",temps=" + std::to_string(temps) + ")";
    }

    unsigned Jouer(P p) override {
        // Étape 1 : réutilisation du sous-arbre correspondant au coup adverse
        if (racine != nullptr) {
            noeud<P>* sous = TrouverSousArbre(p);
            if (sous) {
                sous->pere = nullptr;
                delete racine;
                racine = sous;
            } else {
                delete racine;
                racine = nullptr;
            }
        }

        if (racine == nullptr)
            racine = new noeud<P>(nullptr, p);

        std::clock_t t1 = std::clock() + temps * (CLOCKS_PER_SEC / 1000);
        unsigned iter = 0;

        while (std::clock() < t1) {
            // Étape 2 : simulation
            noeud<P>* no = racine;
            while (no->cross > 0 && no->nbfils > 0) {
                no->CalculMeilleurFils(a);
                no = no->MeilleurFils();
            }

            unsigned re = JeuHasard(no->p);

            // Étape 3 : rétropropagation
            while (no != nullptr) {
                no->cross += 2;
                no->win += re;
                no = no->pere;
            }
            ++iter;
        }

#ifdef JMCTS2_DEBUG
        std::cout << "[JMCTS2] " << iter << " itérations, noeuds actifs : " << noeud<P>::compt << std::endl;
#endif

        racine->CalculMeilleurFils(a);
        this->message = std::to_string(iter) + " itérations\n" + racine->ToString();
        return racine->indiceMeilleurFils;
    }

    void FinPartie() override {
        delete racine;
        racine = nullptr;
    }

private:
    noeud<P>* TrouverSousArbre(const P& p) {
        for (unsigned i = 0; i < racine->nbfils; ++i)
            if (racine->fils[i] && racine->fils[i]->p == p)
                return racine->fils[i];
        return nullptr;
    }

    unsigned JeuHasard(P p) const {
        unsigned re = 1;
        while (p.Nbcoups() > 0)
            p.EffectuerCoup(rand() % p.Nbcoups());
        if (p.Eval() == Resultat::j1gagne) re = 2;
        else if (p.Eval() == Resultat::j0gagne) re = 0;
        return re;
    }
};