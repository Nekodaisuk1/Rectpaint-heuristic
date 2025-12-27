#include <bits/stdc++.h>
using namespace std;

static constexpr int N = 80;
static constexpr int NUM_MODES = 6;

struct Params {
    int noise_points;      // ランダム単点ノイズ数
    int holes;             // くり抜き穴数
    int high_freq_strength; // 高周波成分の強度 (0-100)
    int dot_density;       // '.' の密度 (0-100)
    int small_islands;     // 小島(1-3セル矩形)数
    int overlay_layers;    // 重ねる矩形レイヤー数
    int small_rect_count;  // 小矩形の数（サイズ2-10）
    int medium_rect_count; // 中矩形の数（サイズ10-25）
};

// Tier別パラメータ設定
Params get_tier_params(int tier) {
    Params p;
    switch(tier) {
        case 0: // easy
            p.noise_points = 5;
            p.holes = 2;
            p.high_freq_strength = 10;
            p.dot_density = 5;
            p.small_islands = 3;
            p.overlay_layers = 80;
            p.small_rect_count = 60;
            p.medium_rect_count = 40;
            break;
        case 1: // normal
            p.noise_points = 15;
            p.holes = 5;
            p.high_freq_strength = 25;
            p.dot_density = 15;
            p.small_islands = 10;
            p.overlay_layers = 35;
            p.small_rect_count = 30;
            p.medium_rect_count = 15;
            break;
        case 2: // hard
            p.noise_points = 30;
            p.holes = 10;
            p.high_freq_strength = 45;
            p.dot_density = 25;
            p.small_islands = 20;
            p.overlay_layers = 80;
            p.small_rect_count = 80;
            p.medium_rect_count = 40;
            break;
        case 3: // insane
            p.noise_points = 50;
            p.holes = 15;
            p.high_freq_strength = 70;
            p.dot_density = 30;
            p.small_islands = 35;
            p.overlay_layers = 200;
            p.small_rect_count = 150;
            p.medium_rect_count = 80;
            break;
        default:
            p = get_tier_params(1);
    }
    return p;
}

class Generator {
    mt19937_64 rng;
    vector<string> board;
    
public:
    Generator(long long seed) : rng(seed), board(N, string(N, '.')) {}
    
    char random_color() {
        static const char colors[3] = {'.', '-', '+'};
        return colors[rng() % 3];
    }
    
    char random_nonzero_color() {
        return (rng() % 2 == 0) ? '-' : '+';
    }
    
    void fill_rect(int r1, int c1, int r2, int c2, char color) {
        int rmin = min(r1, r2);
        int rmax = max(r1, r2);
        int cmin = min(c1, c2);
        int cmax = max(c1, c2);
        for (int r = rmin; r <= rmax && r < N; r++) {
            for (int c = cmin; c <= cmax && c < N; c++) {
                board[r][c] = color;
            }
        }
    }
    
    // Mode 0: RectOverlay
    void mode_rect_overlay(const Params& p) {
        int num_rects = 20 + rng() % 30;
        for (int i = 0; i < num_rects; i++) {
            int r1 = rng() % N;
            int c1 = rng() % N;
            int h = 5 + rng() % 30;
            int w = 5 + rng() % 30;
            int r2 = min((int)N-1, r1 + h);
            int c2 = min((int)N-1, c1 + w);
            char color = random_nonzero_color();
            fill_rect(r1, c1, r2, c2, color);
        }
    }
    
