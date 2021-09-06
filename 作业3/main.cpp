#include <iostream>
#include <math.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "default.h"
#include <windows.h>

using namespace std;

//默认迷宫
void __default_maze(vector<vector<char>> &m){
    int data=0;
    cout << "选择缺省迷宫的序号(1到4)：";
    int choose;
    cin >> choose;
    while(choose < 1 || choose >4){
        cout << "错误输入，请重试：";
        cin >> choose;
    }
    for(int i = 0; i < 30; i++)
        for (int j = 0; j < 60; j++) {
            m[i][j] = default_maze[choose - 1][data];
            data++;
        }
}

//生成迷宫
void __initializaion(int row, int col, vector<vector<char>> &m){
    for(int i = 0; i < row; i++){
        if(i == 0 || i == row-1){
            for(int j=0; j < col; j++)
                m[i][j] = '#';
            continue;
        }
        for(int j = 0; j < col; j++){
            if(j == 0 || j == col - 1){
                m[i][j]='#';
                continue;
            }
            if(j == 1 && i == 1){
                m[i][j]='S';
                continue;
            }
            if(j == col - 2 && i == row - 2){
                m[i][j]='E';
                continue;
            }
            if(rand() % 2){
                m[i][j]='#';
            }
        }
    }
}

//输出迷宫
void showmaze(vector<vector<char>> m){
    for(int i = 0; i < m.size(); i++){
        for (int j = 0; j < m[i].size(); j++)
            cout << m[i][j];
        cout<<endl;
    }
}

void highlight_print(const char s, int color){
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | color);
    cout << s;
    SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | 7);
}

//搜索过程中的节点，以链表形式反应其路径
typedef struct node{
    int x;
    int y;
    double dist;
    node *parent;
} node;

class A_Star{
private:
    int s_x;//起点x坐标
    int s_y;//起点y坐标
    int e_x;//终点x坐标
    int e_y;//终点x坐标
    int row;//行数
    int col;//列数
    vector<vector<char>> Storage_Maze;
    vector<node *> Open;//Open表
    vector<node *> Close;//Close表
    vector<node *> Path;//Path表
    vector<node *> Searched;//Searched表
    node *p = new node;

public:
    A_Star(int s_x, int s_y, int e_x, int e_y, int row, int col, vector<vector<char>> m);
    double get_dist(node *i);//计算代价F=G+H,G为路径长度，H为预估剩余路径长度
    double get_cost(double x1, double y1, double x2, double y2);//获取移动距离
    node * get_best();//从Open表中获取F函数值最小点
    int is_in_table(node *i, vector<node *> table);//判断节点i是否在表table中
    void insert_to_table(node *i, vector<node *> &table);//将节点i插入表中
    void delete_from_table(node *i, vector<node *> &table);//将节点i从表中删除
    int where_in_table(node *i, vector<node *> table);//获取在表中的序号
    int is_target(node *i);//判断是否为终点
    int is_valid_coord(int x, int y);//判断目前坐标是否合法
    void make_path(node *temp_node);//生成Path表
    void extend_round(node *p);//拓展节点
    void find_path();//寻找最佳路径
    void update_maze();//更新迷宫图
    void maze_print();//以高亮方式输出迷宫图
    vector<vector<char>> get_maze();//返回迷宫图
};

A_Star::A_Star(int s_x, int s_y, int e_x, int e_y, int row, int col, vector<vector<char>> m) {
    this->s_x = s_x;
    this->s_y = s_y;
    this->e_x = e_x;
    this->e_y = e_y;
    this->row = row;
    this->col = col;
    this->Storage_Maze = m;
}

double A_Star::get_dist(node *i) {
    return i->dist + sqrt((A_Star::e_x - i->x) * (A_Star::e_x - i->x)) + sqrt((A_Star::e_y - i->y) * (A_Star::e_y - i->y));
}

double A_Star::get_cost(double x1, double y1, double x2, double y2) {
    if (x1 == x2 || y1 == y2)
        return 1.0;
    return 1.4;
}

