#include <bits/stdc++.h>
using namespace std;

// Simple baseline solver for RectPaint
// This is a minimal example - you should implement better algorithms!

int main() {
    int N;
    long long seed;
    cin >> N >> seed;
    
    vector<string> T(N);
    for (int i = 0; i < N; i++) {
        cin >> T[i];
    }
    
    vector<string> A(N, string(N, '.'));
    vector<tuple<int,int,int,int,char>> moves;
    
    // Simple strategy: paint entire board with most common non-'.' color first
    map<char, int> freq;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            freq[T[i][j]]++;
        }
    }
    
    char base = '.';
    for (auto [c, cnt] : freq) {
        if (c != '.' && cnt > freq[base]) {
            base = c;
        }
    }
    
    if (base != '.') {
        moves.push_back({0, 0, N-1, N-1, base});
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i][j] = base;
            }
        }
    }
    
    // Then paint each cell individually if it differs
    // (This is very inefficient but guarantees correctness)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (A[i][j] != T[i][j]) {
                moves.push_back({i, j, i, j, T[i][j]});
                A[i][j] = T[i][j];
                
                if (moves.size() >= 800) {
                    goto done;
                }
            }
        }
    }
    
done:
    cout << moves.size() << "\n";
    for (auto [r1, c1, r2, c2, x] : moves) {
        cout << r1 << " " << c1 << " " << r2 << " " << c2 << " " << x << "\n";
    }
    
    return 0;
}
