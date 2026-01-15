#pragma once

#include <vector>
#include "../template/BranchLayout.h"
#include "../template/GeneratedBranch.h"

// Générateur de layout - UNIQUEMENT Grid maintenant
class BranchLayoutGenerator {
public:
    // Génère les branches selon le layout (appelle GenerateGrid)
    static void Generate(
        const BranchLayout& layout,
        std::vector<GeneratedBranch>& outBranches
    );

private:
    // Génère une grille dans la zone sphérique
    static void GenerateGrid(
        const BranchLayout& layout,
        std::vector<GeneratedBranch>& out
    );
};