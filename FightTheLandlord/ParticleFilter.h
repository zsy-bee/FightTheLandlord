#pragma once
#include <vector>
#include <random>
#include <algorithm>
#include"Gamer.h"
using CardNum = short;
class CardRecorder;
enum class GameType;
class Gamer;
class DouDizhuAI;
class ParticleFilter {
public:
    struct Particle {
        std::vector<CardNum> cards;
    };
private:
    

    // 两个对手的粒子池（配对存储）
    std::vector<Particle> particles1;
    std::vector<Particle> particles2;

    const CardRecorder& recorder;
    std::mt19937 rng;

    int hand_size1 = 0;
    int hand_size2 = 0;

    // 明牌（地主的底牌）
    std::vector<CardNum> public_cards;
    bool player1_holds_public = false; 
    bool player2_holds_public = false;
    GamerType player1;
    GamerType player2;

public:
    ParticleFilter(const CardRecorder& rec);

    // 初始化
    void init(GamerType player1, GamerType player2, const std::vector<CardNum>& public_cards);

    // 更新：对手出牌后调用
    void update(GamerType player, const std::vector<CardNum>& played);

    void update(int player_idx, const std::vector<CardNum>& played);
    // 获取粒子池（供决策使用）
    const std::vector<Particle>& getParticles(int player_idx) const;

    // 获取手牌数
    int getHandSize(int player_idx) const;

    // 从粒子构造 Gamer（决策时临时使用）
    Gamer particleToGamer(int player_idx, size_t particle_idx) const;
    bool canPlay(const std::vector<CardNum>& sample, const std::vector<CardNum>& played) const;
private:
    // 检查粒子是否包含指定牌
    

    // 从粒子中移除已打出的牌（原地修改）
    void removeCardsInPlace(std::vector<CardNum>& sample, const std::vector<CardNum>& played) const;

    // 计算有效粒子数
    

    // 重采样（配对）
    void resampleAfterFilter(std::vector<Particle>& p1,
        std::vector<Particle>& p2,
        int target_count);


    void jitterParticle(Particle& p1, Particle& p2);
    // 紧急重采样（配对）
    void emergencyResamplePaired(int size1, int size2);

   
};