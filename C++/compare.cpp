//20031510 scyjm2 Jingyu Ma
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

typedef vector<int> int_vec;
string name;

class cppfile
{
public:
    virtual void read_a_cppfile(string) = 0;
protected:
    int num_of_class;
    int num_of_cin;
    int num_of_cout;
    int num_of_public;
    int num_of_private;
    int num_of_protected;
    int num_of_const;
    int num_of_if;
    int length_of_file;
    int num_of_lines;
    int num_of_blocks;
    int num_of_asterisks;
    int num_of_ampersands;
    int num_of_commas;
    bool use_new;
    bool use_friend;
    bool use_static;
    bool use_argc;
};

class string_search
{
    int_vec shifts;
    void compute_shifts(const std::string &pattern);
public:
    int find_first(const std::string &text, const std::string &pattern);
    int_vec find_all(const std::string &text, const std::string &pattern);

};

// create the shift-lookup-table
void string_search::compute_shifts(const std::string &pattern)
{
    int next_shift = 0;
    shifts.clear();
    shifts.push_back(0); // shift to the first character

    // start with the second character, since the shift to the first is always 0
    for(int i = 1; i < pattern.length(); i++)
    {
        while(next_shift > 0 && pattern[next_shift] != pattern[i])
            next_shift = shifts[next_shift];

        if(pattern[next_shift] == pattern[i])
            next_shift++;

        shifts.push_back(next_shift);
    }
}

// search the string and return when the first occurrence is found
int
string_search::find_first(const std::string &text, const std::string &pattern)
{
    int next_shift = 0;
    compute_shifts(pattern);
    for(int i = 0; i < text.length(); i++)
    {
        while(next_shift > 0 && pattern[next_shift] != text[i])
            next_shift = shifts[next_shift - 1];

        if(pattern[next_shift] == text[i])
            next_shift++;

        if(next_shift == pattern.length())
            return i - (pattern.length() - 1); // found the first so return
    }
    return -1;
}

// search the string and put every occurence in a vector
int_vec string_search::find_all(const std::string &text, const std::string &pattern)
{
    int next_shift = 0;
    int_vec positions;
    compute_shifts(pattern);
    for(int i = 0; i < text.length(); i++)
    {
        while(next_shift > 0 && pattern[next_shift] != text[i])
            next_shift = shifts[next_shift - 1];

        if(pattern[next_shift] == text[i])
            next_shift++;

        if(next_shift == pattern.length())
        {
            positions.push_back(i - (pattern.length() - 1)); // found one, put in list
            next_shift = shifts[next_shift - 1]; // restart pattern with last shift
        }
    }
    return positions;
}

