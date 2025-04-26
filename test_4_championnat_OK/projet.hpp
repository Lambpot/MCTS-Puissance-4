#pragma once

#include <string>
#include <random>
#include <iostream>
#include <ctime>
#include "resultat.hpp"

extern std::random_device rd;
extern std::default_random_engine e;

template <typename P>
struct noeud
{
  noeud<P> *pere;
  noeud<P> **fils;
  unsigned nbfils;
  P p;
  unsigned cross, win;
  unsigned indiceMeilleurFils;

  static unsigned compt;

  explicit noeud(noeud<P> *pere, P p) : pere(pere), p(p), cross{0}, win{0}
  {
    ++compt;
    nbfils = p.Nbcoups();
    fils = new noeud<P> *[nbfils];
    for (unsigned i = 0; i < nbfils; ++i)
      fils[i] = nullptr;
  }

  ~noeud()
  {
    for (unsigned i = 0; i < nbfils; ++i)
    {
      delete fils[i];
    }
    delete[] fils;
    --compt;
  }

  std::string ToString() const
  {
    std::string s = "";
    s = s + "indice MF = " + std::to_string(indiceMeilleurFils);
    double note;
    s += "\n note=";
    if (fils[indiceMeilleurFils])
    {
      note = (1.0 * fils[indiceMeilleurFils]->win) / fils[indiceMeilleurFils]->cross;
      s += std::to_string(note);
    }
    else
      s += "?";
    s += "\n nbC=" + std::to_string(cross / 2);
    s += "\n" + std::to_string(compt);
    return s;
  }

  void CalculMeilleurFils(double a)
  {
    double s;
    double sM = 0;
    if (p.j1aletrait)
    {
      for (unsigned i = 0; i < nbfils; ++i)
      {
        if (fils[i] == nullptr)
        {
          s = 1;
        }
        else
        {
          s = (fils[i]->win + a) / (fils[i]->cross + a);
        }
        if (s > sM)
        {
          sM = s;
          indiceMeilleurFils = i;
        }
      }
    }
    else
    {
      for (unsigned i = 0; i < nbfils; ++i)
      {
        if (fils[i] == nullptr)
        {
          s = 1;
        }
        else
        {
          s = (fils[i]->cross - fils[i]->win + a) / (fils[i]->cross + a);
          ;
        }
        if (s > sM)
        {
          sM = s;
          indiceMeilleurFils = i;
        }
      }
    }
  }

  noeud<P> *MeilleurFils()
  {
    if (fils[indiceMeilleurFils] != nullptr)
    {
      return fils[indiceMeilleurFils];
    }
    P q = p;
    q.EffectuerCoup(indiceMeilleurFils);
    fils[indiceMeilleurFils] = new noeud<P>(this, q);
    return fils[indiceMeilleurFils];
  }
};

template <typename P>
unsigned noeud<P>::compt{0};

template <typename P>
class Joueur
{
public:
  virtual ~Joueur() {}//刚加了个虚析构函数 
  std::string nom;
  virtual unsigned Jouer(P p) = 0;
  virtual void FinPartie() {}
  std::string message;
};

template <typename P>
class JMCTS : public Joueur<P>
{
  double a;
  unsigned temps; // temps de réflexion
  noeud<P> *racine;

public:
  JMCTS(double a, unsigned temps) : a(a), temps(temps)
  {
    this->nom = "JMCTS(a=" + std::to_string(a) + ",temps=" + std::to_string(temps) + ")";
  }

  unsigned JeuHasard(P p) const
  {
    unsigned re = 1;
    while (p.Nbcoups() > 0)
    {
      p.EffectuerCoup(e() % p.Nbcoups());
    }
    if (p.Eval() == Resultat::j1gagne)
    {
      re = 2;
    }
    if (p.Eval() == Resultat::j0gagne)
    {
      re = 0;
    }
    return re;
  }

  unsigned Jouer(P p) override
  {
    std::clock_t t0 = std::clock();
    std::clock_t t1 = t0 + temps * (CLOCKS_PER_SEC / 1000);

    racine = new noeud<P>(nullptr, p);
    unsigned iter = 0;
    while (std::clock() < t1)
    {
      noeud<P> *no = racine;

      do // Sélection
      {
        no->CalculMeilleurFils(a);
        no = no->MeilleurFils();

      } while (no->cross > 0 && no->nbfils > 0);

      unsigned re = JeuHasard(no->p); // Simulation

      while (no != nullptr) // Rétropropagation
      {
        no->cross += 2;
        no->win += re;
        no = no->pere;
      }
      ++iter;
    }
    racine->CalculMeilleurFils(a);

    this->message = std::to_string(iter) + " itérations\n";
    this->message += racine->ToString();

    unsigned res = racine->indiceMeilleurFils;
    delete racine;
    return res;
  }
};

