/**
 * LeetCode 200 - 岛屿数量
 * 
 * 题目描述：
 * 给定一个由 '1'（陆地）和 '0'（水）组成的二维网格，计算网格中岛屿的数量。
 * 岛屿由相邻的陆地连接而成（水平或垂直方向），被水包围。
 * 假设网格四条边均被水包围。
 * 
 * 游戏场景类比：
 * - 地图区域划分（领土/势力范围检测）
 * - 程序化地形生成中的连通区域标记
 * - 关卡可通行区域检测
 * - 战争迷雾/迷雾解锁区域计算
 * - 资源节点聚簇识别（矿脉/宝箱群）
 */

#include <iostream>
#include <vector>
#include <numeric>
using namespace std;

// ============================================================
// 解法一：DFS（深度优先搜索）—— 沉岛法
// ============================================================
class SolutionDFS {
public:
    int numIslands(vector<vector<char>>& grid) {
        if (grid.empty() || grid[0].empty()) return 0;
        
        int rows = grid.size();
        int cols = grid[0].size();
        int count = 0;
        
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (grid[i][j] == '1') {
                    count++;               // 发现新岛屿
                    dfs(grid, i, j, rows, cols);  // 沉掉整个岛屿
                }
            }
        }
        
        return count;
    }
    
private:
    void dfs(vector<vector<char>>& grid, int x, int y, int rows, int cols) {
        // 越界或已经是水，直接返回
        if (x < 0 || x >= rows || y < 0 || y >= cols || grid[x][y] != '1') {
            return;
        }
        
        grid[x][y] = '0';  // 沉岛：标记为已访问（原地修改，省空间）
        
        // 四方向递归
        dfs(grid, x - 1, y, rows, cols);  // 上
        dfs(grid, x + 1, y, rows, cols);  // 下
        dfs(grid, x, y - 1, rows, cols);  // 左
        dfs(grid, x, y + 1, rows, cols);  // 右
    }
};

// ============================================================
// 解法二：BFS（广度优先搜索）—— 迭代实现
// ============================================================
#include <queue>

class SolutionBFS {
public:
    int numIslands(vector<vector<char>>& grid) {
        if (grid.empty() || grid[0].empty()) return 0;
        
        int rows = grid.size();
        int cols = grid[0].size();
        int count = 0;
        
        const int dx[4] = {-1, 1, 0, 0};
        const int dy[4] = {0, 0, -1, 1};
        
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (grid[i][j] == '1') {
                    count++;
                    // BFS 沉岛
                    queue<pair<int, int>> q;
                    q.push({i, j});
                    grid[i][j] = '0';
                    
                    while (!q.empty()) {
                        auto [cx, cy] = q.front();
                        q.pop();
                        
                        for (int d = 0; d < 4; ++d) {
                            int nx = cx + dx[d];
                            int ny = cy + dy[d];
                            if (nx >= 0 && nx < rows && ny >= 0 && ny < cols 
                                && grid[nx][ny] == '1') {
                                grid[nx][ny] = '0';
                                q.push({nx, ny});
                            }
                        }
                    }
                }
            }
        }
        
        return count;
    }
};

// ============================================================
// 解法三：并查集（Union-Find）—— 路径压缩 + 按秩合并
// ============================================================
class SolutionUnionFind {
public:
    int numIslands(vector<vector<char>>& grid) {
        if (grid.empty() || grid[0].empty()) return 0;
        
        int rows = grid.size();
        int cols = grid[0].size();
        
        // 并查集初始化
        parent.resize(rows * cols);
        rank.resize(rows * cols, 0);
        int landCount = 0;
        
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                int idx = i * cols + j;
                parent[idx] = idx;
                if (grid[i][j] == '1') landCount++;
            }
        }
        
        const int dx[2] = {1, 0};   // 只需向右和向下，避免重复合并
        const int dy[2] = {0, 1};
        
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (grid[i][j] != '1') continue;
                
                for (int d = 0; d < 2; ++d) {
                    int nx = i + dx[d];
                    int ny = j + dy[d];
                    if (nx < rows && ny < cols && grid[nx][ny] == '1') {
                        int root1 = find(i * cols + j);
                        int root2 = find(nx * cols + ny);
                        if (root1 != root2) {
                            unite(root1, root2);
                            landCount--;  // 合并一次，岛屿数减一
                        }
                    }
                }
            }
        }
        
        return landCount;
    }
    
