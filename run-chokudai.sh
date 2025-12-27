#!/bin/bash
NAME="chokudai"
# 並列実行する最大数
MAX_JOBS=1  # メモリが厳しい場合は1に設定

for i in $(seq -w 0000 0099); do
  pypy3 ${NAME}.py < in/${i}.txt > out-${NAME}/out-${NAME}_${i}.txt &
  # バックグラウンドジョブがMAX_JOBSに達したら待機
  if (( $(jobs -r | wc -l) >= MAX_JOBS )); then
    wait -n
  fi
done

# 残りのジョブが終わるまで待機
wait