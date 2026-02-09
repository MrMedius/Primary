#pragma once
#include <algorithm>
#include "imgui/imgui.h"

struct PerformanceData
{
    int score{ 0 };

    int totalDefeat{ 0 };
    int rDefeat{ 0 };
    int gDefeat{ 0 };
    int bDefeat{ 0 };

    int rHighestCombo{ 0 };
    int gHighestCombo{ 0 };
    int bHighestCombo{ 0 };

    int totalWeapon{ 0 };
    int rWeapon{ 0 };
    int gWeapon{ 0 };
    int bWeapon{ 0 };

    float lifeTime{ 0.0f };
    bool gameClear{ false };

    float outputDamage{ 0.0f };
    float inputDamage{ 0.0f };

    void Reset() noexcept
    {
        score = 0;

        totalDefeat = rDefeat = gDefeat = bDefeat = 0;

        rHighestCombo = gHighestCombo = bHighestCombo = 0;

        totalWeapon = rWeapon = gWeapon = bWeapon = 0;

        lifeTime = 0.0f;
        gameClear = false;

        outputDamage = 0.0f;
        inputDamage = 0.0f;
    }
};

class GameStatsCodex
{
public:
    // read only
    static const PerformanceData& Get() noexcept { return Get_().data; }

    // write
    static void Reset() noexcept { Get_().data.Reset(); }

    static void Update(float dt) noexcept
    {
        if (dt > 0.0f) Get_().data.lifeTime += dt;
    }

    static void AddScore(int s) noexcept { Get_().data.score += s; }
    static void SetGameClear() noexcept  { Get_().data.gameClear = true; }

    static void AddTotalDefeat(int c = 1) noexcept { Get_().data.totalDefeat += c; }
    static void AddRedDefeat(int c = 1) noexcept   { Get_().data.rDefeat += c; }
    static void AddGreenDefeat(int c = 1) noexcept { Get_().data.gDefeat += c; }
    static void AddBlueDefeat(int c = 1) noexcept  { Get_().data.bDefeat += c; }

    static void ReportRedCombo(int v) noexcept
    {
        if (v >= 0) Get_().data.rHighestCombo = std::max(Get_().data.rHighestCombo, v);
    }
    static void ReportGreenCombo(int v) noexcept
    {
        if (v >= 0) Get_().data.gHighestCombo = std::max(Get_().data.gHighestCombo, v);
    }
    static void ReportBlueCombo(int v) noexcept
    {
        if (v >= 0) Get_().data.bHighestCombo = std::max(Get_().data.bHighestCombo, v);
    }

    static void AddTotalWeapon(int c = 1) noexcept  { Get_().data.totalWeapon += c; }
    static void AddRedWeapon(int c = 1) noexcept    { Get_().data.rWeapon += c; }
    static void AddGreenWeapon(int c = 1) noexcept  { Get_().data.gWeapon += c; }
    static void AddBlueWeapon(int c = 1) noexcept   { Get_().data.bWeapon += c; }

    static void AddOutputDamage(float d) noexcept { if (d > 0) Get_().data.outputDamage += d; }
    static void AddInputDamage(float d) noexcept  { if (d > 0) Get_().data.inputDamage += d; }

    static void SpawnWindow()
    {
        const auto& s = GameStatsCodex::Get();

        ImGui::Begin("Game Stats", nullptr);

        if (ImGui::Button("Reset Stats"))
        {
            GameStatsCodex::Reset();
        }

        ImGui::Separator();

        if (ImGui::CollapsingHeader("Score & State", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Score: %d", s.score);
            ImGui::Text("GameClear: %s", s.gameClear ? "true" : "false");
            ImGui::Text("LifeTime: %.2f sec", s.lifeTime);
        }

        if (ImGui::CollapsingHeader("Defeat", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Total: %d", s.totalDefeat);
            ImGui::BulletText("Red:   %d", s.rDefeat);
            ImGui::BulletText("Green: %d", s.gDefeat);
            ImGui::BulletText("Blue:  %d", s.bDefeat);
        }

        if (ImGui::CollapsingHeader("Highest Combo", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Red:   %d", s.rHighestCombo);
            ImGui::Text("Green: %d", s.gHighestCombo);
            ImGui::Text("Blue:  %d", s.bHighestCombo);
        }

        if (ImGui::CollapsingHeader("Weapon", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Total: %d", s.totalWeapon);
            ImGui::BulletText("Red:   %d", s.rWeapon);
            ImGui::BulletText("Green: %d", s.gWeapon);
            ImGui::BulletText("Blue:  %d", s.bWeapon);
        }

        if (ImGui::CollapsingHeader("Damage", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("OutputDamage: %.1f", s.outputDamage);
            ImGui::Text("InputDamage:  %.1f", s.inputDamage);

            float od = s.outputDamage;
            float id = s.inputDamage;

            const float scale = 1000.0f;
            ImGui::ProgressBar((od / scale) > 1.0f ? 1.0f : (od / scale), ImVec2(220, 0), "Output");
            ImGui::ProgressBar((id / scale) > 1.0f ? 1.0f : (id / scale), ImVec2(220, 0), "Input");
        }

        ImGui::End();
    }

private:
    static GameStatsCodex& Get_() noexcept
    {
        static GameStatsCodex inst;
        return inst;
    }

private:
    PerformanceData data;
};
