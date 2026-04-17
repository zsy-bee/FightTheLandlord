#include "Gamer.h"
#include"ParticleFilter.h"
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

Meld Gamer::recognize(const std::vector<CardNum>& cards) {
    Gamer g;
    g.addCards(cards);
    int total = g.totalCards();
    g.analyze();
    std::cout << "[DEBUG] triples: ";
    for (const auto& t : g.triples) std::cout << t.level << " ";
    std::cout << "\n[DEBUG] singles: ";
    for (const auto& s : g.singles) std::cout << s.level << " ";
    std::cout << "\n[DEBUG] bombs: " << g.bombs.size();
    std::cout << "\n[DEBUG] total: " << total << "\n";
    if (g.singles.size() == 1 && total == 1) {
        return Meld(Single{ g.singles[0].level });
    }

    // 对子
    if (g.pairs.size() == 1 && total == 2) {
        return Meld(Pair{ g.pairs[0].level });
    }

    // 三张
    if (g.triples.size() == 1 && total == 3) {
        return Meld(Triple{ g.triples[0].level });
    }

    // 炸弹
    if (g.bombs.size() == 1 && total == 4) {
        return Meld(Bomb{ g.bombs[0].level });
    }

    // 火箭
    if (g.has_rocket && total == 2) {
        return Meld(Rocket{});
    }

    // 单顺
    if (g.straights.size() == 1 && total == g.straights[0].levels.size()) {
        return Meld(Straight{ g.straights[0].levels });
    }

    // 连对
    if (g.consec_pairs.size() == 1 && total == g.consec_pairs[0].levels.size()) {
        return Meld(ConsecutivePairs{ g.consec_pairs[0].levels });
    }

    // 飞机主体
    if (g.planes.size() == 1 && total == g.planes[0].levels.size()) {
        return Meld(Plane{ g.planes[0].levels });
    }

    // 三带一
    if (g.triples.size() == 1 && g.singles.size() == 2 && total == 4) {
        Level t = g.triples[0].level;
        for (const auto& single : g.singles) {
            if (single.level != t) {
                return Meld(TripleWithSingle{ t, single.level });
            }
        }
    }

    // 三带二
    if (g.triples.size() == 1 && g.pairs.size() >= 1 && total == 5) {
        Level t = g.triples[0].level;
        for (const auto& p : g.pairs) {
            if (p.level != t) {
                return Meld(TripleWithPair{ t, p.level });
            }
        }
    }

    // 四带二单
    if (g.bombs.size() == 1 && total == 6) {
        Level b = g.bombs[0].level;
        std::vector<Level> candidates;
        for (const auto& s : g.singles) {
            if (s.level != b) candidates.push_back(s.level);
        }
        if (candidates.size() >= 2) {
            return Meld(BombWithSingles{ b, candidates[0], candidates[1] });
        }
    }

    // 四带二对
    if (g.bombs.size() == 1 && total == 8) {
        Level b = g.bombs[0].level;
        std::vector<Level> candidates;
        for (const auto& p : g.pairs) {
            if (p.level != b) candidates.push_back(p.level);
        }
        if (candidates.size() >= 2) {
            return Meld(BombWithPairs{ b, candidates[0], candidates[1] });
        }
    }

    for (const auto& plane : g.planes) {
        int triple_count = plane.levels.size() / 3;
        int total_other = total - plane.levels.size();

        // 提取三条点数（去重）
        std::set<Level> triple_levels;
        for (size_t i = 0; i < plane.levels.size(); i += 3) {
            triple_levels.insert(plane.levels[i]);
        }

        // 飞机带小翼
        if (total_other == triple_count) {
            std::vector<Level> candidates;
            for (const auto& s : g.singles) {
                if (triple_levels.count(s.level) == 0) {
                    candidates.push_back(s.level);
                }
            }
            if (candidates.size() == triple_count) {
                return Meld(PlaneWithSingles{ plane.levels, candidates });
            }
        }

        // 飞机带大翼
        if (total_other == 2 * triple_count) {
            std::vector<Level> candidates;
            for (const auto& p : g.pairs) {
                if (triple_levels.count(p.level) == 0) {
                    candidates.push_back(p.level);
                }
            }
            if (candidates.size() == triple_count) {
                return Meld(PlaneWithPairs{ plane.levels, candidates });
            }
        }
    }
    return Meld(Pass{});
}

DouDizhuAI::DouDizhuAI() {
    recorder = new CardRecorder();
    PF = new ParticleFilter(*recorder);
}
DouDizhuAI::~DouDizhuAI() {
    delete PF;
    delete recorder;
}
void DouDizhuAI::initMyHand(const std::vector<CardNum>& cards) {
    my_hand.addCards(cards);
    my_hand.analyze();
    recorder->markPlayed(cards);
}

void DouDizhuAI::myType(const std::vector<CardNum>& cards) {
    //recorder->markPlayed(cards);
    PF->init(player1_type, player2_type,cards);
}