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
     CardNum getCardNum() const {
        return c;
    }
    bool operator<(const card& rhs) const { return l < rhs.l; }
    bool operator==(const card& rhs) const { return l == rhs.l; }
    Level getLevel() const { return l; }
};

enum class MeldType {
    SINGLE, PAIR, TRIPLE, BOMB, ROCKET,
    STRAIGHT, CONSECUTIVE_PAIRS, PLANE,
    TRIPLE_WITH_SINGLE, TRIPLE_WITH_PAIR,
    BOMB_WITH_SINGLES, BOMB_WITH_PAIRS,
    PLANE_WITH_SINGLES, PLANE_WITH_PAIRS,
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
    std::vector<card> playAndReturn(Gamer& hand) const;
};

struct Pair {
    static constexpr MeldType type = MeldType::PAIR;
    Level level;
    int power() const;
    std::vector<Level> required_levels() const;
    bool canPlay(const Gamer& hand) const;
    void play(Gamer& hand) const;
    std::vector<card> playAndReturn(Gamer& hand) const;
};

struct Triple {
    static constexpr MeldType type = MeldType::TRIPLE;
    Level level;
    int power() const;
    std::vector<Level> required_levels() const;
    bool canPlay(const Gamer& hand) const;
    void play(Gamer& hand) const;
    std::vector<card> playAndReturn(Gamer& hand) const;
};

struct Bomb {
    static constexpr MeldType type = MeldType::BOMB;
    Level level;
    int power() const;
    std::vector<Level> required_levels() const;
    bool canPlay(const Gamer& hand) const;
    void play(Gamer& hand) const;
    std::vector<card> playAndReturn(Gamer& hand) const;
};

struct Straight {
    static constexpr MeldType type = MeldType::STRAIGHT;
    std::vector<Level> levels;
    int power() const;
    std::vector<Level> required_levels() const;
    bool canPlay(const Gamer& hand) const;
    void play(Gamer& hand) const;
    std::vector<card> playAndReturn(Gamer& hand) const;
};

struct ConsecutivePairs {
    static constexpr MeldType type = MeldType::CONSECUTIVE_PAIRS;
    std::vector<Level> levels;
    int power() const;
    std::vector<Level> required_levels() const;
    bool canPlay(const Gamer& hand) const;
    void play(Gamer& hand) const;
    std::vector<card> playAndReturn(Gamer& hand) const;
};

struct Plane {
    static constexpr MeldType type = MeldType::PLANE;
    std::vector<Level> levels;
    int power() const;
    std::vector<Level> required_levels() const;
    bool canPlay(const Gamer& hand) const;
    void play(Gamer& hand) const;
    std::vector<card> playAndReturn(Gamer& hand) const;
};

struct Pass {
    static constexpr MeldType type = MeldType::PASS;
    int power() const;
    std::vector<Level> required_levels() const;
    bool canPlay(const Gamer& hand) const;
    void play(Gamer& hand) const;
    std::vector<card> playAndReturn(Gamer& hand) const;
};

struct Rocket {
    static constexpr MeldType type = MeldType::ROCKET;
    int power() const;
    std::vector<Level> required_levels() const;
    bool canPlay(const Gamer& hand) const;
    void play(Gamer& hand) const;
    std::vector<card> playAndReturn(Gamer& hand) const;
};

struct TripleWithSingle {
    static constexpr MeldType type = MeldType::TRIPLE_WITH_SINGLE;
    Level triple_level;
    Level single_level;
    int power() const;
    std::vector<Level> required_levels() const;
    bool canPlay(const Gamer& hand) const;
    void play(Gamer& hand) const;
    std::vector<card> playAndReturn(Gamer& hand) const;
};

struct TripleWithPair {
    static constexpr MeldType type = MeldType::TRIPLE_WITH_PAIR;
    Level triple_level;
    Level pair_level;
    int power() const;
    std::vector<Level> required_levels() const;
    bool canPlay(const Gamer& hand) const;
    void play(Gamer& hand) const;
    std::vector<card> playAndReturn(Gamer& hand) const;
};

