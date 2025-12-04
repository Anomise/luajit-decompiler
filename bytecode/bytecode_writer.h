// Example: BytecodeWriter class structure
// This is a template/skeleton for implementing bytecode writing functionality
// To use this, you need to implement all the write methods

#ifndef BYTECODE_WRITER_H
#define BYTECODE_WRITER_H

#include "bytecode.h"
#include <vector>
#include <string>

class BytecodeWriter {
public:
    BytecodeWriter();
    ~BytecodeWriter();

    // Main write function - writes bytecode to file
    bool write_bytecode(const Bytecode& bytecode, const std::string& outputPath);

private:
    // Header writing
    void write_header(const Bytecode::Header& header);
    
    // Prototype writing
    void write_prototype(const Bytecode::Prototype& proto);
    void write_prototype_header(const Bytecode::Prototype& proto);
    void write_prototype_instructions(const Bytecode::Prototype& proto);
    void write_prototype_upvalues(const Bytecode::Prototype& proto);
    void write_prototype_constants(const Bytecode::Prototype& proto);
    void write_prototype_number_constants(const Bytecode::Prototype& proto);
    void write_prototype_debug_info(const Bytecode::Prototype& proto);
    
    // Instruction encoding
    void write_instruction(const Bytecode::Instruction& inst, uint8_t version);
    void write_instruction_abc(const Bytecode::Instruction& inst);
    void write_instruction_ad(const Bytecode::Instruction& inst);
    
    // Constant writing
    void write_constant(const Bytecode::Constant& constant);
    void write_table_constant(const Bytecode::TableConstant& tableConst);
    
    // Utility functions
    void write_uleb128(uint32_t value);
    void write_string(const std::string& str);
    void write_byte(uint8_t byte);
    void write_word(uint16_t word);
    void write_dword(uint32_t dword);
    void write_qword(uint64_t qword);
    
    // File operations
    bool open_file(const std::string& path);
    void close_file();
    bool flush_to_file();
    
    std::vector<uint8_t> writeBuffer;
    HANDLE file = INVALID_HANDLE_VALUE;
    
    // Track prototype order for linking
    std::vector<const Bytecode::Prototype*> prototypeOrder;
};

#endif // BYTECODE_WRITER_H