template <typename P>
class Partie
{
  P pCourante;
  Joueur<P> &j1, &j0;

public:
  bool affichage;
  Resultat r;
  Partie(Joueur<P> &j1, Joueur<P> &j0, P pInitiale, bool aff = true) : j1(j1), j0(j0), pCourante(pInitiale), affichage(aff) {}

  void Commencer()
  {
    if (affichage)
      std::cout << pCourante.ToString() << std::endl;
    while (pCourante.Nbcoups() > 0)
    {
      if (pCourante.j1aletrait)
      {
        if (affichage)
          std::cout << j1.nom << "(j1) joue" << std::endl;
        pCourante.EffectuerCoup(j1.Jouer(pCourante));
        if (affichage)
          std::cout << j1.message << std::endl;
      }
      else
      {
        if (affichage)
          std::cout << j0.nom << "(j0) joue" << std::endl;
        pCourante.EffectuerCoup(j0.Jouer(pCourante));
        if (affichage)
          std::cout << j0.message << std::endl;
      }
      if (affichage)
        std::cout << pCourante.ToString() << std::endl;
    }
    r = pCourante.Eval();
    if (affichage)
    {
      switch (r)
      {
      case Resultat::j1gagne:
        std::cout << "j1 a gagné" << std::endl;
        break;
      case Resultat::j0gagne:
        std::cout << "j0 a gagné" << std::endl;
        break;
      case Resultat::partieNulle:
        std::cout << "Partie nulle" << std::endl;
        break;
      case Resultat::indetermine:
        std::cout << "Partie indéterminée" << std::endl;
        break;
      }
    }
    j1.FinPartie();
    j0.FinPartie();
  }
};

class PA
{
  unsigned nba;
  unsigned nbcoups;
  Resultat eval;
  void Eval(Resultat r);
  void Nbcoups(unsigned n);
  void Maj_nbcoups();

public:
  bool j1aletrait;
  PA(unsigned n, bool j1aletrait);
  unsigned Nbcoups();
  Resultat Eval() const;
  void EffectuerCoup(unsigned i);
  std::string ToString() const;
};


// ================================================
// =============Fonction ExtraireParamA ===========
// ================================================
inline std::string ExtraireParamA(const std::string& nom) {
    size_t debut = nom.find("(a=");
    size_t fin = nom.find(",", debut);
    if (debut != std::string::npos && fin != std::string::npos) {
        return nom.substr(debut, fin - debut) + ")";
    } else {
        return nom;
    }
}
// ================================================
// =============Fonction Championnat ===========
// ================================================
template <typename P>
void Championnat(std::vector<Joueur<P> *> &v, unsigned nbTours)
{
  unsigned n = v.size(); // 玩家个数

  std::vector<unsigned> scores(n, 0); // 胜场数初始化

  // nbTours 轮比赛
  for (unsigned tour = 0; tour < nbTours; ++tour)
  {
    std::cout << "\n======= Début du Tour " << tour + 1 << " =======\n";

    // 比赛一对玩家两场（交替先手）
    for (unsigned i = 0; i < n; ++i)
    {
      std::cout << "\n--- Début du tour " << (tour+1) << " ---\n";
      for (unsigned j = 0; j < n; ++j)
      {
        if (i == j)
          continue;
        // 输出玩家参数
        std::cout << "joueur " << i+1 << " " << ExtraireParamA(v[i]->nom) << " vs joueur " << j+1 << " " << ExtraireParamA(v[j]->nom) << std::endl;
        for (unsigned swap = 0; swap < 2; ++swap)
        {                 // 先i后j，再j后i
          P etat_initial; // 使用默认构造，不指定true/false

          if (swap == 0)
          { // i 先手
            Partie<P> partie(*v[i], *v[j], etat_initial, false);
            partie.Commencer();
            if (partie.r == Resultat::j1gagne)
              scores[i]++;
            else if (partie.r == Resultat::j0gagne)
              scores[j]++;
          }
          else
          { // j 先手
            Partie<P> partie(*v[j], *v[i], etat_initial, false);
            partie.Commencer();
            if (partie.r == Resultat::j1gagne)
              scores[j]++;
            else if (partie.r == Resultat::j0gagne)
              scores[i]++;
          }

          // 打完一场，输出比分
          std::cout << "Scores actuels:\n";
          for (unsigned k = 0; k < n; ++k)
          {
            std::cout << "- " << v[k]->nom << " : " << scores[k] << " victoires\n";
          }
          std::cout << "-----------------------------\n";
        }
      }
    }
  }

  // 比赛全部完成后，打印最终分数
  std::cout << "\n======= Résultats finaux =======\n";
  for (unsigned i = 0; i < n; ++i)
  {
    std::cout << v[i]->nom << " : " << scores[i] << " victoires\n";
  }
}


