#include "Gamer.h"
#include"ParticleFilter.h"
#include <iostream>
#include"unordered_set"
std::string levelName(Level l) {
    if (l == level_joker) return "小王";
    if (l == level_JOKER) return "大王";
    const char* names[] = { "3","4","5","6","7","8","9","10","J","Q","K","A","2" };
    if (l >= 0 && l < 13) return names[l];
    return "?";
}

// 牌型名称映射
std::string meldTypeName(MeldType t) {
    switch (t) {
    case MeldType::SINGLE: return "单张";
    case MeldType::PAIR: return "对子";
    case MeldType::TRIPLE: return "三张";
    case MeldType::BOMB: return "炸弹";
    case MeldType::ROCKET: return "火箭";
    case MeldType::STRAIGHT: return "单顺";
    case MeldType::CONSECUTIVE_PAIRS: return "连对";
    case MeldType::PLANE: return "飞机不带翼";
    case MeldType::TRIPLE_WITH_SINGLE: return "三带一";
    case MeldType::TRIPLE_WITH_PAIR: return "三带二";
    case MeldType::BOMB_WITH_SINGLES: return "四带二单";
    case MeldType::BOMB_WITH_PAIRS: return "四带二对";
    case MeldType::PLANE_WITH_SINGLES: return "飞机带小翼";
    case MeldType::PLANE_WITH_PAIRS: return "飞机带大翼";
    case MeldType::PASS: return "过牌";
    default: return "未知";
    }
}

void testRecognize(const std::string& desc, const std::vector<CardNum>& cards, MeldType expected) {
    std::cout << "测试: " << desc << "\n";
    std::cout << "  输入: ";
    for (CardNum c : cards) {
        Level l = card2level(c);
        std::cout << levelName(l) << " ";
    }
    std::cout << "\n";
    Gamer g;
    Meld result = g.recognize(cards);
    MeldType actual = result.type();

    std::cout << "  识别: " << meldTypeName(actual) << "\n";
    std::cout << "  结果: " << (actual == expected ? "? 通过" : "? 失败") << "\n\n";
}

bool isSubset(const std::vector<CardNum>& superset,
    const std::vector<CardNum>& subset) {
    std::unordered_multiset<CardNum> super(superset.begin(), superset.end());
    for (CardNum c : subset) {
        auto it = super.find(c);
        if (it == super.end()) return false;
        super.erase(it);
    }
    return true;
}
float jaccardSimilarity(const std::vector<CardNum>& a,
    const std::vector<CardNum>& b) {
    if (a.empty() && b.empty()) return 1.0f;
    if (a.empty() || b.empty()) return 0.0f;

    std::unordered_multiset<CardNum> multiset_a(a.begin(), a.end());
    std::unordered_multiset<CardNum> multiset_b(b.begin(), b.end());

    // 计算交集大小
    size_t intersection = 0;
    for (CardNum c : a) {
        auto it = multiset_b.find(c);
        if (it != multiset_b.end()) {
            intersection++;
            multiset_b.erase(it);
        }
    }

    // 并集大小 = a的大小 + b的大小 - 交集
    size_t union_size = a.size() + b.size() - intersection;

    return union_size > 0 ? (float)intersection / union_size : 0.0f;
}
void test_convergence_with_preset_truth() {
    std::srand(std::time(nullptr));
    std::cout << "========== 粒子滤波收敛性测试 ==========\n";

    // 1. 预设真实手牌（仅用于生成观测和最终验证）
    std::vector<CardNum> public_cards = { 0, 1, 2 };
    std::vector<CardNum> my_cards = { 3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19 };
    std::vector<CardNum> truth_p1_unknown = { 20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36 };
    std::vector<CardNum> truth_p2 = { 37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53 };

    // 2. 粒子滤波只知道公开信息
    CardRecorder recorder;
    recorder.markPlayed(my_cards);
    ParticleFilter pf(recorder);
    pf.init(GamerType::Lord, GamerType::Farmer1, public_cards);

    // 3. 模拟观测序列（从真实手牌中出牌）
    std::vector<std::vector<CardNum>> observations = {
        {0},              // 出明牌
        {20},             // 出单张
        {24, 25},         // 出对子
        {26,27,28},     // 出三张
        {22},
        {21}
    };

    // 剩余真实手牌
    std::vector<CardNum> remaining_truth = truth_p1_unknown;

    std::cout << "初始真实未知牌: " << remaining_truth.size() << " 张\n";

    for (size_t r = 0; r < observations.size(); ++r) {
        std::cout << "\n第 " << r + 1 << " 轮观测: ";
        for (CardNum c : observations[r]) std::cout << (int)c << " ";
        std::cout << "\n";
        recorder.markPlayed(observations[r]);
        // 粒子滤波更新（只知道观测，不知道真实手牌）
        pf.update(GamerType::Lord, observations[r]);

        // 更新剩余真实手牌（仅用于验证）
        for (CardNum c : observations[r]) {
            if (c != 0) {
                auto it = std::find(remaining_truth.begin(), remaining_truth.end(), c);
                if (it != remaining_truth.end()) remaining_truth.erase(it);
            }
        }

        // 验证：检查粒子池中有多少粒子“恰好”包含了剩余真实手牌
        const auto& particles = pf.getParticles(0);
        float entropy = 0;
        std::unordered_map<CardNum, int> freq;
        for (const auto& p : particles) {
            for (CardNum c : p.cards) freq[c]++;
        }
        for (auto& [c, count] : freq) {
            float p = (float)count / particles.size();
            entropy -= p * std::log2(p);
        }
        std::cout << "粒子池熵: " << entropy << "\n";

        // 额外验证：所有幸存粒子是否都能打出历史观测
        bool all_valid = true;
        for (const auto& p : particles) {
            for (size_t i = 0; i <= r; ++i) {
                if (!pf.canPlay(p.cards, observations[i])) {
                    all_valid = false;
                    break;
                }
            }
        }
        std::cout << "所有粒子都能打出历史观测: " << (all_valid ? "?" : "?") << "\n";
    }

    std::cout << "\n========== 测试完成 ==========\n";
}

