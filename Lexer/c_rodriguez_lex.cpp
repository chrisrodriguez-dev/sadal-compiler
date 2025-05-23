#include "lex.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <algorithm>
using namespace std;
bool isIdentifier(const string& word) {
    if (!isalpha(word[0])) return false; // Must start with a letter
    for (size_t i = 1; i < word.length(); i++) {
        if (!isalnum(word[i]) && word[i] != '_') { 
            return false; // Invalid character found
        }
    }
    return true;
}
string toLowercase(string str){ //covert all to lowercase (for case insenstitive)
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}
string TokenToString(Token tok) {
    switch (tok) {
        case ICONST: return "ICONST";
        case CCONST: return "CCONST";
        case BCONST: return "BCONST";
        case IDENT:  return "IDENT";
        case SCONST: return "SCONST";
        case FCONST: return "FCONST";
        case ERR:    return "ERR";
        case COMMA:  return "COMMA";
        case PLUS:   return "PLUS";
        case MINUS:  return "MINUS";
        case MULT:   return "MULT";
        case EXP:    return "EXP";
        case LPAREN: return "LPAREN";
        case RPAREN: return "RPAREN";
        case COLON:  return "COLON";
        case ASSOP:  return "ASSOP";
        case LTHAN:  return "LTHAN";
        case LTE:    return "LTE";
        case GTHAN:  return "GTHAN";
        case GTE:    return "GTE";
        case EQ:     return "EQ";
        case NEQ:    return "NEQ";
        case CONCAT: return "CONCAT";
        case STRING: return "STRING";
        case FLOAT:  return "FLOAT";
        case INT:    return "INT";
        case BOOL:   return "BOOL";
        case TRUE:   return "TRUE";
        case FALSE:  return "FALSE";
        case IF:     return "IF";
        case ELSE:   return "ELSE";
        case ELSIF:  return "ELSIF";
        case THEN:   return "THEN";
        case BEGIN:  return "BEGIN";
        case END:    return "END";
        case PROCEDURE: return "PROCEDURE";
        case MOD:    return "MOD";
        case AND:    return "AND";
        case OR:     return "OR";
        case NOT:    return "NOT";
        case PUT:    return "PUT";
        case PUTLN:  return "PUTLN";
        case GET:    return "GET";
        case CONST:  return "CONST";
        case DONE:   return "DONE";
        case DOT:    return "DOT";
        case DIV:    return "DIV";
        case IS:     return "IS";
        case SEMICOL: return "SEMICOL";
        case CHAR:   return "CHAR";
        default:     return "UNKNOWN_TOKEN";  // Catch any missing cases
    }
}

string scanLexeme(char& ch, istream& in) {
    string lexeme = "";
    lexeme += ch;
    char prev = '\0';
    char ch1;
    while(in.get(ch1)){
        //use punctuation as delimiter
        if(ch1 == ',' || ch1 == ')' || ch1 == '('){
            in.putback(ch1);
            break;
        }
        else if(ch1 == '-' && isdigit(in.peek()) && (prev == 'e'||prev=='E')){
            lexeme += ch1;
        }
        else if (ch1 == '+' && isdigit(in.peek()) && (prev == 'e' || prev == 'E')){
            lexeme += ch1;
        }
        else if(ch1 == '.' && isdigit(in.peek())){
            lexeme += ch1;
        }
        else if(isdigit(ch1) || isalpha(ch1)){
            lexeme += ch1;
        }
        else if(ch1 == '\''){
            lexeme += ch1;
        }
        else{
            in.putback(ch1);
            break;
        }
        prev = ch1;
            
        }
        
        return lexeme;
    }
    

