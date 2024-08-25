#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <unistd.h>
using namespace std;

// B+树叶子节点的结构体，用于存储相关信息
struct Info {
    string name;    // 书名
    string author;  // 著者
    int restNum;    // 现存量
    int sumNum;     // 总库存量
};

// B+树的阶数（M）：一个节点最多有2*M-1个键值
const int M = 3;

// B+树节点类型：内部节点和叶子节点
enum NodeType {
    INTERNAL,  // 内部节点
    LEAF       // 叶子节点
};

// B+树节点结构
struct Node {
    NodeType type;            // 节点类型
    vector<long long> keys;   // 节点中的键值
    vector<Node*> children;   // 子节点指针列表
    Node* parent;             // 父节点指针
    Node* next;               // 叶子节点的下一个节点指针，用于叶子节点链表
    Info* info;               // 叶子节点信息存储

    // 构造函数
    Node(NodeType t) : type(t), parent(nullptr), next(nullptr), info(nullptr) {}
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

    Node* getRoot() {
        return root;
    }

    void show() {
        show_base(root,0);
    }

    // 插入键值对到B+树中
    void insert(long long key, string bookName, string author, int num) {
        // 如果根节点已满，则需要分裂
        if (root->keys.size() == (2 * M - 1)) {
            Node* new_root = new Node(INTERNAL);
            new_root->children.push_back(root);
            split_child(new_root, 0);
            root = new_root;
            height++;
        }
        insert_non_full(root, key, bookName, author, num);
    }

    // 删除键值对（未实现）
    void remove(long long key) {
        // TODO: 实现删除操作
    }


    // 搜索键值对是否存在，返回Node*
    Node* search_key(Node* node, long long key) {
        int i = 0;
        while (i < node->keys.size() && key > node->keys[i]) {
            i++;
        }
        if (i < node->keys.size() && key == node->keys[i]) {
            return node;
        } else if (node->type == LEAF) {
            return nullptr;
        } else {
            return search_key(node->children[i], key);
        }
    }

private:
    // 分裂子节点
    void split_child(Node* parent, int index) {
        Node* child = parent->children[index];
        Node* new_child = new Node(child->type);

        parent->keys.insert(parent->keys.begin() + index, child->keys[M - 1]);
        parent->children.insert(parent->children.begin() + index + 1, new_child);

        if (child->type == LEAF) {
            new_child->keys.assign(child->keys.begin() + M - 1, child->keys.end());
            child->keys.resize(M - 1);

            // 链接叶子节点链表
            new_child->next = child->next;
            child->next = new_child;

            // 为新叶子节点动态分配Info
            if (child->info) {
                new_child->info = new Info{child->info->name, child->info->author, child->info->restNum, child->info->sumNum};
            }

        } else {
            new_child->keys.assign(child->keys.begin() + M, child->keys.end());
            new_child->children.assign(child->children.begin() + M, child->children.end());
            child->keys.resize(M - 1);
            child->children.resize(M);
        }
    }

    // 在非满节点中插入键值对
    void insert_non_full(Node* node, long long key, string bookName, string author, int num) {
        int i = node->keys.size() - 1;
        if (node->type == LEAF) {
            node->keys.push_back(key);
            sort(node->keys.begin(), node->keys.end());

            // 动态分配Info并存储到叶子节点中
            if (!node->info) {
                node->info = new Info{move(bookName), move(author), num,num};
            }
            else {
                node->info->restNum+=num;
                node->info->sumNum+=num;
            }

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
            insert_non_full(node->children[i], key, bookName, author, num);
        }
    }





    // 凹入表形式显示B树的辅助函数
    void show_base(Node* node, int depth) {
        // 输出当前节点的键值，使用缩进表示层次
        for (int i = 0; i < depth; i++) {
            cout << "    ";  // 每层缩进4个空格
        }

        cout << "[";
        for (size_t i = 0; i < node->keys.size(); i++) {
            cout << node->keys[i];
            if (i != node->keys.size() - 1) cout << ", ";
        }
        cout << "]" << endl;

        // 递归处理子节点
        for (auto child : node->children) {
            show_base(child, depth + 1);
        }
    }
};



