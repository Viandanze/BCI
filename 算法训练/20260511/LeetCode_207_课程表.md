# LeetCode 207 - 课程表 (Course Schedule)

## 题目信息

| 项目 | 内容 |
|------|------|
| 题号 | 207 |
| 难度 | 中等 (Medium) |
| 链接 | https://leetcode.com/problems/course-schedule/ |
| 日期 | 2026-05-11 |

---

## 题目描述

你总共要选修 `numCourses` 门课程，编号从 `0` 到 `numCourses-1`。

某些课程可能有先修要求，例如：想要学习课程 0，你需要先完成课程 1。
我们用一对整数来表示先修要求：[0, 1]，表示学习课程 0 之前必须先完成课程 1。

给你课程总数 `numCourses` 和先修课程对列表 `prerequisites`，判断是否可能完成所有课程的学习。

---

## 游戏开发场景映射

这道题在游戏开发中有广泛的应用场景：

| 游戏概念 | 对应算法概念 |
|---------|-------------|
| 技能树/天赋树 | 课程依赖关系图 |
| 任务前置条件 | 先修课程要求 |
| 科技树研发顺序 | 拓扑排序 |
| 成就解锁系统 | 依赖链检测 |
| 装备合成配方 | 资源依赖图 |

**实际问题**：
- 玩家能否解锁某个终极技能？
- 所有支线任务能否全部完成？
- 某个科技路线是否可行？

---

## 解法一：Kahn算法（BFS拓扑排序）

### 核心思路

1. **构建有向图**：用邻接表表示课程之间的依赖关系
2. **计算入度**：统计每门课程有多少前置课程
3. **BFS遍历**：先学习没有前置要求的课程（入度为0），逐步解锁后续课程
4. **环检测**：如果最终学习的课程数 < 总课程数，说明存在环（死锁）

### 代码实现

```cpp
bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
    vector<vector<int>> adj(numCourses);
    vector<int> inDegree(numCourses, 0);
    
    for (auto& pre : prerequisites) {
        adj[pre[1]].push_back(pre[0]);
        inDegree[pre[0]]++;
    }
    
    queue<int> q;
    for (int i = 0; i < numCourses; i++) {
        if (inDegree[i] == 0) q.push(i);
    }
    
    int visited = 0;
    while (!q.empty()) {
        int curr = q.front();
        q.pop();
        visited++;
        
        for (int next : adj[curr]) {
            if (--inDegree[next] == 0) {
                q.push(next);
            }
        }
    }
    
    return visited == numCourses;
}
```

---

## 解法二：DFS（递归检测环）

### 核心思路

使用三种状态标记：
- **0**：未访问
- **1**：正在访问（在当前递归栈中）
- **2**：已访问（已完成探索，无环）

如果在DFS过程中遇到"正在访问"的节点，说明存在环。

### 代码实现

```cpp
bool dfs(int course, vector<vector<int>>& adj, vector<int>& visited) {
    if (visited[course] == 1) return true;   // 环检测
    if (visited[course] == 2) return false;  // 已访问，无环
    
    visited[course] = 1;
    for (int next : adj[course]) {
        if (dfs(next, adj, visited)) return true;
    }
    visited[course] = 2;
    return false;
}
```

---

## 解法三：并查集

适合需要快速判断两门课是否在同一个依赖链中的场景。

---

## 复杂度分析

| 解法 | 时间复杂度 | 空间复杂度 |
|------|-----------|-----------|
| Kahn算法 | O(V + E) | O(V + E) |
| DFS | O(V + E) | O(V + E) |
| 并查集 | O(V × α(E)) ≈ O(E) | O(V) |

**说明**：
- V = 课程数量（numCourses）
- E = 依赖关系数量（prerequisites.size()）
- α(E) 是 Ackermann 函数的反函数，近似常数

---

## 测试结果

```
=== 测试1：链式依赖 ===
课程数: 4
依赖关系: 0->1->2->3
结果: 可以完成 ✓

=== 测试2：菱形依赖 ===
课程数: 4
依赖关系: 1,2都依赖0，3依赖1和2
结果: 可以完成 ✓

=== 测试3：有环依赖 ===
课程数: 3
依赖关系: 0->1->2->0 (循环)
结果: 无法完成 ✓

=== 测试4：RPG技能树场景 ===
课程数: 5
0=基础剑术, 1=火焰剑, 2=冰霜剑, 3=高级火焰剑, 4=元素大师
依赖: 1->0, 2->1, 3->0, 4->2, 4->3
结果: 可以完成 ✓
```

---

## 扩展：如何找到学习顺序？

如果不仅需要判断能否完成，还要给出具体的学习顺序（拓扑排序结果），可以使用 Kahn算法 的变形：

```cpp
vector<int> findOrder(int numCourses, vector<vector<int>>& prerequisites) {
    // ... 前面的构建图和入度数组代码相同 ...
    
    vector<int> result;
    while (!q.empty()) {
        int curr = q.front();
        q.pop();
        result.push_back(curr);
        
        for (int next : adj[curr]) {
            if (--inDegree[next] == 0) {
                q.push(next);
            }
        }
    }
    
    if (result.size() == numCourses) {
        return result;
    }
    return {};  // 有环，无法完成
}
```

---

## 关联题目

| 题号 | 题目名称 | 难度 |
|------|---------|------|
| 210 | 课程表 II | 中等 |
| 269 | 火星词典 | 困难 |
| 953 | 验证外星语词典 | 简单 |
| 1619 | 删除树中所有边之后所需的水果数量 | 中等 |

---

## 总结

这道题是图论中的**环检测**和**拓扑排序**的经典应用，在游戏开发中可用于：
- 技能树/天赋系统的依赖验证
- 任务系统的完成顺序判断
- 科技树研发的可行性分析
- 成就解锁条件的判断

掌握这道题的核心思想，可以解决一类"依赖关系 + 环检测"的问题。
