// Example implementation of BytecodeWriter
// This shows how to write bytecode format (reverse of reading)
// NOTE: This is a SKELETON - full implementation requires extensive work

#include "bytecode_writer.h"
#include "..\main.h"

BytecodeWriter::BytecodeWriter() {}

BytecodeWriter::~BytecodeWriter() {
    close_file();
}

bool BytecodeWriter::write_bytecode(const Bytecode& bytecode, const std::string& outputPath) {
    if (!open_file(outputPath)) return false;
    
    writeBuffer.clear();
    
    // Write header
    write_header(bytecode.header);
    
    // Write all prototypes (need to write in reverse order for linking)
    // Main prototype is written last
    std::vector<const Bytecode::Prototype*> prototypes;
    
    // Collect all prototypes (you need to traverse the prototype tree)
    // This is simplified - actual implementation needs proper traversal
    for (const auto* proto : bytecode.prototypes) {
        if (proto != bytecode.main) {
            prototypes.push_back(proto);
        }
    }
    prototypes.push_back(bytecode.main);
    
    // Write each prototype
    for (const auto* proto : prototypes) {
        write_prototype(*proto);
    }
    
    // Write terminator (0 byte)
    write_uleb128(0);
    
    return flush_to_file();
}

void BytecodeWriter::write_header(const Bytecode::Header& header) {
    // Write magic bytes
    if (header.version == Bytecode::BC_VERSION_MODIFIED) {
        write_byte(BC_HEADER_MODIFIED[0]);
        write_byte(BC_HEADER_MODIFIED[1]);
        write_byte(BC_HEADER_MODIFIED[2]);
        write_byte(BC_VERSION_MODIFIED); // Write modified version
    } else {
        write_byte(BC_HEADER[0]);
        write_byte(BC_HEADER[1]);
        write_byte(BC_HEADER[2]);
        write_byte(header.version);
    }
    
    // Write flags
    write_byte(header.flags);
    
    // Write chunkname if not stripped
    if (!(header.flags & BC_F_STRIP) && header.chunkname.size()) {
        write_uleb128(header.chunkname.size());
        write_string(header.chunkname);
    }
}

void BytecodeWriter::write_prototype(const Bytecode::Prototype& proto) {
    size_t startPos = writeBuffer.size();
    
    // Write prototype size placeholder (will fill later)
    size_t sizePos = writeBuffer.size();
    writeBuffer.resize(sizePos + 4); // Reserve space for ULEB128
    
    // Write prototype header
    write_prototype_header(proto);
    
    // Write instructions
    write_prototype_instructions(proto);
    
    // Write upvalues
    write_prototype_upvalues(proto);
    
    // Write constants
    write_prototype_constants(proto);
    
    // Write number constants
    write_prototype_number_constants(proto);
    
    // Write debug info if present
    if (proto.header.hasDebugInfo) {
        write_prototype_debug_info(proto);
    }
    
    // Calculate and write prototype size
    size_t protoSize = writeBuffer.size() - startPos - 4; // Exclude size field
    // Write ULEB128 at sizePos
    // (Need to implement proper ULEB128 encoding)
}

void BytecodeWriter::write_prototype_header(const Bytecode::Prototype& proto) {
    write_byte(proto.header.flags);
    write_byte(proto.header.parameters);
    write_byte(proto.header.framesize);
    write_byte(proto.upvalues.size());
    write_uleb128(proto.constants.size());
    write_uleb128(proto.numberConstants.size());
    write_uleb128(proto.instructions.size());
    
    if (proto.header.hasDebugInfo) {
        write_uleb128(1); // Has debug info
        write_uleb128(proto.header.firstLine);
        write_uleb128(proto.header.lineCount);
    } else {
        write_uleb128(0); // No debug info
    }
}

void BytecodeWriter::write_prototype_instructions(const Bytecode::Prototype& proto) {
    for (const auto& inst : proto.instructions) {
        write_instruction(inst, /* version from bytecode header */);
    }
}

void BytecodeWriter::write_instruction(const Bytecode::Instruction& inst, uint8_t version) {
    // Encode opcode (need to handle version differences)
    uint8_t opcode = inst.type;
    
    // Handle version 1 opcode mapping
    if (version == Bytecode::BC_VERSION_1) {
        // Map opcodes (reverse of get_op_type)
        // This is complex - see instructions.h for mapping
    }
    
    write_byte(opcode);
    
    // Write operands based on format
    if (is_op_abc_format(inst.type)) {
        write_instruction_abc(inst);
    } else {
        write_instruction_ad(inst);
    }
}

void BytecodeWriter::write_instruction_abc(const Bytecode::Instruction& inst) {
    write_byte(inst.a);
    write_byte(inst.c);
    write_byte(inst.b);
}

void BytecodeWriter::write_instruction_ad(const Bytecode::Instruction& inst) {
    write_byte(inst.a);
    write_byte(inst.d & 0xFF);        // D low byte
    write_byte((inst.d >> 8) & 0xFF); // D high byte
}