int main() {
    cout<<"欢迎使用图书管理系统"<<endl<<"========================"<<endl;
    cout<<"[1]记录新购入图书"<<endl<<"[2]清除指定图书库存"<<endl<<"[3]借阅图书"<<endl<<"[4]归还图书"<<endl<<"[5]以凹入表显示图书资源信息"<<endl<<"------------------------"<<endl<<"请输入功能选项：";
    int choice;
    cin >> choice;
    if (cin.fail()) {
        cout << "[Error 1] 选项输入异常，系统已结束运行" << endl;
        exit(0);  // 退出程序
    }
    BPlusTree bpt;
    //
        bpt.insert(1,"cs","cs",1);
        bpt.insert(2,"cs","cs",1);
        bpt.insert(3,"cs","cs",1);
        bpt.insert(4,"cs","cs",1);
        bpt.insert(5,"cs","cs",1);
        bpt.insert(6,"cs","cs",1);
        bpt.insert(7,"cs","cs",1);
        bpt.insert(8,"cs","cs",1);
    //
    while(choice) {
        if(choice == 1) {
            long long id;
            string bookName;
            string author;
            int num;
            cout<<"请输入新购入书籍的相关信息"<<endl<<"========================"<<endl;
            cout<<"请输入书号：";
            cin>>id;
            if (cin.fail()) {
                cout << "[Error 1] 选项输入异常，系统已结束运行" << endl;
                exit(0);  // 退出程序
            }
            cout<<"请输入书籍名：";
            cin>>bookName;
            cout<<"请输入作者：";
            cin>>author;
            cout<<"请输入新增图书量：";
            cin>>num;
            bpt.insert(id,bookName,author,num);
        }
        else if(choice==2) {
        }
        else if(choice==3) {
            int id;
            Node* node;
            cout<<"请输入借阅图书的编号：";
            cin>>id;
            if (cin.fail()) {
                cout << "[Error 1] 选项输入异常，系统已结束运行" << endl;
                exit(0);  // 退出程序
            }
            node = bpt.search_key(bpt.getRoot(),id);
            if(node) {
                int num = node->info->restNum;
                cout<<"图书信息"<<endl<<"========================"<<endl<<"图书名："<<node->info->name<<endl<<"作者："<<node->info->author<<endl;
                cout<<"当前剩余馆藏："<<num<<"/"<<node->info->sumNum<<" 本 "<<endl;
                if(num) {
                    char temp;
                    cout<<"是否确定借阅(y/n)：";
                    cin >> temp;
                    if(temp=='y') {
                        node->info->restNum-=1;
                    }
                    else {
                        cout<<"您已取消借阅"<<endl;
                    }
                }
                else {
                    cout<<"当前图书馆藏数量不足，无法借阅"<<endl;
                }
            }
            else {
                cout<<"未查找到相关图书"<<endl;
            }
        }
        else if(choice==4) {
            int id;
            Node* node;
            cout<<"请输入归还图书的编号：";
            cin>>id;
            if (cin.fail()) {
                cout << "[Error 1] 选项输入异常，系统已结束运行" << endl;
                exit(0);  // 退出程序
            }
            node = bpt.search_key(bpt.getRoot(),id);
            if(node) {
                int num = node->info->restNum;
                int sumNum = node->info->sumNum;
                cout<<"图书信息"<<endl<<"========================"<<endl<<"图书名："<<node->info->name<<endl<<"作者："<<node->info->author<<endl;
                if(num !=sumNum) {
                    char temp;
                    cout<<"是否确定归还(y/n)：";
                    cin >> temp;
                    if(temp=='y') {
                        node->info->restNum+=1;
                    }
                    else {
                        cout<<"您已取消借阅"<<endl;
                    }
                }
                else {
                    cout<<"当前图书并未被借阅，无法归还"<<endl;
                }
            }
            else {
                cout<<"未查找到相关图书"<<endl;
            }
        }
        else if(choice==5) {
            bpt.show();
        }
        else {
            cout << "[Error 2] 功能选项输入错误，请重试" << endl;
        }
        usleep(500000);
        cout<<"[Finish] Operation Finished"<<endl;
        usleep(1500000);
        cout<<endl<<endl<<endl;
        cout<<"======================="<<endl;
        cout<<"[1]记录新购入图书"<<endl<<"[2]清除指定图书库存"<<endl<<"[3]借阅图书"<<endl<<"[4]归还图书"<<endl<<"[5]以凹入表显示图书资源信息"<<endl<<"------------------------"<<endl<<"请输入功能选项：";
        cin>>choice;
        if (cin.fail()) {
            cout << "[Error 1] 选项输入异常，系统已结束运行" << endl;
            exit(0);  // 退出程序
        }
    }
    return 0;
}

/*
① 采编入库z新购入一种书，经分类和确定书号之后登记到图书账目中去。如果这种书在账中已有，则只将总库存量增加。 
② 清除库存:某种书已无保留价值，将它从图书账目中注销。 
③ 借阅:如果一种书的现存量大于零，则借出一本，登记借阅者的图书证号和归还期限。 
④ 归还z注销对借阅者的登记，改变该书的现存量。 
⑤ 显示:以凹入表的形式显示B树。这个操作是为了调试和维护的目的而设置的。 
 */
