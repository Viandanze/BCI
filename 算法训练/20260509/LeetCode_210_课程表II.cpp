/**
 * LeetCode 210 - 课程表 II (Course Schedule II)
 * 
 * 题目描述：
 * 现在你总共有 numCourses 门课需要选，记为 0 到 numCourses-1。
 * 给你一个数组 prerequisites，其中 prerequisites[i] = [ai, bi]，
 * 表示在选修课程 ai 前必须先选修课程 bi。
 * 返回你为了学完所有课程所安排的学习顺序。如果不可能完成，返回空数组。
 * 
 * 游戏场景类比：
 * - 技能树解锁顺序（前置技能 → 后续技能）
 * - 任务/关卡解锁序列（主线任务依赖链）
 * - 合成配方依赖图（材料 → 中间产物 → 最终装备）
 * - 游戏资源加载顺序（引擎Asset依赖链）
 * - 成就系统前置条件（解锁顺序规划）
 */

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

// ============================================================
// 解法一：BFS 拓扑排序（Kahn 算法）—— 面试首选
// ============================================================
class SolutionBFS {
public:
    vector<int> findOrder(int numCourses, vector<vector<int>>& prerequisites) {
        // 构建邻接表和入度数组
        vector<vector<int>> graph(numCourses);
        vector<int> inDegree(numCourses, 0);
        
        for (const auto& pre : prerequisites) {
            int course = pre[0];      // 要修的课
            int prereq = pre[1];      // 前置课
            graph[prereq].push_back(course);  // prereq → course
            inDegree[course]++;
        }
        
        // 所有入度为 0 的课程入队（没有前置要求，可以立即修）
        queue<int> q;
        for (int i = 0; i < numCourses; ++i) {
            if (inDegree[i] == 0) {
                q.push(i);
            }
        }
        
        vector<int> order;  // 拓扑排序结果
        
        while (!q.empty()) {
            int curr = q.front();
            q.pop();
            order.push_back(curr);
            
            // "修完"当前课程，所有后续课程的入度 -1
            for (int next : graph[curr]) {
                inDegree[next]--;
                if (inDegree[next] == 0) {
                    q.push(next);  // 入度为 0，可以修了
                }
            }
        }
        
        // 如果所有课程都排好了，返回顺序；否则存在环，返回空数组
        if (order.size() == numCourses) {
            return order;
        }
        return {};
    }
};

// ============================================================
// 解法二：DFS 拓扑排序 —— 逆后序即为拓扑序
// ============================================================
class SolutionDFS {
public:
    vector<int> findOrder(int numCourses, vector<vector<int>>& prerequisites) {
        // 重置成员变量（支持同一实例多次调用）
        graph.assign(numCourses, vector<int>());
        state.assign(numCourses, 0);
        order.clear();
        
        // 构建邻接表
        for (const auto& pre : prerequisites) {
            graph[pre[1]].push_back(pre[0]);
        }
        
        // 对每个未访问节点执行 DFS
        for (int i = 0; i < numCourses; ++i) {
            if (state[i] == 0) {
                if (!dfs(i)) {
                    return {};  // 检测到环，无法完成
                }
            }
        }
        
        // 逆后序就是拓扑排序
        reverse(order.begin(), order.end());
        return order;
    }
    
private:
    vector<vector<int>> graph;
    vector<int> state;
    vector<int> order;
    
    bool dfs(int node) {
        state[node] = 1;  // 标记为访问中
        
        for (int next : graph[node]) {
            if (state[next] == 1) {
                // 遇到访问中的节点 → 存在环（反向边）
                return false;
            }
            if (state[next] == 0) {
                if (!dfs(next)) {
                    return false;
                }
            }
            // state[next] == 2 → 已完成，跳过
        }
        
        state[node] = 2;      // 标记为已完成
        order.push_back(node); // 后序位置记录
        return true;
    }
};

