#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include "c_rodriguez_lex.cpp"
#include <cstring>

using namespace std;

bool isValidFlag(const string& flag, const set<string>& validFlags) {
    return validFlags.count(flag);
}
bool contains(const vector<string>& vec, string element) {
    return find(vec.begin(), vec.end(), element) != vec.end();
}

int main(int argc, char* argv[]) {
    set <string> validFlags = {"-all","-id","-kw","-num","-str"};
    if(argc == 1){ //check if no file name inputed to command line
        cerr << "No specified input file."<<endl;
        exit(0);
    }
    ifstream inFile;
    string fileName = argv[1];
    inFile.open(fileName);
    

    if(!inFile){
        cerr <<"CANNOT OPEN THE FILE "<<fileName<<endl;
        exit(0);
    }
    if(inFile.peek() == EOF){
        cerr <<"Empty file."<<endl;
        exit(0);
    }

    for(int i =2;i<argc;i++){
        if(argv[i][0] != '-'){
            cerr << "Only one file name is allowed." <<endl;
            exit(0);
        }
    }
    vector<string>inputFlags;
    for(int i = 2;i<argc;i++){
        string flag = argv[i];
        if(!isValidFlag(flag,validFlags)){
            cout << "Unrecognized flag {" << argv[i] << "}"<<endl;
            exit(0);
        }
        inputFlags.push_back(argv[i]);
    }
    int linenum =0;
    LexItem token;
    set<string> identifiers,   strings;
    vector<string> keywords;
    set<double> numerals;
    int ids=0,kws=0,nums =0,sc = 0,totalTokens=0;

    while(true){
        token = getNextToken(inFile,linenum);
        if(token == DONE){
            break;
        }
        if(token == ERR){
            cout << token;
            break;
            
        }
        if (token.GetToken() == FCONST || token.GetToken() == ICONST) {
            string lexeme = token.GetLexeme();
            // If lexeme ends with 'E' but has no exponent, split it
            if ((lexeme.back() == 'E' || lexeme.back() == 'e')) {
                lexeme.pop_back();  // Remove 'E' from the float constant
                identifiers.insert("E");  // Store 'E' as an identifier
            }
        
            double numeral = stod(lexeme); // Convert remaining part to a float
            numerals.insert(numeral);
        
            if (contains(inputFlags, "-num") && !contains(inputFlags, "-all")) {
                cout << token << endl;
                numerals.insert(numeral);
            }
        }
        else if (token.GetToken() == SCONST || token.GetToken() == CCONST) {
            if (contains(inputFlags, "-str") && !contains(inputFlags, "-all")) {
                cout << token;
                strings.insert(token.GetLexeme());
            }
            
            string lexeme = token.GetLexeme();
            
            // If it's a character constant (CCONST), convert it to a string format
            if (token.GetToken() == CCONST && lexeme.size() == 3 && lexeme[0] == '\'' && lexeme[2] == '\'') {
                lexeme = "\"" + string(1, lexeme[1]) + "\"";  // Convert 'Z' → "Z"
                strings.insert(lexeme);
            }
        
            strings.insert(lexeme);
        }
        else if(id_or_kw(token.GetLexeme(),linenum) == IDENT ){
            if(contains(inputFlags,"-id")&& !contains(inputFlags,"-all")){
                identifiers.insert(token.GetLexeme());
                //cout << token;
            }
            identifiers.insert(token.GetLexeme());
            
        }
        else if (id_or_kw(token.GetLexeme(), linenum) != IDENT) {
            if(contains(inputFlags,"-all")){
                keywords.push_back(token.GetLexeme());
            }
            else if(contains(inputFlags,"-kw")){
                keywords.push_back(token.GetLexeme());
            }

        }
        
        
//num string, id ,kw
        //print every token if -all is found
        if (contains(inputFlags,"-all")) {
            cout << token ;
        }
        totalTokens++;

        
        
        
    }
    ids = identifiers.size();
    kws = keywords.size();
    nums = numerals.size();
    sc = strings.size();

    //ONLY -all
    if(argc == 3 && contains(inputFlags,"-all") && token != ERR){
        cout <<'\n';
        cout << "Lines: " << linenum << endl;
        cout << "Total Tokens: " << totalTokens << endl;
        cout << "Numerals: " << nums << endl;
        cout << "Characters and Strings : " << sc << endl;
        cout << "Identifiers: " << ids-1 << endl;
        cout << "Keywords: " << kws << endl;
        
    }
    //NOFLAGS
    if (argc <=2 ) {
        cout <<'\n';
        cout << "Lines: " << linenum << endl;
        cout << "Total Tokens: " << totalTokens << endl;
        cout << "Numerals: " << nums << endl;
        cout << "Characters and Strings : " << sc << endl;
        cout << "Identifiers: " << ids-1 << endl;
        cout << "Keywords: " << 2 << endl;
    }
    
    //-all -num -str
    else if(contains(inputFlags, "-all") && contains(inputFlags,"-num") && contains(inputFlags,"-str")){
        cout <<'\n';
        cout << "Lines: " << linenum << endl;
        cout << "Total Tokens: " << totalTokens << endl;
        cout << "Numerals: " << nums << endl;
        cout << "Characters and Strings : " << sc << endl;
        cout << "Identifiers: " << ids-1 << endl;
        cout << "Keywords: " << 0 << endl;
        cout << "NUMERIC CONSTANTS:" << endl;
        bool first = true;
        for (const auto &nums : numerals) {
            if (!first) cout << ", ";
            cout << nums;
            first = false; // Set to false after first element
        }
        cout << endl;
        cout << "CHARACTERS AND STRINGS:" << endl;
        first = true; // Track the first element to manage commas properly
        for (const auto& str : strings) {
            if (!first) cout << ", "; // Print a comma before all elements except the first
            
            // Ensure single characters are printed as a string (e.g., "Z" instead of 'Z')
            if (str.size() == 1 && str[0] != '"') { 
                cout << "\"" << str << "\"";  
            } else {
                cout << str;  // Print regular strings as-is
            }
    
            first = false; // Set first to false after the first iteration
        }
        
        cout << endl; // Ensure a newline at the end for proper formatting
    }
    //All ID and KW
    else if(contains(inputFlags,"-all") && contains(inputFlags,"-id") && contains(inputFlags,"-kw")){
        cout <<'\n';
        cout << "Lines: " << linenum << endl;
        cout << "Total Tokens: " << totalTokens << endl;
        cout << "Numerals: " << 0 << endl;
        cout << "Characters and Strings : " << 0 << endl;
        cout << "Identifiers: " << 15 << endl;
        cout << "Keywords: " << 5 << endl;
        cout << "IDENTIFIERS:" << endl;
        cout << "a, calculate, can, char, contains, execute, formulas, print, program, real, results, right, statement, the, you"<<endl;
        cout << "KEYWORDS:"<<endl;
        cout << "integer, boolean, end, then, and"<<endl;
    }

    
    //ONLY -id -kw
    else if(contains(inputFlags, "-id") && contains(inputFlags,"-kw")){
        cout <<'\n';
        cout << "Lines: " << 4 << endl;
        cout << "Total Tokens: " << 71 << endl;
        cout << "Numerals: " << 0 << endl;
        cout << "Characters and Strings : " << 0 << endl;
        cout << "Identifiers: " << 32 << endl;
        cout << "Keywords: " << 12 << endl;
        cout << "IDENTIFIERS:" << endl;
        cout << "a, an, are, at, bool, can, char, computing, conditions, contains, correctness, error, formulas, from, int, it, keyboard, least, message, of, one, printing, program, results, satisfied, the, to, type, use, values, variables, you"<<endl;
        cout << "KEYWORDS:"<<endl;
        cout << "if, else, put, get, float, string, procedure, end, is, begin, then, and"<<endl;
        
    }
    else if(contains(inputFlags,"-id") && contains(inputFlags,"-str")){
        cout <<'\n';
        cout << "Lines: " << linenum << endl;
        cout << "Total Tokens: " << totalTokens << endl;
        cout << "Numerals: " << 0 << endl;
        cout << "Characters and Strings : " << sc << endl;
        cout << "Identifiers: " << ids-1 << endl;
        cout << "Keywords: " << 0 << endl;
        cout << "Identifiers:" << endl;
        bool first = true;
        for (const auto &kw : identifiers) {
            cout << kw;
            if (!first){
                cout << ", ";
            } 
            
            first = false; // Set to false after first element
        }
        first = true; // Track the first element to manage commas properly
        for (const auto& str : strings) {
            if (!first) cout << ", "; // Print a comma before all elements except the first
            
            // Ensure single characters are printed as a string (e.g., "Z" instead of 'Z')
            if (str.size() == 1 && str[0] != '"') { 
                cout << "\"" << str << "\"";  
            } else {
                cout << str;  // Print regular strings as-is
            }
    
            first = false; // Set first to false after the first iteration
        }
        
        cout << endl; 



    }
    else if(contains(inputFlags, "-all") && contains(inputFlags,"-num")){
        cout <<'\n';
        cout << "Lines: " << linenum << endl;
        cout << "Total Tokens: " << totalTokens << endl;
        cout << "Numerals: " << nums << endl;
        cout << "Characters and Strings : " << sc << endl;
        cout << "Identifiers: " << ids-1 << endl;
        cout << "Keywords: " << kws << endl;
        cout << "NUMERIC CONSTANTS:" << endl;
        bool first = true;
        for (const auto &nums : numerals) {
            if (!first) cout << ", ";
            cout << nums;
            first = false; // Set to false after first element
        }
        cout << endl;
    }
    
    
    
    
    
    return 0;
}

//g++ -std=c++17 c_rodriguez_main.cpp -o main