node * A_Star::get_best() {
    node *best = NULL;
    double best_value = 1000000.0;
    vector<node *>::iterator cursor;
    for(cursor = this->Open.begin(); cursor != this->Open.end(); cursor++){
        double value = this->get_dist((*cursor));
        if(value < best_value){//较小的代价时
            best_value = value;//更新最小代价
            best = *cursor;//更新最优节点
        }
    }
    return best;
}

int A_Star::is_in_table(node *i, vector<node *> table){
    vector<node *>::iterator cursor;
    for(cursor = table.begin(); cursor != table.end(); cursor++){
        if((*cursor)->x == i->x && (*cursor)->y == i->y)
            return 1;
    }
    return 0;
}

void A_Star::insert_to_table(node *i, vector<node *> &table) {
    node *temp_pointer = new node;
    temp_pointer = i;
    table.push_back(temp_pointer);
}

void A_Star::delete_from_table(node *i, vector<node *> &table) {
    vector<node *>::iterator cursor;
    for(cursor = table.begin(); cursor != table.end();){
        if((*cursor)->x == i->x && (*cursor)->y == i->y)
            cursor = table.erase(cursor);
        else
            cursor++;
    }
}

int A_Star::where_in_table(node *i, vector<node *> table) {
    vector<node *>::iterator cursor;
    for(cursor = table.begin(); cursor != table.end(); cursor++){
        if((*cursor)->x == i->x && (*cursor)->y == i->y)
            return cursor - table.begin();
    }
}

int A_Star::is_target(node *i) {
    return (i->x == this->e_x && i->y == this->e_y);
}

int A_Star::is_valid_coord(int x, int y) {
    if(x<0 || x>= this->col || y<0 || y>= this->row)
        return 0;
    return (this->Storage_Maze[y][x] != '#');
}

void A_Star::make_path(node *temp_node) {
    while(temp_node){
        //通过结构体记录路径的坐标值
        this->insert_to_table(temp_node, this->Path);
        //回溯父节点
        temp_node = temp_node->parent;
    }
}

void A_Star::extend_round(node *p) {
    //四向移动坐标
    int xs[] = {0,-1,1,0};
    int ys[] = {-1,0,0,1};
    int new_x, new_y;//新坐标值
    for (int i = 0; i < 4; i++) {
        //确定新坐标的值
        new_x = p->x + xs[i];
        new_y = p->y + ys[i];
        if(!this->is_valid_coord(new_x, new_y))
            continue;//新坐标下的位置非法则继续循环
        node *temp_node = new node;
        temp_node->parent = p;
        temp_node->x = new_x;
        temp_node->y = new_y;
        temp_node->dist = p->dist + this->get_cost(p->x, p->x, new_x, new_y);
        if(this->is_in_table(temp_node, this->Close))
            continue;//拓展节点在close表中则继续循环
        if(this->is_in_table(temp_node, Open)){//若拓展节点已在Open表内
            int id = this->where_in_table(temp_node, Open);
            if(Open[id]->dist > temp_node->dist){//比表内已有节点的代价小
                Open[id]->parent = p;//更新父节点
                Open[id]->dist = temp_node->dist;//更新代价
            }
        }else{//若不在，则加入Open表
            this->insert_to_table(temp_node, Open);
        }
    }

}

void A_Star::find_path() {
    //初始化操作节点，位于起始处
    node *pointer = new node;
    pointer->x = this->s_x;
    pointer->y = this->s_y;
    pointer->dist = 0.0;
    pointer->parent = NULL;
    this->insert_to_table(pointer, this->Close);
    while(true){
        this->extend_round(pointer);
        if(this->Open.empty())
            break;//Open表为空时跳出循环，结束find_path过程
        pointer = this->get_best();//转为Open表中代价最小的节点
        if(this->is_target(pointer)){
            this->make_path(pointer);
            break;
            //到达终点时开始将路径记录到Path表中并跳出循环，结束find_path过程
        }
        this->insert_to_table(pointer, this->Close);//新节点加入Close表
        this->delete_from_table(pointer, this->Open);
    }
}

