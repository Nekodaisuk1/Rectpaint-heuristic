#include <bits/stdc++.h>
using namespace std;

int main(int argc, char** argv) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " input.txt output.txt\n";
        return 1;
    }
    
    const string input_file = argv[1];
    const string output_file = argv[2];
    
    // Read input
    ifstream fin(input_file);
    if (!fin) {
        cout << "valid: 0\n";
        cout << "score: 1000000000000000000\n";
        cout << "diff: -1\n";
        cout << "moves: -1\n";
        cout << "reason: Cannot open input file\n";
        return 0;
    }
    
    int N;
    long long seed;
    if (!(fin >> N >> seed)) {
        cout << "valid: 0\n";
        cout << "score: 1000000000000000000\n";
        cout << "diff: -1\n";
        cout << "moves: -1\n";
        cout << "reason: Cannot read N and seed\n";
        return 0;
    }
    
    if (N != 80) {
        cout << "valid: 0\n";
        cout << "score: 1000000000000000000\n";
        cout << "diff: -1\n";
        cout << "moves: -1\n";
        cout << "reason: N must be 80\n";
        return 0;
    }
    
    vector<string> T(N);
    for (int i = 0; i < N; i++) {
        if (!(fin >> T[i])) {
            cout << "valid: 0\n";
            cout << "score: 1000000000000000000\n";
            cout << "diff: -1\n";
            cout << "moves: -1\n";
            cout << "reason: Cannot read board line " << i << "\n";
            return 0;
        }
        if ((int)T[i].size() != N) {
            cout << "valid: 0\n";
            cout << "score: 1000000000000000000\n";
            cout << "diff: -1\n";
            cout << "moves: -1\n";
            cout << "reason: Board line " << i << " has wrong length\n";
            return 0;
        }
        for (char c : T[i]) {
            if (c != '.' && c != '-' && c != '+') {
                cout << "valid: 0\n";
                cout << "score: 1000000000000000000\n";
                cout << "diff: -1\n";
                cout << "moves: -1\n";
                cout << "reason: Invalid character in board\n";
                return 0;
            }
        }
    }
    fin.close();
    
    // Read output
    ifstream fout(output_file);
    if (!fout) {
        cout << "valid: 0\n";
        cout << "score: 1000000000000000000\n";
        cout << "diff: -1\n";
        cout << "moves: -1\n";
        cout << "reason: Cannot open output file\n";
        return 0;
    }
    
    int M;
    if (!(fout >> M)) {
        cout << "valid: 0\n";
        cout << "score: 1000000000000000000\n";
        cout << "diff: -1\n";
        cout << "moves: -1\n";
        cout << "reason: Cannot read M\n";
        return 0;
    }
    
    if (M < 0 || M > 800) {
        cout << "valid: 0\n";
        cout << "score: 1000000000000000000\n";
        cout << "diff: -1\n";
        cout << "moves: " << M << "\n";
        cout << "reason: M must be in [0, 800]\n";
        return 0;
    }
    
    // Initialize board A (all '.')
    vector<string> A(N, string(N, '.'));
    
    // Apply moves
    for (int i = 0; i < M; i++) {
        int r1, c1, r2, c2;
        char x;
        if (!(fout >> r1 >> c1 >> r2 >> c2 >> x)) {
            cout << "valid: 0\n";
            cout << "score: 1000000000000000000\n";
            cout << "diff: -1\n";
            cout << "moves: " << i << "\n";
            cout << "reason: Cannot read move " << i << "\n";
            return 0;
        }
        
        // Validate coordinates
        if (r1 < 0 || r1 >= N || r2 < 0 || r2 >= N ||
            c1 < 0 || c1 >= N || c2 < 0 || c2 >= N) {
            cout << "valid: 0\n";
            cout << "score: 1000000000000000000\n";
            cout << "diff: -1\n";
            cout << "moves: " << i << "\n";
            cout << "reason: Coordinates out of range in move " << i << "\n";
            return 0;
        }
        
        // Validate color
        if (x != '.' && x != '-' && x != '+') {
            cout << "valid: 0\n";
            cout << "score: 1000000000000000000\n";
            cout << "diff: -1\n";
            cout << "moves: " << i << "\n";
            cout << "reason: Invalid color in move " << i << "\n";
            return 0;
        }
        
        // Normalize rectangle
        int rmin = min(r1, r2);
        int rmax = max(r1, r2);
        int cmin = min(c1, c2);
        int cmax = max(c1, c2);
        
        // Apply paint
        for (int r = rmin; r <= rmax; r++) {
            for (int c = cmin; c <= cmax; c++) {
                A[r][c] = x;
            }
        }
    }
    fout.close();
    
    // Calculate diff
    int diff = 0;
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            if (A[r][c] != T[r][c]) {
                diff++;
            }
        }
    }
    
    // Calculate score: 10000 * diff + M
    long long score = 10000LL * diff + M;
    
    // Output result
    cout << "valid: 1\n";
    cout << "score: " << score << "\n";
    cout << "diff: " << diff << "\n";
    cout << "moves: " << M << "\n";
    
    return 0;
}
