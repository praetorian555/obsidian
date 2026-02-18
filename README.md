# Obsidian

Obsidian is a C++20 reflection code generation tool. It uses the Clang C API (libclang) to parse C++ source files, extract metadata about types marked with special macros, and generate C++ headers that provide both compile-time and runtime reflection.

## How It Works

1. You annotate your enums, classes, and properties with `OBS_ENUM()`, `OBS_CLASS()`, and `OBS_PROP()` macros
2. Obsidian parses your source files using libclang and extracts type metadata
3. It generates a `reflection.hpp` header with template specializations and runtime lookup tables

The macros expand to nothing — they serve only as markers for the parser.

## Prerequisites

- **CMake 3.28+**
- **C++20 compiler** (MSVC 17+ on Windows, GCC/Clang on Linux)
- **LLVM/Clang installation** with libclang (header files and libraries)

The [Opal](https://github.com/praetorian555/opal) utility library is fetched automatically via CMake FetchContent.

## Building

```bash
# Windows (MSVC)
cmake -B build -G "Visual Studio 17 2022" -DOBS_LLVM_PATH=<path-to-llvm>
cmake --build build --config Debug

# Linux
cmake -B build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DOBS_LLVM_PATH=<path-to-llvm>
cmake --build build
```

`OBS_LLVM_PATH` must point to the root of your LLVM installation (containing `bin/`, `lib/`, and `include/` directories). On Windows, `libclang.dll` is automatically copied to the build output.

### Build Options

| Option | Default | Description |
|---|---|---|
| `OBS_LLVM_PATH` | *(required)* | Path to LLVM/Clang installation |
| `OBS_HARDENING` | `ON` | Enable AddressSanitizer and UndefinedBehaviorSanitizer |

## Usage

### 1. Annotate Your Types

Include the `obs/obs.hpp` header and mark types with macros:

```cpp
#include "obs/obs.hpp"

namespace MyGame
{

/// A character class.
OBS_ENUM()
enum class CharacterClass : int8_t
{
    /** The warrior. */
    Warrior = 0,
    Mage,
    Rogue,
};

/// Player character data.
OBS_CLASS()
struct Character
{
    OBS_PROP()
    int32_t health = 100;

    OBS_PROP()
    float speed = 5.0f;

    OBS_PROP()
    CharacterClass char_class = CharacterClass::Warrior;
};

} // namespace MyGame
```

### 2. Run the Tool

Process a single file:

```bash
obsidian input-file=my_types.hpp output-dir=generated \
    compile-options=-I/usr/include,-DMY_DEFINE
```

Process all headers in a directory (recursively finds `.h` and `.hpp` files):

```bash
obsidian input-dir=src/include output-dir=generated \
    compile-options=-Isrc/include,-DMY_DEFINE
```

### Command-Line Arguments

| Argument | Description |
|---|---|
| `input-file=<path>` | Path to a single input header file |
| `input-dir=<path>` | Path to a directory of input headers (recursive) |
| `output-dir=<path>` | Output directory for generated headers |
| `compile-options=<opts>` | Comma-separated Clang compile options |
| `dump-ast=true` | Print extracted metadata to stdout |
| `separate-files=true` | Generate separate files per type *(not yet implemented)* |

You must specify either `input-file` or `input-dir`, not both.

### 3. Integrate into CMake

Add a custom target that runs obsidian before building your project:

```cmake
add_custom_target(
    generate_reflection
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/generated
    COMMAND $<TARGET_FILE:obsidian>
        input-file=${CMAKE_CURRENT_SOURCE_DIR}/include/my_types.hpp
        output-dir=${CMAKE_CURRENT_BINARY_DIR}/generated
        compile-options=-I${CMAKE_CURRENT_SOURCE_DIR}/include
    DEPENDS obsidian
    COMMENT "Generating reflection headers..."
    VERBATIM
)

add_executable(my_app src/main.cpp)
add_dependencies(my_app generate_reflection)
target_include_directories(my_app PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)
```

### 4. Use the Generated Reflection

Include the generated `reflection.hpp` in your code. It can be safely included from multiple translation units.

**Compile-time enum reflection:**

```cpp
#include "reflection.hpp"

using namespace MyGame;

// Enum metadata
Obs::Enum<CharacterClass>::GetEnumName();          // "CharacterClass"
Obs::Enum<CharacterClass>::GetFullEnumName();       // "MyGame::CharacterClass"
Obs::Enum<CharacterClass>::GetDescription();        // "A character class."

// Value conversions
Obs::Enum<CharacterClass>::GetValueName(CharacterClass::Warrior);   // "Warrior"
Obs::Enum<CharacterClass>::GetValueDescription(CharacterClass::Warrior); // "The warrior."
Obs::Enum<CharacterClass>::GetValue("Mage");        // CharacterClass::Mage
Obs::Enum<CharacterClass>::GetValue(2);             // CharacterClass::Rogue
Obs::Enum<CharacterClass>::GetUnderlyingValue(CharacterClass::Warrior); // 0
```

**Compile-time class reflection:**

Properties can be both POD types (e.g., `int`, `float`, `enum`) and non-POD types (e.g., `std::string`). For POD types, read and write operations use raw memory copies. For non-POD types, copy assignment is used, so the type must be copy-assignable.

```cpp
// Class metadata
Obs::Class<Character>::GetName();        // "Character"
Obs::Class<Character>::GetScopedName();  // "MyGame::Character"
Obs::Class<Character>::GetDescription(); // "Player character data."

// Iterate properties
for (const auto& prop : Obs::Class<Character>::Get())
{
    printf("%s: type=%s, offset=%d, size=%d\n",
           prop.name, prop.type_name, prop.offset, prop.size);
}

// Read and write by property name
Character player;
int hp = 0;
Obs::Class<Character>::Read(&hp, &player, "health");    // hp == 100

int new_hp = 50;
Obs::Class<Character>::Write(&new_hp, &player, "health"); // player.health == 50
```

**Runtime reflection (string-based lookup):**

```cpp
// Look up enum by name at runtime
const Obs::EnumEntry* entry = nullptr;
Obs::EnumCollection::GetEnum("CharacterClass", entry);
// entry->name, entry->items, entry->underlying_type_size, ...

// Convert enum value by string
CharacterClass cls;
Obs::EnumCollection::GetValue(&cls, "CharacterClass", "Mage");

// Look up class by name at runtime
const Obs::ClassEntry* class_entry = nullptr;
Obs::ClassCollection::GetClassEntry("Character", class_entry);

// Read/write properties by class and property name strings
int hp = 0;
Character player;
Obs::ClassCollection::Read(&hp, &player, "Character", "health");
Obs::ClassCollection::Write(&hp, &player, "Character", "health");
```

## Running Tests

The project includes two test targets that both run the same Catch2 test suite. One generates reflection from a single file, the other from a directory:

```bash
# Windows
./build/Debug/test-cpp-project.exe       # input-file mode
./build/Debug/test-cpp-project-dir.exe   # input-dir mode

# Linux
./build/test-cpp-project
./build/test-cpp-project-dir
```

## Project Structure

```
obsidian/
    obsidian-main.cpp   # Clang AST traversal, macro detection, CLI
    generator.cpp       # Code generation from templates
    generator.hpp       # Generator interface
    templates.hpp       # Embedded C++ templates with __placeholder__ tokens
    types.hpp           # Core data structures (CppEnum, CppClass, CppProperty, etc.)
include/obs/
    obs.hpp             # User-facing header with OBS_ENUM/OBS_CLASS/OBS_PROP macros
test-cpp/
    src/main.cpp        # Catch2 tests for enum/class reflection
    src/secondary.cpp   # Multi-TU inclusion test
    include/types.hpp   # Test types decorated with OBS_* macros
cmake/
    compiler-warnings.cmake
    compiler-options.cmake
    sanitizers.cmake
```

## What Is Left to Implement

### Features

- **Separate-files output mode** — The `separate-files` flag is parsed but the code path is not implemented. When enabled, each enum/class should get its own generated header, plus dedicated headers for `EnumCollection` and `ClassCollection`.
- **Attribute support in code generation** — Attributes are parsed from macro arguments (e.g., `OBS_PROP("min=0" "max=100")`) and stored in the data structures, but the generator does not yet expose them in the generated code.
- **Clang diagnostic checking** — The parser does not check for Clang compilation errors/warnings after parsing a translation unit. Invalid input files are silently accepted.

### Missing Tests

- **`ClassCollection` runtime API** — The runtime class lookup (`GetClassEntry`, `GetClassProperties`, `GetProperty`, `Read`, `Write`) is generated but has no test coverage.
- **`is_pod` property flag** — Extracted for each property but never asserted in tests.
- **Property descriptions** — Class property descriptions are generated but not tested.
- **Macro attributes** — Attribute parsing is implemented but there are no tests for attributes on `OBS_ENUM`, `OBS_CLASS`, or `OBS_PROP`.
- **`class` vs `struct`** — Only structs are tested. Classes marked with `OBS_CLASS()` are not tested.
- **Global namespace types** — All test types are in namespaces. Types in the global namespace are not tested.
- **Empty types** — Enums with no constants and classes with no properties are not tested.
- **Error cases** — No tests for invalid input files, missing macros, or malformed attributes.

## AI Disclosure

Portions of this software were developed with the assistance of AI tools (Claude by Anthropic). All AI-generated code was reviewed and approved by human contributors.

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.
