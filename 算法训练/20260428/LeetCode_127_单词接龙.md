# LeetCode 127 - 单词接龙

## 题目描述

字典 `wordList` 中从 `beginWord` 和 `endWord` 的转换序列是一个按下述规格形成的序列：

- 序列中每个单词是 `wordList` 中的单词
- 序列中相邻的单词在原字典中只有一个字母不同
- 序列长度是最短的

## 游戏开发关联

| 游戏场景 | 算法应用 |
|---------|---------|
| NPC 寻路 | BFS 最短路径搜索 |
| 地图区域可达性 | 单词变换图构建 |
| 资源收集最短路径 | 双向 BFS 优化 |
| AI 状态转移 | 状态机图遍历 |

## 解法对比

### 解法1：单向 BFS

```cpp
int ladderLength(string beginWord, string endWord, vector<string>& wordList)
```

**思路**：
1. 从 `beginWord` 开始进行 BFS 层次遍历
2. 对每个单词，尝试将每个位置替换为 a-z
3. 如果替换后的单词在 `wordList` 中且未访问，加入队列
4. 找到 `endWord` 时返回层数 + 1

### 解法2：双向 BFS（推荐）

```cpp
int ladderLength(string beginWord, string endWord, vector<string>& wordList)
```

**思路**：
- 同时从 `beginWord` 和 `endWord` 两端进行 BFS
- 每次从较小的集合扩展
- 当两端搜索的节点相遇时，找到最短路径

## 复杂度分析

| 指标 | 单向 BFS | 双向 BFS |
|-----|---------|---------|
| **时间复杂度** | O(N × L × 26) | O(N × L × 26 / 2) |
| **空间复杂度** | O(N × L) | O(N × L) |

**参数说明**：
- N = wordList 长度
- L = 单词长度
- 26 = 字母表大小

**推导过程**：
- 最坏情况：每个单词都与所有其他单词相差一个字符
- 单向 BFS：遍历所有单词，每个单词生成 L×26 种变体
- 双向 BFS：搜索空间减半，收敛更快

## 代码实现要点

```cpp
// 1. 边界检查：endWord 必须在 wordList 中
if (wordSet.find(endWord) == wordSet.end()) return 0;

// 2. 哈希表去重，防止重复访问
unordered_set<string> visited;

// 3. 生成通配符变体进行匹配
for (int i = 0; i < wordLen; i++) {
    char original = word[i];
    for (char c = 'a'; c <= 'z'; c++) {
        if (c == original) continue;
        temp[i] = c;
        // 检查是否匹配...
    }
    temp[i] = original;
}

// 4. 双向 BFS 交接检测
if (endSet.find(temp) != endSet.end()) {
    return level + 1;
}
```

## 测试用例

| 用例 | beginWord | endWord | wordList | 预期结果 |
|-----|-----------|---------|----------|---------|
| 正常 | hit | cog | hot,dot,dog,lot,log,cog | 5 |
| 无解 | hit | cog | hot,dot,dog,lot,log | 0 |
| 直接相邻 | hot | hot | - | 2 |

## 优化方向

1. **预处理优化**：预先建立单词图（按层分组），避免运行时生成变体
2. **A* 搜索**：如果有外部启发式信息（如同义词距离），可使用 A* 进一步加速
3. **位运算优化**：使用位掩码表示单词，适用于长单词场景

## 面试拓展问题

> **Q**: 如何找到所有最短转换路径？
> 
> **A**: 在双向 BFS 基础上，增加路径记录（记录每个节点的父节点），最后回溯生成所有路径。时间复杂度会增加，但能获取完整路径信息。

## 总结

| 要点 | 说明 |
|-----|------|
| 核心算法 | BFS / 双向 BFS |
| 关键数据结构 | unordered_set（去重）、queue（BFS） |
| 游戏应用 | NPC 寻路、状态机、资源分配 |
| 优化技巧 | 双向搜索、预处理、位运算 |

---
*训练日期：2026-04-28*
*题目来源：LeetCode 127 - Word Ladder*
