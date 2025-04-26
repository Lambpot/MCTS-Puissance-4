#pragma once

#include <string>
#include <random>
#include <iostream>
#include <iomanip> // 为了设置小数精度
#include <ctime>
#include "resultat.hpp"

extern std::random_device rd;
extern std::default_random_engine e;

// ================================================
// ===============Fonction noeud ==================
// ================================================
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

// ================================================
// ===============Class Joueur =================
// ================================================
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

// ================================================
// ===============Class JMCTS ==================
// ================================================
template <typename P>
class JMCTS : public Joueur<P>
{
  double a;
  unsigned temps; // temps de réflexion
  noeud<P> *racine;

  // 统计模拟次数
  mutable unsigned long long nb_simulations = 0;

public:
  JMCTS(double a, unsigned temps) : a(a), temps(temps)
  {
    this->nom = "JMCTS(a=" + std::to_string(a) + ",temps=" + std::to_string(temps) + ")";
  }

  // 获取模拟次数
  unsigned long long getNbSimulations() const { return nb_simulations; }

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
      ++nb_simulations;
    }
    racine->CalculMeilleurFils(a);

    this->message = std::to_string(iter) + " itérations\n";
    this->message += racine->ToString();

    unsigned res = racine->indiceMeilleurFils;
    delete racine;
    return res;
  }
};

// ================================================
// ===============Class JMCTS2 ==================
// ================================================
template <typename P>
class JMCTS2 : public Joueur<P> {
private:
    double a;
    unsigned temps;
    noeud<P> *racine;
    mutable unsigned long long nb_simulations = 0;
    mutable unsigned long long nb_reuse = 0;
    mutable unsigned long long nb_total_turns = 0;

public:
    JMCTS2(double a, unsigned temps) : a(a), temps(temps), racine(nullptr)
    {
        this->nom = "JMCTS2(a=" + std::to_string(a) + ",temps=" + std::to_string(temps) + ")";
    }

    virtual ~JMCTS2() {
        FinPartie();
    }

    void FinPartie() override {
        if (racine != nullptr) {
            delete racine;
            racine = nullptr;
        }
    }

    unsigned Jouer(P p) override
    {
        ++nb_total_turns;
        if (racine != nullptr) {
            noeud<P>* newRoot = nullptr;
            for (unsigned i = 0; i < racine->nbfils; ++i) {
                if (racine->fils[i] && racine->fils[i]->p == p) {
                    newRoot = racine->fils[i];
                    racine->fils[i] = nullptr;
                    break;
                }
            }
            
            if (newRoot) {
                ++nb_reuse;
                if (racine != newRoot) {
                    delete racine;
                }
                racine = newRoot;
                racine->pere = nullptr;
                // 展开所有合法子节点
                for (unsigned i = 0; i < racine->nbfils; ++i) {
                    if (racine->fils[i] == nullptr) {
                        P q = racine->p;
                        q.EffectuerCoup(i);
                        racine->fils[i] = new noeud<P>(racine, q);
                    }
                }
            } else {
                delete racine;
                racine = new noeud<P>(nullptr, p);
                // 展开所有合法子节点
                for (unsigned i = 0; i < racine->nbfils; ++i) {
                    if (racine->fils[i] == nullptr) {
                        P q = racine->p;
                        q.EffectuerCoup(i);
                        racine->fils[i] = new noeud<P>(racine, q);
                    }
                }
            }
        } else {
            racine = new noeud<P>(nullptr, p);
            // 展开所有合法子节点
            for (unsigned i = 0; i < racine->nbfils; ++i) {
                if (racine->fils[i] == nullptr) {
                    P q = racine->p;
                    q.EffectuerCoup(i);
                    racine->fils[i] = new noeud<P>(racine, q);
                }
            }
        }

        std::clock_t t0 = std::clock();
        std::clock_t t1 = t0 + temps * (CLOCKS_PER_SEC / 1000);

        unsigned iter = 0;
        while (std::clock() < t1)
        {
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
            ++nb_simulations;
        }

        racine->CalculMeilleurFils(a);
        return racine->indiceMeilleurFils;
    }

    unsigned long long getNbSimulations() const { return nb_simulations; }
    unsigned long long getNbReuse() const { return nb_reuse; }
    unsigned long long getNbTotalTurns() const { return nb_total_turns; }

private:
    unsigned JeuHasard(P p)
    {
        unsigned re = 1;
        while (p.Nbcoups() > 0)
        {
            p.EffectuerCoup(rand() % p.Nbcoups());
        }
        if (p.Eval() == Resultat::j0gagne)
        {
            re = 0;
        }
        return re;
    }
};



// template <typename P>
// class JMCTS2 : public Joueur<P> {
//   double a;
//   unsigned temps; // temps de réflexion
//   noeud<P> *racine;

// public:
//   JMCTS2(double a, unsigned temps) : a(a), temps(temps), racine(nullptr)
//   {
//     this->nom = "JMCTS2(a=" + std::to_string(a) + ",temps=" + std::to_string(temps) + ")";
//   }

//   virtual ~JMCTS2() {
//     FinPartie();
//   }

