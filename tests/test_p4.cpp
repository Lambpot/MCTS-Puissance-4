#include "P4.hpp"
#include "JHP4.hpp"
#include "projet.hpp"

int main() {
    // JHP4 joueurHumain;
    JMCTS<P4> j1(1.1, 100);
    JMCTS<P4> j2(1.1, 100);
    //Partie<P4> part(j1, joueurHumain, P4(true));
    Partie<P4> part(j1, j2, P4(true));
    part.Commencer();

    // std::cout << "Résultat final (via pCourante.Eval) : ";
    // switch (part.r) {
    //     case Resultat::j1gagne: std::cout << "j1 a gagné\n"; break;
    //     case Resultat::j0gagne: std::cout << "j0 a gagné\n"; break;
    //     case Resultat::partieNulle: std::cout << "Partie nulle\n"; break;
    //     case Resultat::enCours: std::cout << "Encore en cours\n"; break;
    // }
}