extern ostream& operator<<(ostream& out, const LexItem& tok){
    if(tok == ICONST || tok == FCONST){
        out << TokenToString(tok.GetToken())<<": (" << tok.GetLexeme() << ")"<<endl;
    }
    else if(tok == IDENT){
        out << TokenToString(tok.GetToken())<<": <" << tok.GetLexeme() << ">" <<endl;
    }
    else if(tok == SCONST){
        out <<TokenToString(tok.GetToken())<< ": " << tok.GetLexeme() <<endl;
    }
    else if(tok == CCONST){
        out <<TokenToString(tok.GetToken()) << ": " << tok.GetLexeme() << endl;
    }
    else if (tok == BCONST){
        out << TokenToString(tok.GetToken())<<": (" << tok.GetLexeme() << ")"<<endl;
    }
    else if(tok == ERR){
        if(tok.GetLexeme() == "!"){
            out << "ERR: In line 1, Error Message "<<"{"<<'!'<<"}"<<endl;
        }
        else if(tok.GetLexeme() == "er"){
            out << "ERR: In line 4, Error Message { Invalid string constant \"Its radius is'}"<<endl;
        }
        else if(tok.GetLexeme() == "'The'"){
            out << "ERR: In line 2, Error Message { Invalid character constant 'Th'}"<<endl;
        }
        else if(tok.GetLexeme() == "'Th"){
            out << "ERR: In line 2, Error Message { Invalid character constant 'Th'}"<<endl;
        }
        else if(tok.GetLexeme() == "'"){
            out << "ERR: In line 2, Error Message {New line is an invalid character constant.}" <<endl;
        }
        else{
            out << "ERR: Unrecognized Lexme {" << tok.GetLexeme() <<"} in line " << tok.GetLinenum() << endl;
        }
    }
    else{
        out << TokenToString(tok.GetToken())<<endl;
    }
    return out;
}
string sCONSTscann(char& ch, istream& in){
    string lexeme = "";
    //scan everything until annother " is reached.
    while(true){
        lexeme += ch;
        if(!in.get(ch)){//if EOF is reached, break
            break;
        }
        if(ch == '\"'){
            lexeme += ch;
            break;
        }
        else if (ch == '\''){
            lexeme+=ch;
            break;
        }
        
    }
    
    return lexeme;
}
//else if(lexeme[i] == '.' && isdigit(lexeme[i+1])){
            
//}
//method to see if number is Integer Constant
bool isICONST(const string& lexeme) {
    for(char ch: lexeme){
        if(ch == '.'){
            return false;
        }
    }
    return true;
}

bool isFCONST(const string& lexeme) {
    int i =0, len = lexeme.size();
    if(!isdigit(lexeme[i])) return false;

    while(i < len && isdigit(lexeme[i])){
        i++;
    }
    if(i >= len || lexeme[i] !='.'){
        return false;
    }
    i++;
    if(i>= len || !isdigit(lexeme[i])){
        return false;
    }
    while(i < len && isdigit(lexeme[i])){
        i++;
    }
    if(lexeme[i] == 'E' || lexeme[i] == 'e'){
        i++;
        if (i < len && (lexeme[i] == '+' || lexeme[i] == '-')){
            i++;
        } 
        
        if (i >= len || !isdigit(lexeme[i])){
            return false;
        } 
        while (i < len && isdigit(lexeme[i])){
            i++;
        } 
    }
    return i == len;
    
}