void BytecodeWriter::write_prototype_upvalues(const Bytecode::Prototype& proto) {
    for (uint16_t upvalue : proto.upvalues) {
        write_byte(upvalue & 0xFF);
        write_byte((upvalue >> 8) & 0xFF);
    }
}

void BytecodeWriter::write_prototype_constants(const Bytecode::Prototype& proto) {
    // Write constants in reverse order (for linking child prototypes)
    for (auto it = proto.constants.rbegin(); it != proto.constants.rend(); ++it) {
        write_constant(*it);
    }
}

void BytecodeWriter::write_constant(const Bytecode::Constant& constant) {
    write_uleb128(constant.type);
    
    switch (constant.type) {
    case BC_KGC_CHILD:
        // Child prototype is already written (they're written in reverse order)
        // Just need to track linking
        break;
    case BC_KGC_TAB:
        write_uleb128(constant.array.size());
        write_uleb128(constant.table.size());
        for (const auto& arr : constant.array) {
            write_table_constant(arr);
        }
        for (const auto& tab : constant.table) {
            write_table_constant(tab.key);
            write_table_constant(tab.value);
        }
        break;
    case BC_KGC_I64:
    case BC_KGC_U64:
    case BC_KGC_COMPLEX:
        write_qword(constant.cdata);
        break;
    case BC_KGC_STR:
        write_uleb128(constant.string.size());
        write_string(constant.string);
        break;
    }
}

void BytecodeWriter::write_table_constant(const Bytecode::TableConstant& tableConst) {
    write_byte(tableConst.type);
    
    switch (tableConst.type) {
    case BC_KTAB_NIL:
    case BC_KTAB_FALSE:
    case BC_KTAB_TRUE:
        // No value
        break;
    case BC_KTAB_INT:
        write_dword(tableConst.integer);
        break;
    case BC_KTAB_NUM:
        write_qword(tableConst.number);
        break;
    case BC_KTAB_STR:
        write_uleb128(tableConst.string.size());
        write_string(tableConst.string);
        break;
    }
}

void BytecodeWriter::write_prototype_number_constants(const Bytecode::Prototype& proto) {
    for (const auto& numConst : proto.numberConstants) {
        write_byte(numConst.type);
        if (numConst.type == BC_KNUM_INT) {
            write_dword(numConst.integer);
        } else {
            write_qword(numConst.number);
        }
    }
}

void BytecodeWriter::write_prototype_debug_info(const Bytecode::Prototype& proto) {
    // Write line map
    for (uint32_t line : proto.lineMap) {
        write_uleb128(line);
    }
    
    // Write upvalue names
    for (const auto& name : proto.upvalueNames) {
        write_uleb128(name.size());
        write_string(name);
    }
    
    // Write variable info
    for (const auto& varInfo : proto.variableInfos) {
        write_uleb128(varInfo.type);
        if (varInfo.type == BC_VAR_STR) {
            write_uleb128(varInfo.name.size());
            write_string(varInfo.name);
            write_byte(varInfo.isParameter ? 1 : 0);
            write_uleb128(varInfo.scopeBegin);
            write_uleb128(varInfo.scopeEnd);
        }
    }
    
    // End marker
    write_uleb128(BC_VAR_END);
}

void BytecodeWriter::write_uleb128(uint32_t value) {
    if (value < 0x80) {
        write_byte(value);
    } else {
        write_byte((value & 0x7F) | 0x80);
        value >>= 7;
        while (value >= 0x80) {
            write_byte((value & 0x7F) | 0x80);
            value >>= 7;
        }
        write_byte(value);
    }
}

void BytecodeWriter::write_string(const std::string& str) {
    writeBuffer.insert(writeBuffer.end(), str.begin(), str.end());
}

void BytecodeWriter::write_byte(uint8_t byte) {
    writeBuffer.push_back(byte);
}

void BytecodeWriter::write_word(uint16_t word) {
    write_byte(word & 0xFF);
    write_byte((word >> 8) & 0xFF);
}

void BytecodeWriter::write_dword(uint32_t dword) {
    write_word(dword & 0xFFFF);
    write_word((dword >> 16) & 0xFFFF);
}

void BytecodeWriter::write_qword(uint64_t qword) {
    write_dword(qword & 0xFFFFFFFF);
    write_dword((qword >> 32) & 0xFFFFFFFF);
}

bool BytecodeWriter::open_file(const std::string& path) {
    file = CreateFileA(path.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    return file != INVALID_HANDLE_VALUE;
}

void BytecodeWriter::close_file() {
    if (file != INVALID_HANDLE_VALUE) {
        CloseHandle(file);
        file = INVALID_HANDLE_VALUE;
    }
}

bool BytecodeWriter::flush_to_file() {
    if (file == INVALID_HANDLE_VALUE) return false;
    
    DWORD bytesWritten = 0;
    bool success = WriteFile(file, writeBuffer.data(), writeBuffer.size(), &bytesWritten, NULL);
    return success && (bytesWritten == writeBuffer.size());
}



