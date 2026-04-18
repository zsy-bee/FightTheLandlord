#include "ParticleFilter.h"

#include <numeric>
#include<unordered_set>
ParticleFilter::ParticleFilter(const CardRecorder& rec)
    : recorder(rec), rng(std::random_device{}()) {
}

void ParticleFilter::init(GamerType player1, GamerType player2,
    const std::vector<CardNum>& public_cards) {
    this->public_cards = public_cards;
    this->player1=player1;
    this->player2=player2;
    if (player1 == GamerType::Lord) player1_holds_public = true;
    if (player2 == GamerType::Lord) player2_holds_public = true;
    particles1.clear();
    particles2.clear();
    auto full_pool = recorder.getRemainingCards();
    // 移除所有明牌（因为它们会被强制分配）
    for (CardNum c : public_cards) {
        auto it = std::find(full_pool.begin(), full_pool.end(), c);
        if (it != full_pool.end()) full_pool.erase(it);
    }
    for (int i = 0; i < 100; ++i) {
        
   
        std::shuffle(full_pool.begin(), full_pool.end(), rng);

        std::vector<CardNum> cards1, cards2;
        int idx = 0;

        // 对手1：随机抽牌
        int random_needed1 = 17;  // 20 - 3 = 17
        for (int j = 0; j < random_needed1; ++j) {
            cards1.push_back(full_pool[idx++]);
        }
        
       

        // 对手2：随机抽牌
        int random_needed2 = 17;
        for (int j = 0; j < random_needed2; ++j) {
            cards2.push_back(full_pool[idx++]);
        }
        
        
        particles1.push_back({ cards1});
        particles2.push_back({ cards2});
    }
    hand_size1 = 17;
    hand_size2 = 17;
    
    std::cout << "[DEBUG] 剩余牌池大小: " << full_pool.size() << "\n";
    std::cout << "[DEBUG] 包含0? " << (std::find(full_pool.begin(), full_pool.end(), 0) != full_pool.end()) << "\n";
    std::cout << "[DEBUG] 包含1? " << (std::find(full_pool.begin(), full_pool.end(), 1) != full_pool.end()) << "\n";
    std::cout << "[DEBUG] 包含2? " << (std::find(full_pool.begin(), full_pool.end(), 2) != full_pool.end()) << "\n";
}

void ParticleFilter::update(GamerType player, const std::vector<CardNum>& played) {
    
    auto& particles = (player == player1) ? particles1 : particles2;
    auto& other_particles = (player == player1) ? particles2 : particles1;
    int& hand_size = (player == player1) ? hand_size1 : hand_size2;
    int& other_hand_size = (player == player1) ? hand_size2 : hand_size1;
    
    // 1. 原地过滤：保留能打出这手牌的粒子
    size_t keep = 0;
    auto temp_played= played;
    for (CardNum c : public_cards) {
        auto it = std::find(temp_played.begin(), temp_played.end(), c);
        if (it != temp_played.end()) temp_played.erase(it);
    }
    if(temp_played.size()==0) return;

    for (size_t i = 0; i < particles.size(); ++i) {
        if (canPlay(particles[i].cards, temp_played)) {
            // 原地修改手牌
            removeCardsInPlace(particles[i].cards, temp_played);

            // 移到前面
            if (keep != i) {
                particles[keep] = std::move(particles[i]);
                other_particles[keep] = std::move(other_particles[i]);
            }
            ++keep;
        }
    }

    // 截断
    particles.resize(keep);
    other_particles.resize(keep);
    std::cout << "[DEBUG] 幸存粒子数: " << keep << " / " << 100 << "\n";
    
    hand_size -= static_cast<int>(temp_played.size());
    // 2. 如果幸存者太少，紧急重采样
    if (keep == 0) {
        emergencyResamplePaired(hand_size, other_hand_size);
    }
    else if (keep < 100) {
        resampleAfterFilter(particles, other_particles,100);
    }
    std::cout << "[DEBUG] 过滤后第一个粒子手牌: ";
    for (CardNum c : particles[50].cards) std::cout << (int)c << " ";
    std::cout << "\n";
}

void ParticleFilter::update(int player_idx, const std::vector<CardNum>& played) {
    auto& particles = (player_idx == 0) ? particles1 : particles2;
    auto& other_particles = (player_idx==0) ? particles2 : particles1;
    int& hand_size = (player_idx == 0) ? hand_size1 : hand_size2;
    int& other_hand_size = (player_idx == 0) ? hand_size2 : hand_size1;

    // 1. 原地过滤：保留能打出这手牌的粒子
    size_t keep = 0;
    auto temp_played = played;
    for (CardNum c : public_cards) {
        auto it = std::find(temp_played.begin(), temp_played.end(), c);
        if (it != temp_played.end()) temp_played.erase(it);
    }
    if (temp_played.size() == 0) return;

    for (size_t i = 0; i < particles.size(); ++i) {
        if (canPlay(particles[i].cards, temp_played)) {
            // 原地修改手牌
            removeCardsInPlace(particles[i].cards, temp_played);

            // 移到前面
            if (keep != i) {
                particles[keep] = std::move(particles[i]);
                other_particles[keep] = std::move(other_particles[i]);
            }
            ++keep;
        }
    }

    // 截断
    particles.resize(keep);
    other_particles.resize(keep);
    std::cout << "[DEBUG] 幸存粒子数: " << keep << " / " << 100 << "\n";

    hand_size -= static_cast<int>(temp_played.size());
    // 2. 如果幸存者太少，紧急重采样
    if (keep == 0) {
        emergencyResamplePaired(hand_size, other_hand_size);
    }
    else if (keep < 100) {
        resampleAfterFilter(particles, other_particles, 100);
    }
    std::cout << "[DEBUG] 过滤后第一个粒子手牌: ";
    for (CardNum c : particles[50].cards) std::cout << (int)c << " ";
    std::cout << "\n";
}