struct BombWithSingles {
    static constexpr MeldType type = MeldType::BOMB_WITH_SINGLES;
    Level bomb_level;
    Level single1;
    Level single2;
    int power() const;
    std::vector<Level> required_levels() const;
    bool canPlay(const Gamer& hand) const;
    void play(Gamer& hand) const;
    std::vector<card> playAndReturn(Gamer& hand) const;
};

struct BombWithPairs {
    static constexpr MeldType type = MeldType::BOMB_WITH_PAIRS;
    Level bomb_level;
    Level pair1;
    Level pair2;
    int power() const;
    std::vector<Level> required_levels() const;
    bool canPlay(const Gamer& hand) const;
    void play(Gamer& hand) const;
    std::vector<card> playAndReturn(Gamer& hand) const;
};

struct PlaneWithSingles {
    static constexpr MeldType type = MeldType::PLANE_WITH_SINGLES;
    std::vector<Level> plane_levels;
    std::vector<Level> singles;
    int power() const;
    std::vector<Level> required_levels() const;
    bool canPlay(const Gamer& hand) const;
    void play(Gamer& hand) const;
    std::vector<card> playAndReturn(Gamer& hand) const;
};

struct PlaneWithPairs {
    static constexpr MeldType type = MeldType::PLANE_WITH_PAIRS;
    std::vector<Level> plane_levels;
    std::vector<Level> pairs;
    int power() const;
    std::vector<Level> required_levels() const;
    bool canPlay(const Gamer& hand) const;
    void play(Gamer& hand) const;
    std::vector<card> playAndReturn(Gamer& hand) const;
};



// ========== MeldVariant 和 Meld 类 ==========
using MeldVariant = std::variant<
    Single, Pair, Triple, Bomb, Rocket,
    Straight, ConsecutivePairs, Plane,
    TripleWithSingle, TripleWithPair,
    BombWithSingles, BombWithPairs,
    PlaneWithSingles, PlaneWithPairs,
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
    std::vector<card> playAndReturn(Gamer& hand) const;
};

enum class GamerType {
    Lord, Farmer1, Farmer2
};

class CardRecorder {
    bool played[54] = { false };  // 54张牌，true表示已打出
    std::vector<CardNum> player1;
    std::vector<CardNum> player2;
public:
    // 标记一张牌已打出
    void markPlayed(CardNum c) { played[c] = true; }

    // 标记多张牌已打出
    void markPlayed(const std::vector<CardNum>& cards) {
        for (CardNum c : cards) played[c] = true;
    }
    void markPlayed(int idx,const std::vector<CardNum>& cards) {
        if(idx==0) player1.insert(player1.end(), cards.begin(), cards.end());
        else player2.insert(player2.end(), cards.begin(), cards.end());
    }
    std::vector<CardNum> getRemainingCards(int idx) {
        if(idx==0) return player1;
        else return player2;
    }
    // 获取未打出的牌（剩余牌池）
    std::vector<CardNum> getRemainingCards() const {
        std::vector<CardNum> result;
        for (int i = 0; i < 54; ++i) {
            if (!played[i]) result.push_back(i);
        }
        return result;
    }

    // 某张牌是否已打出
    bool isPlayed(CardNum c) const { return played[c]; }

    // 重置（新一局）
    void reset() { memset(played, 0, sizeof(played)); }
};
   


// ========== Gamer 类完整定义 ==========
class Gamer {
    std::vector<std::vector<card>> levels;
    
    std::vector<std::pair<int, int>> findSegments(int min_count, int min_length, int range_start, int range_end);
    void clearMelds() {
        singles.clear(); pairs.clear(); triples.clear(); bombs.clear();
        straights.clear(); consec_pairs.clear(); planes.clear();
        has_rocket = false;
    }
    
public:
    Gamer clone();
    std::vector<Single> singles;
    std::vector<Pair> pairs;
    std::vector<Triple> triples;
    std::vector<Bomb> bombs;
    std::vector<Straight> straights;
    std::vector<ConsecutivePairs> consec_pairs;
    std::vector<Plane> planes;
    bool has_rocket = false;
    Meld* player1{nullptr};
    Meld* player2{nullptr};
    bool first= true;
    
