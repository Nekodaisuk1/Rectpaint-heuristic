# 開発用ツール

このフォルダには、開発・デバッグ用の補助ツールが含まれています。

## 含まれるツール

### test_server.py

複数のテストケースを自動実行するためのHTTPサーバー。

**起動方法:**
```bash
python3 test_server.py
```

サーバーが `http://localhost:8765` で起動します。

### tester.html

バッチテスト用のWebインターフェース。

**使い方:**
1. `test_server.py` を起動
2. ブラウザで `http://localhost:8765/tester.html` を開く
3. テスト回数、シード、generator、solverを設定
4. 「実行」ボタンでバッチテスト開始

**機能:**
- 複数シードの自動テスト
- リアルタイム進捗表示
- 統計情報（平均/最小/最大手数、Diff）
- 結果のクリックでViewer表示
- input/outputのダウンロード

## 使用上の注意

これらのツールは**開発用**です。公式コンテストでは以下のみ使用してください：

- `tools/gen/gen` - テストケース生成
- `tools/judge/judge` - 公式採点
- `tools/vis/viewer.html` - 可視化

## 設定

test_server.pyはgen/judgeの実行パスを自動検出しますが、環境によっては調整が必要な場合があります。

## トラブルシューティング

**Q: サーバーが起動しない**

A: ポート8765が使用中の可能性があります。以下で確認:
```bash
lsof -i :8765
```

**Q: generatorやsolverが見つからない**

A: test_server.pyと同じディレクトリに実行ファイルがあるか、パスを正しく指定してください。

**Q: CORSエラーが出る**

A: ブラウザで直接HTMLファイルを開かず、必ず `http://localhost:8765/` 経由でアクセスしてください。
