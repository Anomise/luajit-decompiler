# Build trong Cursor/VSCode

## Cách 1: Sử dụng Tasks (Khuyến nghị)

1. Mở folder `luajit-decompiler` trong VSComm/VSCode
2. Nhấn `Ctrl+Shift+P` (hoặc `F1`)
3. Gõ "Tasks: Run Task"
4. Chọn:
   - **"Build LuaJIT Decompiler (MSVC)"** - Nếu có Visual Studio
   - **"Build LuaJIT Decompiler (MinGW)"** - Nếu có MinGW/GCC

Hoặc đơn giản hơn:
- Nhấn `Ctrl+Shift+B` để build (sẽ chạy default task)

## Cách 2: Sử dụng Terminal trong VSComm/VSCode

1. Mở Terminal trong VSComm/VSCode (`Ctrl+`` hoặc `View > Terminal`)
2. Chạy một trong các lệnh sau:

### Nếu có Visual Studio:
```cmd
cd luajit-decompiler-v2-master
build.bat
```

### Hoặc build thủ công:
```cmd
cd luajit-decompiler-v2-master
"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
cl /EHsc /std:c++17 /O2 /W3 main.cpp bytecode\bytecode.cpp bytecode\prototype.cpp lua\lua.cpp ast\ast.cpp /Fe:luajit-decompiler-v2.exe /link /SUBSYSTEM:CONSOLE
```

### Nếu có MinGW:
```cmd
cd luajit-decompiler-v2-master
g++ -std=c++17 -O2 -Wall -Wextra main.cpp bytecode/bytecode.cpp bytecode/prototype.cpp lua/lua.cpp ast/ast.cpp -o luajit-decompiler-v2.exe
```

## Lưu ý

- Nếu build task báo lỗi "cl is not recognized", bạn cần:
  1. Cài Visual Studio với C++ tools
  2. Hoặc mở "Developer Command Prompt for VS" trước khi mở Cursor
  3. Hoặc chạy task "Initialize VS Environment" trước

- File `.vscode/tasks.json` đã được tạo sẵn với cấu hình build
- Sau khi build thành công, file `luajit-decompiler-v2.exe` sẽ ở trong thư mục `luajit-decompiler-v2-master`

