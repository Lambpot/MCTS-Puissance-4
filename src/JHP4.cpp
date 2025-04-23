#include "JHP4.hpp"

unsigned JHP4::Jouer(P4 p)
{
    std::cout << p.ToString() << std::endl; // Affiche l'état actuel du plateau
    unsigned coup;

    while (true)
    {
        std::cout << "\nVotre tour (Joueur Humain - X) :\n";
        std::cout << "Entrez un numéro de colonne (0–6) : "; // Invite utilisateur
        
        if (!(std::cin >> coup)) {
            // Gérer les entrées non numériques
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Entrée invalide. Veuillez entrer un chiffre entre 0 et 6.\n";
            continue;
        }

        if (coup < P4::COLS && p.hauteur[coup] < P4::LIGNES)
            break;

        std::cout << "Colonne invalide ou pleine. Réessayez." << std::endl;
    }

    return coup;
}