//   unsigned Jouer(P p) override
//   {
//     // 尝试复用旧树
//     if (racine != nullptr) {
//       noeud<P>* newRoot = nullptr;
//       for (unsigned i = 0; i < racine->nbfils; ++i) {
//         if (racine->fils[i] && racine->fils[i]->p == p) { // 要求p有operator==
//           newRoot = racine->fils[i];
//           break;
//         }
//       }
//       if (newRoot) {
//         // 找到对应局面，设为新的根
//         racine = newRoot;
//       } else {
//         // 找不到，旧树无用，释放
//         std::cout << "找不到，旧树无用，释放" << std::endl;
//         delete racine;
//         racine = new noeud<P>(nullptr, p);
//       }
//     } else {
//       // 没有旧树，初始化新树
//       racine = new noeud<P>(nullptr, p);
//     }

//     std::clock_t t0 = std::clock();
//     std::clock_t t1 = t0 + temps * (CLOCKS_PER_SEC / 1000);

//     unsigned iter = 0;
//     while (std::clock() < t1)
//     {
//       noeud<P>* no = racine;
//       do
//       {
//         no->CalculMeilleurFils(a);
//         no = no->MeilleurFils();
//       } while (no->cross > 0 && no->nbfils > 0);

//       unsigned re = JeuHasard(no->p);

//       while (no != nullptr)
//       {
//         no->cross += 2;
//         no->win += re;
//         no = no->pere;
//       }
//       ++iter;
//     }
//     racine->CalculMeilleurFils(a);

//     this->message = std::to_string(iter) + " itérations\n";
//     this->message += racine->ToString();

//     unsigned res = racine->indiceMeilleurFils;
//     return res;
//   }

//   void FinPartie() override
//   {
//     if (racine != nullptr) {
//       delete racine;
//       racine = nullptr;
//     }
//   }

// private:
//   unsigned JeuHasard(P p) const
//   {
//     unsigned re = 1;
//     while (p.Nbcoups() > 0)
//     {
//       p.EffectuerCoup(e() % p.Nbcoups());
//     }
//     if (p.Eval() == Resultat::j1gagne)
//     {
//       re = 2;
//     }
//     if (p.Eval() == Resultat::j0gagne)
//     {
//       re = 0;
//     }
//     return re;
//   }
// };


// ================================================
// ===============Class Partie ==================
// ================================================
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

  unsigned NombreCoupsTotal() const { return pCourante.NombreCoupsTotal(); }
};

// ================================================
// ===============Class PA ==================
// ================================================
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
    unsigned n = v.size();

    if (n < 2) {
        std::cerr << "Erreur : pas assez de joueurs pour le championnat !" << std::endl;
        return;
    }

    std::vector<unsigned> scores(n, 0);
    unsigned totalMatches = 2 * nbTours * n * (n - 1); // 总场数（每对2场）
    unsigned matchesPlayed = 0;

    for (unsigned tour = 0; tour < nbTours; ++tour)
    {
        for (unsigned i = 0; i < n; ++i)
        {
            for (unsigned j = 0; j < n; ++j)
            {
                if (i == j)
                    continue;

                for (unsigned swap = 0; swap < 2; ++swap)
                {
                    P etat_initial;

                    Joueur<P> *premier = (swap == 0) ? v[i] : v[j];
                    Joueur<P> *second = (swap == 0) ? v[j] : v[i];

                    Partie<P> partie(*premier, *second, etat_initial, false);
                    partie.Commencer();

                    if (partie.r == Resultat::j1gagne)
                        scores[(swap == 0) ? i : j]++;
                    else if (partie.r == Resultat::j0gagne)
                        scores[(swap == 0) ? j : i]++;

                    // 更新场次
                    matchesPlayed++;

                    // 简单进度条
                    if (matchesPlayed % (totalMatches / 100) == 0 || matchesPlayed == totalMatches) {
                        int percent = (100 * matchesPlayed) / totalMatches;
                        std::cout << "\r[" << std::setw(3) << percent << "%] "
                                  << matchesPlayed << "/" << totalMatches << " matchs joués" << std::flush;
                    }
                }
            }
        }
    }

    std::cout << "\n\n======= Résultats finaux =======\n";
    unsigned bestScore = 0;
    unsigned championIndex = 0;

    for (unsigned i = 0; i < n; ++i)
    {
        double winrate = (100.0 * scores[i]) / (2 * nbTours * (n - 1));
        std::cout << "- " << ExtraireParamA(v[i]->nom) << " : "
                  << scores[i] << " victoires, "
                  << std::fixed << std::setprecision(2) << winrate << "% de victoire\n";

        if (scores[i] > bestScore) {
            bestScore = scores[i];
            championIndex = i;
        }
    }

    std::cout << "\n🏆 Champion : " << ExtraireParamA(v[championIndex]->nom)
              << " avec " << scores[championIndex] << " victoires ("
              << std::fixed << std::setprecision(2)
              << (100.0 * scores[championIndex]) / (2 * nbTours * (n - 1)) << "% de victoire)\n";
}