    Gamer() {
        for (int i = 0; i < MAX_LEVEL; ++i) {
            levels.push_back(std::vector<card>());
        }
        
    }
    
    void analyze();

    int simulatePlay(Gamer& hand, std::vector<Meld>& move);

    Meld recognize(const std::vector<CardNum>& cards);
    
    //接受对方出牌
    void opponent_cards(const std::vector<std::vector<CardNum>>& cards) {
        /*if (first) {
            first = false;
            if (cards[0].empty()&&cards[1].empty()) {
                type = GamerType::Lord;
            }
            else if (cards[0].empty()&&!cards[1].empty()) {
                type = GamerType::Farmer1;
            }
            else if (!cards[0].empty() && cards[1].empty()) {
                type = GamerType::Farmer2;
            }
            return;
        }*/
        if(player1!=nullptr) delete player1;
        if (player2 != nullptr) delete player2;
        player1 = new Meld(recognize(cards[0]));
        player2 = new Meld(recognize(cards[1]));
    }

    int totalCards() const {
        int total = 0;
        for (const auto& bucket : levels) {
            total += bucket.size();
        }
        return total;
    }

    

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

    card tryPlayAndReturn(Level lvl) {
        auto& bucket = levels[static_cast<int>(lvl)];
        if (bucket.empty()) return card(-1);  // 返回无效牌
        card c = bucket.back();
        bucket.pop_back();
        return c;
    }

