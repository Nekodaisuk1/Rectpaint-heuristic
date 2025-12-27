# サンプルケース

## sample_input.txt

Tier 1 (normal)、seed=100のテストケースです。

入力形式:
```
80 100
<80行の盤面データ>
```

## sample_output.txt

サンプルの解答例です（最適解ではありません）。

5手の操作で簡単なパターンを描いています:
1. 全体を `-` で塗る
2. 内側を `+` で塗る
3. 中央を `.` で塗る
4. さらに小さい矩形を `-` で塗る
5. 最小の矩形を `+` で塗る

## 試してみる

### 1. 可視化

`../tools/vis/viewer.html` をブラウザで開き、この2つのファイルを読み込んでください。

### 2. 採点

```bash
cd ..
./tools/judge/judge examples/sample_input.txt examples/sample_output.txt
```

出力例:
```
valid: 1
score: 633405
diff: 6334
moves: 5
```

このサンプルは最適解ではないため、Diffが大きくなっています。
より良い解を見つけてスコアを改善してみてください！

## 自分のソルバーで試す

```bash
# あなたのソルバーで解く
./your_solver < examples/sample_input.txt > my_output.txt

# 採点
./tools/judge/judge examples/sample_input.txt my_output.txt

# 可視化
# tools/vis/viewer.htmlで my_output.txt を読み込む
```
