#include <bits/stdc++.h>
using namespace std;

static constexpr int N = 20;               // 固定
static constexpr double TIME_LIMIT = 1.85;  // 秒
static constexpr int MAX_WIDTH = 1000;     // chokudai_level の最大幅

struct Pos {
  int x, y;
};

// マンハッタン距離計算
static inline int manhattan(const Pos& a, const Pos& b) {
  return abs(a.x - b.x) + abs(a.y - b.y);
}

// 20x20=400 マスの使用済み管理
struct IsUsedBB {
  std::bitset<N * N> board;

  inline bool is_used(int x, int y) const {
    return board.test(x * N + y);
  }
  inline void set_used(int x, int y) {
    board.set(x * N + y);
  }
};

// num のもう片方の座標を返す
static inline Pos get_pair_pos(int num, const vector<array<Pos, 2>>& nums_idx, const Pos& other) {
  const Pos& p1 = nums_idx[num][0];
  const Pos& p2 = nums_idx[num][1];
  if (p1.x == other.x && p1.y == other.y) return p2;
  return p1;
}

struct Node {
  IsUsedBB used;
  Pos current_pos{-1, -1};

  int uid = -1;  // 重複順序を安定させるための一意ID

  bool has_stack_top = false;
  Pos stack_top{-1, -1};

  int prev_path_length = 0;
  // const Node* prev_node = nullptr;  // 元コードとは異なり、ノード間のつながりは別の木で管理
  int tree_index = -1; // ノード木でのインデックス

  // ノードの総コスト計算 Python版ではもう少し下にあったやつ
  inline int total_cost() const {
    // Python: prev_path_length + dist(current_pos, stack_top) if stack_top else prev_path_length
    return has_stack_top ? (prev_path_length + manhattan(current_pos, stack_top)) : prev_path_length;
  }

  // 次ノード生成（Python の next_nodes 相当）
  vector<Node*> next_nodes(const vector<vector<int>>& grid, const vector<array<Pos, 2>>& nums_idx, deque<Node>& pool) const {
    vector<Node*> res;
    res.reserve(N * N);

    for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j++) {
        if (used.is_used(i, j)) continue;

        Pos candidate{i, j};
        int num = grid[i][j];
        Pos pair_pos = get_pair_pos(num, nums_idx, candidate);

        pool.emplace_back();
        Node* nn = &pool.back();

        nn->used = used;  // copy
        nn->used.set_used(i, j);
        nn->used.set_used(pair_pos.x, pair_pos.y);

        nn->current_pos = candidate;
        nn->has_stack_top = true;
        nn->stack_top = pair_pos;

        int dist_1 = manhattan(current_pos, candidate);
        int dist_2 = has_stack_top ? manhattan(stack_top, pair_pos) : 0;

        nn->prev_path_length = prev_path_length + dist_1 + dist_2;
        // nn->prev_node = this;  // 元コードとは異なり、ノード間のつながりは別の木で管理
        nn->tree_index = -1; // ノード木でのインデックスは後で設定

        res.push_back(nn);
      }
    }
    return res;
  }
};

static int GLOBAL_NODE_UID = 0;

// chokudai_level 用の順序（Python版の heap の key 相当）
// prev_path_length が小さいものが「最小」
struct NodePtrCmp {
  bool operator()(const Node* a, const Node* b) const {
    if (a->prev_path_length != b->prev_path_length) return a->prev_path_length < b->prev_path_length;
    return a->uid < b->uid;
  }
};

// 最大/最小の両端を O(log n) で扱えるAVL木（重複OK）
template <class T, class Cmp>
class AvlMultiset {
  struct AvlNode {
    T val;
    AvlNode* l = nullptr;
    AvlNode* r = nullptr;
    int h = 1;
    explicit AvlNode(const T& v) : val(v) {}
  };

  AvlNode* root_ = nullptr;
  size_t size_ = 0;
  Cmp cmp_{};

  static int height(AvlNode* n) { return n ? n->h : 0; }
  static void update(AvlNode* n) {
    n->h = 1 + max(height(n->l), height(n->r));
  }
  static int balance_factor(AvlNode* n) { return height(n->l) - height(n->r); }

  static AvlNode* rotate_right(AvlNode* y) {
    AvlNode* x = y->l;
    AvlNode* t2 = x->r;
    x->r = y;
    y->l = t2;
    update(y);
    update(x);
    return x;
  }

  static AvlNode* rotate_left(AvlNode* x) {
    AvlNode* y = x->r;
    AvlNode* t2 = y->l;
    y->l = x;
    x->r = t2;
    update(x);
    update(y);
    return y;
  }