private:
    vector<int> parent;
    vector<int> rank;
    
    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);  // 路径压缩
        }
        return parent[x];
    }
    
    void unite(int x, int y) {
        // 按秩合并
        if (rank[x] < rank[y]) {
            parent[x] = y;
        } else if (rank[x] > rank[y]) {
            parent[y] = x;
        } else {
            parent[y] = x;
            rank[x]++;
        }
    }
};

// ============================================================
// 测试代码
// ============================================================
int main() {
    // 测试用例1：标准3x3网格
    vector<vector<char>> grid1 = {
        {'1', '1', '0', '0', '0'},
        {'1', '1', '0', '0', '0'},
        {'0', '0', '1', '0', '0'},
        {'0', '0', '0', '1', '1'}
    };
    
    // 复制网格给不同解法使用（因为原地修改）
    auto g1_dfs = grid1;
    auto g1_bfs = grid1;
    auto g1_uf  = grid1;
    
    SolutionDFS solDFS;
    SolutionBFS solBFS;
    SolutionUnionFind solUF;
    
    cout << "=== 测试用例1 ===" << endl;
    cout << "DFS解法:      " << solDFS.numIslands(g1_dfs) << " (预期: 3)" << endl;
    cout << "BFS解法:      " << solBFS.numIslands(g1_bfs) << " (预期: 3)" << endl;
    cout << "Union-Find:   " << solUF.numIslands(g1_uf)   << " (预期: 3)" << endl;
    
    // 测试用例2：全部陆地
    vector<vector<char>> grid2 = {
        {'1', '1'},
        {'1', '1'}
    };
    auto g2_dfs = grid2, g2_bfs = grid2, g2_uf = grid2;
    
    cout << "\n=== 测试用例2（全部陆地）===" << endl;
    cout << "DFS解法:      " << solDFS.numIslands(g2_dfs) << " (预期: 1)" << endl;
    cout << "BFS解法:      " << solBFS.numIslands(g2_bfs) << " (预期: 1)" << endl;
    cout << "Union-Find:   " << solUF.numIslands(g2_uf)   << " (预期: 1)" << endl;
    
    // 测试用例3：全部水面
    vector<vector<char>> grid3 = {
        {'0', '0', '0'},
        {'0', '0', '0'}
    };
    auto g3_dfs = grid3, g3_bfs = grid3, g3_uf = grid3;
    
    cout << "\n=== 测试用例3（全部水面）===" << endl;
    cout << "DFS解法:      " << solDFS.numIslands(g3_dfs) << " (预期: 0)" << endl;
    cout << "BFS解法:      " << solBFS.numIslands(g3_bfs) << " (预期: 0)" << endl;
    cout << "Union-Find:   " << solUF.numIslands(g3_uf)   << " (预期: 0)" << endl;
    
    // 测试用例4：棋盘格交错
    vector<vector<char>> grid4 = {
        {'1', '0', '1'},
        {'0', '1', '0'},
        {'1', '0', '1'}
    };
    auto g4_dfs = grid4, g4_bfs = grid4, g4_uf = grid4;
    
    cout << "\n=== 测试用例4（棋盘格交错）===" << endl;
    cout << "DFS解法:      " << solDFS.numIslands(g4_dfs) << " (预期: 5)" << endl;
    cout << "BFS解法:      " << solBFS.numIslands(g4_bfs) << " (预期: 5)" << endl;
    cout << "Union-Find:   " << solUF.numIslands(g4_uf)   << " (预期: 5)" << endl;
    
    // 测试用例5：单格陆地
    vector<vector<char>> grid5 = {{'1'}};
    auto g5_dfs = grid5, g5_bfs = grid5, g5_uf = grid5;
    
    cout << "\n=== 测试用例5（单格陆地）===" << endl;
    cout << "DFS解法:      " << solDFS.numIslands(g5_dfs) << " (预期: 1)" << endl;
    cout << "BFS解法:      " << solBFS.numIslands(g5_bfs) << " (预期: 1)" << endl;
    cout << "Union-Find:   " << solUF.numIslands(g5_uf)   << " (预期: 1)" << endl;
    
    return 0;
}
