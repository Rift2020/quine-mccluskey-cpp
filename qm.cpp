#include <iostream>
#include <fstream>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <cstring>
#define bit(x,b) x&(1<<(b))
using namespace std;

unordered_map<char,int> priority={{'~',4},{'&',3},{'^',2},{'|',1}};

int indexCnt=0;
unordered_map<string,int> symbol2index;
unordered_map<int,string> index2symbol;

vector<char> infix;
vector<char> suffix;

unordered_set<string> mint,nextMint;
vector<string> genOverMint;

vector<string> finalMint;
vector<int> m;
vector<bool> X;





void toInfix(string input){
    for(int le=0; le < input.size();){
        int ri;
        if(isalpha(input[le]) || input[le] == '_'){
            for(ri=le;; ++ri){
                if(ri==input.size()-1||(!isalpha(input[ri+1])&&!isdigit(input[ri+1])&&input[ri+1]!='['&&input[ri+1]!=']'&&input[ri+1]!='_'))
                    break;
            }
            string symbol=input.substr(le,ri-le+1);
            if(symbol2index.find(symbol)!=symbol2index.end()){
                infix.push_back('0'+symbol2index[symbol]);
            }
            else {
                symbol2index[symbol] = indexCnt;
                index2symbol[indexCnt] = symbol;
                infix.push_back('0' + indexCnt);
                indexCnt++;
            }
            le=ri+1;
        }
        else if(input[le]=='~'||input[le]=='&'||input[le]=='^'||input[le]=='|'||input[le]=='('||input[le]==')'){
            infix.push_back(input[le]);
            le++;
        }
        else{
            cerr<<"Unknown symbol:"<<input[le]<<endl;
            exit(1);
        }
    }
    if(indexCnt>20){
        cerr<<"there is too much variable"<<endl;
        exit(1);
    }
}
void toSuffix(){
    stack<char> ope;
    for(char c:infix){
        if(isdigit(c))
            suffix.push_back(c);
        else if(c=='(')
            ope.push(c);
        else if(c==')'){
            while(ope.top()!='('){
                suffix.push_back(ope.top());
                ope.pop();
            }
            ope.pop();
        }
        else{
            while(!ope.empty()&&ope.top()!='('&&priority[c]<priority[ope.top()]){
                suffix.push_back(ope.top());
                ope.pop();
            }
            ope.push(c);
        }
    }
    while(!ope.empty()){
        suffix.push_back(ope.top());
        ope.pop();
    }
}
string binaryString(int x){
    string ans(indexCnt,'0');
    int index=0;
    while(x!=0){
        if(x&1)
            ans[index]='1';
        x>>=1;
        index++;
    }
    return ans;
}
void inputMint(string filename){
    ifstream file;
    file.open(filename, ios::in);
    file>>indexCnt;
    X.resize(indexCnt);
    for(int i=0;i<X.size();++i)X[i]=false;
    for(int i=indexCnt-1;i>=0;--i){
        string symbol;
        file>>symbol;
        symbol2index[symbol]=i;
        index2symbol[i]=symbol;
    }
    file.ignore();
    string line;
    for(int i=0;getline(file,line)&&i<(1<<indexCnt);++i){
        if(line.back()=='1'){
            mint.insert(binaryString(i));
        }
        else if(line.back()=='x'||line.back()=='X'){
            mint.insert(binaryString(i));
            X[i]=true;
        }
        else if(line.back()!='0'){
            cerr<<"truth value should be 1 , 0 or X"<<endl;
            exit(1);
        }
    }
    file.close();
}
void calMint(){
    X.resize((1<<indexCnt));
    for(int i=0;i<(1<<indexCnt);++i)X[i]=false;
    for(int i=0;i<(1<<(indexCnt));++i){
        stack<bool> num;
        for(char c:suffix){
            if(isdigit(c))
                num.push(bit(i,c-'0'));
            else if(c=='~'){
                num.top()=!num.top();
            }
            else if(c=='&'){
                bool x=num.top();
                num.pop();
                bool y=num.top();
                num.pop();
                num.push(x&y);
            }
            else if(c=='^'){
                bool x=num.top();
                num.pop();
                bool y=num.top();
                num.pop();
                num.push(x^y);
            }
            else if(c=='|'){
                bool x=num.top();
                num.pop();
                bool y=num.top();
                num.pop();
                num.push(x|y);
            }
            else {
                cerr<<"internal error1"<<endl;
                exit(1);
            }
        }
        if(num.size()!=1){
            cerr<<"internal error2"<<endl;
            exit(1);
        }
        if(num.top()==true)
            mint.insert(binaryString(i));
    }
}
bool generate(){
    for(string s:mint){
        bool fl=false;
        for(int i=0;i<s.size();++i){
            if(s[i]=='-')continue;
            string t=s;
            if(t[i]=='0')
                t[i]='1';
            else
                t[i]='0';
            if(mint.find(t)!=mint.end()){
                fl=true;
                t[i]='-';
                nextMint.insert(t);
            }
        }
        if(fl==false)
            genOverMint.push_back(s);
    }
    mint=nextMint;
    nextMint.clear();
    if(mint.empty())
        return false;
    return true;
}
void dfs(const string &s,int index,int num){
    if(index==indexCnt){
        m.push_back(num);
        return;
    }
    if(s[index]=='-'||s[index]=='1')
        dfs(s,index+1,num+(1<<index));
    if(s[index]=='-'||s[index]=='0')
        dfs(s,index+1,num);
}
void PIChart(){
    bool chart[(1<<indexCnt)];
    memset(chart,0,sizeof(chart));
    for(string s:genOverMint){
        bool fl= false;
        m.clear();
        dfs(s,0,0);
        for(int i:m){
            if(chart[i]==false&&X[i]==false){
                chart[i]=true;
                fl= true;
            }
        }
        if(fl)
            finalMint.push_back(s);
    }
}
void printFM(){
    bool fl=true;
    for(int i=0;i<finalMint.size();++i){
        if(fl==false){
            cout<<"|";
        }
        fl=false;
        string s=finalMint[i];
        bool fl2=true;
        for(int j=0; j < s.size(); ++j){
            if(s[j]=='-')continue;
            if(fl2== false)
                cout<<"&";
            fl2=false;
            if(s[j]=='0')
                cout<<"~";
            cout<<index2symbol[j];
        }
    }
    cout<<endl;
}

void help(){
    cout<<string(
    "more information:https://github.com/Rift2020/quine-mccluskey-cpp\n\n"
    "- print true assignment for given boolean expression\n"
    "qm true [boolean expression]\n\n"
    "- find the simplest expression which is equal to given boolean expression\n"
    "qm simp [boolean expression]\n\n"
    "- find the simplest expression which is equal to given truth table(.csv file)\n"
    "qm table [file path]\n"
    );
}
int main(int argc, char **argv) {
    if(argc <= 1 || argc > 3){
        help();
        return 1;
    }
    string op=*(argv + 1);
    if(argc == 2){

        help();
        if(op == "help")
            return 0;
        return 1;
    }
    if(op=="true"){
        string input=*(argv + 2);
        toInfix(input);
        toSuffix();
        calMint();
        for(string c:mint)cout<<c<<" ";
        cout<<endl;
    }
    else if(op=="simp"){
        string input=*(argv + 2);
        toInfix(input);
        toSuffix();
        calMint();
        while(generate());
        PIChart();
        printFM();
    }
    else if(op=="table"){
        inputMint(*(argv+2));
        while(generate());
        PIChart();
        printFM();
    }
    return 0;
}

