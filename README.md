# RectPaint Heuristic Contest

これは80×80のグリッドを3色('.', '-', '+')で塗り分けるヒューリスティック最適化問題です。

## 📋 問題概要

- 初期盤面: 80×80グリッドで構成される。初期状態では全グリッドは '.'で塗りつぶされている。
- 目標: 与えられた盤面を一致する盤面を作ること
- 操作: 長方形領域((x1,y1) (x2,y2))を指定して任意の一色('.', '-', '+'のいずれか)で塗りつぶす
- 制限: 最大800手
- スコア: `10000 × (不一致セル数) + 手数`（小さいほど良い）

詳細は [problem.md](problem.md) を参照してください。

## 🚀 クイックスタート

### 1. ツールのビルド

```bash
# Generator
cd tools/gen
g++ -std=c++17 -O2 gen.cpp -o gen

# Judge
cd ../judge
g++ -std=c++17 -O2 judge.cpp -o judge
```

### 2. 入力生成

```bash
# Tier 1 (normal), seed=100でテストケース生成
./tools/gen/gen 100 1 > input.txt
```

Tier:
- 0: easy（初心者向け）
- 1: normal（標準）
- 2: hard（難）
- 3: insane（超難）

### 3. ソルバー実行

自分のソルバー `./solver` で解く:

```bash
./solver < input.txt > output.txt
```

### 4. 採点

```bash
./tools/judge/judge input.txt output.txt
```

出力例:
```
valid: 1
score: 1234
diff: 0
moves: 234
```

### 5. ビジュアライザ

`tools/vis/viewer.html` をブラウザで開き、`input.txt` と `output.txt` を読み込んでください。

## 📁 ファイル構成

```
rectpaint-heuristic/
├─ README.md           # このファイル
├─ problem.md          # 問題詳細仕様
├─ seeds/              # 公式シードセット
│  ├─ tier0_easy.txt
│  ├─ tier1_normal.txt
│  ├─ tier2_hard.txt
│  └─ tier3_insane.txt
├─ tools/
│  ├─ gen/gen.cpp      # テストケース生成器
│  ├─ judge/judge.cpp  # 公式ジャッジプログラム
│  └─ vis/viewer.html  # 可視化ツール
├─ examples/           # サンプル
└─ dev/                # 開発用ツール（オプション）
```

## 📊 公式シードセット

各難易度でのスコアは `seeds/tier1_normal.txt` の100シードで集計します。

- `tier0_easy.txt`: seeds 0-99（練習用）
- `tier1_normal.txt`: seeds 100-199（メインランキング）
- `tier2_hard.txt`: seeds 200-299（チャレンジ）
- `tier3_insane.txt`: seeds 300-399（エクストリーム）

## 🎯 入力形式

入力は
```
80 <seed>
<80行の盤面データ>
```
の合計81行で構成されます。
2行目以降81行目までは80文字で、使用できる文字は `.`, `-`, `+` のみ。

## 📤 出力形式

```
M
r1 c1 r2 c2 x
r1 c1 r2 c2 x
...
```

- `M`: 手数（0 ≤ M ≤ 800）
- 各行: `(r1,c1)` から `(r2,c2)` の長方形を色 `x` で塗る（0-indexed）

## ⚖️ 採点方法

```
score = 10000 × diff + moves
```

- `diff`: 目標盤面との不一致セル数
- `moves`: 使用手数

## 🛠️ 開発用ツール

`dev/` フォルダには以下が含まれています：

- `test_server.py`: バッチテスト用HTTPサーバー
- `tester.html`: 複数シードを自動テストするGUI
- `viewer.html`: より高機能なビューア

使用方法は `dev/README.md` を参照してください。

## 📝 ライセンス

なし

## 🤝 コントリビューション

Issue、Pull Requestを歓迎します！
# Rectpaint-heuristic
