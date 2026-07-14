# LeetCode 62. 不同路径 (Unique Paths) - 解题报告

## 📌 题目信息

| 属性 | 值 |
|------|-----|
| 题目编号 | 62 |
| 题目名称 | 不同路径 (Unique Paths) |
| 难度 | 中等 (Medium) |
| 分类 | 动态规划 / 数学 |
| 算法标签 | DP、组合数学 |

---

## 🎮 游戏开发关联

这道题在游戏开发中的典型应用场景：

1. **角色寻路系统**：简化版网格地图寻路
2. **棋盘类游戏**：如跳棋、国际象棋的马步问题
3. **资源采集路线**：计算从基地到矿点的不同路线
4. **Roguelike游戏**：房间之间的随机生成路径

---

## 💡 解题思路

### 核心观察

机器人从左上角(0,0)移动到右下角(m-1,n-1)，每次只能**向右**或**向下**移动。

无论选择哪条路径，总步数 = (m-1)次向下 + (n-1)次向右 = m+n-2步。

**关键洞察**：路径的选择本质上是在 m+n-2 步中选择何时向下（或何时向右）。

### 动态规划解法

**状态定义**：`dp[i][j]` = 到达位置(i,j)的不同路径数

**转移方程**：
```
dp[i][j] = dp[i-1][j] + dp[i][j-1]
         ↑ 从上方到达  ↑ 从左方到达
```

**边界条件**：
- 第一行：`dp[0][j] = 1`（只能从左边到达）
- 第一列：`dp[i][0] = 1`（只能从上方到达）

### 数学组合解法

问题等价于：从 m+n-2 步中选取 m-1 步向下移动：

```
C(m+n-2, m-1) = (m+n-2)! / [(m-1)! * (n-1)!]
```

---

## 🔥 三种解法对比

### 解法一：二维动态规划

```cpp
int uniquePaths_2D(int m, int n) {
    vector<vector<int>> dp(m, vector<int>(n, 1));
    for (int i = 1; i < m; i++)
        for (int j = 1; j < n; j++)
            dp[i][j] = dp[i-1][j] + dp[i][j-1];
    return dp[m-1][n-1];
}
```

### 解法二：一维动态规划（空间优化）

```cpp
int uniquePaths_1D(int m, int n) {
    vector<int> dp(n, 1);
    for (int i = 1; i < m; i++)
        for (int j = 1; j < n; j++)
            dp[j] += dp[j-1];
    return dp[n-1];
}
```

### 解法三：数学组合公式（最优解）

```cpp
long long uniquePaths_Math(int m, int n) {
    long long result = 1;
    int k = min(m, n) - 1;
    int total = m + n - 2;
    for (int i = 1; i <= k; i++)
        result = result * (total - k + i) / i;
    return result;
}
```

---

## 📊 复杂度分析

| 解法 | 时间复杂度 | 空间复杂度 | 备注 |
|------|-----------|-----------|------|
| 二维DP | O(m × n) | O(m × n) | 最直观，容易理解 |
| 一维DP | O(m × n) | O(n) | 空间优化版本 |
| 数学公式 | O(min(m, n)) | O(1) | 最优解，面试推荐 |

### 复杂度推导

**动态规划**：
- 双层循环遍历整个 m×n 网格 → O(m×n)
- 存储完整二维表 → O(m×n)

**数学公式**：
- 只计算组合数，循环 k = min(m,n)-1 次 → O(min(m,n))
- 只用几个变量 → O(1)

---

## 🎯 扩展：带障碍物版本 (LeetCode 63)

```cpp
int uniquePathsWithObstacles(vector<vector<int>>& grid) {
    int m = grid.size(), n = grid[0].size();
    if (grid[0][0] == 1 || grid[m-1][n-1] == 1) return 0;
    
    vector<int> dp(n, 0);
    dp[0] = 1;
    
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (grid[i][j] == 1) dp[j] = 0;
            else if (j > 0) dp[j] += dp[j-1];
        }
    }
    return dp[n-1];
}
```

---

## ✅ 测试验证

```
测试用例：
m=3, n=7 → 28 条路径
m=3, n=2 → 3 条路径
m=7, n=3 → 28 条路径
m=3, n=3 → 6 条路径

带障碍物 [0,0,0; 0,1,0; 0,0,0] → 2 条路径
```

---

## 💼 面试要点总结

1. **必须掌握**：二维DP → 一维DP的空间优化
2. **加分项**：数学组合公式解法，O(1)空间
3. **扩展能力**：能否快速想到带障碍物版本的变体
4. **代码风格**：循环边界处理、边界条件检查

---

## 📚 相关题目

- [63. 不同路径 II](https://leetcode.com/problems/unique-paths-ii/) - 带障碍物
- [64. 最小路径和](https://leetcode.com/problems/minimum-path-sum/) - 求最小值
- [174. 地下城游戏](https://leetcode.com/problems/dungeon-game/) -  harder变体
- [120. 三角形最小路径和](https://leetcode.com/problems/triangle/) - 类似DP

---

*训练日期：2026-05-02*
*题目来源：LeetCode*
