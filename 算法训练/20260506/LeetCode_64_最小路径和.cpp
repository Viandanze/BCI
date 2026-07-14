/**
 * LeetCode 64 - 最小路径和
 * 
 * 题目描述：
 * 给定一个 m x n 的网格 grid，网格中的每个单元格包含一个非负费用。
 * 找到一条从左上角到右下角的路径，使得路径上所有单元格费用之和最小。
 * 每次只能向下或向右移动。
 * 
 * 游戏开发场景：
 * - 游戏中角色从起点到终点的最短路径规划
 * - RTS游戏中的单位移动路径选择
 * - 自动寻路系统（A*的简化版）
 * 
 * 解法：动态规划
 * 状态定义：dp[i][j] 表示从起点到网格(i,j)位置的最小费用
 * 状态转移：dp[i][j] = grid[i][j] + min(dp[i-1][j], dp[i][j-1])
 * 边界条件：dp[0][0] = grid[0][0]
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <chrono>

using namespace std;

/**
 * Solution 1: 基础动态规划 - O(m*n) 空间
 * 
 * 思路：
 * - dp[i][j] = 从(0,0)到(i,j)的最小费用
 * - 每个位置只能从上方或左方到达
 * - 取两者的较小值加上当前费用
 */
int minPathSum_Basic(vector<vector<int>>& grid) {
    if (grid.empty() || grid[0].empty()) return 0;
    
    int m = grid.size();
    int n = grid[0].size();
    
    // dp[i][j] 表示到达 (i,j) 的最小路径和
    vector<vector<int>> dp(m, vector<int>(n, 0));
    
    // 初始化起点
    dp[0][0] = grid[0][0];
    
    // 初始化第一行（只能从左边到达）
    for (int j = 1; j < n; j++) {
        dp[0][j] = dp[0][j-1] + grid[0][j];
    }
    
    // 初始化第一列（只能从上边到达）
    for (int i = 1; i < m; i++) {
        dp[i][0] = dp[i-1][0] + grid[i][0];
    }
    
    // 填表：每个位置取上方或左方的最小值
    for (int i = 1; i < m; i++) {
        for (int j = 1; j < n; j++) {
            dp[i][j] = grid[i][j] + min(dp[i-1][j], dp[i][j-1]);
        }
    }
    
    return dp[m-1][n-1];
}

/**
 * Solution 2: 空间优化 - O(n) 空间
 * 
 * 思路：
 * - 由于 dp[i][j] 只依赖 dp[i-1][j] 和 dp[i][j-1]
 * - 可以只使用一维数组，通过从左到右遍历来避免覆盖
 * - dp[j] 在更新前保存的是上一行的值 dp[i-1][j]
 * - dp[j-1] 在更新后是当前行的值 dp[i][j-1]
 */
int minPathSum_Optimized(vector<vector<int>>& grid) {
    if (grid.empty() || grid[0].empty()) return 0;
    
    int m = grid.size();
    int n = grid[0].size();
    
    // 一维 dp 数组
    vector<int> dp(n, INT_MAX);
    dp[0] = 0;  // 第一列的累加起点
    
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            // 更新 dp[j]：
            // - 更新前 dp[j] = dp[i-1][j]（上一行的值）
            // - dp[j-1] = dp[i][j-1]（当前行左边的值，已更新）
            if (j == 0) {
                dp[j] = dp[j] + grid[i][j];  // 第一列，只能从上方来
            } else {
                dp[j] = grid[i][j] + min(dp[j], dp[j-1]);
            }
        }
    }
    
    return dp[n-1];
}

/**
 * Solution 3: 原地修改 - O(1) 空间
 * 
 * 思路：
 * - 直接在 grid 上修改，不使用额外空间
 * - grid[i][j] 变成到达该位置的最小费用
 */
int minPathSum_InPlace(vector<vector<int>>& grid) {
    if (grid.empty() || grid[0].empty()) return 0;
    
    int m = grid.size();
    int n = grid[0].size();
    
    // 第一行累加
    for (int j = 1; j < n; j++) {
        grid[0][j] += grid[0][j-1];
    }
    
    // 第一列累加
    for (int i = 1; i < m; i++) {
        grid[i][0] += grid[i-1][0];
    }
    
    // 其余位置取最小值累加
    for (int i = 1; i < m; i++) {
        for (int j = 1; j < n; j++) {
            grid[i][j] += min(grid[i-1][j], grid[i][j-1]);
        }
    }
    
    return grid[m-1][n-1];
}

/**
 * Solution 4: 回溯法（暴力枚举）- O(2^(m+n)) 时间，仅作对比
 * 
 * 递归树：每个位置可以选择向下或向右
 * 指数级时间复杂度，实际不可用，仅用于理解问题本质
 */
