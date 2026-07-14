/**
 * LeetCode 62. 不同路径 (Unique Paths)
 * 
 * 游戏开发场景：角色在网格地图中从起点移动到终点，
 * 只能向右或向下移动，求所有可能的路径数量。
 * 
 * 适用场景：路径规划游戏、棋盘类游戏、AI寻路（简化版）
 */

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * 解法一：二维动态规划
 * 时间复杂度：O(m*n)
 * 空间复杂度：O(m*n)
 */
int uniquePaths_2D(int m, int n) {
    vector<vector<int>> dp(m, vector<int>(n, 1));
    
    for (int i = 1; i < m; i++) {
        for (int j = 1; j < n; j++) {
            dp[i][j] = dp[i-1][j] + dp[i][j-1];
        }
    }
    
    return dp[m-1][n-1];
}

/**
 * 解法二：一维动态规划（空间优化）
 * 思路：只保留当前行的状态，因为 dp[j] 依赖 dp[j-1]（当前行）和 dp[j]（上一行）
 * 
 * 时间复杂度：O(m*n)
 * 空间复杂度：O(n) - 只使用一维数组
 */
int uniquePaths_1D(int m, int n) {
    vector<int> dp(n, 1);  // 初始化为1，第一行只有一种走法
    
    for (int i = 1; i < m; i++) {
        for (int j = 1; j < n; j++) {
            dp[j] += dp[j-1];  // dp[j]是上一行的值，dp[j-1]是当前行左边的值
        }
    }
    
    return dp[n-1];
}

/**
 * 解法三：数学组合公式（最优解）
 * 思路：从(m+n-2)步中选(m-1)步向下（或n-1步向右）
 * C(m+n-2, m-1) = (m+n-2)! / ((m-1)! * (n-1)!)
 * 
 * 时间复杂度：O(min(m, n))
 * 空间复杂度：O(1)
 */
long long uniquePaths_Math(int m, int n) {
    long long result = 1;
    int k = min(m, n) - 1;  // 取较小值以减少计算量
    int total = m + n - 2;
    
    // C(total, k) = total! / (k! * (total-k)!)
    // 优化：连乘形式避免大数溢出
    for (int i = 1; i <= k; i++) {
        result = result * (total - k + i) / i;
    }
    
    return result;
}

/**
 * 【扩展】带障碍物的版本（LeetCode 63）
 * 障碍物位置标记为1，无法通过
 */
int uniquePathsWithObstacles(vector<vector<int>>& obstacleGrid) {
    int m = obstacleGrid.size();
    int n = obstacleGrid[0].size();
    
    // 如果起点或终点有障碍，直接返回0
    if (obstacleGrid[0][0] == 1 || obstacleGrid[m-1][n-1] == 1) {
        return 0;
    }
    
    vector<int> dp(n, 0);
    dp[0] = 1;  // 起点可达
    
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (obstacleGrid[i][j] == 1) {
                dp[j] = 0;  // 障碍物，路径数置0
            } else if (j > 0) {
                dp[j] += dp[j-1];  // 从左边到达
            }
            // dp[j]保持原值表示从上边到达
        }
    }
    
    return dp[n-1];
}

// ==================== 测试代码 ====================
int main() {
    int m, n;
    
    cout << "=== LeetCode 62. 不同路径 ===" << endl;
    cout << "请输入网格大小 (m n): ";
    cin >> m >> n;
    
    cout << "\n【测试用例】" << endl;
    cout << "网格大小: " << m << " x " << n << endl;
    
    cout << "\n【解法对比】" << endl;
    
    // 解法一：二维DP
    cout << "解法一（二维DP）: " << uniquePaths_2D(m, n) << endl;
    
    // 解法二：一维DP
    cout << "解法二（一维DP）: " << uniquePaths_1D(m, n) << endl;
    
    // 解法三：数学公式
    cout << "解法三（数学公式）: " << uniquePaths_Math(m, n) << endl;
    
    // 验证结果一致性
    if (uniquePaths_2D(m, n) == uniquePaths_1D(m, n) && 
        uniquePaths_1D(m, n) == uniquePaths_Math(m, n)) {
        cout << "\n✓ 三种解法结果一致！" << endl;
    }
    
    // 复杂度分析输出
    cout << "\n【复杂度分析】" << endl;
    cout << "解法一（二维DP）: 时间 O(m*n), 空间 O(m*n)" << endl;
    cout << "解法二（一维DP）: 时间 O(m*n), 空间 O(n)" << endl;
    cout << "解法三（数学公式）: 时间 O(min(m,n)), 空间 O(1)" << endl;
    
    // 测试带障碍物版本
    cout << "\n【扩展测试：带障碍物版本】" << endl;
    vector<vector<int>> grid = {
        {0, 0, 0},
        {0, 1, 0},
        {0, 0, 0}
    };
    cout << "障碍物网格 (3x3): " << uniquePathsWithObstacles(grid) << " 条路径" << endl;
    
    // 额外测试用例
    cout << "\n【LeetCode标准测试用例】" << endl;
    cout << "m=3, n=7: " << uniquePaths_Math(3, 7) << endl;  // 答案: 28
    cout << "m=3, n=2: " << uniquePaths_Math(3, 2) << endl;  // 答案: 3
    cout << "m=7, n=3: " << uniquePaths_Math(7, 3) << endl;  // 答案: 28
    cout << "m=3, n=3: " << uniquePaths_Math(3, 3) << endl;  // 答案: 6
    
    return 0;
}