// ============================================================
// 解法三：DFS 拓扑排序（迭代版，避免栈溢出）
// ============================================================
class SolutionDFSIter {
public:
    vector<int> findOrder(int numCourses, vector<vector<int>>& prerequisites) {
        vector<vector<int>> graph(numCourses);
        vector<int> inDegree(numCourses, 0);
        
        for (const auto& pre : prerequisites) {
            graph[pre[1]].push_back(pre[0]);
            inDegree[pre[0]]++;
        }
        
        // 迭代 DFS：手动维护栈和状态
        // state: 0=未访问, 1=访问中, 2=已完成
        vector<int> state(numCourses, 0);
        vector<int> order;
        
        for (int start = 0; start < numCourses; ++start) {
            if (state[start] != 0) continue;
            
            // 手动 DFS 栈，存 (节点, 邻居迭代索引)
            vector<pair<int, int>> stk;
            stk.push_back({start, 0});
            state[start] = 1;
            
            while (!stk.empty()) {
                auto& [node, idx] = stk.back();
                
                if (idx < (int)graph[node].size()) {
                    int next = graph[node][idx];
                    idx++;
                    
                    if (state[next] == 1) {
                        return {};  // 检测到环
                    }
                    if (state[next] == 0) {
                        state[next] = 1;
                        stk.push_back({next, 0});
                    }
                } else {
                    // 所有邻居处理完毕
                    state[node] = 2;
                    order.push_back(node);
                    stk.pop_back();
                }
            }
        }
        
        reverse(order.begin(), order.end());
        return order;
    }
};

// ============================================================
// 辅助函数：打印 vector
// ============================================================
void printResult(const string& label, const vector<int>& result, const vector<int>& expected) {
    bool pass = (result == expected);
    cout << label << ": [";
    for (int i = 0; i < (int)result.size(); ++i) {
        cout << result[i] << (i < (int)result.size() - 1 ? ", " : "");
    }
    cout << "] " << (pass ? "✓" : "✗ 期望 [") ;
    if (!pass) {
        for (int i = 0; i < (int)expected.size(); ++i) {
            cout << expected[i] << (i < (int)expected.size() - 1 ? ", " : "");
        }
        cout << "]";
    }
    cout << endl;
}

// 判断结果是否为合法拓扑序（可能有多解）
bool isValidTopoOrder(const vector<int>& order, int numCourses,
                      const vector<vector<int>>& prerequisites) {
    if ((int)order.size() != numCourses) return false;
    
    vector<int> pos(numCourses);
    for (int i = 0; i < numCourses; ++i) {
        pos[order[i]] = i;
    }
    
    for (const auto& pre : prerequisites) {
        if (pos[pre[1]] >= pos[pre[0]]) {
            return false;  // 前置课排在后面，不合法
        }
    }
    return true;
}

