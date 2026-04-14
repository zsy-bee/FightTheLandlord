#include "Gamer.h"

using namespace std;

Gamer Gamer::clone() {
    Gamer res;
    res.levels = levels;
    res.singles = singles;
    res.pairs = pairs;
    res.triples = triples;
    res.bombs = bombs;
    res.straights = straights;
    res.consec_pairs = consec_pairs;
    res.planes = planes;
    res.has_rocket = has_rocket;
    return res;
}

void Gamer::analyze() {
    clearMelds();
    // 基础牌型：3-A 和 2
    for (int i = 0; i <= 12; ++i) {
        Level lvl = static_cast<Level>(i);
        int cnt = static_cast<int>(levels[i].size());
        if (cnt >= 1) singles.push_back({ lvl });
        if (cnt >= 2) pairs.push_back({ lvl });
        if (cnt >= 3) triples.push_back({ lvl });
        if (cnt == 4) bombs.push_back({ lvl });
    }

    // 单顺：min_count=1, min_length=5, 范围 0-11
    for (auto& seg : findSegments(1, 5, 0, 11)) {
        std::vector<Level> levs;
        for (int i = seg.first; i <= seg.second; ++i) {
            levs.push_back(static_cast<Level>(i));
        }
        straights.push_back({ levs });
    }

    // 连对：min_count=2, min_length=3, 范围 0-11
    for (auto& seg : findSegments(2, 3, 0, 11)) {
        std::vector<Level> levs;
        for (int i = seg.first; i <= seg.second; ++i) {
            Level lvl = static_cast<Level>(i);
            levs.push_back(lvl);
            levs.push_back(lvl);
        }
        consec_pairs.push_back({ levs });
    }

    // 飞机：min_count=3, min_length=2, 范围 0-11
    for (auto& seg : findSegments(3, 2, 0, 11)) {
        std::vector<Level> levs;
        for (int i = seg.first; i <= seg.second; ++i) {
            Level lvl = static_cast<Level>(i);
            levs.push_back(lvl);
            levs.push_back(lvl);
            levs.push_back(lvl);
        }
        planes.push_back({ levs });
    }

    // 火箭
    has_rocket = (levels[level_joker].size() > 0 && levels[level_JOKER].size() > 0);
}

std::vector<std::pair<int, int>> Gamer::findSegments(int min_count, int min_length,
    int range_start, int range_end) {
    std::vector<std::pair<int, int>> result;
    int start = -1;

    for (int i = range_start; i <= range_end; ++i) {
        if (levels[i].size() >= static_cast<size_t>(min_count)) {
            if (start == -1) start = i;
        }
        else {
            if (start != -1 && i - start >= min_length) {
                result.push_back({ start, i - 1 });
            }
            start = -1;
        }
    }
    if (start != -1 && range_end + 1 - start >= min_length) {
        result.push_back({ start, range_end });
    }
    return result;
}

int Gamer::simulatePlay(Gamer& hand, std::vector<Meld>& move) {
    Gamer copy = hand.clone();   // 克隆
    for (auto& meld : move) {
        meld.play(copy);
    }
    copy.analyze();              // 重新分析

    int broke_bomb = (copy.bombs.size() < hand.bombs.size());
    int broke_plane = (copy.planes.size() < hand.planes.size());
    int straights_lost = hand.straights.size() - copy.straights.size();

    int cost = 0;
    if (broke_bomb) cost += 500;
    if (broke_plane) cost += 300;
    int before_straight_power = 0;
    for (auto& s : hand.straights) {
        before_straight_power += s.power();
    }
    int after_straight_power = 0;
    for (auto& s : copy.straights) {
        after_straight_power += s.power();
    }
    cost += (before_straight_power - after_straight_power);  // 牌力下降就是代价

    // 3. 单张数量增加也是一种代价
    int singles_increased = static_cast<int>(copy.singles.size() - hand.singles.size());
    if (singles_increased > 0) {
        cost += singles_increased * 30;
    }

    return cost;
}