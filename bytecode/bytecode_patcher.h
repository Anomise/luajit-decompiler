// BytecodePatcher - Patch compiled bytecode to match original structure
// Use this when you have both original bytecode and decompiled Lua source

#ifndef BYTECODE_PATCHER_H
#define BYTECODE_PATCHER_H

#include "bytecode.h"
#include <vector>
#include <map>
#include <string>

struct BytecodeDiff {
    // Structure differences
    bool headerDifferent = false;
    bool prototypeCountDifferent = false;
    bool instructionOrderDifferent = false;
    bool constantIndicesDifferent = false;
    
    // Detailed differences
    std::vector<uint32_t> differentPrototypes;
    std::map<uint32_t, std::vector<uint32_t>> instructionOrderDiffs; // prototype index -> instruction indices
    std::map<uint32_t, std::map<uint32_t, uint32_t>> constantIndexMapping; // prototype index -> (new index -> old index)
    
    // Similarity percentage
    double similarity = 0.0;
};

class BytecodePatcher {
public:
    BytecodePatcher();
    ~BytecodePatcher();
    
    // Compare two bytecode files
    BytecodeDiff compare(const Bytecode& original, const Bytecode& compiled);
    
    // Patch compiled bytecode to match original structure
    bool patch(Bytecode& compiled, const Bytecode& original, const BytecodeDiff& diff);
    
    // Patch only constants (safest, highest similarity)
    bool patch_constants_only(Bytecode& target, const Bytecode& reference);
    
    // Patch instruction order
    bool patch_instruction_order(Bytecode& target, const Bytecode& reference);
    
    // Patch prototype structure
    bool patch_prototype_structure(Bytecode& target, const Bytecode& reference);
    
    // Calculate similarity percentage
    double calculate_similarity(const Bytecode& a, const Bytecode& b);
    
    // Save/load metadata for round-trip
    bool save_metadata(const Bytecode& bytecode, const std::string& path);
    bool load_metadata(Bytecode& bytecode, const std::string& path);
    
private:
    // Helper functions
    bool compare_headers(const Bytecode::Header& a, const Bytecode::Header& b);
    bool compare_prototypes(const Bytecode::Prototype& a, const Bytecode::Prototype& b);
    bool compare_instructions(const std::vector<Bytecode::Instruction>& a, 
                              const std::vector<Bytecode::Instruction>& b);
    bool compare_constants(const std::vector<Bytecode::Constant>& a,
                          const std::vector<Bytecode::Constant>& b);
    
    // Patch helpers
    void remap_constant_indices(Bytecode::Prototype& proto, 
                                const std::map<uint32_t, uint32_t>& indexMap);
    void reorder_instructions(Bytecode::Prototype& proto,
                              const std::vector<uint32_t>& newOrder);
};

#endif // BYTECODE_PATCHER_H



