/**
 * LeetCode 127. 单词接龙 (Word Ladder)
 * 
 * 题目链接：https://leetcode.cn/problems/word-ladder/
 * 
 * 【游戏开发相关】路径搜索与状态转移
 * 
 * 在游戏AI中，类似的BFS最短路径问题常用于：
 * - NPC寻路导航
 * - 游戏内物品转换链
 * - 技能树解锁路径
 * - 状态机状态转移
 */

#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <queue>
using namespace std;

/**
 * 解法一：广度优先搜索（BFS）- 标准版本
 * 
 * 思路：每次变换一个字母，查找所有可能的转换，记录层数（转换次数）
 * 直到找到目标单词，即为最短转换序列
 * 
 * 时间复杂度：O(N * L * 26)，N为单词数量，L为单词长度
 * 空间复杂度：O(N * L)，存储所有单词的哈希集合
 */
class Solution_BFS {
public:
    int ladderLength(string beginWord, string endWord, vector<string>& wordList) {
        unordered_set<string> wordSet(wordList.begin(), wordList.end());
        
        // 边界检查
        if (wordSet.find(endWord) == wordSet.end()) {
            return 0;
        }
        
        // BFS队列：(当前单词, 转换次数)
        queue<pair<string, int>> q;
        q.push({beginWord, 1});
        wordSet.erase(beginWord);  // 避免重复访问
        
        while (!q.empty()) {
            auto [currentWord, depth] = q.front();
            q.pop();
            
            // 遍历当前单词的每一个字符位置
            for (int i = 0; i < currentWord.size(); i++) {
                string nextWord = currentWord;
                
                // 尝试将每个位置替换为26个字母
                for (char c = 'a'; c <= 'z'; c++) {
                    if (c == currentWord[i]) continue;
                    
                    nextWord[i] = c;
                    
                    // 找到目标单词
                    if (nextWord == endWord) {
                        return depth + 1;
                    }
                    
                    // 如果在单词表中，添加到队列
                    if (wordSet.find(nextWord) != wordSet.end()) {
                        q.push({nextWord, depth + 1});
                        wordSet.erase(nextWord);  // 剪枝：避免重复
                    }
                }
            }
        }
        
        return 0;  // 无法转换
    }
};

/**
 * 解法二：BFS + 双向搜索（优化版本）
 * 
 * 思路：从起点和终点同时进行BFS，双向收敛
 * 适用于单词表很大的情况，减少搜索空间
 * 
 * 时间复杂度：O(N * L * 26 / 2) = O(N * L * 26)
 * 空间复杂度：O(N * L)
 */
class Solution_Bidirectional {
public:
    int ladderLength(string beginWord, string endWord, vector<string>& wordList) {
        unordered_set<string> wordSet(wordList.begin(), wordList.end());
        if (wordSet.find(endWord) == wordSet.end()) {
            return 0;
        }
        
        unordered_set<string> beginSet = {beginWord};
        unordered_set<string> endSet = {endWord};
        
        int depth = 1;
        
        while (!beginSet.empty() && !endSet.empty()) {
            // 确保从较小的集合开始扩展
            if (beginSet.size() > endSet.size()) {
                swap(beginSet, endSet);
            }
            
            unordered_set<string> tempSet;
            
            for (const string& word : beginSet) {
                string candidate = word;
                for (int i = 0; i < word.size(); i++) {
                    char original = word[i];
                    for (char c = 'a'; c <= 'z'; c++) {
                        if (c == original) continue;
                        
                        candidate[i] = c;
                        
                        // 找到了交集，直接返回
                        if (endSet.find(candidate) != endSet.end()) {
                            return depth + 1;
                        }
                        
                        if (wordSet.find(candidate) != wordSet.end()) {
                            tempSet.insert(candidate);
                            wordSet.erase(candidate);
                        }
                    }
                    candidate[i] = original;
                }
            }
            
            beginSet = tempSet;
            depth++;
        }
        
        return 0;
    }
};