// 辅助函数：计算熵
float computeEntropy(const std::vector<ParticleFilter::Particle>& particles) {
    if (particles.empty()) return 0;

    std::unordered_map<CardNum, int> freq;
    for (const auto& p : particles) {
        for (CardNum c : p.cards) freq[c]++;
    }

    float entropy = 0;
    for (auto& [c, count] : freq) {
        float p = (float)count / particles.size();
        entropy -= p * std::log2(p);
    }
    return entropy;
}

void test_particle_filter_both_players() {
    std::srand(std::time(nullptr));
    std::cout << "========== 粒子滤波双方交替测试 ==========\n";

    // 1. 预设牌张分配
    std::vector<CardNum> public_cards = { 0, 1, 2 };
    std::vector<CardNum> my_cards = { 3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19 };
    std::vector<CardNum> truth_p1 = { 20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36 };
    std::vector<CardNum> truth_p2 = { 37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53 };

    // 2. 初始化
    CardRecorder recorder;
    recorder.markPlayed(my_cards);
    ParticleFilter pf(recorder);
    pf.init(GamerType::Lord, GamerType::Farmer1, public_cards);

    std::cout << "对手1真实手牌: ";
    for (CardNum c : truth_p1) std::cout << (int)c << " "; std::cout << "\n";
    std::cout << "对手2真实手牌: ";
    for (CardNum c : truth_p2) std::cout << (int)c << " "; std::cout << "\n";

    // 3. 双方交替出牌序列
    //    格式: {出牌者(0=对手1, 1=对手2), 打出的牌}
    std::vector<std::pair<int, std::vector<CardNum>>> rounds = {
        {0, {0}},              // 对手1出明牌
        {1, {37, 38}},         // 对手2出对子
        {0, {20}},             // 对手1出单张
        {1, {41, 42, 43}},     // 对手2出三张
        {0, {24, 25}},         // 对手1出对子
        {1, {39}},             // 对手2出单张
        {0, {26, 27, 28}},     // 对手1出三张
        {1, {44, 45}},         // 对手2出对子
    };

    // 记录剩余真实手牌
    std::vector<CardNum> remaining_p1 = truth_p1;
    std::vector<CardNum> remaining_p2 = truth_p2;

    // 记录历史观测（用于验证合法性）
    std::vector<std::pair<int, std::vector<CardNum>>> history;

    for (size_t r = 0; r < rounds.size(); ++r) {
        int player = rounds[r].first;
        auto& played = rounds[r].second;

        std::cout << "\n第 " << r + 1 << " 轮: 对手" << (player + 1) << " 打出 ";
        for (CardNum c : played) std::cout << (int)c << " ";
        std::cout << "\n";

        // 更新粒子滤波
        
        pf.update(player, played);
        history.push_back({ player, played });

        // 更新剩余真实手牌
        auto& remaining = (player == 0) ? remaining_p1 : remaining_p2;
        for (CardNum c : played) {
            if (c != 0) {
                auto it = std::find(remaining.begin(), remaining.end(), c);
                if (it != remaining.end()) remaining.erase(it);
            }
        }

        // 计算两个对手粒子池的熵
        float entropy1 = computeEntropy(pf.getParticles(0));
        float entropy2 = computeEntropy(pf.getParticles(1));

        std::cout << "对手1粒子池熵: " << entropy1 << "\n";
        std::cout << "对手2粒子池熵: " << entropy2 << "\n";
        std::cout << "对手1剩余手牌: " << pf.getHandSize(0) << " 张\n";
        std::cout << "对手2剩余手牌: " << pf.getHandSize(1) << " 张\n";

        // 验证所有粒子是否都能打出历史观测
        bool all_valid = true;
        for (const auto& [hist_player, hist_played] : history) {
            const auto& particles = pf.getParticles(hist_player);
            for (const auto& p : particles) {
                if (!pf.canPlay(p.cards, hist_played)) {
                    all_valid = false;
                    break;
                }
            }
        }
        std::cout << "所有粒子都能打出历史观测: " << (all_valid ? "?" : "?") << "\n";
    }

    std::cout << "\n========== 测试完成 ==========\n";
}

