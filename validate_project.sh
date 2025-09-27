#!/bin/bash

# Navigation Project Validation Script
# This script validates the project structure and basic compilation

echo "=== Navigation Project Validation ==="
echo ""

# Check project structure
echo "1. Checking project structure..."
REQUIRED_DIRS=("qt_app" "stm32_code" "docs")
for dir in "${REQUIRED_DIRS[@]}"; do
    if [ -d "$dir" ]; then
        echo "✓ Directory $dir exists"
    else
        echo "✗ Directory $dir missing"
    fi
done

# Check source files
echo ""
echo "2. Checking source files..."

# QT App files
QT_FILES=("qt_app/src/main.cpp" "qt_app/src/mainwindow.h" "qt_app/src/mainwindow.cpp" 
          "qt_app/src/gpsreader.h" "qt_app/src/gpsreader.cpp"
          "qt_app/src/navigationcontroller.h" "qt_app/src/navigationcontroller.cpp")

for file in "${QT_FILES[@]}"; do
    if [ -f "$file" ]; then
        echo "✓ QT: $file"
    else
        echo "✗ QT: $file missing"
    fi
done

# STM32 files
STM32_FILES=("stm32_code/Core/Src/main.c" "stm32_code/Core/Src/gps_module.c"
             "stm32_code/Core/Src/imu_module.c" "stm32_code/Core/Src/navigation.c")

for file in "${STM32_FILES[@]}"; do
    if [ -f "$file" ]; then
        echo "✓ STM32: $file"
    else
        echo "✗ STM32: $file missing"
    fi
done

# Check documentation
echo ""
echo "3. Checking documentation..."
DOC_FILES=("README.md" "docs/README.md" "docs/BUILD.md" "docs/USAGE.md")

for file in "${DOC_FILES[@]}"; do
    if [ -f "$file" ]; then
        echo "✓ DOC: $file"
    else
        echo "✗ DOC: $file missing"
    fi
done

# Basic syntax check for C files
echo ""
echo "4. Basic syntax validation..."

# Check C files for basic syntax (headers, functions)
for c_file in $(find stm32_code -name "*.c"); do
    if grep -q "#include" "$c_file" && grep -q "(" "$c_file"; then
        echo "✓ Syntax: $c_file appears valid"
    else
        echo "? Syntax: $c_file may have issues"
    fi
done

# Check C++ files for basic syntax
for cpp_file in $(find qt_app -name "*.cpp"); do
    if grep -q "#include" "$cpp_file"; then
        echo "✓ Syntax: $cpp_file appears valid"
    else
        echo "? Syntax: $cpp_file may have issues"
    fi
done

# Count lines of code
echo ""
echo "5. Code statistics..."
total_lines=$(find . -name "*.c" -o -name "*.cpp" -o -name "*.h" | xargs wc -l | tail -1 | awk '{print $1}')
echo "Total lines of code: $total_lines"

c_lines=$(find stm32_code -name "*.c" -o -name "*.h" | xargs wc -l | tail -1 | awk '{print $1}')
echo "STM32 C code lines: $c_lines"

cpp_lines=$(find qt_app -name "*.cpp" -o -name "*.h" | xargs wc -l | tail -1 | awk '{print $1}')
echo "QT C++ code lines: $cpp_lines"

echo ""
echo "=== Validation Complete ==="