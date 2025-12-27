#!/usr/bin/env python3
"""
Paint Problem Test Server
ランダムシードでテストを実行し、結果をJSON形式で返すHTTPサーバー
"""

import os
import sys
import subprocess
import tempfile
import time
import json
from http.server import HTTPServer, BaseHTTPRequestHandler
from urllib.parse import parse_qs

class TestHandler(BaseHTTPRequestHandler):
    def do_OPTIONS(self):
        """CORS preflight request"""
        self.send_response(200)
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'POST, GET, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type')
        self.end_headers()
    
    def do_GET(self):
        """静的ファイル配信"""
        if self.path == '/' or self.path == '/tester.html':
            file_path = 'tester.html'
        elif self.path == '/viewer.html' or self.path.startswith('/viewer.html?'):
            file_path = 'viewer.html'
        else:
            self.send_error(404)
            return
        
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            self.send_response(200)
            self.send_header('Content-Type', 'text/html; charset=utf-8')
            self.send_header('Access-Control-Allow-Origin', '*')
            self.end_headers()
            self.wfile.write(content.encode('utf-8'))
        except FileNotFoundError:
            self.send_error(404)

    def do_POST(self):
        """テスト実行リクエストを処理"""
        if self.path == '/run':
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            data = json.loads(post_data.decode('utf-8'))
            
            seed = data.get('seed', 12345)
            gen_type = data.get('genType', 'gen_hard80')
            solver_path = data.get('solverPath', './greedy_solver')
            
            # genTypeからgenPathを決定
            gen_path = f'./{gen_type}'
            
            result = self.run_test(seed, gen_path, solver_path)
            
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.send_header('Access-Control-Allow-Origin', '*')
            self.end_headers()
            self.wfile.write(json.dumps(result).encode('utf-8'))
        elif self.path == '/get_data':
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            data = json.loads(post_data.decode('utf-8'))
            
            seed = data.get('seed', 12345)
            gen_type = data.get('genType', 'gen_hard80')
            solver_path = data.get('solverPath', './greedy_solver')
            
            gen_path = f'./{gen_type}'
            
            result = self.get_data(seed, gen_path, solver_path)
            
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.send_header('Access-Control-Allow-Origin', '*')
            self.end_headers()
            self.wfile.write(json.dumps(result).encode('utf-8'))
        else:
            self.send_error(404)

    def run_test(self, seed, gen_path, solver_path):
        """単一のテストを実行"""
        start_time = time.time()
        
        try:
            # 一時ファイルを作成
            with tempfile.NamedTemporaryFile(mode='w+', suffix='.txt', delete=False) as input_file:
                input_path = input_file.name
                
                # ジェネレーターを実行
                gen_cmd = [gen_path, str(seed)]
                gen_result = subprocess.run(
                    gen_cmd,
                    capture_output=True,
                    text=True,
                    encoding='utf-8',
                    errors='replace',
                    timeout=10
                )
                
                if gen_result.returncode != 0:
                    return {
                        'status': 'error',
                        'error': f'Generator failed: {gen_result.stderr}',
                        'moves': None,
                        'diff': None,
                        'time': (time.time() - start_time) * 1000
                    }
                
                input_file.write(gen_result.stdout)
                input_file.flush()
            
            # ソルバーを実行
            solver_cmd = [solver_path, input_path]
            solver_result = subprocess.run(
                solver_cmd,
                capture_output=True,
                text=True,
                encoding='utf-8',
                errors='replace',
                timeout=30
            )
            
            if solver_result.returncode != 0:
                os.unlink(input_path)
                return {
                    'status': 'error',
                    'error': f'Solver failed: {solver_result.stderr}',
                    'moves': None,
                    'diff': None,
                    'time': (time.time() - start_time) * 1000
                }
            
            # 手数を抽出（最初の行）
            lines = solver_result.stdout.strip().split('\n')
            if len(lines) == 0:
                os.unlink(input_path)
                return {
                    'status': 'error',
                    'error': 'No output from solver',
                    'moves': None,
                    'diff': None,
                    'time': (time.time() - start_time) * 1000
                }
            
            try:
                moves = int(lines[0])
            except ValueError:
                os.unlink(input_path)
                return {
                    'status': 'error',
                    'error': f'Invalid move count: {lines[0]}',
                    'moves': None,
                    'diff': None,
                    'time': (time.time() - start_time) * 1000
                }
            
            # Calculate diff by replaying the solution (before deleting the file)
            diff = self.calculate_diff(input_path, lines)
            
            # 一時ファイルを削除
            os.unlink(input_path)
            
            elapsed_time = (time.time() - start_time) * 1000
            
            return {
                'status': 'success',
                'moves': moves,
                'diff': diff,
                'error': None,
                'time': elapsed_time
            }
            
        except subprocess.TimeoutExpired:
            return {
                'status': 'error',
                'error': 'Timeout',
                'moves': None,
                'diff': None,
                'time': (time.time() - start_time) * 1000
            }
        except Exception as e:
            return {
                'status': 'error',
                'error': str(e),
                'moves': None,
                'diff': None,
                'time': (time.time() - start_time) * 1000
            }
    
    def calculate_diff(self, input_path, solution_lines):
        """Calculate the difference between target and result"""
        try:
            # Read input file
            with open(input_path, 'r') as f:
                lines = f.readlines()
            
            first_line = lines[0].strip().split()
            
            # Check format: gen_multi80 uses "N seed", gen_hard80 uses just "seed"
            if len(first_line) == 2:
                # gen_multi80 format: "N seed"
                H = int(first_line[0])
                W = H  # Square grid
                seed = int(first_line[1])
            else:
                # gen_hard80 format: just "seed"
                seed = int(first_line[0])
                H = 80
                W = 80
            
            T = [lines[i+1].strip() for i in range(H)]
            
            # Initialize board with '.'
            A = [['.' for _ in range(W)] for _ in range(H)]
            
            # Apply moves
            num_moves = int(solution_lines[0])
            for i in range(1, min(len(solution_lines), num_moves + 1)):
                parts = solution_lines[i].strip().split()
                if len(parts) < 5:
                    continue
                r1, c1, r2, c2, x = int(parts[0]), int(parts[1]), int(parts[2]), int(parts[3]), parts[4]
                
                # Normalize rectangle
                if r1 > r2:
                    r1, r2 = r2, r1
                if c1 > c2:
                    c1, c2 = c2, c1
                
                # Apply paint
                for r in range(r1, r2 + 1):
                    for c in range(c1, c2 + 1):
                        if 0 <= r < H and 0 <= c < W:
                            A[r][c] = x
            
            # Count differences
            diff = 0
            for r in range(H):
                for c in range(W):
                    if A[r][c] != T[r][c]:
                        diff += 1
            
            return diff
            
        except Exception as e:
            return None
    
    def get_data(self, seed, gen_path, solver_path):
        """指定されたseed、generator、solverでinputとoutputのテキストを返す"""
        try:
            # ジェネレーターを実行
            gen_cmd = [gen_path, str(seed)]
            gen_result = subprocess.run(
                gen_cmd,
                capture_output=True,
                text=True,
                encoding='utf-8',
                errors='replace',
                timeout=10
            )
            
            if gen_result.returncode != 0:
                return {
                    'status': 'error',
                    'error': f'Generator failed: {gen_result.stderr}',
                    'input': None,
                    'output': None
                }
            
            input_text = gen_result.stdout
            
            # 一時ファイルを作成してソルバーを実行
            with tempfile.NamedTemporaryFile(mode='w+', suffix='.txt', delete=False) as input_file:
                input_path = input_file.name
                input_file.write(input_text)
                input_file.flush()
            
            # ソルバーを実行
            solver_cmd = [solver_path, input_path]
            solver_result = subprocess.run(
                solver_cmd,
                capture_output=True,
                text=True,
                encoding='utf-8',
                errors='replace',
                timeout=30
            )
            
            os.unlink(input_path)
            
            if solver_result.returncode != 0:
                return {
                    'status': 'error',
                    'error': f'Solver failed: {solver_result.stderr}',
                    'input': None,
                    'output': None
                }
            
            output_text = solver_result.stdout
            
            return {
                'status': 'success',
                'input': input_text,
                'output': output_text,
                'error': None
            }
            
        except subprocess.TimeoutExpired:
            return {
                'status': 'error',
                'error': 'Timeout',
                'input': None,
                'output': None
            }
        except Exception as e:
            return {
                'status': 'error',
                'error': str(e),
                'input': None,
                'output': None
            }

    def log_message(self, format, *args):
        """ログメッセージをカスタマイズ"""
        sys.stderr.write("[%s] %s\n" % (self.log_date_time_string(), format % args))


def main():
    port = 8765
    server_address = ('', port)
    httpd = HTTPServer(server_address, TestHandler)
    
    print(f"🚀 Test Server starting on http://localhost:{port}")
    print(f"📁 Working directory: {os.getcwd()}")
    print("Press Ctrl+C to stop")
    
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\n👋 Server stopped")
        httpd.shutdown()


if __name__ == '__main__':
    main()