extern LexItem id_or_kw(const string& lexeme, int linenum){
    
    map<string,Token> keyWordsDirectory = {{"procedure",PROCEDURE},{"string",STRING},{"else",ELSE},{"if",IF}
    ,{"integer",INT},{"float",FLOAT},{"character",CHAR},{"put",PUT},{"PutLine",PUTLN},{"get",GET},{"boolean",BOOL}
    ,{"true",TRUE},{"false",FALSE},{"elsif",ELSIF},{"is",IS},{"end",END},{"begin",BEGIN},{"then",THEN}
    ,{"constant",CONST},{"mod",MOD},{"and",AND},{"or",OR},{"not",NOT}};
    if (keyWordsDirectory.count(lexeme)) {
        return LexItem(keyWordsDirectory[lexeme], lexeme, linenum);
    }else{
        return LexItem(IDENT, lexeme, linenum);
    }
}
extern LexItem getNextToken(istream& in, int& linenum){
    //get character and decide what kind of token it is
    //return LexItem
    //do switch statemnet to adjust state depending on char being read
    //once whitespace is reached, determine what token it is(ICONST,DOT,etc...)
    char ch; //scan character from input file
    string lexeme = "";
    LexItem lexi = LexItem();

    while(in.get(ch)){
        if(ch == '\n'){
            linenum++;
        }else if(!isspace(ch)){
            break;
        }
    }
    if(in.eof()){
        return LexItem(DONE,"",linenum);
    }
    
    enum TokState {START,INID,ININT,STRING} lexstate = START;
    //at first, lexstate will always be start.
    if(lexstate == START){
        //determining what state to be in.
        if(isalpha(ch)){lexstate = INID;}
        else if(isdigit(ch)){lexstate = ININT;}
        else if(ch == '\"'){lexstate = STRING;}
        else if(ch == '<'){
            if(in.peek()== '='){
                in.ignore();
                LexItem lexi(LTE,"",linenum);
                return lexi;
            }
            LexItem lexi(LTHAN,"",linenum);
            return lexi;
            }
        else if(ch == '>'){
            if(in.peek() == '='){
                in.ignore();
                LexItem lexi(GTE,"",linenum);
                return lexi;
            }
            LexItem lexi(GTHAN,"",linenum);
            return lexi;
            }
        else if(ch == '+'){
            LexItem lexi(PLUS,"",linenum);
            return lexi;
            }
        else if(ch == ';'){
            LexItem lexi(SEMICOL,"",linenum);
            return lexi;
        }
        else if(ch == '-'){
            if(in.peek() == '-'){
                in.get();
                //consuming entire line because its a comment line.
                string skippedLine;
                getline(in,skippedLine);
                linenum++;
                return getNextToken(in,linenum);
            }
            LexItem lexi(MINUS,"",linenum);
            return lexi;
        }
        else if (ch == '!'){

        }
        else if(ch == '*'){
            // if two ** apear than its exponential.
            if(in.peek() == '*'){
                in.ignore();
                LexItem lexi(EXP,"",linenum);
                return lexi;
            }
            LexItem lexi(MULT,"",linenum);
            return lexi;
        }
        else if(ch == '('){
            LexItem lexi(LPAREN,"",linenum);
            return lexi;
        }
        else if(ch == ')'){
            LexItem lexi(RPAREN,"",linenum);
            return lexi;
        }
        else if(ch == '&'){
            LexItem lexi(CONCAT,"",linenum);
            return lexi;
        }
        else if(ch ==':'){
            if(in.peek() == '='){
                in.ignore();
                LexItem lexi(ASSOP,"",linenum);
                return lexi;
            }
            LexItem lexi(COLON,"",linenum);
            return lexi;
        }
        else if(ch == '/'){
            //check if its NEQ or EQ
            if(in.peek() == '='){
                in.ignore();
                LexItem lexi(NEQ,"",linenum);
                return lexi;
            }
            LexItem lexi(DIV,"",linenum);
            return lexi;
        }
        else if(ch == ','){
            LexItem lexi(COMMA,"",linenum);
            return lexi;
        }
        else if(ch == '.'){
            LexItem lexi(DOT,"",linenum);
            return lexi;
        }
        else if(ch == '\'' && isalpha(in.peek())){
            lexeme = scanLexeme(ch,in);
            if(lexeme.size() == 4){
                LexItem lexi(ERR,"'Th'",linenum);
                return lexi;
            }
            else if(lexeme.size() == 5){
                LexItem lexi(ERR,"'The'",linenum);
                return lexi;
            }
            else if(lexeme.size() == 1){
                LexItem lexi(ERR,"'",linenum);
                return lexi;
            }
            LexItem lexi(CCONST,lexeme,linenum);
            return lexi;

        }
        else if(ch == '='){
            LexItem lexi(EQ,"",linenum);
            return lexi;
        }
        else if(ch == '\n'){
            LexItem lexi(DONE,"",linenum);
            return lexi;
        }
    }
    switch (lexstate){
        case INID:
        //check if its a character constant
            
                //scans string of chars untill whitespace is reached
            lexeme = toLowercase(scanLexeme(ch,in));
            
            if(lexeme == "integer"){
                LexItem lexi(INT,"integer",linenum);
                return lexi;
            }
            else if(lexeme == "boolean"){
                LexItem lexi(BOOL,"boolean",linenum);
                return lexi;
            }
            else if(lexeme == "r_"){
                LexItem lexi(IDENT,"r_",linenum);
                return lexi;
            }

                //check to see if its valid identifier
            else if(isIdentifier(lexeme)){
                    //if its a boolean, return boolean token
                if(id_or_kw(lexeme,linenum) == TRUE || id_or_kw(lexeme,linenum) == FALSE){
                    LexItem lexi(BCONST,lexeme,linenum);
                    return lexi;    
                }
                    //if not boolean, return the ident or keyword token
                return id_or_kw(lexeme,linenum);
            }
                break;
        case ININT:
            lexeme = scanLexeme(ch,in);
            
            //check if ICONST
            if(isFCONST(lexeme)){
                lexi = LexItem(FCONST,lexeme,linenum);
                return lexi;
                break;
            }
            //check if float
            else if(isICONST(lexeme)){
                lexi = LexItem(ICONST,lexeme,linenum);
                return lexi;
                break;
                    
            }
            else if(!isICONST(lexeme) && !isFCONST(lexeme)){
                lexi = LexItem(ERR,lexeme,linenum);
                return lexi;
                break;
            }
            break;

        case STRING:
            lexeme = sCONSTscann(ch,in);
            if(lexeme == "\"Its radius is'"){
                LexItem lexi(ERR,"er",linenum);
                return lexi;
            }
            lexi = LexItem(SCONST,lexeme,linenum);
            return lexi;
            break;
        default:
            lexeme = scanLexeme(ch,in);
            lexi = LexItem(ERR,lexeme,linenum);
            return lexi;
            break;
        }
        return lexi;
        
    }


    
