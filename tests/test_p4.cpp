#include "P4.hpp"
#include "JHP4.hpp"
#include "projet.hpp"

int main() {
    JHP4 joueurHumain;
    JMCTS<P4> ia(1.1, 100);
    Partie<P4> part(ia, joueurHumain, P4(true));
    part.Commencer();
}