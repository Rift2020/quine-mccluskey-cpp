/*

 将逻辑表达式的所有取值打表输出，并且使用不完全的quine mccluskey算法找出一个稍简单的等价表达式
 不完全的意思是它缺少素项表部分，因此得到的表达式通常都不是最简表达式

 一时兴起的为了实现出来玩一玩
 一天闲暇时间写出来的垃圾东西，算法性能也不好（主要是常数不好），可读性也烂的一批，维护性完全没有的小辣鸡代码
 qm算法因为要把真值表打出来才能开始所以复杂度至少是O(2^n)吧，所以说其实没有什么好的办法更快？
 中缀转后缀大约勉强可以看看，qm就别看了，乱得一批
 不保证没有bug
 */
#include <iostream>
#include <stack>
#include <unordered_map>
#include <vector>
#include<algorithm>
#include<string.h>
using namespace std;
//运算符优先级
unordered_map<char,int> priority={{'!',5},{'&',4},{'|',3},{'>',4},{'=',5}};
unordered_map<char,int> ind;//代数符号->代数序号，比如!b&a|c中，b->0,a->1,c->2
unordered_map<int,char> rind;//代数序号->代数符号
string input,suffix;//输入字符串，后缀表达式字符串
stack<char> op;//运算符栈，用于转换后缀
bool real,unreal;//是否永真，是否永假
int cnt;//用来辅助ind和rind
vector<bool> digit;//变元对应取值，比如digit[0]里的bool值对应第0个代数的值
typedef pair<int,vector<int> > pr;//组序号（1的个数），该组对应的digit状态
vector<pr> minterm;
vector<pr> newMin;
vector<pr> finalMint;
bool operator<(const pr &a,const pr &b){
    if(a.first!=b.first)return a.first<b.first;
    int n=a.second.size();
    for(int i=0;i<n;++i){
        if(a.second[i]!=b.second[i])return a.second[i]<b.second[i];
    }
    return false;
}
pr merge(const pr &a,const pr &b){
    pr re;
    re.first=0;
    int n=a.second.size();
    re.second.resize(n);
    for(int i=0;i<n;++i){
        if(a.second[i]==b.second[i])
            re.second[i]=a.second[i];
        else
            re.second[i]=-1;
        if(re.second[i]==1)re.first++;
    }
    return re;
}
bool checkMint(const pr &a,const pr &b){
    if(abs(a.first-b.first)!=1)return false;
    int n=a.second.size();
    int diff=0;
    for(int i=0;i<n;++i){
        if(a.second[i]!=b.second[i]){
            if(a.second[i]==-1||b.second[i]==-1)return false;
            diff++;
        }
        if(diff==2)return false;
    }
    if(diff==0)return false;
    return true;
}
bool conditionOperator(const bool &a,const bool &b){
    if(a== true&&b== false)return false;
    return true;
}
string toSuffix(string input){
    string suffix;
    for(char i:input){
        if(isalpha(i)) {
            suffix += i;
            if(ind.find(i)==ind.end()){
                rind[cnt]=i;
                ind[i]=cnt++;
            }

        }
        else{
            if(i=='(')
                op.push(i);
            else if(i==')'){
                while (!op.empty()){
                    char tp=op.top();
                    op.pop();
                    if(tp=='(')break;
                    suffix+=tp;
                }
            }
            else{
                while (!op.empty()){
                    char tp=op.top();
                    if(tp=='(')break;
                    if(priority[tp]>priority[i]){
                        suffix+=tp;
                        op.pop();
                    }
                    else
                        break;
                }
                op.push(i);
            }
        }
    }
    while (!op.empty()){
        suffix+=op.top();
        op.pop();
    }
    return suffix;
}
bool cal(){
    stack<bool> calNum;
    for(char i:suffix){
        if(isalpha(i)){
            int index=ind[i];
            calNum.push(digit[index]);
        }
        else if(i=='!'){
            bool tp=calNum.top();
            calNum.pop();
            calNum.push(!tp);
        }
        else if(i=='&'){
            bool tp=calNum.top();
            calNum.pop();
            bool tp2=calNum.top();
            calNum.pop();
            calNum.push(tp&tp2);
        }
        else if(i=='|'){
            bool tp=calNum.top();
            calNum.pop();
            bool tp2=calNum.top();
            calNum.pop();
            calNum.push(tp|tp2);
        }
        else if(i=='>'){
            bool tp=calNum.top();
            calNum.pop();
            bool tp2=calNum.top();
            calNum.pop();
            calNum.push(conditionOperator(tp2,tp));
        }
        else if(i=='='){
            bool tp=calNum.top();
            calNum.pop();
            bool tp2=calNum.top();
            calNum.pop();
            calNum.push(tp==tp2);
        }
    }
    return calNum.top();
}
void fillDigit(int x){
    for(int i=0;i<digit.size();++i){
        digit[i]=(1<<i)&x;
    }
}
void printTable(){
    real= true,unreal=true;
    int n=ind.size();
    digit.resize(n);
    for(int i=0;i<=(1<<n)-1;++i){
        fillDigit(i);
        for(int j=0;j<n;++j){
            cout<<digit[j];
        }
        cout<<" "<<cal()<<endl;
        if(cal()){
            unreal= false;
            int s=0;
            vector<int> v;
            v.resize(n);
            for(int k=0;k<n;++k){
                if(digit[k])s++;
                v[k]=digit[k]?1:0;
            }
            minterm.emplace_back(s,v);
        }
        else
            real= false;
    }

}
void findSimpleExpresion(){
    while (1) {
        int n = minterm.size();
        bool used[n];
        memset(used, false, sizeof(used));
        sort(minterm.begin(), minterm.end());
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                const pr &a = minterm[i], &b = minterm[j];
                if (b.first - a.first > 1)break;
                if (checkMint(a, b)) {
                    used[i] = used[j] = true;
                    newMin.push_back(merge(a, b));
                }
            }
        }
        for (int i = 0; i < n; ++i) {
            if (used[i] == false) {
                finalMint.push_back(minterm[i]);
            }
        }
        sort(newMin.begin(), newMin.end());
        newMin.erase(unique(newMin.begin(), newMin.end()), newMin.end());
        minterm=newMin;
        newMin.clear();
        if (minterm.size() == 0)break;
    }
}
void showSimpleExpression(){
    cout<<"SimpleExpression:";
    string output;
    for(const auto &i:finalMint){
        string s;
        for(int j=0;j<i.second.size();++j){
            if(i.second[j]==-1)continue;
            if(i.second[j]==1)s+=rind[j];
            else if(i.second[j]==0){
                s+=rind[j];
                s+='\'';
            }
        }
        output+=s+'+';
    }
    output.erase(output.end()-1);
    cout<<output<<endl;
}
int main() {
    cin>>input;
    suffix=toSuffix(input);//转后缀表达式
    printTable();//打表并且记录为真的情况
    if(real)
        cout<<"always true"<<endl;
    else if(unreal)
        cout<<"always false"<<endl;
    else {
        findSimpleExpresion();//qm算法
        showSimpleExpression();
    }

    return 0;
}
