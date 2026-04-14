#pragma once
#include <iostream>
#include <set>
#include <string>
#include <cassert>
#include <cstring> // 注意memset是cstring里的
#include <algorithm>
#include <vector>
#include<variant>
#include<unordered_map>
//#include "jsoncpp/json.h" // 在平台上，C++编译时默认包含此库

using namespace std;
using std::vector;
using std::sort;
using std::unique;
using std::set;
using std::string;

constexpr int PLAYER_COUNT = 3;
enum class CardComboType
{
    PASS, // 过
    SINGLE, // 单张
    PAIR, // 对子
    STRAIGHT, // 顺子
    STRAIGHT2, // 双顺
    TRIPLET, // 三条
    TRIPLET1, // 三带一
    TRIPLET2, // 三带二
    BOMB, // 炸弹
    QUADRUPLE2, // 四带二（只）
    QUADRUPLE4, // 四带二（对）
    PLANE, // 飞机
    PLANE1, // 飞机带小翼
    PLANE2, // 飞机带大翼
    SSHUTTLE, // 航天飞机
    SSHUTTLE2, // 航天飞机带小翼
    SSHUTTLE4, // 航天飞机带大翼
    ROCKET, // 火箭
    INVALID // 非法牌型
};
inline int cardComboScores[] = {
    0, // 过
    1, // 单张
    2, // 对子
    6, // 顺子
    6, // 双顺
    4, // 三条
    4, // 三带一
    4, // 三带二
    10, // 炸弹
    8, // 四带二（只）
    8, // 四带二（对）
    8, // 飞机
    8, // 飞机带小翼
    8, // 飞机带大翼
    10, // 航天飞机（需要特判：二连为10分，多连为20分）
    10, // 航天飞机带小翼
    10, // 航天飞机带大翼
    16, // 火箭
    0 // 非法牌型
};

#ifndef _BOTZONE_ONLINE
inline string cardComboStrings[] = {
    "PASS",
    "SINGLE",
    "PAIR",
    "STRAIGHT",
    "STRAIGHT2",
    "TRIPLET",
    "TRIPLET1",
    "TRIPLET2",
    "BOMB",
    "QUADRUPLE2",
    "QUADRUPLE4",
    "PLANE",
    "PLANE1",
    "PLANE2",
    "SSHUTTLE",
    "SSHUTTLE2",
    "SSHUTTLE4",
    "ROCKET",
    "INVALID"
};
#endif

using CardNum = short;
constexpr CardNum card_joker = 52;
constexpr CardNum card_JOKER = 53;


using Level = short;
constexpr Level MAX_LEVEL = 15;
constexpr Level MAX_STRAIGHT_LEVEL = 11;
constexpr Level level_joker = 13;
constexpr Level level_JOKER = 14;


constexpr Level card2level(CardNum card)
{
    return card / 4 + card / 53;
}

class card {
    CardNum c;
    Level l;
public:
    card(CardNum card) : c(card) {
        l = card2level(c);
    }
    card(){}
    bool operator<(const card& rhs) const { return l < rhs.l; }
    bool operator==(const card& rhs) const { return l == rhs.l; }
    Level getLevel() const { return l; }
};

enum class MeldType {
    SINGLE, PAIR, TRIPLE, BOMB, ROCKET,
    STRAIGHT,
    CONSECUTIVE_PAIRS,
    PLANE,
    PASS
};

// ========== 前向声明 ==========
class Gamer;

// ========== 牌型结构体（只声明，不实现） ==========
struct Single {
    static constexpr MeldType type = MeldType::SINGLE;
    Level level;
    int power() const;
    std::vector<Level> required_levels() const;
    bool canPlay(const Gamer& hand) const;
    void play(Gamer& hand) const;
};

struct Pair {
    static constexpr MeldType type = MeldType::PAIR;
    Level level;
    int power() const;
    std::vector<Level> required_levels() const;
    bool canPlay(const Gamer& hand) const;
    void play(Gamer& hand) const;
};

struct Triple {
    static constexpr MeldType type = MeldType::TRIPLE;
    Level level;
    int power() const;
    std::vector<Level> required_levels() const;
    bool canPlay(const Gamer& hand) const;
    void play(Gamer& hand) const;
};

