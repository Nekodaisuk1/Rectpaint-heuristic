#!/bin/bash
# Quick start script for RectPaint Heuristic

set -e

echo "🎨 RectPaint Heuristic - Quick Start"
echo "===================================="
echo ""

# Check if tools are compiled
if [ ! -f "tools/gen/gen" ]; then
    echo "📦 Compiling generator..."
    cd tools/gen
    g++ -std=c++17 -O2 gen.cpp -o gen
    cd ../..
    echo "✅ Generator compiled"
fi

if [ ! -f "tools/judge/judge" ]; then
    echo "📦 Compiling judge..."
    cd tools/judge
    g++ -std=c++17 -O2 judge.cpp -o judge
    cd ../..
    echo "✅ Judge compiled"
fi

echo ""
echo "📝 Generating test case (seed=100, tier=1)..."
./tools/gen/gen 100 1 > test_input.txt

echo "✅ Generated test_input.txt"
echo ""
echo "📊 Sample test case info:"
head -1 test_input.txt
echo ""

echo "Next steps:"
echo "1. Create your solver and run: ./your_solver < test_input.txt > test_output.txt"
echo "2. Judge your solution: ./tools/judge/judge test_input.txt test_output.txt"
echo "3. Visualize: Open tools/vis/viewer.html in browser"
echo ""
echo "For more info, see README.md"
