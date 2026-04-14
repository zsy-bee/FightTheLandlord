#include "Gamer.h"
#include <iostream>





int main() {
   
    Gamer hand;
    // 4张3: CardNum 0,1,2,3
    hand.addCards({ 0, 1, 2, 3 });
  
    hand.analyze();
    std::cout << "=== 牌型分析结果 ===\n";
    std::cout << "单张数量: " << hand.singles.size() << "\n";
    std::cout << "对子数量: " << hand.pairs.size() << "\n";
    std::cout << "三张数量: " << hand.triples.size() << "\n";
    std::cout << "炸弹数量: " << hand.bombs.size() << "\n";
    std::cout << "顺子数量: " << hand.straights.size() << "\n";
    std::cout << "连对数量: " << hand.consec_pairs.size() << "\n";
    std::cout << "飞机数量: " << hand.planes.size() << "\n";
    std::cout << "是否有火箭: " << (hand.has_rocket ? "是" : "否") << "\n";
    std::cout << "\n--- 单张 ---\n";
    for (auto& s : hand.singles) {
        std::cout << "Level " << s.level << "\n";
    }

    std::cout << "\n--- 对子 ---\n";
    for (auto& p : hand.pairs) {
        std::cout << "Level " << p.level << "\n";
    }

    std::cout << "\n--- 三张 ---\n";
    for (auto& t : hand.triples) {
        std::cout << "Level " << t.level << "\n";
    }

    std::cout << "\n--- 炸弹 ---\n";
    for (auto& b : hand.bombs) {
        std::cout << "Level " << b.level << "\n";
    }

    std::cout << "\n--- 顺子 ---\n";
    for (auto& s : hand.straights) {
        std::cout << "长度: " << s.levels.size() << ", 最大点数: " << s.levels.back() << "\n";
    }

    std::cout << "\n--- 连对 ---\n";
    for (auto& c : hand.consec_pairs) {
        std::cout << "长度: " << c.levels.size() << ", 最大点数: " << c.levels.back() << "\n";
    }

    std::cout << "\n--- 飞机 ---\n";
    for (auto& p : hand.planes) {
        std::cout << "长度: " << p.levels.size() << ", 最大点数: " << p.levels.back() << "\n";
    }
    std::vector<Meld> moves = { Triple{0} };
    int cost = hand.simulatePlay(hand, moves);

    std::cout << "测试1 - 拆炸弹代价: " << cost << " (预期 >= 500)\n";
}