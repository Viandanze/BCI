#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>
using namespace std;

/**
 * LeetCode 127 - 单词接龙
 * 
 * 题目描述：
 * 字典 wordList 中从 beginWord 和 endWord 的转换序列是一个按下述规格形成的序列：
 * - 序列中每个单词是 wordList 中的单词
 * - 序列中相邻的单词在原字典中只有一个字母不同
 * - 序列长度是最短的
 * 
 * 游戏开发场景：
 * - NPC寻路（A*、BFS变种）
 * - 地图区域可达性判断
 * - 资源收集最短路径
 */

/**
 * 解法1：BFS + 虚拟节点优化
 * 
 * 思路：
 * 1. 将所有单词按层构建图，连接只差一个字符的单词
 * 2. 从beginWord开始BFS，找到endWord即返回最短路径长度
 * 
 * 优化：使用哈希表存储所有单词，生成通配符模式进行跳转
 * 例如：hit -> *it, h*t, hi*
 */
class Solution {
public:
    int ladderLength(string beginWord, string endWord, vector<string>& wordList) {
        unordered_set<string> wordSet(wordList.begin(), wordList.end());
        
        // 边界检查：endWord必须在wordList中
        if (wordSet.find(endWord) == wordSet.end()) {
            return 0;
        }
        
        // BFS队列：(当前单词, 转换次数)
        queue<pair<string, int>> q;
        q.push({beginWord, 1});
        
        // 访问集合，防止重复
        unordered_set<string> visited;
        visited.insert(beginWord);
        
        int wordLen = beginWord.length();
        
        while (!q.empty()) {
            string currWord = q.front().first;
            int level = q.front().second;
            q.pop();
            
            // 生成所有可能的中间状态
            string temp = currWord;
            for (int i = 0; i < wordLen; i++) {
                char originalChar = currWord[i];
                
                // 尝试替换为a-z中的每个字符
                for (char c = 'a'; c <= 'z'; c++) {
                    if (c == originalChar) continue;
                    
                    temp[i] = c;
                    
                    // 找到目标单词
                    if (temp == endWord) {
                        return level + 1;
                    }
                    
                    // 在wordList中找到且未被访问
                    if (wordSet.find(temp) != wordSet.end() && 
                        visited.find(temp) == visited.end()) {
                        visited.insert(temp);
                        q.push({temp, level + 1});
                    }
                }
                temp[i] = originalChar;  // 恢复字符
            }
        }
        
        return 0;  // 无法转换
    }
};

/**
 * 解法2：双向BFS（更优）
 * 
 * 思路：从beginWord和endWord两端同时进行BFS
 * 优势：搜索空间减半，特别适合大规模单词表
 */
class SolutionBidirectional {
public:
    int ladderLength(string beginWord, string endWord, vector<string>& wordList) {
        unordered_set<string> wordSet(wordList.begin(), wordList.end());
        if (wordSet.find(endWord) == wordSet.end()) return 0;
        
        // 双端集合
        unordered_set<string> beginSet{beginWord};
        unordered_set<string> endSet{endWord};
        
        int level = 1;
        int wordLen = beginWord.length();
        
        while (!beginSet.empty() && !endSet.empty()) {
            // 永远从较小的集合扩展
            if (beginSet.size() > endSet.size()) {
                swap(beginSet, endSet);
            }
            
            unordered_set<string> tempSet;
            for (string word : beginSet) {
                string temp = word;
                for (int i = 0; i < wordLen; i++) {
                    char original = word[i];
                    for (char c = 'a'; c <= 'z'; c++) {
                        if (c == original) continue;
                        
                        temp[i] = c;
                        
                        // 遇到另一端的单词
                        if (endSet.find(temp) != endSet.end()) {
                            return level + 1;
                        }
                        
                        if (wordSet.find(temp) != wordSet.end()) {
                            tempSet.insert(temp);
                            wordSet.erase(temp);  // 标记为已访问
                        }
                    }
                    temp[i] = original;
                }
            }
            beginSet = tempSet;
            level++;
        }
        
        return 0;
    }
};

// 测试代码
int main() {
    Solution sol;
    SolutionBidirectional solBidir;
    
    // 测试用例1
    vector<string> wordList1 = {"hot", "dot", "dog", "lot", "log", "cog"};
    string beginWord1 = "hit";
    string endWord1 = "cog";
    
    cout << "=== 测试用例1 ===" << endl;
    cout << "beginWord: " << beginWord1 << endl;
    cout << "endWord: " << endWord1 << endl;
    cout << "wordList: ";
    for (auto& w : wordList1) cout << w << " ";
    cout << endl;
    cout << "单向BFS结果: " << sol.ladderLength(beginWord1, endWord1, wordList1) << endl;
    cout << "双向BFS结果: " << solBidir.ladderLength(beginWord1, endWord1, wordList1) << endl;
    cout << "预期结果: 5 (hit->hot->dot->dog->cog)" << endl;
    cout << endl;
    
    // 测试用例2（无解）
    vector<string> wordList2 = {"hot", "dot", "dog", "lot", "log"};
    string beginWord2 = "hit";
    string endWord2 = "cog";
    
    cout << "=== 测试用例2（无解）===" << endl;
    cout << "单向BFS结果: " << sol.ladderLength(beginWord2, endWord2, wordList2) << endl;
    cout << "预期结果: 0" << endl;
    
    return 0;
}
