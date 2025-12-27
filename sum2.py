N = 100

ans = 0

for _ in range(N):
  score = int(input())
  _ = input()  # 読み飛ばし
  ans += score

print(f"sum_score: {ans}")