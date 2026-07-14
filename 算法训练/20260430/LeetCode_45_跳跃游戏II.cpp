/**
 * LeetCode 45: 跳跃游戏 II
 * 
 * 题目描述：
 * 给定一个非负整数数组 nums，你最初位于数组的第一个位置。
 * 数组中的每个元素代表你在该位置可以跳跃的最大长度。
 * 目标是使用最少的跳跃次数到达数组的最后一个位置。
 * 
 * 解法：贪心算法 O(n) 时间复杂度
 * 
 * @author UE5算法训练
 * @date 2026-04-30
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>

using namespace std;

/**
 * 跳跃游戏 II 解决方案类
 */
class JumpGameII {
public:
    /**
     * 贪心算法：维护当前跳跃边界，在边界处进行跳跃
     * 
     * 算法核心：
     * 1. 维护 current_end（当前跳跃边界）和 farthest（下一跳最远可达）
     * 2. 遍历到当前边界时，执行一次跳跃
     * 3. 局部最优选择（每次跳到能到达最远的位置）=> 全局最优
     * 
     * @param nums 跳跃能力数组
     * @return 最少跳跃次数
     */
    static int jump(vector<int>& nums) {
        int n = nums.size();
        if (n <= 1) return 0;
        
        int jumps = 0;           // 跳跃次数计数
        int current_end = 0;     // 当前跳跃的边界
        int farthest = 0;       // 下一跳能到达的最远位置
        
        // 遍历到倒数第二个元素（最后一位已到达终点）
        for (int i = 0; i < n - 1; i++) {
            // 更新最远可达位置
            farthest = max(farthest, i + nums[i]);
            
            // 到达当前边界，执行跳跃
            if (i == current_end) {
                jumps++;
                current_end = farthest;
                
                // 提前终止：如果当前边界已覆盖终点
                if (current_end >= n - 1) {
                    break;
                }
            }
        }
        
        return jumps;
    }
    
    /**
     * 详细版本：记录每一步的决策过程
     * 
     * @param nums 跳跃能力数组
     * @param verbose 是否输出详细过程
     * @return 最少跳跃次数
     */
    static int jumpVerbose(vector<int>& nums, bool verbose = false) {
        int n = nums.size();
        if (n <= 1) return 0;
        
        int jumps = 0;
        int current_end = 0;
        int farthest = 0;
        int step_start = 0;
        
        if (verbose) {
            cout << "跳跃过程追踪:" << endl;
            cout << "========================================" << endl;
        }
        
        for (int i = 0; i < n - 1; i++) {
            farthest = max(farthest, i + nums[i]);
            
            if (i == current_end) {
                jumps++;
                if (verbose) {
                    cout << "第 " << jumps << " 跳: 从位置 " << step_start 
                         << " 开始，跳跃范围 [0, " << farthest << "]" << endl;
                }
                
                step_start = current_end + 1;
                current_end = farthest;
                
                if (current_end >= n - 1) {
                    if (verbose) {
                        cout << "已达到终点！" << endl;
                    }
                    break;
                }
            }
        }
        
        if (verbose) {
            cout << "========================================" << endl;
        }
        
        return jumps;
    }
};

/**
 * ==================== 测试代码 ====================
 */

/**
 * 测试用例结构
 */
struct TestCase {
    vector<int> nums;
    int expected;
};

/**
 * 运行单个测试
 */
void runTest(const TestCase& test, int testNum) {
    vector<int> nums = test.nums;
    int result = JumpGameII::jump(nums);
    
    cout << "测试 " << testNum << ": ";
    for (int num : nums) {
        cout << num << " ";
    }
    cout << "=> 期望: " << test.expected 
         << ", 实际: " << result 
         << (result == test.expected ? " [通过]" : " [失败]") << endl;
}

/**
 * 运行所有测试
 */
void runAllTests() {
    cout << "\n========== 测试用例运行 ==========" << endl;
    
    vector<TestCase> testCases = {
        // 标准案例
        {{2, 3, 1, 1, 4}, 2},
        
        // 含0但可达的情况
        {{2, 3, 0, 1, 4}, 2},
        {{1, 0, 2, 1, 0}, 2},  // 可达：0→1(2)→3(1)→4
        
        // 最坏情况（每次只能跳1步）
        {{1, 1, 1, 1, 1}, 4},
        
        // 大跨度（第一步直接到终点）
        {{10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0}, 1},
        
        // 边界情况
        {{0}, 0},
        {{1}, 0},
        {{1, 2}, 1},
        {{1, 2, 3}, 2},       // 0→1(2)→3(终点)
        
        // 递增数组
        {{1, 2, 3, 4, 5}, 3},     // 0→1→3→5(终点), 3步
        {{1, 2, 3}, 2},          // 0→1→3(终点), 2步
        
        // 递减数组
        {{5, 4, 3, 2, 1}, 1},
        
        // 混合案例
        {{3, 4, 3, 2, 1, 1, 1}, 3},  // 0→1(4)→5(1)→6(终点), 3步
    };
    
    for (size_t i = 0; i < testCases.size(); i++) {
        runTest(testCases[i], i + 1);
    }
}

/**
 * 主函数
 */
int main() {
    cout << "========================================" << endl;
    cout << "   LeetCode 45: 跳跃游戏 II" << endl;
    cout << "   贪心算法 | O(n) 时间 | O(1) 空间" << endl;
    cout << "========================================" << endl;
    
    // 详细演示标准案例
    cout << "\n【标准案例分析】nums = {2, 3, 1, 1, 4}" << endl;
    vector<int> demo = {2, 3, 1, 1, 4};
    JumpGameII::jumpVerbose(demo, true);
    cout << "最少跳跃次数: " << JumpGameII::jump(demo) << endl;
    
    // 运行所有测试
    runAllTests();
    
    cout << "\n========== 算法分析总结 ==========" << endl;
    cout << "时间复杂度: O(n) - 单次遍历" << endl;
    cout << "空间复杂度: O(1) - 常数额外空间" << endl;
    cout << "核心思想: 贪心选择，每次跳到下一跳能到达最远的位置" << endl;
    cout << "===================================" << endl;
    
    return 0;
}
