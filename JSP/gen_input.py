import argparse
import os
import random
from typing import List, Tuple


def generate_case(rng: random.Random, j: int = 50, m: int = 20) -> Tuple[List[List[int]], List[List[int]]]:
  a: List[List[int]] = []
  p: List[List[int]] = []

  for _job in range(j):
    machines = list(range(m))
    rng.shuffle(machines)
    a.append(machines)
    p.append([rng.randint(1, 100) for _ in range(m)])

  return a, p


def write_case(file_path: str, a: List[List[int]], p: List[List[int]]) -> None:
  j = len(a)
  m = len(a[0]) if j > 0 else 0

  with open(file_path, "w", encoding="utf-8") as f:
    f.write(f"{j} {m}\n")
    for row in a:
      f.write(" ".join(map(str, row)) + "\n")
    for row in p:
      f.write(" ".join(map(str, row)) + "\n")


def main() -> None:
  parser = argparse.ArgumentParser(description="ジョブスケジューリング問題のテストケースを生成します。")
  parser.add_argument("-n", "--num", type=int, default=100, help="生成するテストケースの数")
  parser.add_argument("-o", "--out", default="in", help="出力ディレクトリ")
  parser.add_argument("--seed", type=int, default=0, help="基準となるシード値（ケースiはseed + iを使用）")
  parser.add_argument("--jobs", type=int, default=50, help="ジョブの数 (J)")
  parser.add_argument("--machines", type=int, default=20, help="マシンの数 (M)")
  args = parser.parse_args()

  if args.num <= 0:
    raise SystemExit("--num must be positive")
  if args.jobs <= 0 or args.machines <= 0:
    raise SystemExit("--jobs and --machines must be positive")

  os.makedirs(args.out, exist_ok=True)

  for i in range(args.num):
    rng = random.Random(args.seed + i)
    a, p = generate_case(rng, j=args.jobs, m=args.machines)
    file_path = os.path.join(args.out, f"{i:04}.txt")
    write_case(file_path, a, p)


if __name__ == "__main__":
  main()