    // Mode 1: Frames+Holes
    void mode_frames_holes(const Params& p) {
        // Fill with base color
        char base = random_nonzero_color();
        fill_rect(0, 0, N-1, N-1, base);
        
        // Add frames
        int num_frames = 3 + rng() % 5;
        for (int i = 0; i < num_frames; i++) {
            int thickness = 2 + rng() % 5;
            int offset = i * 8 + 5;
            if (offset + thickness >= N/2) break;
            char color = (base == '-') ? '+' : '-';
            fill_rect(offset, offset, N-1-offset, N-1-offset, color);
            fill_rect(offset+thickness, offset+thickness, 
                     N-1-offset-thickness, N-1-offset-thickness, base);
        }
        
        // Add holes
        for (int i = 0; i < p.holes; i++) {
            int r = 10 + rng() % 60;
            int c = 10 + rng() % 60;
            int size = 3 + rng() % 8;
            fill_rect(r, c, r+size, c+size, '.');
        }
    }
    
    // Mode 2: Stripes+Shift
    void mode_stripes_shift(const Params& p) {
        int stripe_width = 3 + rng() % 8;
        char c1 = '-', c2 = '+';
        
        // Vertical stripes
        for (int c = 0; c < N; c++) {
            char color = ((c / stripe_width) % 2 == 0) ? c1 : c2;
            for (int r = 0; r < N; r++) {
                board[r][c] = color;
            }
        }
        
        // Add horizontal shifts
        for (int r = 0; r < N; r += 10) {
            int shift = rng() % stripe_width;
            if (shift > 0) {
                for (int c = 0; c < N; c++) {
                    int src_c = (c + shift) % N;
                    board[r][c] = board[r][src_c];
                }
            }
        }
    }
    
    // Mode 3: ThueMorseLite
    void mode_thue_morse(const Params& p) {
        int scale = (p.high_freq_strength < 30) ? 4 : 
                    (p.high_freq_strength < 50) ? 2 : 1;
        
        auto thue_morse = [](int n) {
            return __builtin_popcount(n) % 2;
        };
        
        for (int r = 0; r < N; r++) {
            for (int c = 0; c < N; c++) {
                int val = thue_morse((r / scale) ^ (c / scale));
                board[r][c] = (val == 0) ? '-' : '+';
            }
        }
    }
    
    // Mode 4: QuadtreeMosaic
    void mode_quadtree(const Params& p) {
        function<void(int,int,int,int,int)> subdivide = 
            [&](int r1, int c1, int r2, int c2, int depth) {
            if (depth > 5 || r2 - r1 < 4 || c2 - c1 < 4) {
                fill_rect(r1, c1, r2, c2, random_nonzero_color());
                return;
            }
            if (rng() % 100 < 40) {
                fill_rect(r1, c1, r2, c2, random_nonzero_color());
                return;
            }
            int rmid = (r1 + r2) / 2;
            int cmid = (c1 + c2) / 2;
            subdivide(r1, c1, rmid, cmid, depth+1);
            subdivide(r1, cmid+1, rmid, c2, depth+1);
            subdivide(rmid+1, c1, r2, cmid, depth+1);
            subdivide(rmid+1, cmid+1, r2, c2, depth+1);
        };
        subdivide(0, 0, N-1, N-1, 0);
    }
    
    // Mode 5: Sprite (simple pattern)
    void mode_sprite(const Params& p) {
        // Fill with base
        fill_rect(0, 0, N-1, N-1, '-');
        
        // Add simple cross pattern
        for (int i = 20; i < 60; i++) {
            board[40][i] = '+';
            board[i][40] = '+';
        }
        
        // Add circles
        for (int dr = -10; dr <= 10; dr++) {
            for (int dc = -10; dc <= 10; dc++) {
                if (dr*dr + dc*dc <= 100) {
                    int r = 25 + dr;
                    int c = 25 + dc;
                    if (r >= 0 && r < N && c >= 0 && c < N) {
                        board[r][c] = '+';
                    }
                    r = 55 + dr;
                    c = 55 + dc;
                    if (r >= 0 && r < N && c >= 0 && c < N) {
                        board[r][c] = '+';
                    }
                }
            }
        }
    }
    