    std::vector<card> tryPlayAndReturn(const std::vector<Level>& lvls) {
        std::unordered_map<int, int> need;
        for (Level l : lvls) need[static_cast<int>(l)]++;
        for (auto& [idx, cnt] : need) {
            if (static_cast<int>(levels[idx].size()) < cnt) return {};
        }
        std::vector<card> result;
        for (Level l : lvls) {
            result.push_back(levels[static_cast<int>(l)].back());
            levels[static_cast<int>(l)].pop_back();
            
        }
        return result;
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
    bool canPlayTogether(const Gamer& hand, const std::vector<Meld>& melds);
};

class ParticleFilter;
class CardRecorder;
class DouDizhuAI {
public:
    CardRecorder* recorder;
    ParticleFilter* PF;
    GamerType my_type;
    GamerType player1_type;
    GamerType player2_type;
    Gamer my_hand;
    DouDizhuAI();
    ~DouDizhuAI();
    //得到自己手牌
    void initMyHand(const std::vector<CardNum>& cards);
    //记录另外两方身份，告知明牌
    bool publicCards(GamerType player1, GamerType player2,const std::vector<CardNum>& public_cards);
    //另外两方出牌后更新记牌器和粒子滤波器
    void update(GamerType idx, const std::vector<CardNum>& cards);
    //自己出牌，返回自己出牌的牌，如果不合法返回空容器
    std::vector<card> play(const std::vector<Meld>& cards);
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

inline int Rocket::power() const { return 2000; }
inline std::vector<Level> Rocket::required_levels() const { return { level_joker, level_JOKER }; }

inline bool Rocket::canPlay(const Gamer& hand) const {
    return hand.count(level_joker) >= 1 && hand.count(level_JOKER) >= 1;
}

inline void Rocket::play(Gamer& hand) const {  
    hand.tryPlay(level_joker);
    hand.tryPlay(level_JOKER);
}
inline int Pass::power() const { return -1; }
inline std::vector<Level> Pass::required_levels() const { return {}; }
inline bool Pass::canPlay(const Gamer& hand) const { return true; }
inline void Pass::play(Gamer& hand) const { /* 不做任何事 */ }
// ========== Meld 类的成员函数实现 ==========


inline int TripleWithSingle::power() const { return 400 + static_cast<int>(triple_level); }
inline std::vector<Level> TripleWithSingle::required_levels() const {
    return { triple_level, triple_level, triple_level, single_level };
}
inline bool TripleWithSingle::canPlay(const Gamer& hand) const {
    return hand.count(triple_level) >= 3 && hand.count(single_level) >= 1;
}
inline void TripleWithSingle::play(Gamer& hand) const {
    hand.tryPlay({ triple_level, triple_level, triple_level });
    hand.tryPlay(single_level);
}

// ========== 三带二 ==========
inline int TripleWithPair::power() const { return 400 + static_cast<int>(triple_level); }
inline std::vector<Level> TripleWithPair::required_levels() const {
    return { triple_level, triple_level, triple_level, pair_level, pair_level };
}
inline bool TripleWithPair::canPlay(const Gamer& hand) const {
    return hand.count(triple_level) >= 3 && hand.count(pair_level) >= 2;
}
inline void TripleWithPair::play(Gamer& hand) const {
    hand.tryPlay({ triple_level, triple_level, triple_level });
    hand.tryPlay({ pair_level, pair_level });
}

// ========== 四带二单 ==========
inline int BombWithSingles::power() const { return 1000 + static_cast<int>(bomb_level); }
inline std::vector<Level> BombWithSingles::required_levels() const {
    return { bomb_level, bomb_level, bomb_level, bomb_level, single1, single2 };
}
inline bool BombWithSingles::canPlay(const Gamer& hand) const {
    return hand.count(bomb_level) >= 4 && hand.count(single1) >= 1 && hand.count(single2) >= 1;
}
inline void BombWithSingles::play(Gamer& hand) const {
    hand.tryPlay({ bomb_level, bomb_level, bomb_level, bomb_level });
    hand.tryPlay(single1);
    hand.tryPlay(single2);
}

// ========== 四带二对 ==========
inline int BombWithPairs::power() const { return 1000 + static_cast<int>(bomb_level); }
inline std::vector<Level> BombWithPairs::required_levels() const {
    return { bomb_level, bomb_level, bomb_level, bomb_level, pair1, pair1, pair2, pair2 };
}
inline bool BombWithPairs::canPlay(const Gamer& hand) const {
    return hand.count(bomb_level) >= 4 && hand.count(pair1) >= 2 && hand.count(pair2) >= 2;
}
inline void BombWithPairs::play(Gamer& hand) const {
    hand.tryPlay({ bomb_level, bomb_level, bomb_level, bomb_level });
    hand.tryPlay({ pair1, pair1 });
    hand.tryPlay({ pair2, pair2 });
}

// ========== 飞机带单 ==========
inline int PlaneWithSingles::power() const {
    return 500 + static_cast<int>(plane_levels.size()) * 30 + static_cast<int>(plane_levels.back());
}
inline std::vector<Level> PlaneWithSingles::required_levels() const {
    std::vector<Level> result;
    for (Level l : plane_levels) {
        result.push_back(l); result.push_back(l); result.push_back(l);
    }
    for (Level l : singles) result.push_back(l);
    return result;
}
inline bool PlaneWithSingles::canPlay(const Gamer& hand) const {
    for (Level l : plane_levels) if (hand.count(l) < 3) return false;
    for (Level l : singles) if (hand.count(l) < 1) return false;
    return true;
}
inline void PlaneWithSingles::play(Gamer& hand) const {
    for (Level l : plane_levels) hand.tryPlay({ l, l, l });
    for (Level l : singles) hand.tryPlay(l);
}

// ========== 飞机带对 ==========
inline int PlaneWithPairs::power() const {
    return 500 + static_cast<int>(plane_levels.size()) * 30 + static_cast<int>(plane_levels.back());
}
inline std::vector<Level> PlaneWithPairs::required_levels() const {
    std::vector<Level> result;
    for (Level l : plane_levels) {
        result.push_back(l); result.push_back(l); result.push_back(l);
    }
    for (Level l : pairs) {
        result.push_back(l); result.push_back(l);
    }
    return result;
}
inline bool PlaneWithPairs::canPlay(const Gamer& hand) const {
    for (Level l : plane_levels) if (hand.count(l) < 3) return false;
    for (Level l : pairs) if (hand.count(l) < 2) return false;
    return true;
}
inline void PlaneWithPairs::play(Gamer& hand) const {
    for (Level l : plane_levels) hand.tryPlay({ l, l, l });
    for (Level l : pairs) hand.tryPlay({ l, l });
}


// ========== Meld 类实现 ==========


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

inline std::vector<card> Meld::playAndReturn(Gamer& hand) const {
    return std::visit([&hand](const auto& m) { return m.playAndReturn(hand); }, data);
}

inline std::vector<card> Single::playAndReturn(Gamer& hand) const {
    return { hand.tryPlayAndReturn(level) };
}

inline std::vector<card> Pair::playAndReturn(Gamer& hand) const {
    return hand.tryPlayAndReturn({ level, level });
}

inline std::vector<card> Triple::playAndReturn(Gamer& hand) const {
    return hand.tryPlayAndReturn({ level, level, level });
}

inline std::vector<card> Bomb::playAndReturn(Gamer& hand) const {
    return hand.tryPlayAndReturn({ level, level, level, level });
}

inline std::vector<card> Straight::playAndReturn(Gamer& hand) const {
    return hand.tryPlayAndReturn(levels);
}

inline std::vector<card> ConsecutivePairs::playAndReturn(Gamer& hand) const {
    return hand.tryPlayAndReturn(levels);
}

inline std::vector<card> Plane::playAndReturn(Gamer& hand) const {
    return hand.tryPlayAndReturn(levels);
}

inline std::vector<card> Pass::playAndReturn(Gamer& hand) const {
    return {};
}

inline std::vector<card> Rocket::playAndReturn(Gamer& hand) const {
    return hand.tryPlayAndReturn({ level_joker, level_JOKER });
}

inline std::vector<card> TripleWithSingle::playAndReturn(Gamer& hand) const {
    std::vector<card> result = hand.tryPlayAndReturn({ triple_level, triple_level, triple_level });
    auto s = hand.tryPlayAndReturn(single_level);
    result.push_back(s);
    return result;
}

inline std::vector<card> TripleWithPair::playAndReturn(Gamer& hand) const {
    std::vector<card> result = hand.tryPlayAndReturn({ triple_level, triple_level, triple_level });
    auto p = hand.tryPlayAndReturn({ pair_level, pair_level });
    result.insert(result.end(), p.begin(), p.end());
    return result;
}

inline std::vector<card> BombWithSingles::playAndReturn(Gamer& hand) const {
    std::vector<card> result = hand.tryPlayAndReturn({ bomb_level, bomb_level, bomb_level, bomb_level });
    auto s1 = hand.tryPlayAndReturn(single1);
    auto s2 = hand.tryPlayAndReturn(single2);
    result.push_back(s1);
    result.push_back(s2);
    return result;
}

inline std::vector<card> BombWithPairs::playAndReturn(Gamer& hand) const {
    std::vector<card> result = hand.tryPlayAndReturn({ bomb_level, bomb_level, bomb_level, bomb_level });
    auto p1 = hand.tryPlayAndReturn({ pair1, pair1 });
    auto p2 = hand.tryPlayAndReturn({ pair2, pair2 });
    result.insert(result.end(), p1.begin(), p1.end());
    result.insert(result.end(), p2.begin(), p2.end());
    return result;
}

inline std::vector<card> PlaneWithSingles::playAndReturn(Gamer& hand) const {
    std::vector<card> result;
    for (Level l : plane_levels) {
        auto t = hand.tryPlayAndReturn({ l, l, l });
        result.insert(result.end(), t.begin(), t.end());
    }
    for (Level l : singles) {
        auto s = hand.tryPlayAndReturn(l);
        result.push_back(s);
    }
    return result;
}

inline std::vector<card> PlaneWithPairs::playAndReturn(Gamer& hand) const {
    std::vector<card> result;
    for (Level l : plane_levels) {
        auto t = hand.tryPlayAndReturn({ l, l, l });
        result.insert(result.end(), t.begin(), t.end());
    }
    for (Level l : pairs) {
        auto p = hand.tryPlayAndReturn({ l, l });
        result.insert(result.end(), p.begin(), p.end());
    }
    return result;
}