int minPathSum_Backtrack(vector<vector<int>>& grid, int i, int j) {
    if (i == grid.size() - 1 && j == grid[0].size() - 1) {
        return grid[i][j];  // 到达终点
    }
    
    if (i >= grid.size() || j >= grid[0].size()) {
        return INT_MAX;  // 越界
    }
    
    return grid[i][j] + min(
        minPathSum_Backtrack(grid, i + 1, j),  // 向下
        minPathSum_Backtrack(grid, i, j + 1)   // 向右
    );
}

// ==================== 测试代码 ====================

void printGrid(const vector<vector<int>>& grid) {
    cout << "Grid [" << grid.size() << "x" << grid[0].size() << "]:" << endl;
    for (const auto& row : grid) {
        cout << "  [";
        for (int i = 0; i < row.size(); i++) {
            cout << row[i];
            if (i < row.size() - 1) cout << ", ";
        }
        cout << "]" << endl;
    }
}

void testCase(const vector<vector<int>>& grid) {
    cout << "\n========================================" << endl;
    printGrid(grid);
    
    // 复制网格用于不同算法测试
    vector<vector<int>> grid1 = grid;
    vector<vector<int>> grid2 = grid;
    vector<vector<int>> grid3 = grid;
    
    cout << "\n算法对比测试:" << endl;
    cout << "----------------------------------------" << endl;
    
    // Solution 1
    auto start1 = chrono::high_resolution_clock::now();
    int result1 = minPathSum_Basic(grid1);
    auto end1 = chrono::high_resolution_clock::now();
    cout << "Solution 1 (基础DP):      最小路径和 = " << result1;
    cout << " | 耗时: " << chrono::duration_cast<chrono::microseconds>(end1 - start1).count() << " us" << endl;
    
    // Solution 2
    auto start2 = chrono::high_resolution_clock::now();
    int result2 = minPathSum_Optimized(grid2);
    auto end2 = chrono::high_resolution_clock::now();
    cout << "Solution 2 (空间优化):   最小路径和 = " << result2;
    cout << " | 耗时: " << chrono::duration_cast<chrono::microseconds>(end2 - start2).count() << " us" << endl;
    
    // Solution 3
    auto start3 = chrono::high_resolution_clock::now();
    int result3 = minPathSum_InPlace(grid3);
    auto end3 = chrono::high_resolution_clock::now();
    cout << "Solution 3 (原地修改):    最小路径和 = " << result3;
    cout << " | 耗时: " << chrono::duration_cast<chrono::microseconds>(end3 - start3).count() << " us" << endl;
}

int main() {
    cout << "===== LeetCode 64: 最小路径和 =====" << endl;
    cout << "游戏开发中的寻路算法训练" << endl;
    
    // 测试用例 1：标准网格
    vector<vector<int>> grid1 = {
        {1, 3, 1},
        {1, 5, 1},
        {4, 2, 1}
    };
    testCase(grid1);
    
    // 测试用例 2：游戏地图场景（0表示空地，数字表示代价）
    // 模拟：S=起点, E=终点, 数字=移动代价
    vector<vector<int>> gameMap = {
        {2, 2, 3, 1, 5},  // 第一行：起点(0,0)到(0,4)
        {1, 5, 2, 4, 2},  // 第二行
        {3, 1, 4, 2, 3},  // 第三行
        {2, 4, 1, 3, 2}   // 第四行：终点(3,4)
    };
    testCase(gameMap);
    
    // 测试用例 3：最小网格
    vector<vector<int>> grid3 = {
        {1, 2},
        {1, 1}
    };
    testCase(grid3);
    
    // 复杂度分析总结
    cout << "\n========================================" << endl;
    cout << "复杂度分析总结:" << endl;
    cout << "----------------------------------------" << endl;
    cout << "| 方案    | 时间复杂度 | 空间复杂度 | 适用场景         |" << endl;
    cout << "|----------|------------|------------|------------------|" << endl;
    cout << "| 基础DP   | O(m*n)     | O(m*n)     | 教学/理解        |" << endl;
    cout << "| 空间优化 | O(m*n)     | O(n)       | 生产环境推荐     |" << endl;
    cout << "| 原地修改 | O(m*n)     | O(1)       | 内存敏感场景     |" << endl;
    cout << "| 回溯暴力 | O(2^(m+n)) | O(m+n)     | 仅作理论对比     |" << endl;
    cout << "----------------------------------------" << endl;
    cout << "\n游戏开发应用场景:" << endl;
    cout << "1. RTS单位移动路径规划" << endl;
    cout << "2. 回合制游戏的移动代价计算" << endl;
    cout << "3. 地牢探索的最优路径" << endl;
    cout << "4. 资源采集的最短路线" << endl;
    
    return 0;
}
