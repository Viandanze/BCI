/**
 * LeetCode 79. Word Search
 * 单词搜索 - 经典DFS+回溯算法
 * 
 * 题目描述：
 * 给定一个 m x n 的字符网格 board 和一个字符串 word，判断 word 是否存在于网格中。
 * 路径规则：从某个格子开始，可以向上下左右四个方向移动
 * 每个格子只能使用一次
 * 
 * 游戏开发场景：地牢探索、迷宫寻找出口、地图路径匹配
 */

#include <iostream>
#include <vector>
#include <string>
using namespace std;

class Solution {
private:
    // 四个方向：上、下、左、右
    const int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    int m, n;  // 网格尺寸
    
    /**
     * DFS回溯函数
     * @param board 字符网格
     * @param word 要匹配的单词
     * @param i 当前所在行
     * @param j 当前所在列
     * @param idx 当前匹配到单词的第几个字符
     * @return 是否能匹配成功
     */
    bool dfs(vector<vector<char>>& board, const string& word, int i, int j, int idx) {
        // 递归终止条件：所有字符都匹配成功
        if (idx == word.size() - 1) {
            return board[i][j] == word[idx];
        }
        
        // 当前字符匹配，继续向四个方向探索
        if (board[i][j] == word[idx]) {
            char original = board[i][j];
            board[i][j] = '#';  // 标记为已访问（相当于visited数组）
            
            for (auto& dir : directions) {
                int newI = i + dir[0];
                int newJ = j + dir[1];
                
                // 检查边界条件
                if (newI >= 0 && newI < m && newJ >= 0 && newJ < n) {
                    if (dfs(board, word, newI, newJ, idx + 1)) {
                        return true;
                    }
                }
            }
            
            board[i][j] = original;  // 回溯：恢复原字符
        }
        
        return false;
    }
    
public:
    bool exist(vector<vector<char>>& board, string word) {
        if (board.empty() || board[0].empty() || word.empty()) {
            return false;
        }
        
        m = board.size();
        n = board[0].size();
        
        // 从网格中每个格子开始尝试匹配
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                if (dfs(board, word, i, j, 0)) {
                    return true;
                }
            }
        }
        
        return false;
    }
};

// ==================== 测试代码 ====================
int main() {
    Solution solution;
    
    // 测试用例1：经典网格
    vector<vector<char>> board1 = {
        {'A', 'B', 'C', 'E'},
        {'S', 'F', 'C', 'S'},
        {'A', 'D', 'E', 'E'}
    };
    
    cout << "=== 测试用例1 ===" << endl;
    cout << "网格:" << endl;
    for (auto& row : board1) {
        for (char c : row) cout << c << " ";
        cout << endl;
    }
    
    vector<string> words1 = {"ABCCED", "SEE", "ABCB"};
    for (const string& w : words1) {
        vector<vector<char>> boardCopy = board1;
        bool result = solution.exist(boardCopy, w);
        cout << "单词 \"" << w << "\": " << (result ? "存在" : "不存在") << endl;
    }
    
    cout << "\n=== 测试用例2：游戏地牢场景 ===" << endl;
    // 模拟游戏地牢地图
    vector<vector<char>> dungeon = {
        {'D', 'U', 'N', 'G', 'E'},
        {'X', 'T', 'R', 'E', 'A'},
        {'P', 'A', 'S', 'S', 'T'},
        {'K', 'E', 'Y', 'O', 'U'}
    };
    
    cout << "地牢地图:" << endl;
    for (auto& row : dungeon) {
        for (char c : row) cout << c << " ";
        cout << endl;
    }
    
    vector<string> quests = {"DUNGEON", "PASS", "KEY", "TREASURE"};
    for (const string& quest : quests) {
        vector<vector<char>> dungeonCopy = dungeon;
        bool result = solution.exist(dungeonCopy, quest);
        cout << "任务 \"" << quest << "\": " << (result ? "完成!" : "未找到") << endl;
    }
    
    cout << "\n=== 测试用例3：边界情况 ===" << endl;
    vector<vector<char>> single = {{'A'}};
    vector<vector<char>> singleCopy = single;
    cout << "单字符网格查找 'A': " << (solution.exist(singleCopy, "A") ? "存在" : "不存在") << endl;
    cout << "单字符网格查找 'B': " << (solution.exist(singleCopy, "B") ? "存在" : "不存在") << endl;
    
    return 0;
}