class file : public cppfile
{
public:
    int num_of_void;
    int num_of_int;
    int num_of_for;
    int num_of_char;
    int num_of_else;
    int num_of_while;
    int num_of_do;
    int num_of_break;
    int num_of_float;
    int num_of_double;
    int num_of_return;
    int num_of_dot;
    int num_of_arrow;
    int num_of_headers;
    int summary[32];
    void read_a_cppfile(string filename) {
        // read the file. Since the file is read like this all white-characters
        // are eaten, so a search including white-characters will fail...
        fstream fs;
        string text, temp;
        fs.open(filename, ios::in);
        while(!fs.eof()){
            fs >> temp;
            text += temp;
        }
        fs.close();

        // search the file
        string_search *search = new (string_search);
        int_vec pos_list = search->find_all(text, "class");
        this->num_of_class=pos_list.size();
        summary[0]=pos_list.size();

        search = new (string_search);
        pos_list = search->find_all(text, "cin");
        this->num_of_cin=pos_list.size();
        summary[1]=pos_list.size();

        search = new (string_search);
        pos_list = search->find_all(text, "cout");
        this->num_of_cout=pos_list.size();
        summary[2]=pos_list.size();

        search = new (string_search);
        pos_list = search->find_all(text, "public");
        this->num_of_public=pos_list.size();
        summary[3]=pos_list.size();


        search = new (string_search);
        pos_list = search->find_all(text, "private");
        this->num_of_private=pos_list.size();
        summary[4]=pos_list.size();

        search = new (string_search);
        pos_list = search->find_all(text, "protected");
        this->num_of_protected=pos_list.size();
        summary[5]=pos_list.size();

        search = new (string_search);
        pos_list = search->find_all(text, "const");
        this->num_of_const=pos_list.size();
        summary[6]=pos_list.size();

        search = new (string_search);
        pos_list = search->find_all(text, "if");
        this->num_of_if=pos_list.size();
        summary[7]=pos_list.size();

        ifstream in(filename);
        in.seekg(0, ios::end);      //设置文件指针到文件流的尾部
        streampos ps = in.tellg();  //读取文件指针的位置
        int length=ps;
        in.close();                 //关闭文件流
        summary[8]=1;


        string str;
        int n=0;
        ifstream fin(filename);
        while(fin)
        {
            getline(fin,str);
            //  cout << str << endl;
            n++;
        }
        summary[9]=n;
        this->num_of_lines=n;

        search = new (string_search);
        pos_list = search->find_all(text, "*");
        this->num_of_asterisks=pos_list.size();
        summary[10]=pos_list.size();

        search = new (string_search);
        pos_list = search->find_all(text, "&");
        this->num_of_protected=pos_list.size();
        summary[11]=pos_list.size();

        search = new (string_search);
        pos_list = search->find_all(text, ",");
        summary[12]=pos_list.size();
        this->num_of_commas=pos_list.size();

        search = new (string_search);
        pos_list = search->find_all(text, "new");
        if(pos_list.size()>=1){
            this->use_new=true;
        }else{
            this->use_new= false;
        }
        summary[13]=pos_list.size();

        search = new (string_search);
        pos_list = search->find_all(text, "friend");
        if(pos_list.size()>=1){
            this->use_friend=true;
        }else{this->use_friend= false;}
        summary[14]=pos_list.size();

        search = new (string_search);
        pos_list = search->find_all(text, "static");
        if(pos_list.size()>=1){
            this->use_static=true;
        }else{this->use_static= false;}
        summary[15]=pos_list.size();

        search = new (string_search);
        pos_list = search->find_all(text, "argc");
        if(pos_list.size()>=2){
            this->use_argc=true;
        } else{this->use_argc= false;}
        summary[16]=pos_list.size();


        search = new (string_search);
        pos_list = search->find_all(text, "{");
        this->num_of_blocks=pos_list.size();
        summary[17]=pos_list.size();

        search = new (string_search);
        pos_list = search->find_all(text, "void");
        this->num_of_void=pos_list.size();
        summary[18]=pos_list.size();

        search = new (string_search);
        pos_list = search->find_all(text, "int");
        this->num_of_int=pos_list.size();
        summary[19]=pos_list.size();

        search = new (string_search);
        pos_list = search->find_all(text, "for");
        this->num_of_for=pos_list.size();
        summary[20]=pos_list.size();

        search = new (string_search);
        pos_list = search->find_all(text, "char");
        this->num_of_char=pos_list.size();
        summary[21]=pos_list.size();

        search = new (string_search);
        pos_list = search->find_all(text, "else");
        this->num_of_else=pos_list.size();
        summary[22]=pos_list.size();

        search = new (string_search);
        pos_list = search->find_all(text, "while");
        this->num_of_while=pos_list.size();
        summary[23]=pos_list.size();

        search = new (string_search);
        pos_list = search->find_all(text, "do");
        this->num_of_do=pos_list.size();
        summary[24]=pos_list.size();

        search = new (string_search);
        pos_list = search->find_all(text, "break");
        this->num_of_break=pos_list.size();
        summary[25]=pos_list.size();

        search = new (string_search);
        pos_list = search->find_all(text, "double");
        this->num_of_double=pos_list.size();
        summary[26]=pos_list.size();

        search = new (string_search);
        pos_list = search->find_all(text, "float");
        this->num_of_float=pos_list.size();
        summary[27]=pos_list.size();

        search = new (string_search);
        pos_list = search->find_all(text, "return");
        this->num_of_return=pos_list.size();
        summary[28]=pos_list.size();

        search = new (string_search);
        pos_list = search->find_all(text, ".");
        this->num_of_dot=pos_list.size();
        summary[29]=pos_list.size();

        search = new (string_search);
        pos_list = search->find_all(text, "->");
        this->num_of_arrow=pos_list.size();
        summary[30]=pos_list.size();

        search = new (string_search);
        pos_list = search->find_all(text, "include");
        this->num_of_headers=pos_list.size();
        summary[31]=pos_list.size();








        //int summary[]={(this->num_of_lines),(this->num_of_commas)};

    }
    int getValue(){
        return this->num_of_lines;
    }
};