const std::vector<ParticleFilter::Particle>& ParticleFilter::getParticles(int player_idx) const {
    return (player_idx == 0) ? particles1 : particles2;
}

int ParticleFilter::getHandSize(int player_idx) const {
    return (player_idx == 0) ? hand_size1 : hand_size2;
}

Gamer ParticleFilter::particleToGamer(int player_idx, size_t particle_idx) const {
    const auto& particles = (player_idx == 0) ? particles1 : particles2;
    Gamer g;
    for (CardNum c : particles[particle_idx].cards) {
        g.addCard(c);
    }
    g.analyze();
    return g;
}

bool ParticleFilter::canPlay(const std::vector<CardNum>& sample,
    const std::vector<CardNum>& played) const {
    // 把 sample 转成 multiset，检查是否包含 played 的所有元素
    std::unordered_multiset<CardNum> sample_set(sample.begin(), sample.end());

    for (CardNum c : played) {
        auto it = sample_set.find(c);
        if (it == sample_set.end()) {
            return false;  // 缺少这张具体的物理牌
        }
        sample_set.erase(it);
    }
    return true;
}

void ParticleFilter::removeCardsInPlace(std::vector<CardNum>& sample,
    const std::vector<CardNum>& played) const {
    std::unordered_multiset<CardNum> to_remove(played.begin(), played.end());

    std::vector<CardNum> result;
    result.reserve(sample.size() - played.size());

    for (CardNum c : sample) {
        auto it = to_remove.find(c);
        if (it != to_remove.end()) {
            to_remove.erase(it);  // 移除这张具体牌
        }
        else {
            result.push_back(c);  // 保留
        }
    }
    sample = std::move(result);
}



void ParticleFilter::jitterParticle(Particle& p1, Particle& p2) {
    if (p1.cards.empty() || p2.cards.empty()) return;

    // 随机决定扰动幅度：交换几张牌（1~2张）
    int swap_count = (rand() % 2) + 1;

    for (int k = 0; k < swap_count; ++k) {
        if (p1.cards.empty() || p2.cards.empty()) break;

        // 从两人手牌中各自随机选一张
        int idx1 = rand() % p1.cards.size();
        int idx2 = rand() % p2.cards.size();

        // 交换
        std::swap(p1.cards[idx1], p2.cards[idx2]);
    }
}

void ParticleFilter::resampleAfterFilter(std::vector<Particle>& p1,
    std::vector<Particle>& p2,
    int target_count) {
    if (p1.empty()) {
        emergencyResamplePaired(hand_size1, hand_size2);
        return;
    }

    std::vector<Particle> new_p1, new_p2;

    // 1. 原合法粒子原封不动地保留
    new_p1 = p1;
    new_p2 = p2;

    // 2. 缺多少补多少：从原合法粒子克隆 + 扰动
    int needed = target_count - static_cast<int>(new_p1.size());

    for (int i = 0; i < needed; ++i) {
        // 从原合法粒子中随机选一个作为模板
        int idx = rand() % p1.size();
        Particle clone1 = p1[idx];
        Particle clone2 = p2[idx];

        
        jitterParticle(clone1, clone2);

        new_p1.push_back(clone1);
        new_p2.push_back(clone2);
    }

    p1 = std::move(new_p1);
    p2 = std::move(new_p2);
}


void ParticleFilter::emergencyResamplePaired(int size1, int size2) {
    particles1.clear();
    particles2.clear();

    auto full_pool = recorder.getRemainingCards();

    // 只从牌池中移除“未被打出的明牌”，因为它们需要被强制分配
    std::vector<CardNum> unplayed_public_cards;
    for (CardNum c : public_cards) {
        if (!recorder.isPlayed(c)) {
            unplayed_public_cards.push_back(c);
            auto it = std::find(full_pool.begin(), full_pool.end(), c);
            if (it != full_pool.end()) full_pool.erase(it);
        }
        // 如果已经打出，它们已经在 recorder 中被标记，full_pool 自然不包含
    }

    

    for (int i = 0; i < 100; ++i) {
        std::vector<CardNum> shuffled = full_pool;
        std::shuffle(shuffled.begin(), shuffled.end(), rng);

        std::vector<CardNum> cards1, cards2;
        int idx = 0;

        // 对手1：随机抽牌
        int random_needed1 = size1;
        
        for (int j = 0; j < random_needed1; ++j) {
            cards1.push_back(shuffled[idx++]);
        }
        // 如果对手1是地主，把未打出的明牌给他
        

        // 对手2：随机抽牌
        int random_needed2 = size2;
        
        for (int j = 0; j < random_needed2; ++j) {
            cards2.push_back(shuffled[idx++]);
        }
        // 如果对手2是地主，把未打出的明牌给他
        
        
        particles1.push_back({ cards1 });
        particles2.push_back({ cards2 });
    }
}

