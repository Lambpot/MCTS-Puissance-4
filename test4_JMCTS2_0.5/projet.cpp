#include "projet.hpp"

std::random_device rd;
std::default_random_engine e(rd());

PA::PA(unsigned n, bool j1aletrait): j1aletrait(j1aletrait), nba(n){ Maj_nbcoups();}

void PA::Eval(Resultat r){ eval = r;}

void PA::Nbcoups(unsigned n){ nbcoups=n;}

void PA::Maj_nbcoups() { Nbcoups((nba >=3)? 3:nba);}

unsigned PA::Nbcoups(){ return nbcoups;}

Resultat PA::Eval() const { return eval;}

void PA::EffectuerCoup(unsigned i)
{
  nba = nba - i -1; Maj_nbcoups();
  if (nba==0) Eval(j1aletrait? Resultat::j0gagne : Resultat::j1gagne);
  j1aletrait = !j1aletrait;
}

std::string PA::ToString() const
{
  return (j1aletrait? "j1 a le trait": "j0 a le trait")
  + (" nb alu = " + std::to_string(nba));
}

