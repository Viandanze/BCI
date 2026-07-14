/**
 * LeetCode 994 - 腐烂的橘子
 * 
 * 题目描述：
 * 在一个给定的 grid 中：
 * - 0 代表空单元格
 * - 1 代表新鲜橘子
 * - 2 代表腐烂橘子
 * 每分钟，所有腐烂的橘子都会让相邻（上、下、左、右）的新鲜橘子腐烂。
 * 求所有橘子腐烂所需的最少分钟数，如果无法全部腐烂则返回 -1。
 * 
 * 游戏场景类比：
 * - 火焰蔓延系统
 * - 病毒/僵尸感染扩散
 * - 毒圈收缩效果
 * - 范围伤害传播
 */

#include <iostream>
#include <vector>
#include <queue>
using namespace std;

struct Point {
    int x, y, time;
};

class Solution {
public:
    int orangesRotting(vector<vector<int>>& grid) {
        if (grid.empty() || grid[0].empty()) return 0;
        
        int rows = grid.size();
        int cols = grid[0].size();
        queue<Point> q;
        int freshCount = 0;
        
        // 初始化：找到所有腐烂橘子，加入队列；统计新鲜橘子数量
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (grid[i][j] == 2) {
                    q.push({i, j, 0});
                } else if (grid[i][j] == 1) {
                    freshCount++;
                }
            }
        }
        
        // 边界情况：没有新鲜橘子
        if (freshCount == 0) return 0;
        
        // 方向向量：上右下左
        const int dx[4] = {-1, 0, 1, 0};
        const int dy[4] = {0, 1, 0, -1};
        
        int maxTime = 0;
        
        // 多源BFS
        while (!q.empty()) {
            Point curr = q.front();
            q.pop();
            
            maxTime = curr.time;  // 记录当前层的时间
            
            // 遍历四个方向
            for (int dir = 0; dir < 4; ++dir) {
                int nx = curr.x + dx[dir];
                int ny = curr.y + dy[dir];
                
                // 检查边界
                if (nx < 0 || nx >= rows || ny < 0 || ny >= cols) continue;
                
                // 如果是新鲜橘子，腐烂它
                if (grid[nx][ny] == 1) {
                    grid[nx][ny] = 2;  // 标记为腐烂
                    freshCount--;      // 新鲜橘子数量减一
                    q.push({nx, ny, curr.time + 1});
                }
            }
        }
        
        // 如果还有新鲜橘子，说明无法全部腐烂
        return freshCount == 0 ? maxTime : -1;
    }
};

// ============ 测试代码 ============
int main() {
    Solution solution;
    
    // 测试用例1：标准情况
    vector<vector<int>> grid1 = {
        {2, 1, 1},
        {1, 1, 0},
        {0, 1, 1}
    };
    cout << "测试1: " << solution.orangesRotting(grid1) << endl;  // 预期: 4
    
    // 测试用例2：多个腐烂源快速感染
    vector<vector<int>> grid2 = {
        {2, 1, 1},
        {0, 1, 1},
        {0, 1, 2}
    };
    cout << "测试2: " << solution.orangesRotting(grid2) << endl;  // 预期: 3
    
    // 测试用例3：无法全部腐烂
    vector<vector<int>> grid3 = {
        {1, 2},
        {0, 1}
    };
    cout << "测试3: " << solution.orangesRotting(grid3) << endl;  // 预期: -1
    
    // 测试用例4：没有新鲜橘子
    vector<vector<int>> grid4 = {
        {0, 2},
        {2, 0}
    };
    cout << "测试4: " << solution.orangesRotting(grid4) << endl;  // 预期: 0
    
    // 测试用例5：全部是新鲜橘子
    vector<vector<int>> grid5 = {
        {1, 1},
        {1, 1}
    };
    cout << "测试5: " << solution.orangesRotting(grid5) << endl;  // 预期: -1
    
    return 0;
}