  AvlNode* rebalance(AvlNode* n) {
    update(n);
    int bf = balance_factor(n);
    if (bf > 1) {
      if (balance_factor(n->l) < 0) n->l = rotate_left(n->l);
      return rotate_right(n);
    }
    if (bf < -1) {
      if (balance_factor(n->r) > 0) n->r = rotate_right(n->r);
      return rotate_left(n);
    }
    return n;
  }

  AvlNode* insert_rec(AvlNode* n, const T& v) {
    if (!n) return new AvlNode(v);
    if (cmp_(v, n->val)) n->l = insert_rec(n->l, v);
    else n->r = insert_rec(n->r, v);
    return rebalance(n);
  }

  static AvlNode* min_node(AvlNode* n) {
    while (n && n->l) n = n->l;
    return n;
  }

  static AvlNode* max_node(AvlNode* n) {
    while (n && n->r) n = n->r;
    return n;
  }

  AvlNode* erase_min_rec(AvlNode* n) {
    if (!n) return nullptr;
    if (!n->l) {
      AvlNode* r = n->r;
      delete n;
      return r;
    }
    n->l = erase_min_rec(n->l);
    return rebalance(n);
  }

  AvlNode* erase_max_rec(AvlNode* n) {
    if (!n) return nullptr;
    if (!n->r) {
      AvlNode* l = n->l;
      delete n;
      return l;
    }
    n->r = erase_max_rec(n->r);
    return rebalance(n);
  }

  static void inorder_collect(AvlNode* n, vector<T>& out) {
    if (!n) return;
    inorder_collect(n->l, out);
    out.push_back(n->val);
    inorder_collect(n->r, out);
  }

  static void clear_rec(AvlNode* n) {
    if (!n) return;
    clear_rec(n->l);
    clear_rec(n->r);
    delete n;
  }

public:
  AvlMultiset() = default;
  AvlMultiset(const AvlMultiset&) = delete;
  AvlMultiset& operator=(const AvlMultiset&) = delete;

  AvlMultiset(AvlMultiset&& other) noexcept {
    root_ = other.root_;
    size_ = other.size_;
    other.root_ = nullptr;
    other.size_ = 0;
  }
  AvlMultiset& operator=(AvlMultiset&& other) noexcept {
    if (this == &other) return *this;
    clear_rec(root_);
    root_ = other.root_;
    size_ = other.size_;
    other.root_ = nullptr;
    other.size_ = 0;
    return *this;
  }

  ~AvlMultiset() { clear_rec(root_); }

  bool empty() const { return size_ == 0; }
  size_t size() const { return size_; }

  void insert(const T& v) {
    root_ = insert_rec(root_, v);
    ++size_;
  }

  T pop_min() {
    AvlNode* m = min_node(root_);
    T v = m->val;
    root_ = erase_min_rec(root_);
    --size_;
    return v;
  }

  T pop_max() {
    AvlNode* m = max_node(root_);
    T v = m->val;
    root_ = erase_max_rec(root_);
    --size_;
    return v;
  }

  vector<T> to_vector_inorder() const {
    vector<T> out;
    out.reserve(size_);
    inorder_collect(root_, out);
    return out;
  }
};

// コマンド列生成
static vector<char> make_commands(const vector<Pos>& collect_order, Pos current_pos) {
  vector<char> commands;
  int x = current_pos.x, y = current_pos.y;

  for (const auto& target : collect_order) {
    int tx = target.x, ty = target.y;

    while (x < tx) { commands.push_back('D'); x++; }
    while (x > tx) { commands.push_back('U'); x--; }
    while (y < ty) { commands.push_back('R'); y++; }
    while (y > ty) { commands.push_back('L'); y--; }

    commands.push_back('Z'); // 収集
  }
  return commands;
}

