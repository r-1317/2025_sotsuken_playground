import os
import sys
from typing import List, Tuple
import time

MyPC = os.path.basename(__file__) != "Main.py"
MyPC = False
if MyPC:
  from icecream import ic
  ic.disable()
else:
  def ic(*args):
    return None

ic.enable() if MyPC else None

K = 300  # ビーム幅

sys.setrecursionlimit(2000)  # 再帰上限を引き上げ

start_time = time.time()

# ビームサーチのノードを表すクラス
class Node:
  def __init__(self, machine_available_time: list[int], job_available_time: list[int], next_operation_index: list[int], prev_node: 'Node' = None) -> None:
    self.machine_available_time = machine_available_time[:]
    self.job_available_time = job_available_time[:]
    self.operation = []  # 今回のノードで追加されるオペレーション [job_id, operation_index]
    self.next_operation_index = next_operation_index[:]
    self.prev_node = prev_node
  
  def copy(self) -> 'Node':
    return Node(self.machine_available_time, self.job_available_time, self.next_operation_index, self)
  
  def evaluate(self, a_list: List[List[int]], p_list: List[List[int]]) -> int:  # スケジュールされた全工程の最大終了時刻を返す
    if not self.operation:
      return float('inf')  # 初期ノードの場合は無限大を返す

    C_max =  max(self.job_available_time)
    return C_max
  
  def next_jobs(self, J: int, M: int) -> List[int]:
    candidates = []
    for job_id in range(J):
      if self.next_operation_index[job_id] < M:
        candidates.append(job_id)
    return candidates
  
  def schedule_job(self, job_id: int, a_list: List[List[int]], p_list: List[List[int]]) -> None:
    operation_index = self.next_operation_index[job_id]  # ジョブの次の工程のインデックス
    self.operation = [job_id, operation_index]  # 今回のノードで追加されるオペレーション
    self.next_operation_index[job_id] += 1  # 進んだ工程のインデックスを更新
    # マシンとジョブの利用可能時間を更新
    machine_id = a_list[job_id][operation_index]  # 使用するマシンID
    processing_time = p_list[job_id][operation_index]  # 処理時間
    start_time = max(self.machine_available_time[machine_id], self.job_available_time[job_id])  # オペレーションの開始時刻
    end_time = start_time + processing_time  # オペレーションの終了時刻
    self.machine_available_time[machine_id] = end_time  # マシンの利用可能時間を更新
    self.job_available_time[job_id] = end_time  # ジョブの利用可能時間を更新

  def get_job_queue(self) -> List[Tuple[int, int]]:
    if self.prev_node is None:
      return self.operation if self.operation else []

    prev_queue = self.prev_node.get_job_queue()
    if self.operation:
      prev_queue.append(self.operation)
    return prev_queue
  
  def get_self(self) -> 'Node':
    return self

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

  # ノードの初期化
  initial_node = Node(machine_available_time, job_available_time, next_operation_index, None)
  beam = [initial_node]

  # ビームサーチでスケジューリングを行う
  for _ in range(J * M):
    next_beam = []
    for node in beam:
      candidates = node.next_jobs(J, M)
      for job_id in candidates:
        new_node = node.copy()
        new_node.schedule_job(job_id, a_list, p_list)
        next_beam.append(new_node)
    
    # ノードを評価してソート
    next_beam.sort(key=lambda n: n.evaluate(a_list, p_list))
    # ビーム幅Kで絞り込み
    beam = next_beam[:K]

  # 最良ノードを選択
  best_node = beam[0]
  job_available_time = best_node.job_available_time
  job_queue = best_node.get_job_queue()

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
  print(f"実行時間: {(time.time() - start_time):.1f}秒", file=sys.stderr)