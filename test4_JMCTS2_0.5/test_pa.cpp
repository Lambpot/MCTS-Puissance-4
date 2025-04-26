#include "projet.cpp"

int main()
{
  JMCTS<PA> j1(1.1,1), j0(1.1,1);
  Partie<PA> part(j1,j0, PA(100,true));
  part.Commencer();
}
