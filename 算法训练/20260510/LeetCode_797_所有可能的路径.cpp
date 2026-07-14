/**
 * LeetCode 797 - 所有可能的路径
 * All Paths From Source to Target
 * 
 * 题目描述：
 * 给定一个有向无环图（DAG），包含 n 个节点，节点编号从 0 到 n-1。
 * 给你一个二维整数数组 graph，其中 graph[i] 表示节点 i 的所有出边邻居。
 * 返回从节点 0 到节点 n-1 的所有可能路径，以列表形式返回。
 * 
 * 游戏开发关联：
 * - 游戏AI状态机：枚举从初始状态到目标状态的所有可能转换路径
 * - 对话树/任务树：RPG游戏中所有可能的剧情分支路径
 * - 关卡路径规划：计算玩家从起点到终点的所有可行路线
 * - 技能树遍历：MMO中从起点到目标技能的所有解锁路径
 * 
 * 解法：深度优先搜索 (DFS) + 回溯
 */

#include <iostream>
#include <vector>
using namespace std;

class Solution {
public:
    vector<vector<int>> allPathsSourceTarget(vector<vector<int>>& graph) {
        vector<vector<int>> result;
        vector<int> path;
        
        // 从节点0开始DFS
        dfs(graph, 0, path, result);
        
        return result;
    }
    
private:
    void dfs(const vector<vector<int>>& graph, int node, 
             vector<int>& path, vector<vector<int>>& result) {
        // 将当前节点加入路径
        path.push_back(node);
        
        // 到达目标节点，保存路径（目标节点是 n-1）
        if (node == graph.size() - 1) {
            result.push_back(path);
        } else {
            // 遍历所有出边
            for (int neighbor : graph[node]) {
                dfs(graph, neighbor, path, result);
            }
        }
        
        // 回溯：移除当前节点，尝试其他分支
        path.pop_back();
    }
};

// ==================== 辅助函数 ====================

void printResult(const vector<vector<int>>& paths) {
    cout << "[" << endl;
    for (size_t i = 0; i < paths.size(); ++i) {
        cout << "  [";
        for (size_t j = 0; j < paths[i].size(); ++j) {
            cout << paths[i][j];
            if (j < paths[i].size() - 1) cout << ", ";
        }
        cout << "]";
        if (i < paths.size() - 1) cout << ",";
        cout << endl;
    }
    cout << "]" << endl;
}

// ==================== 测试用例 ====================

int main() {
    Solution solution;
    
    // 测试用例1：经典DAG
    // 0 -> 1 -> 3
    // |    |
    // v    v
    // 2 -> 4 -> 5
    cout << "===== 测试用例1 =====" << endl;
    cout << "图结构:" << endl;
    cout << "0 -> [1, 2]" << endl;
    cout << "1 -> [3, 4]" << endl;
    cout << "2 -> [4]" << endl;
    cout << "3 -> [5]" << endl;
    cout << "4 -> [5]" << endl;
    cout << "5 -> []" << endl;
    
    vector<vector<int>> graph1 = {
        {1, 2},    // 0
        {3, 4},    // 1
        {4},       // 2
        {5},       // 3
        {5},       // 4
        {}         // 5
    };
    
    cout << "所有路径 (0 -> 5):" << endl;
    auto result1 = solution.allPathsSourceTarget(graph1);
    printResult(result1);
    cout << "路径数量: " << result1.size() << endl;
    
    // 测试用例2：简单线性图
    // 0 -> 1 -> 2 -> 3
    cout << "\n===== 测试用例2 (线性) =====" << endl;
    vector<vector<int>> graph2 = {
        {1},  // 0
        {2},  // 1
        {3},  // 2
        {}    // 3
    };
    
    auto result2 = solution.allPathsSourceTarget(graph2);
    printResult(result2);
    cout << "路径数量: " << result2.size() << endl;
    
    // 测试用例3：多条分支
    //      1
    //     / \
    // 0  2   3  4
    //     \ /
    //      5
    cout << "\n===== 测试用例3 (多分支) =====" << endl;
    vector<vector<int>> graph3 = {
        {1, 2, 3},  // 0
        {4},        // 1
        {4},        // 2
        {4},        // 3
        {5},        // 4
        {}          // 5
    };
    
    auto result3 = solution.allPathsSourceTarget(graph3);
    printResult(result3);
    cout << "路径数量: " << result3.size() << endl;
    
    return 0;
}
