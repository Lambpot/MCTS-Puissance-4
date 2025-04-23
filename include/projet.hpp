#pragma once
#include <string>
#include <iostream>
#include <ctime>
#include <random>

/**
 * @brief Énumération représentant le résultat d'une partie.
 * 
 * - j1gagne : le joueur 1 a gagné
 * - j0gagne : le joueur 0 a gagné
 * - partieNulle : match nul ou non déterminé
 */
enum class Resultat { j1gagne, j0gagne, partieNulle, enCours};

/**
 * @brief Nœud de l'arbre de recherche pour l'algorithme MCTS.
 * 
 * Contient un état de jeu, les statistiques de simulation, les pointeurs vers les enfants,
 * et les informations pour sélectionner le meilleur fils via UCB.
 */
template <typename P>
struct noeud {
    noeud<P>* pere;        // Pointeur vers le noeud parent
    noeud<P>** fils;       // Tableau de pointeurs vers les noeuds fils
    unsigned nbfils;       // Nombre de fils
    P p;                   // Position du jeu à ce noeud
    unsigned cross, win;   // Nombre de simulations (cross) et de victoires (win)
    unsigned indiceMeilleurFils; // Indice du meilleur fils
    static unsigned compt; // Compteur du nombre de noeuds

    explicit noeud(noeud<P>* pere, P p);
    ~noeud();
    std::string ToString() const;
    void CalculMeilleurFils(double a);
    noeud<P>* MeilleurFils();
};

template <typename P> unsigned noeud<P>::compt{0};

template <typename P>
noeud<P>::noeud(noeud<P>* pere, P p) : pere(pere), p(p), cross(0), win(0) {
    ++compt;
    nbfils = p.Nbcoups();
    fils = new noeud<P>*[nbfils];
    for (unsigned i = 0; i < nbfils; ++i) fils[i] = nullptr;
}

template <typename P>
noeud<P>::~noeud() {
    for (unsigned i = 0; i < nbfils; ++i) delete fils[i];
    delete[] fils;
    --compt;
}

template <typename P>
noeud<P>* noeud<P>::MeilleurFils() {
    if (fils[indiceMeilleurFils] != nullptr) return fils[indiceMeilleurFils];
    P q = p;
    q.EffectuerCoup(indiceMeilleurFils);
    fils[indiceMeilleurFils] = new noeud<P>(this, q);
    return fils[indiceMeilleurFils];
}

template <typename P>
void noeud<P>::CalculMeilleurFils(double a) {
    double s, sM = 0;
    if (p.j1aletrait) {
        for (unsigned i = 0; i < nbfils; ++i) {
            s = (fils[i] == nullptr) ? 1 : (fils[i]->win + a) / (fils[i]->cross + a);
            if (s > sM) { sM = s; indiceMeilleurFils = i; }
        }
    } else {
        for (unsigned i = 0; i < nbfils; ++i) {
            s = (fils[i] == nullptr) ? 1 : (fils[i]->cross - fils[i]->win + a) / (fils[i]->cross + a);
            if (s > sM) { sM = s; indiceMeilleurFils = i; }
        }
    }
}

template <typename P>
std::string noeud<P>::ToString() const {
    std::string s = "indice MF = " + std::to_string(indiceMeilleurFils);
    s += "\n note=";
    if (fils[indiceMeilleurFils])
        s += std::to_string(1.0 * fils[indiceMeilleurFils]->win / fils[indiceMeilleurFils]->cross);
    else s += "?";
    s += "\n nbC=" + std::to_string(cross / 2);
    s += "\n" + std::to_string(compt);
    return s;
}

/**
 * @brief Classe abstraite représentant un joueur (humain ou IA).
 * 
 * Fournit une interface commune utilisée par la classe Partie et les implémentations de joueurs.
 */
template <typename P>
class Joueur {
public:
    std::string nom;      // Nom du joueur
    virtual unsigned Jouer(P p) = 0;   // Fonction pour jouer un coup
    virtual void FinPartie() {}        // Fonction appelée en fin de partie
    std::string message;               // Message d'information après un coup
};

/**
 * @brief Implémentation d'un joueur IA utilisant MCTS (Monte Carlo Tree Search).
 * 
 * Effectue une série de simulations aléatoires pour choisir le meilleur coup.
 * Les résultats sont rétropropagés dans l'arbre de recherche.
 */
template <typename P>
class JMCTS : public Joueur<P> {
    double a;           // Paramètre pour l'exploration
    unsigned temps;     // Temps de réflexion en millisecondes
    noeud<P>* racine;   // Racine de l'arbre de recherche
public:
    JMCTS(double a, unsigned temps) : a(a), temps(temps) {
        this->nom = "JMCTS(a=" + std::to_string(a) + ",temps=" + std::to_string(temps) + ")";
    }

