# LeetCode 127. 单词接龙 (Word Ladder)

## 题目信息

- **题目链接**：https://leetcode.cn/problems/word-ladder/
- **难度**：中等 (Medium)
- **分类**：BFS、广度优先搜索、图论、最短路径

---

## 题目描述

字典 `wordList` 中从单词 `beginWord` 和 `endWord` 的**转换序列**是一个按下述规格形成的序列：

1. 序列中第一个单词是 `beginWord`
2. 序列中最后一个单词是 `endWord`
3. 每次转换只能改变一个字母
4. 转换过程中的中间单词都是字典 `wordList` 中的单词

给你两个单词 `beginWord` 和 `endWord` 和一个字典 `wordList`，找到从 `beginWord` 到 `endWord` 的**最短转换序列**的长度。如果不存在这样的转换序列，返回 0。

---

## 解题思路

### 核心思想：广度优先搜索 (BFS)

这是一道经典的**最短路径**问题，与游戏开发中的以下场景高度相关：

| 游戏开发场景 | 对应关系 |
|-------------|---------|
| NPC寻路导航 | 网格中找最短路径 |
| 技能树解锁 | 节点间最小转换次数 |
| 物品合成路径 | 资源转换最短链 |
| 状态机转移 | 状态图最短跳转 |

### BFS 核心步骤

```
1. 将 beginWord 加入队列
2. 每次取出队首单词
3. 对单词的每个位置，尝试替换为 a-z
4. 如果新单词在字典中：
   - 是目标单词 → 返回答案
   - 不是目标单词 → 加入队列，从字典删除（剪枝）
5. 重复直到队列为空
```

### 优化：双向 BFS

当单词表很大时，使用**双向搜索**从两端同时逼近：

```
起点 BFS ──────────→ ←────────── 终点 BFS
   hit ──→ hot ──→ ? ←── cog ──←
   
双向收敛时，路径 = 起点深度 + 终点深度 + 1
```

---

## 代码实现

### 解法一：BFS（标准版）

```cpp
int ladderLength(string beginWord, string endWord, vector<string>& wordList) {
    unordered_set<string> wordSet(wordList.begin(), wordList.end());
    if (wordSet.find(endWord) == wordSet.end()) return 0;
    
    queue<pair<string, int>> q;
    q.push({beginWord, 1});
    wordSet.erase(beginWord);
    
    while (!q.empty()) {
        auto [word, depth] = q.front();
        q.pop();
        
        for (int i = 0; i < word.size(); i++) {
            string next = word;
            for (char c = 'a'; c <= 'z'; c++) {
                if (c == word[i]) continue;
                next[i] = c;
                
                if (next == endWord) return depth + 1;
                if (wordSet.find(next) != wordSet.end()) {
                    q.push({next, depth + 1});
                    wordSet.erase(next);  // 关键剪枝
                }
            }
        }
    }
    return 0;
}
```

### 解法二：双向 BFS（优化版）

```cpp
int ladderLength(string beginWord, string endWord, vector<string>& wordList) {
    unordered_set<string> wordSet(wordList.begin(), wordList.end());
    if (!wordSet.count(endWord)) return 0;
    
    unordered_set<string> beginSet = {beginWord};
    unordered_set<string> endSet = {endWord};
    int depth = 1;
    
    while (!beginSet.empty() && !endSet.empty()) {
        if (beginSet.size() > endSet.size())
            swap(beginSet, endSet);
        
        unordered_set<string> temp;
        for (const string& word : beginSet) {
            string candidate = word;
            for (int i = 0; i < word.size(); i++) {
                char orig = word[i];
                for (char c = 'a'; c <= 'z'; c++) {
                    if (c == orig) continue;
                    candidate[i] = c;
                    if (endSet.count(candidate)) return depth + 1;
                    if (wordSet.count(candidate)) {
                        temp.insert(candidate);
                        wordSet.erase(candidate);
                    }
                }
                candidate[i] = orig;
            }
        }
        beginSet = temp;
        depth++;
    }
    return 0;
}
```

---

## 复杂度分析

### 解法一：BFS 标准版

| 指标 | 复杂度 | 说明 |
|-----|-------|------|
| **时间复杂度** | O(N × L × 26) | N=单词数，L=单词长度，26=字母表大小 |
| **空间复杂度** | O(N × L) | 哈希集合存储所有单词 |

### 解法二：双向 BFS

| 指标 | 复杂度 | 说明 |
|-----|-------|------|
| **时间复杂度** | O(N × L × 26 / 2) | 搜索空间减半，渐近相同 |
| **空间复杂度** | O(N × L) | 两个集合 + 临时集合 |

---

## 优化方向

### 1. 预计算通用状态
```
将 "hot" 预处理为通配符状态 "h*t"
- 可以快速匹配所有相似单词
- 时间优化：O(N×L) → O(N)
```

### 2. A* 搜索算法
```
使用启发式函数（编辑距离）引导搜索方向
适用于大规模单词表
```

### 3. 预处理建图
```
预先计算所有单词之间的边关系
将转换问题转化为无权图最短路径
```

---

## 游戏开发实战应用

### 场景一：NPC 寻路系统
```cpp
// 游戏网格地图
vector<string> grid = {
    "abc", "aec", "afc"
};
// BFS 找从 (0,0) 到 (2,2) 的最短路径
```

### 场景二：技能树解锁
```
当前技能: "fire_ball"
目标技能: "ultimate_inferno"

转换链: fire_ball → fire_bolt → flame_bolt → inferno → ultimate_inferno
```

### 场景三：物品合成优化
```
游戏内合成: wood + wood = plank
           plank + wood = stick
           stick + stick = handle
           
从 raw_materials 到 weapon 的最短合成路径
```

---

## 测试用例

```cpp
// 示例 1
Input: beginWord="hit", endWord="cog", wordList=["hot","dot","dog","lot","log","cog"]
Output: 5
路径: hit → hot → dot → dog → cog

// 示例 2
Input: beginWord="hit", endWord="cog", wordList=["hot","dot","dog","lot","log"]
Output: 0
无法转换

// 示例 3
Input: beginWord="a", endWord="c", wordList=["a","b","c"]
Output: 2
路径: a → c
```

---

## 总结

| 要点 | 内容 |
|-----|-----|
| **核心算法** | BFS 广度优先搜索 |
| **关键技巧** | 哈希集合剪枝、双向搜索 |
| **复杂度** | O(N × L × 26) / O(N × L) |
| **应用场景** | 寻路、状态转移、资源转换 |

> 🎯 **每日一题，坚持算法训练，为大厂面试做准备！**