struct Bomb {
    static constexpr MeldType type = MeldType::BOMB;
    Level level;
    int power() const;
    std::vector<Level> required_levels() const;
    bool canPlay(const Gamer& hand) const;
    void play(Gamer& hand) const;
};

struct Straight {
    static constexpr MeldType type = MeldType::STRAIGHT;
    std::vector<Level> levels;
    int power() const;
    std::vector<Level> required_levels() const;
    bool canPlay(const Gamer& hand) const;
    void play(Gamer& hand) const;
};

struct ConsecutivePairs {
    static constexpr MeldType type = MeldType::CONSECUTIVE_PAIRS;
    std::vector<Level> levels;
    int power() const;
    std::vector<Level> required_levels() const;
    bool canPlay(const Gamer& hand) const;
    void play(Gamer& hand) const;
};

struct Plane {
    static constexpr MeldType type = MeldType::PLANE;
    std::vector<Level> levels;
    int power() const;
    std::vector<Level> required_levels() const;
    bool canPlay(const Gamer& hand) const;
    void play(Gamer& hand) const;
};

struct Pass {
    static constexpr MeldType type = MeldType::PASS;
    int power() const;
    std::vector<Level> required_levels() const;
    bool canPlay(const Gamer& hand) const;
    void play(Gamer& hand) const;
};

// ========== MeldVariant 和 Meld 类 ==========
using MeldVariant = std::variant<
    Single, Pair, Triple, Bomb,
    Straight, ConsecutivePairs, Plane,
    Pass
>;

class Meld {
public:
    MeldVariant data;

    template<typename T>
    Meld(T&& t) : data(std::forward<T>(t)) {}

    int power() const;
    std::vector<Level> required_levels() const;
    bool canPlay(const Gamer& hand) const;
    void play(Gamer& hand) const;
    MeldType type() const;
};

// ========== Gamer 类完整定义 ==========
class Gamer {
    std::vector<std::vector<card>> levels;
    Gamer clone();
public:
    std::vector<Single> singles;
    std::vector<Pair> pairs;
    std::vector<Triple> triples;
    std::vector<Bomb> bombs;
    std::vector<Straight> straights;
    std::vector<ConsecutivePairs> consec_pairs;
    std::vector<Plane> planes;
    bool has_rocket = false;

    Gamer() {
        for (int i = 0; i < MAX_LEVEL; ++i) {
            levels.push_back(std::vector<card>());
        }
    }

    void clearMelds() {
        singles.clear(); pairs.clear(); triples.clear(); bombs.clear();
        straights.clear(); consec_pairs.clear(); planes.clear();
        has_rocket = false;
    }

    std::vector<std::pair<int, int>> findSegments(int min_count, int min_length,int range_start, int range_end);

    void analyze();

    int simulatePlay(Gamer& hand, std::vector<Meld>& move);

    

    void addCard(CardNum c) {
        card cd(c);
        levels[cd.getLevel()].push_back(cd);
    }

    void addCards(const std::vector<CardNum>& cs) {
        for (auto& c : cs) {
            card cd(c);
            levels[(int)cd.getLevel()].push_back(cd);
        }
    }

    void clear() {
        for (auto& level : levels) {
            level.clear();
        }
    }

    int count(Level lvl) const {
        return static_cast<int>(levels[static_cast<int>(lvl)].size());
    }

    bool tryPlay(Level lvl) {
        auto& bucket = levels[static_cast<int>(lvl)];
        if (bucket.empty()) return false;
        bucket.pop_back();
        return true;
    }

    bool tryPlay(const std::vector<Level>& lvls) {
        std::unordered_map<int, int> need;
        for (Level l : lvls) need[static_cast<int>(l)]++;
        for (auto& [idx, cnt] : need) {
            if (static_cast<int>(levels[idx].size()) < cnt) return false;
        }
        for (Level l : lvls) {
            levels[static_cast<int>(l)].pop_back();
        }
        return true;
    }

    struct Snapshot {
        std::vector<size_t> sizes;
    };