/**
 * 【游戏开发扩展】实际应用示例
 * 
 * 场景：游戏内技能树解锁系统
 * - 每个技能代表一个单词
 * - 技能之间通过消耗资源可以互相转换
 * - 求从当前技能到目标技能的最短路径
 */
class SkillTreeUnlocked {
public:
    // 游戏技能转换图（简化模型）
    struct SkillNode {
        string skillName;
        int unlockCost;
    };
    
    // BFS寻路算法在游戏AI中的应用
    vector<string> findShortestSkillPath(
        const string& currentSkill,
        const string& targetSkill,
        const vector<string>& availableSkills
    ) {
        Solution_BFS solver;
        vector<string> wordList = availableSkills;
        
        int steps = solver.ladderLength(currentSkill, targetSkill, wordList);
        
        if (steps == 0) {
            return {};  // 无法解锁
        }
        
        // 返回转换步骤数
        vector<string> result;
        result.push_back(currentSkill);
        result.push_back("->");
        result.push_back(to_string(steps));
        result.push_back("steps to unlock");
        
        return result;
    }
};

int main() {
    cout << "========== LeetCode 127. 单词接龙 ==========" << endl;
    cout << endl;
    
    // 测试用例
    vector<string> wordList = {"hot", "dot", "dog", "lot", "log", "cog"};
    string beginWord = "hit";
    string endWord = "cog";
    
    cout << "测试用例：" << endl;
    cout << "  beginWord: " << beginWord << endl;
    cout << "  endWord: " << endWord << endl;
    cout << "  wordList: [hot, dot, dog, lot, log, cog]" << endl;
    cout << endl;
    
    Solution_BFS bfs;
    Solution_Bidirectional bidirectional;
    
    int result1 = bfs.ladderLength(beginWord, endWord, wordList);
    cout << "【BFS方法】最短转换长度: " << result1 << endl;
    
    int result2 = bidirectional.ladderLength(beginWord, endWord, wordList);
    cout << "【双向BFS】最短转换长度: " << result2 << endl;
    cout << endl;
    
    cout << "预期答案: 5 (hit -> hot -> dot -> dog -> cog)" << endl;
    cout << endl;
    
    cout << "========== 复杂度分析 ==========" << endl;
    cout << endl;
    cout << "【BFS方法】" << endl;
    cout << "  时间复杂度: O(N × L × 26)" << endl;
    cout << "    - N: 单词列表长度" << endl;
    cout << "    - L: 单词长度" << endl;
    cout << "    - 26: 每个位置可替换的字母数" << endl;
    cout << "  空间复杂度: O(N × L)" << endl;
    cout << "    - 哈希集合存储所有单词" << endl;
    cout << "    - BFS队列最多存储N个单词" << endl;
    cout << endl;
    
    cout << "【双向BFS方法】" << endl;
    cout << "  时间复杂度: O(N × L × 26 / 2) = O(N × L × 26)" << endl;
    cout << "    - 每次从较小集合扩展，实际搜索空间减半" << endl;
    cout << "  空间复杂度: O(N × L)" << endl;
    cout << "    - 两个哈希集合 + 一个临时集合" << endl;
    cout << endl;
    
    cout << "========== 游戏开发应用场景 ==========" << endl;
    cout << endl;
    cout << "1. NPC寻路导航 - BFS找最短路径" << endl;
    cout << "2. 游戏内物品合成路径" << endl;
    cout << "3. 技能树解锁最短路径" << endl;
    cout << "4. 地图传送点最短跳转序列" << endl;
    cout << "5. 状态机状态转移最优化" << endl;
    cout << endl;
    
    cout << "========== 优化方向 ==========" << endl;
    cout << endl;
    cout << "1. 预处理：预计算每个单词的通用状态（如'h*t'匹配'hot','hit'）" << endl;
    cout << "2. 记忆化：缓存已访问节点，避免重复计算" << endl;
    cout << "3. A*算法：使用启发式函数优化搜索方向" << endl;
    cout << "4. 分层BFS：记录每层节点数，快速定位目标层" << endl;
    
    return 0;
}