void A_Star::update_maze() {
    vector<node *>::iterator cursor;
    for (cursor = this->Open.begin(); cursor != this->Open.end(); cursor++)
        this->insert_to_table(*cursor, this->Searched);//将Open表中节点全放入Searched表中
    for (cursor = this->Close.begin(); cursor != this->Close.end(); cursor++)
        this->insert_to_table(*cursor, this->Searched);//将Close表中节点全放入Searched表中
    for (cursor = this->Searched.begin(); cursor != this->Searched.end(); cursor++)
        this->Storage_Maze[(*cursor)->y][(*cursor)->x] = '*';//将Searched表中含有的节点在迷宫图中标记出来
    for (cursor = this->Path.begin(); cursor != this->Path.end(); cursor++)
        this->Storage_Maze[(*cursor)->y][(*cursor)->x] = '+';//将Path表中含有的节点在迷宫图中标记出来
    this->Storage_Maze[this->row - 2][this->col - 2] = 'E';
    this->Storage_Maze[1][1] = 'S';
}

void A_Star::maze_print() {
    for(int i = 0; i < this->Storage_Maze.size(); i++){
        for (int j = 0; j < this->Storage_Maze[i].size(); j++)
            switch (Storage_Maze[i][j]) {
            case '#':
                highlight_print(Storage_Maze[i][j], 7);//白色
                break;
            case 'S':
                highlight_print(Storage_Maze[i][j], 2);//绿色
                break;
            case 'E':
                highlight_print(Storage_Maze[i][j], 2);//绿色
                break;
            case '.':
                highlight_print(Storage_Maze[i][j], 1);//灰色
                break;
            case '*':
                highlight_print(Storage_Maze[i][j], 6);//黄色
                break;
            case '+':
                highlight_print(Storage_Maze[i][j], 4);//红色
                break;
            }
        cout << endl;
    }
}

vector<vector<char>> A_Star::get_maze() {
    return this->Storage_Maze;
}

void start_finding(int row, int col, vector<vector<char>> m){//开始路径搜索
    A_Star algorithm(1, 1, col - 2, row - 2, row, col, m);
    algorithm.find_path();
    algorithm.update_maze();
    cout << "最后的迷宫如下\n";
    algorithm.maze_print();
    cout << "S与E表示起点终点\n" << "*表示已经搜索过的位置\n" << "+表示为最终路径\n" << ".表示尚未搜索的区域\n";
}


//主程序
int main() {
    vector<vector<char>> maze;
    cout << "由于随机迷宫的不确定性，随机生成的迷宫有极大可能无法从起点到达终点，为此，头函数中定义了几个60x30的有效迷宫。\n";
    cout << "是否生成随机迷宫？（输入1为是，输入0为否）：";
    int choose, col,row;;
    cin >> choose;
    if(choose == 1){
        cout << "请键入所建迷宫的总列数（包含外围墙壁。注意，过大的值可能会导致较长的运行时间）：";
        cin >> col;
        cout << "请键入所建迷宫的总行数（包含外围墙壁。注意，过大的值可能会导致较长的运行时间）：";
        cin >> row;
        maze.resize(row);
        //生成二维表
        for(int i = 0; i < maze.size(); i++){
            maze[i].resize(col);
        }
        for(int i = 0; i < maze.size(); i++)
            for(int j = 0; j < maze[i].size(); j++)
                maze[i][j] = '.';
        //初始化迷宫
        srand((int)time(0));
        __initializaion(row,col,maze);
    }else if(choose == 0){
        col = 60;
        row = 30;
        maze.resize(row);
        for(int i = 0; i < row; i++){
            maze[i].resize(col);
        }
        __default_maze(maze);
    }else{
        cout << "错误输入，程序退出";
        return 1;
    }
    //显示生成的迷宫
    cout << "生成的迷宫如下：\n";
    showmaze(maze);
    system("pause");
    system("cls");
    start_finding(row, col, maze);
    system("pause");
    return 0;
}
