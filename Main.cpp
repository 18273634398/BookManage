#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// B+树的阶数（M），一个节点最多有2*M-1个键值
const int M = 3;  // 示例中设置较小的M值，方便调试

// B+树节点类型：内部节点和叶子节点
enum NodeType {
    INTERNAL,  // 内部节点
    LEAF       // 叶子节点
};

// B+树节点结构
struct Node {
    NodeType type;            // 节点类型（内部节点或叶子节点）
    vector<int> keys;         // 节点中的键值
    vector<Node*> children;   // 子节点指针列表，仅在内部节点中使用
    Node* parent;             // 父节点指针
    Node* next;               // 叶子节点的下一个节点指针，用于叶子节点链表

    // 构造函数
    Node(NodeType t) : type(t), parent(nullptr), next(nullptr) {}
};

class BPlusTree {
private:
    Node* root;   // B+树的根节点指针
    int height;   // B+树的高度

public:
    // 构造函数：初始化B+树
    BPlusTree() {
        root = new Node(LEAF);  // 初始时树为空，根节点为叶子节点
        height = 0;  // 初始高度为0
    }

    // 插入键值对到B+树中
    // 参数：key - 要插入的键值
    void insert(int key) {
        // 如果根节点已满，则需要分裂
        if (root->keys.size() == (2 * M - 1)) {
            // 创建一个新的根节点
            Node* new_root = new Node(INTERNAL);
            // 新根节点的第一个子节点是当前的根节点
            new_root->children.push_back(root);
            // 分裂子节点，将根节点分裂
            split_child(new_root, 0);
            // 更新根节点
            root = new_root;
            // 树的高度增加
            height++;
        }
        // 插入键值到非满节点中
        insert_non_full(root, key);
    }

    // 删除键值对（未实现）
    // 参数：key - 要删除的键值
    void remove(int key) {
        // TODO: 实现删除操作
    }

    // 查找键值对是否存在
    // 参数：key - 要查找的键值
    // 返回值：true 表示找到，false 表示未找到
    bool search(int key) {
        return search_key(root, key);
    }

private:
    // 分裂子节点
    // 参数：
    // - parent: 父节点指针
    // - index: 分裂的子节点在父节点中的索引位置
    // 功能：将一个已满的子节点分裂为两个节点，并将中间键值上升到父节点中
    void split_child(Node* parent, int index) {
        Node* child = parent->children[index];  // 获取要分裂的子节点
        Node* new_child = new Node(child->type);  // 创建一个新的子节点
        
        // 将中间键值插入到父节点
        parent->keys.insert(parent->keys.begin() + index, child->keys[M - 1]);
        // 新子节点插入到父节点的子节点列表中
        parent->children.insert(parent->children.begin() + index + 1, new_child);
        
        // 如果分裂的是叶子节点
        if (child->type == LEAF) {
            // 将键值复制到新节点
            new_child->keys.assign(child->keys.begin() + M - 1, child->keys.end());
            // 调整原节点的键值
            child->keys.resize(M - 1);
            
            // 更新叶子节点链表的链接
            new_child->next = child->next;
            child->next = new_child;
        } else {  // 如果分裂的是内部节点
            // 将右半部分的键值和子节点复制到新节点
            new_child->keys.assign(child->keys.begin() + M, child->keys.end());
            new_child->children.assign(child->children.begin() + M, child->children.end());
            // 调整原节点的键值和子节点列表
            child->keys.resize(M - 1);
            child->children.resize(M);
        }
    }

    // 在非满节点中插入键值对
    // 参数：
    // - node: 当前节点指针
    // - key: 要插入的键值
    // 功能：递归寻找合适的叶子节点，将键值插入到叶子节点中
    void insert_non_full(Node* node, int key) {
        int i = node->keys.size() - 1;  // 从最后一个键值开始比较
        // 如果当前节点是叶子节点
        if (node->type == LEAF) {
            // 将键值插入到叶子节点中，并保持有序
            node->keys.push_back(key);
            sort(node->keys.begin(), node->keys.end());
        } else {  // 如果当前节点是内部节点
            // 找到子节点的索引位置
            while (i >= 0 && key < node->keys[i]) {
                i--;
            }
            i++;  // i为子节点的位置
            
            // 如果子节点已满，则先分裂子节点
            if (node->children[i]->keys.size() == (2 * M - 1)) {
                split_child(node, i);
                // 分裂后，判断插入到哪个子节点
                if (key > node->keys[i]) {
                    i++;
                }
            }
            // 递归插入到子节点中
            insert_non_full(node->children[i], key);
        }
    }

    // 搜索键值对是否存在
    // 参数：
    // - node: 当前节点指针
    // - key: 要查找的键值
    // 返回值：true 表示找到，false 表示未找到
    bool search_key(Node* node, int key) {
        int i = 0;
        // 在当前节点的键值中寻找键值
        while (i < node->keys.size() && key > node->keys[i]) {
            i++;
        }
        // 如果找到键值，则返回true
        if (i < node->keys.size() && key == node->keys[i]) {
            return true;
        }
        // 如果是叶子节点，则找不到，返回false
        else if (node->type == LEAF) {
            return false;
        }
        // 如果是内部节点，递归搜索子节点
        else {
            return search_key(node->children[i], key);
        }
    }
};

int main() {
    BPlusTree bpt;
    // 插入一些键值对
    bpt.insert(10);
    bpt.insert(20);
    bpt.insert(5);
    bpt.insert(6);
    bpt.insert(12);
    bpt.insert(30);
    bpt.insert(7);
    bpt.insert(17);

    // 查找键值
    cout << "Search for 6: " << (bpt.search(6) ? "Found" : "Not found") << endl;
    cout << "Search for 15: " << (bpt.search(15) ? "Found" : "Not found") << endl;

    return 0;
}