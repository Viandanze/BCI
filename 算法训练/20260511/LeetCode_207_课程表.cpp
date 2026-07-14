/**
 * LeetCode 207 - 课程表 (Course Schedule)
 * 
 * 游戏开发映射：技能树依赖系统、任务前置条件解锁、科技树顺序
 * 
 * 题目描述：
 * 你想选修 numCourses 门课程，记为 0 到 numCourses-1 。
 * 在选修某些课程之前可能需要先修其他课程。例如，要学习课程 0 ，
 * 你需要先完成课程 1，我们用一对数据表示：[0,1]。
 * 
 * 给定课程总数 numCourses 和先修课程对 prerequisites，
 * 判断是否可能完成所有课程的学习（无环依赖）。
 */

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>

using namespace std;

/**
 * 解法一：Kahn算法（拓扑排序 - BFS）
 * 
 * 核心思路：
 * - 构建有向图，计算每个节点的入度
 * - 从入度为0的节点开始，逐步移除（相当于学习该课程）
 * - 如果最终能移除所有节点，说明无环
 * 
 * 游戏开发场景：
 * - 入度为0的技能 = 可以直接学习的技能（没有前置要求）
 * - 移除节点 = 学习该技能后解锁依赖它的技能
 */
class Solution_Kahn {
public:
    bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
        // 构建邻接表和入度数组
        vector<vector<int>> adj(numCourses);
        vector<int> inDegree(numCourses, 0);
        
        for (auto& pre : prerequisites) {
            int course = pre[0];      // 目标课程（后置）
            int prereq = pre[1];      // 先修课程（前置）
            adj[prereq].push_back(course);  // prereq -> course
            inDegree[course]++;              // course的入度+1
        }
        
        // BFS：先将所有入度为0的节点加入队列
        queue<int> q;
        for (int i = 0; i < numCourses; i++) {
            if (inDegree[i] == 0) {
                q.push(i);
            }
        }
        
        int visited = 0;  // 已学习的课程数
        
        while (!q.empty()) {
            int curr = q.front();
            q.pop();
            visited++;
            
            // 移除当前节点，更新相邻节点的入度
            for (int next : adj[curr]) {
                inDegree[next]--;
                if (inDegree[next] == 0) {
                    q.push(next);
                }
            }
        }
        
        // 如果能学习所有课程，说明无环
        return visited == numCourses;
    }
};

/**
 * 解法二：DFS（递归检测环）
 * 
 * 核心思路：
 * - 对每个节点进行DFS
 * - 使用三种状态：0=未访问，1=正在访问（在当前路径上），2=已访问
 * - 如果在DFS过程中遇到"正在访问"的节点，说明有环
 * 
 * 游戏开发场景：
 * - 状态1 = 正在尝试解锁的技能路径
 * - 遇到已经在当前路径上的技能 = 循环依赖
 */
class Solution_DFS {
public:
    bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
        // 构建邻接表
        vector<vector<int>> adj(numCourses);
        for (auto& pre : prerequisites) {
            adj[pre[1]].push_back(pre[0]);
        }
        
        // 0=未访问, 1=正在访问, 2=已访问
        vector<int> visited(numCourses, 0);
        
        // 对每个未访问的节点进行DFS
        for (int i = 0; i < numCourses; i++) {
            if (dfs(i, adj, visited)) {
                return false;  // 发现环，无法完成
            }
        }
        
        return true;
    }
    
private:
    bool dfs(int course, vector<vector<int>>& adj, vector<int>& visited) {
        if (visited[course] == 1) return true;   // 环检测
        if (visited[course] == 2) return false;  // 已访问，无环
        
        visited[course] = 1;  // 标记为正在访问
        
        for (int next : adj[course]) {
            if (dfs(next, adj, visited)) {
                return true;  // 子路径有环
            }
        }
        
        visited[course] = 2;  // 标记为已访问
        return false;
    }
};

