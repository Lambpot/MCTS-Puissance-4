#pragma once

#include "projet.hpp"
#include "p4.hpp"

// 人类玩家类
class JHP4 : public Joueur<P4> {
public:
    JHP4();
    unsigned Jouer(P4 p) override;
}; 