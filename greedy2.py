import os
import sys

MyPC = os.path.basename(__file__) != "Main.py"
MyPC = False
if MyPC:
  from icecream import ic
  ic.disable()
else:
  def ic(*args):
    return None

ic.enable() if MyPC else None

# 貪欲法によるジョブスケジューリング問題の解法
def greedy_schedule(J: int, M: int, a_list: list[list[int]], p_list: list[list[int]], machine_available_time: list[int], job_available_time: list[int], job_queue: list[list[int]], next_operation_index: list[int]) -> int:
  """
  greedy_schedule の Docstring
  オペレーションの開始時刻が最も早くなるジョブを選択してスケジューリングする貪欲法の関数。
  同率の場合は、終了時刻が最も早いジョブを選択し、さらに同率の場合はジョブIDが最も小さいジョブを選択する。
  
  inputs:
    J: ジョブ数
    M: マシン数
    a_list: 各ジョブのマシン順序を格納したリスト
    p_list: 各ジョブの各マシンでの処理時間を格納したリスト
    machine_available_time: 各マシンが空くまでの時間を格納したリスト
    job_available_time: 各ジョブが次に開始可能な時間を格納したリスト
    job_queue: 現在スケジューリングされているジョブとその工程を格納したリスト
    next_operation_index: 各ジョブの次にスケジューリングすべき工程のインデックスを格納したリスト
  outputs:
    next_job_id: 次にスケジューリングすべきジョブのID
  """

  best_C_max = float('inf')  # 全工程の最大終了時刻を最小化するための変数
  next_job_id = None

  for job_id in range(J):
    if next_operation_index[job_id] >= M:
      continue  # すでに全ての工程がスケジューリングされているジョブはスキップ

    operation_index = next_operation_index[job_id]  # ジョブの次の工程のインデックス
    machine_id = a_list[job_id][operation_index]  # ジョブの次の工程で使用するマシンID
    processing_time = p_list[job_id][operation_index]  # ジョブの次の工程の処理時間

    # オペレーションの開始時刻と終了時刻を計算
    C_max = float('-inf')

    flag = False
    # 次の工程を仮にスケジューリングした場合の最大終了時刻を計算
    for j_id in range(J):
      if j_id == job_id:
        # 今回スケジューリングするジョブ
        start_time = max(machine_available_time[machine_id], job_available_time[job_id])
        end_time = start_time + processing_time
      else:
        # 他のジョブは現在の利用可能時間のまま
        end_time = job_available_time[j_id]
      if end_time > C_max:
        C_max = end_time
    if C_max < best_C_max:
      next_job_id = job_id
      flag = True

    if flag:
      best_C_max = C_max

  return next_job_id


def main():
  J, M = map(int, input().split())  # ジョブ数、マシン数
  a_list = [list(map(int, input().split())) for _ in range(J)]  # 各ジョブのマシン順序
  p_list = [list(map(int, input().split())) for _ in range(J)]  # 各ジョブの各マシンでの処理時間

  # マシンが空くまでの時間を管理するリスト
  machine_available_time = [0] * M

  # ジョブが次に開始可能な時間を管理するリスト
  job_available_time = [0] * J

  # 工程の順番を格納するリスト
  job_queue = []  # [[ジョブID, 工程の番号], ...]

  # ジョブごとに次の工程のインデックスを管理するリスト
  next_operation_index = [0] * J

  # 貪欲法で1つずつスケジューリングを行う
  while len(job_queue) < J * M:
    next_job_id = greedy_schedule(J, M, a_list, p_list, machine_available_time, job_available_time, job_queue, next_operation_index)
    if next_job_id is None:
      print("スケジューリングできるジョブが見つかりません。")
      sys.exit(1)
    # 次の工程をjob_queueに追加
    operation_index = next_operation_index[next_job_id]
    job_queue.append([next_job_id, operation_index])
    next_operation_index[next_job_id] += 1  # 進んだ工程のインデックスを更新
    # マシンとジョブの利用可能時間を更新
    machine_id = a_list[next_job_id][operation_index]  # 使用するマシンID
    processing_time = p_list[next_job_id][operation_index]  # 処理時間
    start_time = max(machine_available_time[machine_id], job_available_time[next_job_id])  # オペレーションの開始時刻
    end_time = start_time + processing_time  # オペレーションの終了時刻
    machine_available_time[machine_id] = end_time  # マシンの利用可能時間を更新
    job_available_time[next_job_id] = end_time  # ジョブの利用可能時間を更新

  # オペレーションをマシンごとに仕分ける
  machine_schedules = [[] for _ in range(M)]  # 各マシンのスケジュールを格納するリスト
  for job_id, operation_index in job_queue:
    machine_id = a_list[job_id][operation_index]
    machine_schedules[machine_id].append((job_id, operation_index))

  # スケジュールを出力
  for m_id in range(M):
    schedule = machine_schedules[m_id]
    for job_id, operation_index in schedule:
      print(f"{job_id} {operation_index}", end=' ')
    print()

  # スケジュールの最大終了時刻を計算
  C_max = max(job_available_time)
  print(C_max, file=sys.stderr)

if __name__ == "__main__":
  main()