void test_DouDizhuAI_full_flow() {
    std::cout << "========== DouDizhuAI 完整流程测试 ==========\n";

    // ========== 第一部分：正常出牌流程 ==========
    {
        DouDizhuAI ai;

        std::vector<CardNum> my_cards = {
            0,1,2,3,           // 四张3（炸弹）
            4,5,6,             // 三张4
            8,9,               // 对子5
            12,16,20,24,28,    // 单张6,7,8,9,10
            52,53              // 大小王
        };
        ai.initMyHand(my_cards);
        ai.publicCards(GamerType::Lord, GamerType::Farmer1, {});

        std::cout << "我的手牌: " << ai.my_hand.totalCards() << " 张\n";

        // 从容器中选择动作
        std::vector<Meld> to_play;
        if (!ai.my_hand.singles.empty()) {
            to_play.push_back(ai.my_hand.singles[0]);
            std::cout << "选择打出单张\n";
        }
        if (!ai.my_hand.pairs.empty()) {
            to_play.push_back(ai.my_hand.pairs[0]);
            std::cout << "选择打出对子\n";
        }

        std::cout << "\n--- 打出前 ---\n";
        std::cout << "手牌数: " << ai.my_hand.totalCards() << "\n";

        auto played = ai.play(to_play);

        std::cout << "\n--- 打出后 ---\n";
        std::cout << "打出的牌: ";
        for (const card& c : played) {
            std::cout << (int)c.getCardNum() << " ";
        }
        std::cout << "\n剩余手牌: " << ai.my_hand.totalCards() << " 张\n";
    }

    // ========== 第二部分：专门测试冲突打出 ==========
    {
        std::cout << "\n--- 测试冲突打出（全新手牌） ---\n";
        DouDizhuAI ai;

        // 只加四张3，保证炸弹存在
        std::vector<CardNum> my_cards = { 0, 1, 2, 3 };
        ai.initMyHand(my_cards);
        ai.publicCards(GamerType::Lord, GamerType::Farmer1, {});

        std::cout << "手牌: 四张3 (炸弹)\n";

        std::vector<Meld> conflict_plays;

        // 从容器中取炸弹
        if (!ai.my_hand.bombs.empty()) {
            conflict_plays.push_back(ai.my_hand.bombs[0]);
            std::cout << "加入: 炸弹3\n";
        }

        // 从容器中取同点数的单张
        for (const auto& s : ai.my_hand.singles) {
            if (!ai.my_hand.bombs.empty() && s.level == ai.my_hand.bombs[0].level) {
                conflict_plays.push_back(s);
                std::cout << "加入: 单张3\n";
                break;
            }
        }

        if (conflict_plays.size() >= 2) {
            std::cout << "尝试同时打出...\n";
            auto failed = ai.play(conflict_plays);
            std::cout << "冲突打出返回: " << (failed.empty() ? "空（失败）" : "成功") << "\n";
            std::cout << "手牌数不变: " << ai.my_hand.totalCards() << " 张\n";
        }
        else {
            std::cout << "未找到冲突牌型（容器中可能没有同时存在炸弹和单张）\n";
        }
    }

    // ========== 第三部分：测试合法组合打出 ==========
    {
        std::cout << "\n--- 测试合法组合打出 ---\n";
        DouDizhuAI ai;

        std::vector<CardNum> my_cards = {
            0,1,2,           // 三张3
            4,5,             // 两张4（保证有单张4）
            8,9,             // 对子5
        };
        ai.initMyHand(my_cards);
        ai.publicCards(GamerType::Lord, GamerType::Farmer1, {});

        std::cout << "手牌: 三张3, 两张4, 对子5\n";
        std::cout << "容器: 单张=" << ai.my_hand.singles.size()
            << ", 三张=" << ai.my_hand.triples.size() << "\n";

        std::vector<Meld> legal_plays;

        if (!ai.my_hand.triples.empty()) {
            legal_plays.push_back(ai.my_hand.triples[0]);
            Level t_level = ai.my_hand.triples[0].level;
            std::cout << "加入: 三张" << t_level << "\n";

            // 找点数不同的单张
            for (const auto& s : ai.my_hand.singles) {
                if (s.level != t_level) {
                    legal_plays.push_back(s);
                    std::cout << "加入: 单张" << s.level << "\n";
                    break;
                }
            }
        }

        if (legal_plays.size() == 2) {
            std::cout << "打出前手牌: " << ai.my_hand.totalCards() << " 张\n";
            auto played = ai.play(legal_plays);
            std::cout << "打出后手牌: " << ai.my_hand.totalCards() << " 张\n";
            std::cout << "打出的牌: ";
            for (const card& c : played) {
                std::cout << (int)c.getCardNum() << " ";
            }
            std::cout << "\n";
        }
    }

    std::cout << "\n========== 测试完成 ==========\n";
}

int main() {
    test_DouDizhuAI_full_flow();
    
     //return 0;
    for (int i = 0; i < 1; i++) {
        test_particle_filter_both_players();
    }
    
    
    return 0;
}