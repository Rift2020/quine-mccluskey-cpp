/*
 abort qm algorithm,see:https://en.wikipedia.org/wiki/Quine%E2%80%93McCluskey_algorithm
 time complexity is Exponential function
 */

#include <iostream>
#include <fstream>
#include <stack>
#include <string>
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
vector<string> genOverImplicant;

unordered_set<string> finalImplicant;
vector<int> m;//for dfs
vector<bool> X;//boolean value which you don't care whether it's 0 or 1

//change input string to (split) infix
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

//change infix to suffix
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

//change integer to binaryString whose length is equal to indexCnt
//example:x=5,indexCnt=4,it return "1010"
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

//read csv file -> Mint
void inputMint(string filename,bool rev){
    ifstream file;
    file.open(filename, ios::in);
    file>>indexCnt;
    X.resize((1<<indexCnt));
    for(int i=0;i<(1<<indexCnt);++i)X[i]=false;
    for(int i=indexCnt-1;i>=0;--i){
        string symbol;
        file>>symbol;
        symbol2index[symbol]=i;
        index2symbol[i]=symbol;
    }
	string line;
	getline(file,line);
    for(int i=0;getline(file,line)&&i<(1<<indexCnt);++i){
        char back=line.back();
        if(rev == true){
            if(back=='1')
                back='0';
            else if(back=='0')
                back='1';
        }
        if(back=='1'){
            mint.insert(binaryString(i));
        }
        else if(back=='x'||back=='X'){
            mint.insert(binaryString(i));
            X[i]=true;
        }
        else if(back!='0'){
            cerr<<"truth value should be 1 , 0 or X"<<endl;
            exit(1);
        }
    }
    file.close();
}
//calculate all minterms by suffix
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
//merge Size x Implicants to Size 2x Implicants
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
            genOverImplicant.push_back(s);
    }
    mint=nextMint;
    nextMint.clear();
    if(mint.empty())
        return false;
    return true;
}
//Implicant s -> minterms (save in m)
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
//prime implicant chart step
void PIChart(){
    bool chart[(1<<indexCnt)];
    int cnt[(1<<indexCnt)];
    int which[(1<<indexCnt)];
    memset(cnt,0,sizeof(cnt));
    memset(chart,0,sizeof(chart));
    for(int i=0;i<genOverImplicant.size();++i){
        bool fl= false;
        m.clear();
        dfs(genOverImplicant[i],0,0);
        for(int j:m){
            if(X[i]==false){
                cnt[j]++;
                which[j]=i;
            }
        }
    }
    for(int i=0;i<(1<<indexCnt);++i){
        if(cnt[i]==1){
            finalImplicant.insert(genOverImplicant[which[i]]);
            m.clear();
            dfs(genOverImplicant[which[i]],0,0);
            for(int j:m){
                chart[j]=true;
            }
        }
    }
    for(string s:genOverImplicant){
        if(finalImplicant.find(s)!=finalImplicant.end())continue;
        bool fl= false;
        m.clear();
        dfs(s,0,0);
        for(int i:m){
            if(chart[i]==false&&X[i]==false){
                chart[i]=true;
                fl= true;
            }
        }
        if(fl){
            finalImplicant.insert(s);
        }

    }
}
//print SOP boolean expression by finalImplicant
void printSOP(){
    bool fl=true;
    for(string s:finalImplicant){
        if(fl==false){
            cout<<"|";
        }
        fl=false;
        bool fl2=true;
        for(int j=s.size()-1; j >= 0; --j){
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
//print POS boolean expression by finalImplicant
void printPOS(){
    bool fl=true;
    for(string s:finalImplicant){
        if(fl==false){
            cout<<"&";
        }
        fl=false;
        string output;
        bool fl2=true;
        for(int j=s.size()-1; j >=0; --j){
            if(s[j]=='-')continue;
            if(fl2== false)
                output+="|";
            fl2=false;
            if(s[j]=='1')
                output+="~";
            output+=index2symbol[j];
        }
        if(output.size()>1)
            output="("+output+")";
        cout<<output;
    }
    cout<<endl;
}
void help(){
    cout<<string(
    "more information:https://github.com/Rift2020/quine-mccluskey-cpp\n\n"
    "- print true assignment for given boolean expression\n"
    "qm true <boolean expression>\n\n"
    "- find the simplest expression which is equal to given boolean expression\n"
    "qm simp [format] <boolean expression>\n\n"
    "default format: -SOP\n"
    "format: { -SOP | -POS }\n\n"
    "- find the simplest expression which is equal to given truth table(.csv file)\n"
    "qm table [format] <file path>\n\n"
    "default format: -SOP\n"
    "format: { -SOP | -POS }\n\n"
    "- generate a csv file for (qm table),you need write truth value for 2nd~last linei\n"
	"qm csv <variable nums> [file path]\n"
	"default file path: qm-table-sample.csv\n"
	"caution!!!: qm csv will cover this file\n"
	);
}
int main(int argc, char **argv) {
    if(argc <= 1 || argc > 4){
        help();
        return 1;
    }
    string op=*(argv + 1);
	string op2,op3;
	if(argc>=3)
		op2=*(argv+2);
    if(argc>=4)
        op3=*(argv+3);
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
        for(int i=0;i<indexCnt;++i)
            cout<<index2symbol[i]<<" ";
        cout<<endl;
        for(string c:mint)
            cout<<c<<" ";
        cout<<endl;
    }
    else if(op=="simp"){
		string input;
		if(argc==3)
			input=op2;
		else
			input=op3;
		if(argc==3||op2=="-sop"||op2=="-SOP"){
			toInfix(input);
			toSuffix();
			calMint();
			while(generate());
			PIChart();
            printSOP();
		}
		else if(op2=="-pos"||op2=="-POS"){
			input="~("+input+")";
            toInfix(input);
            toSuffix();
            calMint();
            while(generate());
            PIChart();
            printPOS();
		}
        else{
            cerr<<"unknown options "<<op2<<endl;
        }
    }
    else if(op=="table"){
        string input;
        if(argc==3)
            input=op2;
        else
            input=op3;
        if(argc==3||op2=="-sop"||op2=="-SOP"){
            inputMint(input, false);
            while(generate());
            PIChart();
            printSOP();
        }
        else if(op2=="-pos"||op2=="-POS"){
            inputMint(input, true);
            while(generate());
            PIChart();
            printPOS();
        }
        else{
            cerr<<"unknown options "<<op2<<endl;
        }
    }
	else if(op=="csv"){
		indexCnt=atoi(*(argv+2));
		if(indexCnt<=0){
			cerr<<"indexCnt should be positive"<<endl;
			exit(1);
		}
		if(indexCnt>20){
			cerr<<"indexCnt too big"<<endl;
			exit(1);
		}
		string filename="qm-table-sample.csv";
		if(argc==4)
			filename=(*(argv+3));
		ofstream outfile;
		outfile.open(filename, ios::out | ios::trunc );
		outfile<<indexCnt<<" ";
		for(int i=0;i<indexCnt;++i){
			outfile<<(char)('a'+i)<<" ";
		}
		outfile<<endl;
		for(int i=0;i<(1<<indexCnt);++i){
			string s=binaryString(i);
			reverse(s.begin(),s.end());
			outfile<<s<<","<<endl;
		}
		outfile.close();
	}
    else
        cerr<<"unknown op "<<op<<endl;
    return 0;
}

