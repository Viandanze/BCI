# LeetCode 743 - 网络延迟时间 (Network Delay Time)

## 题目描述

n 个网络节点，标记为 1 到 n。给你一个 times 数组，表示信号通过特定边直接传递的时间。

`times[i] = (u, v, w)`：
- u：源节点
- v：目标节点  
- w：信号传递所需时间

从节点 k 出发向所有节点发送信号，返回所有节点收到信号的最长时间。如果无法让所有节点收到信号，返回 -1。

## 游戏开发关联

| 场景 | 说明 |
|------|------|
| 游戏服务器同步 | 计算玩家到各服务器节点的最大延迟 |
| 网络同步延迟 | 确定游戏状态同步的最坏情况延迟 |
| AI寻路 | Dijkstra 是 A* 算法的基础 |
| 网络同步 | 确定心跳包同步周期 |

---

## 解题思路

### 核心算法：Dijkstra 单源最短路径

从源点 k 出发，求到所有节点的最短路径，然后取最大值。

**为什么选 Dijkstra？**
- 非负权重最短路问题
- 贪心 + 优先队列，时间复杂度 O(E log V)
- 游戏场景中权重通常非负（延迟 >= 0）

---

## 代码实现

### 方法1：朴素实现 O(V²)

```cpp
class Solution_Basic {
public:
    int networkDelayTime(vector<vector<int>>& times, int n, int k) {
        const int INF = INT_MAX;
        vector<vector<int>> graph(n + 1, vector<int>(n + 1, INF));
        
        for (auto& t : times) {
            graph[t[0]][t[1]] = t[2];
        }
        
        vector<int> dist(n + 1, INF);
        vector<bool> visited(n + 1, false);
        dist[k] = 0;
        
        for (int i = 1; i <= n; i++) {
            // 找未访问节点中距离最小的
            int u = -1, minDist = INF;
            for (int j = 1; j <= n; j++) {
                if (!visited[j] && dist[j] < minDist) {
                    minDist = dist[j];
                    u = j;
                }
            }
            
            if (u == -1) break;
            visited[u] = true;
            
            // 更新邻居
            for (int v = 1; v <= n; v++) {
                if (!visited[v] && graph[u][v] != INF) {
                    dist[v] = min(dist[v], dist[u] + graph[u][v]);
                }
            }
        }
        
        int result = 0;
        for (int i = 1; i <= n; i++) {
            if (dist[i] == INF) return -1;
            result = max(result, dist[i]);
        }
        return result;
    }
};
```

### 方法2：堆优化 O(E log V) 【推荐】

```cpp
class Solution_Optimized {
public:
    int networkDelayTime(vector<vector<int>>& times, int n, int k) {
        vector<vector<pair<int, int>>> adj(n + 1);
        for (auto& t : times) {
            adj[t[0]].push_back({t[1], t[2]});
        }
        
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
        const int INF = INT_MAX;
        vector<int> dist(n + 1, INF);
        dist[k] = 0;
        pq.push({0, k});
        
        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();
            if (d > dist[u]) continue;  // 跳过过期路径
            
            for (auto& [v, w] : adj[u]) {
                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    pq.push({dist[v], v});
                }
            }
        }
        
        int result = 0;
        for (int i = 1; i <= n; i++) {
            if (dist[i] == INF) return -1;
            result = max(result, dist[i]);
        }
        return result;
    }
};
```

---

## 复杂度分析

| 方法 | 时间复杂度 | 空间复杂度 | 适用场景 |
|------|-----------|-----------|---------|
| 朴素 Dijkstra | O(V²) | O(V²) | 稠密图 (E ≈ V²) |
| 堆优化 Dijkstra | O(E log V) | O(V + E) | 稀疏图 (E << V²) |
| SPFA | O(kE) 期望 | O(V + E) | 负权重图 |

**实际选择：**
- 游戏网络：通常使用堆优化版本，因为图相对稀疏
- 实时性要求高：可预先计算所有节点对的最短路径

---

## 测试用例

```
测试1:
times = [[2,1,1],[2,3,1],[3,4,1]], n=4, k=2
结果: 2
解释: 从节点2出发，延迟最长的节点是4，需要2个单位时间

测试2:
times = [[1,2,1],[1,3,3],[2,3,1],[2,4,4],[3,4,1]], n=4, k=1  
结果: 3
解释: 1→2=1, 1→3=2, 1→4=3

测试3:
times = [[1,2,1]], n=2, k=2
结果: -1
解释: 无法从节点2到达节点1
```

---

## 游戏开发中的扩展

### 1. 多玩家延迟同步
```cpp
// 在游戏服务器中计算所有玩家的最大延迟
int maxLatency = sol.networkDelayTime(networkTimes, serverCount, playerNode);

// 决定同步策略
if (maxLatency > 100ms) {
    // 使用保守的延迟补偿
    enableDelayCompensation();
}
```

### 2. 延迟分级匹配
```cpp
// 按延迟分区，减少作弊和体验差异
vector<int> getLatencyTier(int latency) {
    if (latency < 50) return 0;  // 电竞级
    if (latency < 100) return 1; // 正常
    if (latency < 200) return 2; // 勉强
    return 3;  // 高延迟匹配
}
```

### 3. A* 寻路（基于 Dijkstra）
```cpp
// f(n) = g(n) + h(n)
// Dijkstra 是 h(n) = 0 的特殊情况
float AStar(Node start, Node goal) {
    float g = 0;  // 实际代价
    float h = heuristic(start, goal);  // 启发函数
    return g + h;
}
```

---

## 总结

- **核心**：`Dijkstra + 取最大值`
- **时间复杂度**：O(E log V)（堆优化）
- **空间复杂度**：O(V + E)
- **游戏应用**：网络同步、延迟补偿、AI寻路

---

## 相关题目

| 题目 | 难度 | 关联 |
|------|------|------|
| 743. 网络延迟时间 | 中等 | 本题 |
| 787. K 站中转内最便宜的航班 | 中等 | 多限制最短路 |
| 1514. 概率最大的路径 | 中等 | 期望最短路 |
| 1631. 最小体力消耗路径 | 中等 | 二维 Dijkstra |
| 1786. 从出发点到家的距离 | 中等 | BFS + Dijkstra |
