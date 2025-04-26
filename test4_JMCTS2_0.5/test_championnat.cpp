#include "projet.hpp"
#include "p4.hpp"
#include <vector>

int main() {
    // 创建 AI 玩家，a=1到5，思考时间1ms
    std::vector<Joueur<P4>*> joueurs;
    joueurs.push_back(new JMCTS<P4>(1, 1));
    joueurs.push_back(new JMCTS<P4>(5, 1));
    joueurs.push_back(new JMCTS<P4>(10, 1));
    joueurs.push_back(new JMCTS<P4>(15, 1));
    joueurs.push_back(new JMCTS<P4>(20, 1));

    unsigned nbTours = 20; // 比赛轮数，自己可以改
    Championnat(joueurs, nbTours); // 开始比赛

    // 比赛结束后释放内存
    for (auto joueur : joueurs) {
        delete joueur;
    }

    return 0;
}