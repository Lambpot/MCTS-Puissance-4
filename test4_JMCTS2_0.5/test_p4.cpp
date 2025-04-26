#include <iostream>
#include "projet.hpp"
#include "p4.hpp"
#include "jhp4.hpp"

int main() {
    // 创建游戏状态
    P4 game;
    
    // 创建人类玩家和AI玩家
    JHP4 human;
    JMCTS<P4> ai(1.4, 1000);  // a=1.4, 思考时间1000ms
    
    // 创建游戏
    Partie<P4> partie(human, ai, game, true);
    
    // 开始游戏
    partie.Commencer();
    
    return 0;
} 