// パス長計算
static int get_path_length(const vector<Pos>& path) {
  int length = 0;
  for (size_t i = 1; i < path.size(); i++) {
    length += manhattan(path[i - 1], path[i]);
  }
  return length;
}

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  int Nin;
  cin >> Nin; // N=20 固定のため無視（Python と同じ）

  vector<vector<int>> grid(N, vector<int>(N));
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) cin >> grid[i][j];
  }

  // Python: nums_idx_list = [[] for _ in range(N**2)]
  // ここも 400 確保（実際に使う num は 0..199）
  vector<array<Pos, 2>> nums_idx_list(N * N);
  vector<int> cnt(N * N, 0);

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      int num = grid[i][j];
      int k = cnt[num]++;
      if (k < 2) nums_idx_list[num][k] = Pos{i, j};
      // 3つ目以降は来ない前提
    }
  }

  // ノード実体を保持（prev_node ポインタを安定させるため deque）
  deque<Node> pool;

  pool.emplace_back();
  Node* root = &pool.back();
  root->used = IsUsedBB{};
  root->current_pos = Pos{0, 0};
  root->has_stack_top = false;
  root->prev_path_length = 0;
  root->uid = GLOBAL_NODE_UID++;
  // root->prev_node = nullptr;  // 元コードとは異なり、ノード間のつながりは別の木で管理

  const int LEVELS = (N * N) / 2 + 1; // 201
  vector<AvlMultiset<Node*, NodePtrCmp>> chokudai_levels(LEVELS);
  chokudai_levels[0].insert(root);

  // ノードをつなぐ木を構築するための可変長配列
  // 各要素は{親index, current_pos, stack_top}のタプル
  vector<tuple<int, Pos, Pos>> node_tree;
  node_tree.emplace_back(-1, root->current_pos, Pos{0, 0}); // rootノード
  root->tree_index = 0;  // rootノードのインデックスを設定

  auto start = chrono::steady_clock::now();
  bool flag = true;

  while (flag) {
    for (int i = 0; i < (N * N) / 2; i++) {
      auto& cur_level = chokudai_levels[i];
      auto& next_level = chokudai_levels[i + 1];

      if (cur_level.empty()) continue;

      // Python: node = heapq.heappop(chokudai_level)
      Node* node = cur_level.pop_min();

      // Python: next_nodes = node.next_nodes(...)
      auto next_nodes = node->next_nodes(grid, nums_idx_list, pool);

      for (Node* nn : next_nodes) {
        nn->uid = GLOBAL_NODE_UID++;
      }

      // Python: for next_node in next_nodes: heapq.heappush(next_level, next_node)
      for (Node* nn : next_nodes) {
        next_level.insert(nn);
      }

      // ノード木に新ノードを追加し、インデックスを設定
      for (Node* nn : next_nodes) {
        int parent_index = node->tree_index;
        node_tree.emplace_back(parent_index, nn->current_pos, nn->stack_top);
        nn->tree_index = static_cast<int>(node_tree.size()) - 1;
      }

      
      /* ここに最大要素の削除を追加したい */
      while (next_level.size() > static_cast<size_t>(MAX_WIDTH)) {
        next_level.pop_max();
      }

      // 時間制限チェック
      double elapsed = chrono::duration<double>(chrono::steady_clock::now() - start).count();
      if (elapsed > TIME_LIMIT) {
        flag = false;
        break;
      }
    }
  }

  // Python は最終レベルで total_cost ソートして [0] を取るが、
  // タイムアウトで最終レベルが空の場合があるので、C++ では「最も深い非空レベル」から選ぶ
  Node* best_node = root;
  for (int lv = LEVELS - 1; lv >= 0; lv--) {
    if (!chokudai_levels[lv].empty()) {
      auto tmp = chokudai_levels[lv].to_vector_inorder();
      sort(tmp.begin(), tmp.end(), [](const Node* a, const Node* b) {
        return a->total_cost() < b->total_cost();
      });
      best_node = tmp[0];
      break;
    }
  }

  // vector<Pos> collect_order = best_node->reconstruct_path();

  // ノード木をたどって経路復元
  vector<Pos> collect_order;
  vector<Pos> collect_order_2;
  int index = best_node->tree_index;  // best_node のインデックスを取得
  while (index != -1) {
    const auto& [parent_index, current_pos, stack_top] = node_tree[index];
    collect_order.push_back(current_pos);  // current_pos を追加
    if (stack_top.x != -1 || stack_top.y != -1) {  // stack_top が存在する場合
      collect_order_2.push_back(stack_top);  // stack_top を追加
    }
    index = parent_index;
  }
  // collect_order と collect_order_2 を結合 ただし、collect_order_2 は逆順に追加
  if (!collect_order.empty()) {
    collect_order.pop_back();  // (0,0) を想定して除去
  }
  reverse(collect_order.begin(), collect_order.end());
  collect_order.insert(collect_order.end(), collect_order_2.begin(), collect_order_2.end());
  collect_order.pop_back(); // 最後の余分な要素を削除

  vector<char> commands = make_commands(collect_order, Pos{0, 0});

  for (char c : commands) {
    cout << c << "\n";
  }

  // デバッグ（Python は stderr に出している）
  cerr << "Total path length: " << get_path_length(collect_order) << "\n";

  return 0;
}