// ============================================================
// 测试代码
// ============================================================
int main() {
    SolutionBFS solBFS;
    SolutionDFS solDFS;
    SolutionDFSIter solIter;
    
    // 测试用例1：标准依赖图
    // 0 → 1 → 2, 0 → 2
    // 合法拓扑序可能是 [0,1,2] 或 [0,1,2]
    {
        int n = 4;
        vector<vector<int>> prereq = {{1,0},{2,0},{3,1},{3,2}};
        
        cout << "=== 测试用例1: 4门课, 依赖=[[1,0],[2,0],[3,1],[3,2]] ===" << endl;
        auto r1 = solBFS.findOrder(n, prereq);
        auto r2 = solDFS.findOrder(n, prereq);
        auto r3 = solIter.findOrder(n, prereq);
        
        cout << "BFS拓扑排序:   ["; for(int i=0;i<(int)r1.size();++i) cout<<r1[i]<<(i<(int)r1.size()-1?",":""); cout << "] ";
        cout << (isValidTopoOrder(r1, n, prereq) ? "✓" : "✗") << endl;
        
        cout << "DFS拓扑排序:   ["; for(int i=0;i<(int)r2.size();++i) cout<<r2[i]<<(i<(int)r2.size()-1?",":""); cout << "] ";
        cout << (isValidTopoOrder(r2, n, prereq) ? "✓" : "✗") << endl;
        
        cout << "DFS迭代排序:   ["; for(int i=0;i<(int)r3.size();++i) cout<<r3[i]<<(i<(int)r3.size()-1?",":""); cout << "] ";
        cout << (isValidTopoOrder(r3, n, prereq) ? "✓" : "✗") << endl;
    }
    
    // 测试用例2：无依赖，任意顺序
    {
        int n = 2;
        vector<vector<int>> prereq;
        
        cout << "\n=== 测试用例2: 2门课, 无依赖 ===" << endl;
        auto r1 = solBFS.findOrder(n, prereq);
        auto r2 = solDFS.findOrder(n, prereq);
        auto r3 = solIter.findOrder(n, prereq);
        
        cout << "BFS拓扑排序:   ["; for(int i=0;i<(int)r1.size();++i) cout<<r1[i]<<(i<(int)r1.size()-1?",":""); cout << "] ";
        cout << (isValidTopoOrder(r1, n, prereq) ? "✓" : "✗") << endl;
        
        cout << "DFS拓扑排序:   ["; for(int i=0;i<(int)r2.size();++i) cout<<r2[i]<<(i<(int)r2.size()-1?",":""); cout << "] ";
        cout << (isValidTopoOrder(r2, n, prereq) ? "✓" : "✗") << endl;
        
        cout << "DFS迭代排序:   ["; for(int i=0;i<(int)r3.size();++i) cout<<r3[i]<<(i<(int)r3.size()-1?",":""); cout << "] ";
        cout << (isValidTopoOrder(r3, n, prereq) ? "✓" : "✗") << endl;
    }
    
    // 测试用例3：存在环，无法完成
    {
        int n = 2;
        vector<vector<int>> prereq = {{0,1},{1,0}};  // 0→1→0 环
        
        cout << "\n=== 测试用例3: 2门课, 循环依赖(环) ===" << endl;
        auto r1 = solBFS.findOrder(n, prereq);
        auto r2 = solDFS.findOrder(n, prereq);
        auto r3 = solIter.findOrder(n, prereq);
        
        cout << "BFS拓扑排序:   " << (r1.empty() ? "[] ✓(检测到环)" : "✗") << endl;
        cout << "DFS拓扑排序:   " << (r2.empty() ? "[] ✓(检测到环)" : "✗") << endl;
        cout << "DFS迭代排序:   " << (r3.empty() ? "[] ✓(检测到环)" : "✗") << endl;
    }
    
    // 测试用例4：链式依赖 3→2→1→0
    {
        int n = 4;
        vector<vector<int>> prereq = {{1,0},{2,1},{3,2}};
        
        cout << "\n=== 测试用例4: 4门课, 链式依赖 ===" << endl;
        auto r1 = solBFS.findOrder(n, prereq);
        auto r2 = solDFS.findOrder(n, prereq);
        auto r3 = solIter.findOrder(n, prereq);
        
        vector<int> expected = {0,1,2,3};  // 唯一合法顺序
        
        cout << "BFS拓扑排序:   ["; for(int i=0;i<(int)r1.size();++i) cout<<r1[i]<<(i<(int)r1.size()-1?",":""); cout << "] ";
        cout << (r1 == expected ? "✓" : "(合法: " + string(isValidTopoOrder(r1,n,prereq)?"yes":"no") + ")") << endl;
        
        cout << "DFS拓扑排序:   ["; for(int i=0;i<(int)r2.size();++i) cout<<r2[i]<<(i<(int)r2.size()-1?",":""); cout << "] ";
        cout << (r2 == expected ? "✓" : "(合法: " + string(isValidTopoOrder(r2,n,prereq)?"yes":"no") + ")") << endl;
        
        cout << "DFS迭代排序:   ["; for(int i=0;i<(int)r3.size();++i) cout<<r3[i]<<(i<(int)r3.size()-1?",":""); cout << "] ";
        cout << (r3 == expected ? "✓" : "(合法: " + string(isValidTopoOrder(r3,n,prereq)?"yes":"no") + ")") << endl;
    }
    
    // 测试用例5：复杂DAG（菱形+尾巴）
    {
        int n = 6;
        vector<vector<int>> prereq = {{1,0},{2,0},{3,1},{3,2},{4,3},{5,4}};
        
        cout << "\n=== 测试用例5: 6门课, 菱形+尾巴 ===" << endl;
        auto r1 = solBFS.findOrder(n, prereq);
        auto r2 = solDFS.findOrder(n, prereq);
        auto r3 = solIter.findOrder(n, prereq);
        
        cout << "BFS拓扑排序:   ["; for(int i=0;i<(int)r1.size();++i) cout<<r1[i]<<(i<(int)r1.size()-1?",":""); cout << "] ";
        cout << (isValidTopoOrder(r1, n, prereq) ? "✓" : "✗") << endl;
        
        cout << "DFS拓扑排序:   ["; for(int i=0;i<(int)r2.size();++i) cout<<r2[i]<<(i<(int)r2.size()-1?",":""); cout << "] ";
        cout << (isValidTopoOrder(r2, n, prereq) ? "✓" : "✗") << endl;
        
        cout << "DFS迭代排序:   ["; for(int i=0;i<(int)r3.size();++i) cout<<r3[i]<<(i<(int)r3.size()-1?",":""); cout << "] ";
        cout << (isValidTopoOrder(r3, n, prereq) ? "✓" : "✗") << endl;
    }
    
    return 0;
}