/**
 * 解法三：并查集（检测是否有直接冲突）
 * 
 * 适合场景：需要快速判断两门课是否在同一个"依赖链"中
 */
class Solution_UnionFind {
public:
    bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
        UnionFind uf(numCourses);
        
        for (auto& pre : prerequisites) {
            int course = pre[0];
            int prereq = pre[1];
            
            // 如果course和prereq已经在同一个集合，说明有环
            if (uf.find(course) == uf.find(prereq)) {
                return false;
            }
            
            uf.unite(course, prereq);
        }
        
        return true;
    }
    
private:
    class UnionFind {
    public:
        vector<int> parent;
        
        UnionFind(int n) {
            parent.resize(n);
            iota(parent.begin(), parent.end(), 0);
        }
        
        int find(int x) {
            if (parent[x] != x) {
                parent[x] = find(parent[x]);
            }
            return parent[x];
        }
        
        void unite(int x, int y) {
            parent[find(y)] = find(x);
        }
    };
};

// ==================== 测试代码 ====================

int main() {
    Solution_Kahn sol;
    
    // 测试用例1：基本测试
    // 课程0依赖课程1，课程1依赖课程2，课程2依赖课程3
    // 问：能否完成所有课程？
    int numCourses1 = 4;
    vector<vector<int>> prerequisites1 = {{1,0}, {2,1}, {3,2}};
    
    cout << "=== 测试1：链式依赖 ===" << endl;
    cout << "课程数: " << numCourses1 << endl;
    cout << "依赖关系: 0->1->2->3" << endl;
    cout << "结果: " << (sol.canFinish(numCourses1, prerequisites1) ? "可以完成" : "无法完成") << endl;
    cout << endl;
    
    // 测试用例2：菱形依赖
    //     1
    //    / \
    //   0   2
    //    \ /
    //     3
    int numCourses2 = 4;
    vector<vector<int>> prerequisites2 = {{1,0}, {2,0}, {3,1}, {3,2}};
    
    cout << "=== 测试2：菱形依赖 ===" << endl;
    cout << "课程数: " << numCourses2 << endl;
    cout << "依赖关系: 1,2都依赖0，3依赖1和2" << endl;
    cout << "结果: " << (sol.canFinish(numCourses2, prerequisites2) ? "可以完成" : "无法完成") << endl;
    cout << endl;
    
    // 测试用例3：有环依赖
    // 0 -> 1 -> 2 -> 0
    int numCourses3 = 3;
    vector<vector<int>> prerequisites3 = {{1,0}, {2,1}, {0,2}};
    
    cout << "=== 测试3：有环依赖 ===" << endl;
    cout << "课程数: " << numCourses3 << endl;
    cout << "依赖关系: 0->1->2->0 (循环)" << endl;
    cout << "结果: " << (sol.canFinish(numCourses3, prerequisites3) ? "可以完成" : "无法完成") << endl;
    cout << endl;
    
    // 测试用例4：技能树场景
    // 假设一个RPG游戏的技能树
    // 基础剑术 -> 火焰剑 -> 高级火焰剑
    // 基础剑术 -> 冰霜剑 -> 高级冰霜剑
    // 火焰剑 + 冰霜剑 -> 元素大师
    int numCourses4 = 5;
    vector<vector<int>> prerequisites4 = {{1,0}, {2,1}, {3,0}, {4,2}, {4,3}};
    
    cout << "=== 测试4：RPG技能树场景 ===" << endl;
    cout << "课程数: " << numCourses4 << endl;
    cout << "0=基础剑术, 1=火焰剑, 2=冰霜剑, 3=高级火焰剑, 4=元素大师" << endl;
    cout << "依赖: 1->0, 2->1, 3->0, 4->2, 4->3" << endl;
    cout << "结果: " << (sol.canFinish(numCourses4, prerequisites4) ? "可以完成" : "无法完成") << endl;
    
    return 0;
}
