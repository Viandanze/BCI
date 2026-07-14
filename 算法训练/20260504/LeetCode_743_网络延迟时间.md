# 743. 网络延迟时间

## 题目描述

> 给定一个网络 `times`，表示信号传播的有向边的传输时间，计算所有节点收到信号所需的最短时间。如果有节点无法收到信号，返回 -1。

**游戏开发场景**：
- 游戏服务器向所有玩家广播事件，计算最远玩家收到消息的延迟
- 网络同步时评估同步完成时间

## 解题思路

**核心算法**：Dijkstra 单源最短路径（贪心 + 优先队列）

**步骤**：
1. 用邻接表存储有向图
2. 从节点 k 出发，使用最小堆不断扩展最近节点
3. 维护每个节点的最短延迟
4. 最终答案为所有节点中的最大延迟

**为什么用 Dijkstra**：
- 非负权重 → 保证贪心正确性
- 游戏延迟均为正值 → 适用

## C++ 实现

```cpp
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

class Solution {
public:
    int networkDelayTime(vector<vector<int>>& times, int n, int k) {
        // 构建邻接表
        vector<vector<pair<int, int>>> graph(n + 1);
        for (auto& t : times) {
            graph[t[0]].push_back({t[1], t[2]});
        }
        
        // 距离数组，初始化为无穷大
        vector<int> dist(n + 1, INT_MAX);
        dist[k] = 0;
        
        // 最小堆：(延迟, 节点)
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
        pq.push({0, k});
        
        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();
            
            // 跳过已处理的节点
            if (d > dist[u]) continue;
            
            // 遍历邻居
            for (auto& [v, w] : graph[u]) {
                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    pq.push({dist[v], v});
                }
            }
        }
        
        // 找最大延迟
        int maxDelay = 0;
        for (int i = 1; i <= n; i++) {
            if (dist[i] == INT_MAX) return -1; // 有节点不可达
            maxDelay = max(maxDelay, dist[i]);
        }
        
        return maxDelay;
    }
};
```

## 复杂度分析

| 指标 | 复杂度 | 说明 |
|------|--------|------|
| **时间复杂度** | O(E log V) | E 为边数，V 为节点数；优先队列每个节点出队一次 |
| **空间复杂度** | O(V + E) | 邻接表存储 + 距离数组 + 优先队列 |

## 优化方向

### 游戏开发特定优化

1. **A* 搜索**
   - 当只需要计算部分节点时，使用启发式函数剪枝
   - 适用于实时游戏路径规划

2. **JPS (Jump Point Search)**
   - 网格地图最优路径搜索
   - UE5 导航系统使用类似思想

3. **多源预处理**
   - 使用 FLood fill 预处理固定拓扑的延迟场
   - 适合 MMORPG 区域服务器

4. **近似算法**
   - 游戏延迟容忍度 ±10ms 可忽略
   - 可用跳数限制替代精确计算

## 相关题目

- 787. K 站中转内最便宜的航班（带约束最短路径）
- 1334. 阈值距离内邻居最少的城市（多源 Dijkstra）
- 1514. 概率最大的路径（概率最短路）