float comparefunc(file file_row, file file_column);

bool IsDigit(string& str)
{
    bool flag=true;
    for(unsigned int i=0; i<str.length(); i++)//length是string类的一个成员函数返回字符串的长度
    {//用for循环访问字符串中的每个字符
        if(!isdigit(str[i]))//额，这儿还是用if，这儿用while也没错，有点把while当if用的意思
        {//判断字符是否是数字，如果不是isdigit返回false，则!isdigit为true，执行后面的把flag设为false
            flag=false;
            break;
        }
    }
    return flag;//返回flag，如果字符串为数字则flag为true，否则为false
}

int main(int argc, char *argv[])
{
    int num_of_file;
    string cin_str; //声明类对象

    while(true) {
        loop_begin:cout << "Enter the number of cpp files to compare (no more than 15):" ;
        cin >> cin_str;
        if(IsDigit(cin_str)){
            num_of_file=stoi(cin_str);
            if (num_of_file > 15 ) {
                cout << endl;
                cout << "Must less than 15 files!" << endl;
                cout << endl;
                continue;
            } else if(num_of_file<=1){
                cout << endl;
                cout << "Have some mind, don't input unreasonable numbers!" << endl;
                cout << endl;
            }
            else{
                break;
            }
        }else{
            cout << "\nInput must be a  number!\n\n" ;
            goto loop_begin;
        }
    }

    float similarity_matrix[num_of_file][num_of_file];
    // memset(similarity_matrix,0,sizeof(similarity_matrix));
    file files[num_of_file];


    // read the file. Since the file is read like this all white-characters
    // are eaten, so a search including white-characters will fail...
    for(int i=0;i<num_of_file;i++){
        back_to_loop:  cout << "Enter the name of file "<<i<<":" ;
        cin>>name;
        if(name=="cpp"){
            cout << "What are you doing up there?\n";
            goto back_to_loop;
        }
        if(name.find("cpp")==name.npos)//在a中查找c，不存在
        {
            cout << "Wrong file please input\n";
            goto back_to_loop;
        }
        else//否则存在
        {
            files[i].read_a_cppfile(name);
            //  cout << files[0].summary[1] << endl;
        }
    }

    for(int i=0;i<num_of_file;i++){
        for(int j=0;j<num_of_file;j++){
            similarity_matrix[i][j]=comparefunc(files[i],files[j]);
        }
    }
    // file a;
    //a.read_a_cppfile("a.cpp");
    // print out result
    cout<<"Simalarity matrix:"<<endl;
    for(int i=0;i<num_of_file;i++){
        for(int j=0;j<num_of_file;j++){
            cout.setf(ios::fixed);
            cout <<fixed<< setprecision(5) << similarity_matrix[i][j]<<" " ; //输出a=0.2
        }
        cout<<endl;
    }
    //cout<<files[0].summary[8]<<endl;

}

float comparefunc(file file_row, file file_column) {
    float numerator=0;
    float denominator=0;

    for(int i=0;i<32;i++){
        if(file_row.summary[i]>=file_column.summary[i]){
            numerator+=file_column.summary[i];
            denominator+=file_row.summary[i];
        }else{
            numerator+=file_row.summary[i];
            denominator+=file_column.summary[i];
        }
    }



    return (numerator/denominator);
}
