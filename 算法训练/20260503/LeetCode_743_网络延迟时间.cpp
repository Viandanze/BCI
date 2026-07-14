/**
 * LeetCode 743 - 网络延迟时间 (Network Delay Time)
 * 
 * 题目描述：
 * n 个网络节点，标记为 1 到 n。给你一个 times数组，表示信号通过特定边直接传递的时间。
 * times[i] = (u, v, w)，其中 u 是源节点，v 是目标节点，w 是信号从一个节点到另一个节点所需的时间。
 * 现在，从节点 k 出发，向所有节点发送信号。
 * 返回所有节点（包括 k）收到信号所需的最长时间。
 * 如果无法让所有节点收到信号，返回 -1。
 * 
 * 这道题与游戏开发的关联：
 * - 游戏服务器同步：计算玩家到各服务器节点的最大延迟
 * - 网络同步延迟：确定游戏状态同步的最坏延迟
 * - 路径规划：游戏AI寻路的基础算法
 */

#include <iostream>
#include <vector>
#include <queue>
#include <climits>

using namespace std;

// ==================== 方法1: 朴素Dijkstra O(V²) ====================
class Solution_Basic {
public:
    int networkDelayTime(vector<vector<int>>& times, int n, int k) {
        // 构建邻接矩阵
        const int INF = INT_MAX;
        vector<vector<int>> graph(n + 1, vector<int>(n + 1, INF));
        
        for (auto& t : times) {
            graph[t[0]][t[1]] = t[2];
        }
        
        // 距离数组，dist[i] 表示从 k 到 i 的最短距离
        vector<int> dist(n + 1, INF);
        vector<bool> visited(n + 1, false);
        dist[k] = 0;
        
        // 找到未访问节点中距离最小的
        for (int i = 1; i <= n; i++) {
            int u = -1;
            int minDist = INF;
            for (int j = 1; j <= n; j++) {
                if (!visited[j] && dist[j] < minDist) {
                    minDist = dist[j];
                    u = j;
                }
            }
            
            // 所有节点都已访问或无法到达
            if (u == -1) break;
            visited[u] = true;
            
            // 更新邻居节点的距离
            for (int v = 1; v <= n; v++) {
                if (!visited[v] && graph[u][v] != INF) {
                    if (dist[u] + graph[u][v] < dist[v]) {
                        dist[v] = dist[u] + graph[u][v];
                    }
                }
            }
        }
        
        // 找到最大距离
        int result = 0;
        for (int i = 1; i <= n; i++) {
            if (dist[i] == INF) return -1;
            result = max(result, dist[i]);
        }
        
        return result;
    }
};


// ==================== 方法2: 堆优化Dijkstra O(E log V) ====================
class Solution_Optimized {
public:
    int networkDelayTime(vector<vector<int>>& times, int n, int k) {
        // 构建邻接表
        vector<vector<pair<int, int>>> adj(n + 1);
        for (auto& t : times) {
            adj[t[0]].push_back({t[1], t[2]});  // {目标节点, 权重}
        }
        
        // 小顶堆：{距离, 节点}
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
        
        const int INF = INT_MAX;
        vector<int> dist(n + 1, INF);
        dist[k] = 0;
        pq.push({0, k});
        
        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();
            
            // 跳过已更新的较长路径
            if (d > dist[u]) continue;
            
            // 遍历邻居
            for (auto& [v, w] : adj[u]) {
                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    pq.push({dist[v], v});
                }
            }
        }
        
        // 找到最大距离
        int result = 0;
        for (int i = 1; i <= n; i++) {
            if (dist[i] == INF) return -1;
            result = max(result, dist[i]);
        }
        
        return result;
    }
};


// ==================== 方法3: SPFA (Bellman-Ford队列优化) ====================
class Solution_SPFA {
public:
    int networkDelayTime(vector<vector<int>>& times, int n, int k) {
        // 构建邻接表
        vector<vector<pair<int, int>>> adj(n + 1);
        for (auto& t : times) {
            adj[t[0]].push_back({t[1], t[2]});
        }
        
        const int INF = INT_MAX;
        vector<int> dist(n + 1, INF);
        vector<int> inQueue(n + 1, false);
        queue<int> q;
        
        dist[k] = 0;
        q.push(k);
        inQueue[k] = true;
        
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            inQueue[u] = false;
            
            for (auto& [v, w] : adj[u]) {
                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    if (!inQueue[v]) {
                        q.push(v);
                        inQueue[v] = true;
                    }
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


// ==================== 测试代码 ====================
int main() {
    // 测试用例1: 官方示例
    vector<vector<int>> times1 = {
        {2, 1, 1},
        {2, 3, 1},
        {3, 4, 1}
    };
    int n1 = 4, k1 = 2;
    // 从节点2出发: 2->1(1), 2->3(1), 3->4(1) = 2->4(2)
    // 最长延迟 = 2
    
    // 测试用例2: 多个路径
    vector<vector<int>> times2 = {
        {1, 2, 1},
        {1, 3, 3},
        {2, 3, 1},
        {2, 4, 4},
        {3, 4, 1}
    };
    int n2 = 4, k2 = 1;
    // 从节点1出发: 
    // 1->2: 1
    // 1->3: min(3, 1+1) = 2
    // 1->4: 1->2->4 = 5 或 1->3->4 = 3，最短3
    // 最长延迟 = 3
    
    // 测试用例3: 无法到达
    vector<vector<int>> times3 = {
        {1, 2, 1}
    };
    int n3 = 2, k3 = 2;
    // 从节点2出发，但只有1->2的边，无法到达节点1
    // 返回 -1
    
    Solution_Optimized sol;
    
    cout << "===== 网络延迟时间测试 =====" << endl;
    cout << "测试1 (预期: 2): " << sol.networkDelayTime(times1, n1, k1) << endl;
    cout << "测试2 (预期: 3): " << sol.networkDelayTime(times2, n2, k2) << endl;
    cout << "测试3 (预期: -1): " << sol.networkDelayTime(times3, n3, k3) << endl;
    
    return 0;
}
