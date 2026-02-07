# メモ

## 何をするか
- chokudaiサーチのメモリ効率化について
- 取り出す回数の上限を $K$ とする
- 常に上位 $K$ 個のノードを保つ必要がある
- それ以外は捨ててもいい

## 計画01
- データベース工学のB-treeをみて思いついた
- 並行二分探索木で管理
- 並行二分探索木の要素数が $2K$ を超えた場合
  - 斬首
  - 根の子のうち良い方を新たな根とする
- これで要素数は最大でも $2K + 1$ となるはず
- 根が変わる関係上、配列ではなくポインタをつなげて実装する必要がありそう
- ならB木のほうがいいのかな
- 偏りが生じるから要素数を把握している必要がありそう
- まずはB木について知る必要がある
- 難しそう
- 1つ入れるごとに1つ押し出せばいいのでは
  - 定数倍だが負担は大きいだろう
- でもAVL木で実装できると思う
  - 多分

## C++への移植
- AHC059のa05.pyを移植
- https://chatgpt.com/share/6986881c-c048-8004-a43a-c01e2e56cff1
  - これを参考にする
- 幅制限で落とす項目を除外
- あまり元コードを覚えていない
- ノード木を追加
- (Illegal action: Take from empty cell)
- できた
- やはりメモリ不足でAtCoderでは実行できない
- inの0000.txt実行
- `g++ AHC059-a05.cpp -O2 -std=c++23 -o AHC059-a05.out`
```
        Command being timed: "./AHC059-a05.out"
        User time (seconds): 1.02
        System time (seconds): 1.04
        Percent of CPU this job got: 99%
        Elapsed (wall clock) time (h:mm:ss or m:ss): 0:02.07
        Average shared text size (kbytes): 0
        Average unshared data size (kbytes): 0
        Average stack size (kbytes): 0
        Average total size (kbytes): 0
        Maximum resident set size (kbytes): 2284332
        Average resident set size (kbytes): 0
        Major (requiring I/O) page faults: 0
        Minor (reclaiming a frame) page faults: 712125
        Voluntary context switches: 1
        Involuntary context switches: 97
        Swaps: 0
        File system inputs: 0
        File system outputs: 16
        Socket messages sent: 0
        Socket messages received: 0
        Signals delivered: 0
        Page size (bytes): 4096
        Exit status: 0
```
- AVL木の実装はCopilotに任せてみる
- `g++ a05-opt.cpp -O2 -std=c++23 -o a05-opt.out`
```
/usr/bin/time -v  ./a05-opt.out < AHC059-in/0000.txt > result.txt
Total path length: 1346
        Command being timed: "./a05-opt.out"
        User time (seconds): 1.52
        System time (seconds): 0.46
        Percent of CPU this job got: 99%
        Elapsed (wall clock) time (h:mm:ss or m:ss): 0:01.99
        Average shared text size (kbytes): 0
        Average unshared data size (kbytes): 0
        Average stack size (kbytes): 0
        Average total size (kbytes): 0
        Maximum resident set size (kbytes): 1087336
        Average resident set size (kbytes): 0
        Major (requiring I/O) page faults: 0
        Minor (reclaiming a frame) page faults: 330717
        Voluntary context switches: 1
        Involuntary context switches: 7
        Swaps: 0
        File system inputs: 0
        File system outputs: 8
        Socket messages sent: 0
        Socket messages received: 0
        Signals delivered: 0
        Page size (bytes): 4096
        Exit status: 0
```
- 1.85秒に変更
```
Total path length: 1346
        Command being timed: "./a05-opt.out"
        User time (seconds): 1.44
        System time (seconds): 0.49
        Percent of CPU this job got: 100%
        Elapsed (wall clock) time (h:mm:ss or m:ss): 0:01.94
        Average shared text size (kbytes): 0
        Average unshared data size (kbytes): 0
        Average stack size (kbytes): 0
        Average total size (kbytes): 0
        Maximum resident set size (kbytes): 1087476
        Average resident set size (kbytes): 0
        Major (requiring I/O) page faults: 0
        Minor (reclaiming a frame) page faults: 340452
        Voluntary context switches: 1
        Involuntary context switches: 5
        Swaps: 0
        File system inputs: 0
        File system outputs: 8
        Socket messages sent: 0
        Socket messages received: 0
        Signals delivered: 0
        Page size (bytes): 4096
        Exit status: 0
```
## 提出
- 2026-02-07 11:26:27
- https://atcoder.jp/contests/ahc059/submissions/73034545
- 最大メモリ886716 KiB