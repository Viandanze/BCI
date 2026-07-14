# LeetCode 79. 单词搜索 - 解题报告

## 题目信息
- **题目编号**：79
- **题目名称**：Word Search
- **难度等级**：Medium
- **算法类型**：DFS + 回溯（Backtracking）
- **链接**：https://leetcode.com/problems/word-search/

---

## 1. 题目描述

给定一个 `m x n` 的字符网格 `board` 和一个字符串 `word`，判断 `word` 是否存在于网格中。

**路径规则**：
- 从某个格子开始，可以向上下左右四个方向移动
- 每个格子只能使用一次
- 路径必须严格按照单词顺序

---

## 2. 游戏开发场景映射

| 游戏场景 | 本题对应 |
|---------|---------|
| 地牢探索游戏 | 地图路径匹配 |
| 迷宫寻路 | 寻找出口路径 |
| 文字类RPG | 技能/道具名称搜索 |
| 连连看/消除游戏 | 连续格子匹配 |
| 单词游戏（如Boggle） | 字母网格搜索 |

---

## 3. 解题思路

### 核心思想：DFS + 回溯

```
                    开始
                      │
            ┌─────────┼─────────┐
            ▼         ▼         ▼         ▼
           [0,0]    [0,1]    [1,0]    [0,-1]  ← 四个方向探索
            │
      ┌─────┼─────┐
      ▼     ▼     ▼
   继续   回溯   回溯  ← 走不通就回退
   搜索   恢复   跳过
```

### 算法步骤

1. **遍历起点**：从网格中每个格子作为起始点尝试
2. **深度优先搜索**：沿着四个方向递归探索
3. **剪枝条件**：
   - 越界 → 停止
   - 字符不匹配 → 停止
4. **回溯**：探索完一条路径后，恢复原状态

---

## 4. C++实现代码

```cpp
class Solution {
private:
    const int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    int m, n;
    
    bool dfs(vector<vector<char>>& board, const string& word, int i, int j, int idx) {
        if (idx == word.size() - 1) {
            return board[i][j] == word[idx];
        }
        
        if (board[i][j] == word[idx]) {
            char original = board[i][j];
            board[i][j] = '#';  // 标记已访问
            
            for (auto& dir : directions) {
                int newI = i + dir[0];
                int newJ = j + dir[1];
                
                if (newI >= 0 && newI < m && newJ >= 0 && newJ < n) {
                    if (dfs(board, word, newI, newJ, idx + 1)) {
                        return true;
                    }
                }
            }
            
            board[i][j] = original;  // 回溯恢复
        }
        
        return false;
    }
    
public:
    bool exist(vector<vector<char>>& board, string word) {
        if (board.empty() || board[0].empty() || word.empty()) {
            return false;
        }
        
        m = board.size();
        n = board[0].size();
        
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                if (dfs(board, word, i, j, 0)) {
                    return true;
                }
            }
        }
        
        return false;
    }
};
```

---

## 5. 复杂度分析

### 时间复杂度：O(m × n × 4^L)

| 符号 | 含义 |
|-----|------|
| m × n | 网格中每个格子都可能是起点 |
| 4^L | 每个位置向4个方向扩展，深度为L（单词长度） |

**最坏情况**：每个格子都要探索4条路径，深度为L

### 空间复杂度：O(m × n + L)

| 组成部分 | 原因 |
|---------|------|
| O(m × n) | 修改board标记已访问（原地修改） |
| O(L) | 递归栈深度最多为单词长度 |

---

## 6. 优化方向

### 优化1：前缀预检查
```cpp
// 统计单词首字母出现次数，提前剪枝
if (countFirstLetter < needed) return false;
```

### 优化2：限制搜索起点
```cpp
// 只从与单词首字母相同的格子开始
if (board[i][j] == word[0]) {
    // 开始搜索
}
```

### 优化3：字典树（Trie）优化
适用于**批量查询多个单词**时：
- 构建Trie树存储所有待查询单词
- 一次搜索可同时检查多个单词
- 适合Boggle类游戏

### 优化4：记忆化（剪枝优化）
```cpp
// 记忆化搜索，对某些中间状态不再重复搜索
// 但本题由于每个格子只能用一次，记忆化效果有限
```

---

## 7. 游戏开发实战建议

### 地牢探索游戏实现
```cpp
// 游戏场景：玩家在地牢中寻找宝藏路径
struct Position { int x, y; };

class DungeonExplorer {
public:
    bool findTreasure(vector<vector<Tile>>& map, vector<Position>& path) {
        string treasureName = "TREASURE";
        return searchPath(map, treasureName, 0, startX, startY, path);
    }
};
```

### 关键技巧
1. **避免重复访问**：用`visited`数组或原地标记
2. **边界检查**：始终检查`x >= 0 && x < width && y >= 0 && y < height`
3. **回溯恢复**：确保路径探索后状态恢复

---

## 8. 相关题目

| 题号 | 题目名称 | 难度 |
|-----|---------|------|
| 79 | Word Search | Medium |
| 212 | Word Search II | Hard |
| 79 | 单词搜索 II | Hard |
| 130 | Surrounded Regions | Medium |
| 200 | Number of Islands | Medium |

---

## 9. 总结

| 要点 | 内容 |
|-----|------|
| 核心算法 | DFS + 回溯 |
| 关键技巧 | 原地标记 + 回溯恢复 |
| 时间复杂度 | O(m × n × 4^L) |
| 空间复杂度 | O(m × n + L) |
| 适用场景 | 迷宫、地图探索、路径匹配 |

---

*训练日期：2026-05-05*
*耗时：约25分钟*