    // Joue une partie aléatoire depuis la position p et retourne le résultat (2: j1 gagne, 0: j0 gagne, 1: nul)
    unsigned JeuHasard(P p) const {
        unsigned re = 1;
        while (p.Nbcoups() > 0) p.EffectuerCoup(rand() % p.Nbcoups());
        if (p.Eval() == Resultat::j1gagne) re = 2;
            
        else if (p.Eval() == Resultat::j0gagne) re = 0;
        return re;
    }

    // Choisit le meilleur coup selon MCTS
    unsigned Jouer(P p) override {
        std::clock_t t1 = std::clock() + temps * (CLOCKS_PER_SEC / 1000);
        racine = new noeud<P>(nullptr, p);
        unsigned iter = 0;

        while (std::clock() < t1) {
            noeud<P>* no = racine;
            do {
                no->CalculMeilleurFils(a);
                no = no->MeilleurFils();
            } while (no->cross > 0 && no->nbfils > 0);
            unsigned re = JeuHasard(no->p);
            while (no != nullptr) {
                no->cross += 2;
                no->win += re;
                no = no->pere;
            }
            ++iter;
        }

        racine->CalculMeilleurFils(a);
        this->message = std::to_string(iter) + " itérations\n" + racine->ToString();
        unsigned res = racine->indiceMeilleurFils;
        delete racine;
        return res;
    }
};

/**
 * @brief Contrôle le déroulement d'une partie entre deux joueurs.
 * 
 * Alterne les tours entre les deux joueurs jusqu'à la fin de la partie,
 * affiche les états si souhaité et annonce le résultat.
 */
template <typename P>
class Partie {
    P pCourante;           // Position courante
    Joueur<P>& j1;         // Joueur 1
    Joueur<P>& j0;         // Joueur 0
public:
    bool affichage;        // Affichage activé/désactivé
    Resultat r;            // Résultat final
    Partie(Joueur<P>& j1, Joueur<P>& j0, P pInitiale, bool aff = true)
        : j1(j1), j0(j0), pCourante(pInitiale), affichage(aff) {}

    // Lance la partie
    void Commencer() {
        if (affichage) std::cout << pCourante.ToString() << std::endl;
        while (pCourante.Nbcoups() > 0) {
            if (pCourante.j1aletrait) {
                if (affichage) std::cout << j1.nom << "(j1) joue\n";
                pCourante.EffectuerCoup(j1.Jouer(pCourante));
                if (affichage) std::cout << j1.message << std::endl;
            } else {
                if (affichage) std::cout << j0.nom << "(j0) joue\n";
                pCourante.EffectuerCoup(j0.Jouer(pCourante));
                if (affichage) std::cout << j0.message << std::endl;
            }
            if (affichage) std::cout << pCourante.ToString() << std::endl;
        }
        r = pCourante.Eval();
        if (affichage) {
            switch (r) {
                case Resultat::j1gagne: std::cout << "j1 a gagné\n"; break;
                case Resultat::j0gagne: std::cout << "j0 a gagné\n"; break;
                case Resultat::partieNulle: std::cout << "Partie nulle\n"; break;
            }
        }
        j1.FinPartie(); j0.FinPartie();
    }
};

/**
 * @brief Jeu d'exemple simple : le jeu des allumettes.
 * 
 * Chaque joueur peut retirer 1 à 3 allumettes. Le joueur qui retire la dernière perd.
 * Utilisé pour tester et valider l'algorithme MCTS sans complexité graphique.
 */
class PA {
    unsigned nba;       // Nombre d'allumettes restantes
    unsigned nbcoups;   // Nombre de coups possibles
    Resultat eval;      // Résultat courant
    void Eval(Resultat r) { eval = r; }
    void Nbcoups(unsigned n) { nbcoups = n; }
    void Maj_nbcoups() { Nbcoups((nba >= 3) ? 3 : nba); }
public:
    bool j1aletrait;    // Indique si c'est au tour de j1
    PA(unsigned n = 20, bool j1aletrait = true) : j1aletrait(j1aletrait), nba(n) {
        Maj_nbcoups();
    }
    unsigned Nbcoups() const { return nbcoups; }
    Resultat Eval() const { return eval; }
    void EffectuerCoup(unsigned i) {
        nba = nba - i - 1; Maj_nbcoups();
        if (nba == 0) Eval(j1aletrait ? Resultat::j0gagne : Resultat::j1gagne);
        j1aletrait = !j1aletrait;
    }
    std::string ToString() const {
        return (j1aletrait ? "j1 a le trait" : "j0 a le trait") +
               (" | nb allumettes = " + std::to_string(nba));
    }
    bool operator==(const PA& other) const {
        return nba == other.nba && j1aletrait == other.j1aletrait;
    }
};