    void add_noise(const Params& p) {
        // Add random noise points
        for (int i = 0; i < p.noise_points; i++) {
            int r = rng() % N;
            int c = rng() % N;
            board[r][c] = random_color();
        }
        
        // Add small islands
        for (int i = 0; i < p.small_islands; i++) {
            int r = rng() % N;
            int c = rng() % N;
            int size = 1 + rng() % 3;
            fill_rect(r, c, min(N-1, r+size), min(N-1, c+size), random_nonzero_color());
        }
        
        // Add '.' density
        int dot_count = (N * N * p.dot_density) / 100;
        for (int i = 0; i < dot_count; i++) {
            int r = rng() % N;
            int c = rng() % N;
            if (rng() % 2 == 0) { // 50% chance to actually place
                board[r][c] = '.';
            }
        }
    }
    
    void add_overlay_layers(const Params& p) {
        // Add many small rectangles (2-10 size)
        for (int i = 0; i < p.small_rect_count; i++) {
            int r = rng() % N;
            int c = rng() % N;
            int h = 2 + rng() % 9;
            int w = 2 + rng() % 9;
            int r2 = min(N-1, r + h);
            int c2 = min(N-1, c + w);
            fill_rect(r, c, r2, c2, random_nonzero_color());
        }
        
        // Add medium rectangles (10-25 size)
        for (int i = 0; i < p.medium_rect_count; i++) {
            int r = rng() % N;
            int c = rng() % N;
            int h = 10 + rng() % 16;
            int w = 10 + rng() % 16;
            int r2 = min(N-1, r + h);
            int c2 = min(N-1, c + w);
            fill_rect(r, c, r2, c2, random_nonzero_color());
        }
        
        // Add overlay layers - smaller rectangles (3-12 size)
        for (int i = 0; i < p.overlay_layers; i++) {
            int r = rng() % N;
            int c = rng() % N;
            int h = 3 + rng() % 10;
            int w = 3 + rng() % 10;
            int r2 = min(N-1, r + h);
            int c2 = min(N-1, c + w);
            fill_rect(r, c, r2, c2, random_color());
        }
        
        // Add fragmentation dots for hard/insane tiers
        if (p.overlay_layers >= 60) {
            int dot_count = p.overlay_layers * 3;
            for (int i = 0; i < dot_count; i++) {
                int r = rng() % N;
                int c = rng() % N;
                int size = (rng() % 4 == 0) ? 2 : 1; // 75% 1x1, 25% 2x2
                int r2 = min(N-1, r + size - 1);
                int c2 = min(N-1, c + size - 1);
                fill_rect(r, c, r2, c2, random_nonzero_color());
            }
        }
    }
    
    void generate(int mode, const Params& p) {
        switch(mode) {
            case 0: mode_rect_overlay(p); break;
            case 1: mode_frames_holes(p); break;
            case 2: mode_stripes_shift(p); break;
            case 3: mode_thue_morse(p); break;
            case 4: mode_quadtree(p); break;
            case 5: mode_sprite(p); break;
            default: mode_rect_overlay(p);
        }
        add_overlay_layers(p);
        add_noise(p);
    }
    
    void print(long long seed) {
        cout << N << " " << seed << "\n";
        for (int i = 0; i < N; i++) {
            cout << board[i] << "\n";
        }
    }
};

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <seed> [tier=1] [mode=-1]\n";
        cerr << "  tier: 0=easy, 1=normal, 2=hard, 3=insane\n";
        cerr << "  mode: 0-5 or -1 for auto (seed % 6)\n";
        return 1;
    }
    
    long long seed = stoll(argv[1]);
    int tier = (argc >= 3) ? stoi(argv[2]) : 1;
    int mode = (argc >= 4) ? stoi(argv[3]) : -1;
    
    if (tier < 0 || tier > 3) tier = 1;
    if (mode == -1) mode = seed % NUM_MODES;
    if (mode < 0 || mode >= NUM_MODES) mode = 0;
    
    Params p = get_tier_params(tier);
    Generator gen(seed);
    gen.generate(mode, p);
    gen.print(seed);
    
    return 0;
}
