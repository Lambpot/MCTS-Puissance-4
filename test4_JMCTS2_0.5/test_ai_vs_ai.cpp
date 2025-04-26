#include "projet.hpp"
#include "p4.hpp"
#include "jhp4.hpp"
#include <memory>

int main() {
    // 创建游戏状态
    P4 game;
    
    // 创建人类玩家和AI玩家
    JMCTS<P4> ai1(1.4, 100);  // a=1.4, 思考时间100ms
    JMCTS<P4> ai2(1.4, 100);  // a=1.4, 思考时间100ms
    
    // 创建游戏
    Partie<P4> partie(ai1, ai2, game, true);
    
    // 开始游戏
    partie.Commencer();
    
    return 0;
} 