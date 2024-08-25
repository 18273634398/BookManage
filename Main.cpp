#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// B+树的阶数
const int M = 3;  // 示例中设置较小的M值，方便调试

// B+树节点类型
enum NodeType {
    INTERNAL,  // 内部节点
    LEAF       // 叶子节点
};

// B+树节点结构
struct Node {
    NodeType type;
    vector<int> keys;
    vector<Node*> children;
    Node* parent;
    Node* next;  // 用于叶子节点之间的链接

    Node(NodeType t) : type(t), parent(nullptr), next(nullptr) {}
};

class BPlusTree {
private:
    Node* root;
    int height;

public:
    BPlusTree() {
        root = new Node(LEAF);  // 初始时树为空，根节点为叶子节点
        height = 0;
    }

    // 插入键值对
    void insert(int key) {
        if (root->keys.size() == (2 * M - 1)) {
            Node* new_root = new Node(INTERNAL);
            new_root->children.push_back(root);
            split_child(new_root, 0);
            root = new_root;
            height++;
        }
        insert_non_full(root, key);
    }

    // 删除键值对
    void remove(int key) {
        // TODO: 实现删除操作
    }

    // 查找键值对
    bool search(int key) {
        return search_key(root, key);
    }

private:
    // 分裂子节点
    void split_child(Node* parent, int index) {
        Node* child = parent->children[index];
        Node* new_child = new Node(child->type);
        
        // 分割键值
        parent->keys.insert(parent->keys.begin() + index, child->keys[M - 1]);
        parent->children.insert(parent->children.begin() + index + 1, new_child);
        
        // 处理叶子节点
        if (child->type == LEAF) {
            new_child->keys.assign(child->keys.begin() + M - 1, child->keys.end());
            child->keys.resize(M - 1);
            
            // 更新叶子节点的链接
            new_child->next = child->next;
            child->next = new_child;
        } else {  // 处理内部节点
            new_child->keys.assign(child->keys.begin() + M, child->keys.end());
            new_child->children.assign(child->children.begin() + M, child->children.end());
            child->keys.resize(M - 1);
            child->children.resize(M);
        }
    }

    // 在非满节点中插入键值对
    void insert_non_full(Node* node, int key) {
        int i = node->keys.size() - 1;
        if (node->type == LEAF) {
            node->keys.push_back(key);
            sort(node->keys.begin(), node->keys.end());
        } else {
            while (i >= 0 && key < node->keys[i]) {
                i--;
            }
            i++;
            if (node->children[i]->keys.size() == (2 * M - 1)) {
                split_child(node, i);
                if (key > node->keys[i]) {
                    i++;
                }
            }
            insert_non_full(node->children[i], key);
        }
    }

    // 搜索键值对
    bool search_key(Node* node, int key) {
        int i = 0;
        while (i < node->keys.size() && key > node->keys[i]) {
            i++;
        }
        if (i < node->keys.size() && key == node->keys[i]) {
            return true;
        } else if (node->type == LEAF) {
            return false;
        } else {
            return search_key(node->children[i], key);
        }
    }
};

int main() {
    BPlusTree bpt;
    bpt.insert(10);
    bpt.insert(20);
    bpt.insert(5);
    bpt.insert(6);
    bpt.insert(12);
    bpt.insert(30);
    bpt.insert(7);
    bpt.insert(17);

    cout << "Search for 6: " << (bpt.search(6) ? "Found" : "Not found") << endl;
    cout << "Search for 15: " << (bpt.search(15) ? "Found" : "Not found") << endl;

    return 0;
}