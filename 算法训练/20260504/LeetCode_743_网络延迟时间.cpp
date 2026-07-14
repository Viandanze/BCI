#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
#include <iostream>
using namespace std;

/**
 * LeetCode 743. 网络延迟时间
 * 
 * 题目：给定网络 times（信号传播延迟），求所有节点收到信号的最短时间
 * 算法：Dijkstra 最短路径（贪心 + 优先队列）
 * 
 * 时间复杂度：O(E log V)
 * 空间复杂度：O(V + E)
 */

class Solution {
public:
    int networkDelayTime(vector<vector<int>>& times, int n, int k) {
        // 构建邻接表
        vector<vector<pair<int, int>>> graph(n + 1);
        for (auto& t : times) {
            int u = t[0], v = t[1], w = t[2];
            graph[u].push_back({v, w});
        }
        
        // 距离数组：dist[i] = 从 k 到 i 的最短延迟
        vector<int> dist(n + 1, INT_MAX);
        dist[k] = 0;
        
        // 最小堆：(当前延迟, 节点)
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
        pq.push({0, k});
        
        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();
            
            // 已经找到更短的路径，跳过
            if (d > dist[u]) continue;
            
            // 遍历所有邻居
            for (auto& [v, w] : graph[u]) {
                int newDist = dist[u] + w;
                if (newDist < dist[v]) {
                    dist[v] = newDist;
                    pq.push({newDist, v});
                }
            }
        }
        
        // 找最大延迟（即所有节点收到信号的时间）
        int maxDelay = 0;
        for (int i = 1; i <= n; i++) {
            if (dist[i] == INT_MAX) {
                return -1;  // 有节点不可达
            }
            maxDelay = max(maxDelay, dist[i]);
        }
        
        return maxDelay;
    }
};

// ============== 测试代码 ==============
int main() {
    Solution sol;
    
    // 示例 1
    vector<vector<int>> times1 = {
        {2, 1, 1},
        {2, 3, 1},
        {3, 4, 1}
    };
    int n1 = 4, k1 = 2;
    cout << "Test 1: " << sol.networkDelayTime(times1, n1, k1) << endl;  // 输出: 2
    
    // 示例 2
    vector<vector<int>> times2 = {
        {1, 2, 1},
        {2, 1, 3}
    };
    int n2 = 2, k2 = 2;
    cout << "Test 2: " << sol.networkDelayTime(times2, n2, k2) << endl;  // 输出: 3
    
    return 0;
}