    Snapshot save() const {
        Snapshot ss;
        for (auto& bucket : levels) ss.sizes.push_back(bucket.size());
        return ss;
    }

    void load(const Snapshot& ss) {
        for (size_t i = 0; i < levels.size(); ++i) {
            levels[i].resize(ss.sizes[i]);
        }
    }
};

// ========== 牌型结构体的成员函数实现 ==========

inline int Single::power() const { return 100 + static_cast<int>(level); }
inline std::vector<Level> Single::required_levels() const { return { level }; }
inline bool Single::canPlay(const Gamer& hand) const { return hand.count(level) >= 1; }
inline void Single::play(Gamer& hand) const { hand.tryPlay(level); }

inline int Pair::power() const { return 200 + static_cast<int>(level); }
inline std::vector<Level> Pair::required_levels() const { return { level, level }; }
inline bool Pair::canPlay(const Gamer& hand) const { return hand.count(level) >= 2; }
inline void Pair::play(Gamer& hand) const { hand.tryPlay({ level, level }); }

inline int Triple::power() const { return 300 + static_cast<int>(level); }
inline std::vector<Level> Triple::required_levels() const { return { level, level, level }; }
inline bool Triple::canPlay(const Gamer& hand) const { return hand.count(level) >= 3; }
inline void Triple::play(Gamer& hand) const { hand.tryPlay({ level, level, level }); }

inline int Bomb::power() const { return 1000 + static_cast<int>(level); }
inline std::vector<Level> Bomb::required_levels() const { return { level, level, level, level }; }
inline bool Bomb::canPlay(const Gamer& hand) const { return hand.count(level) >= 4; }
inline void Bomb::play(Gamer& hand) const { hand.tryPlay({ level, level, level, level }); }

inline int Straight::power() const {
    return static_cast<int>(levels.size()) * 10 + static_cast<int>(levels.back());
}
inline std::vector<Level> Straight::required_levels() const { return levels; }
inline bool Straight::canPlay(const Gamer& hand) const {
    for (Level l : levels) if (hand.count(l) < 1) return false;
    return true;
}
inline void Straight::play(Gamer& hand) const {
    for (Level l : levels) hand.tryPlay(l);
}

inline int ConsecutivePairs::power() const {
    return static_cast<int>(levels.size()) * 5 + static_cast<int>(levels.back());
}
inline std::vector<Level> ConsecutivePairs::required_levels() const { return levels; }
inline bool ConsecutivePairs::canPlay(const Gamer& hand) const {
    for (Level l : levels) if (hand.count(l) < 2) return false;
    return true;
}
inline void ConsecutivePairs::play(Gamer& hand) const {
    for (Level l : levels) hand.tryPlay(l);
}

inline int Plane::power() const {
    return static_cast<int>(levels.size()) * 3 + static_cast<int>(levels.back());
}
inline std::vector<Level> Plane::required_levels() const { return levels; }
inline bool Plane::canPlay(const Gamer& hand) const {
    for (Level l : levels) if (hand.count(l) < 3) return false;
    return true;
}
inline void Plane::play(Gamer& hand) const {
    for (Level l : levels) hand.tryPlay(l);
}

inline int Pass::power() const { return -1; }
inline std::vector<Level> Pass::required_levels() const { return {}; }
inline bool Pass::canPlay(const Gamer& hand) const { return true; }
inline void Pass::play(Gamer& hand) const { /* 不做任何事 */ }

// ========== Meld 类的成员函数实现 ==========
inline int Meld::power() const {
    return std::visit([](const auto& m) -> int { return m.power(); }, data);
}

inline std::vector<Level> Meld::required_levels() const {
    return std::visit([](const auto& m) -> std::vector<Level> {
        return m.required_levels();
        }, data);
}

inline bool Meld::canPlay(const Gamer& hand) const {
    return std::visit([&hand](const auto& m) -> bool {
        return m.canPlay(hand);
        }, data);
}

inline void Meld::play(Gamer& hand) const {
    std::visit([&hand](const auto& m) { m.play(hand); }, data);
}

inline MeldType Meld::type() const {
    return std::visit([](const auto& m) -> MeldType {
        return std::decay_t<decltype(m)>::type;
        }, data);
}