/* Implementation of Recursive-Descent Parser
	for the Simple Ada-Like (SADAL) Language
 * parser.cpp
 * Programming Assignment 2
 * Spring 2025
*/
#include <queue>
#include "parser.h"


map<string, bool> defVar;

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

//StmtList ::= Stmt { Stmt }
bool StmtList(istream& in, int& line)
{
    bool status = true;
    LexItem tok;

    while (true) {
        tok = Parser::GetNextToken(in, line);
        // check if we reached the end of the statement list
        if (tok == END){
			tok = Parser::GetNextToken(in,line);
			if(tok == IF){
				tok = Parser::GetNextToken(in,line);
				if(tok == SEMICOL){
					break;
				}
			}else{
				Parser::PushBackToken(tok);
				return true;
			}
		}
		if(tok == ELSIF || tok == ELSE){
			Parser::PushBackToken(tok);
			return true;
		}
        
        // push back the token to let Stmt() handle it
        Parser::PushBackToken(tok);
        
        // process the statement
        status = Stmt(in, line);
        
        if (!status) {
            // break if a statement parsing fails
			return false;
        }

        // check for a semicolon after a valid statement
        tok = Parser::GetNextToken(in, line);
        

        if (tok == SEMICOL) {
            // continue to the next statement if semicolon is found
            continue;
        } else if (tok == END || tok == ELSIF || tok == ELSE) {
            // push back these tokens and break, letting ProcBody handle them
            Parser::PushBackToken(tok);
            break;
        } else {
            // if it's not a semicolon, END, ELSIF, or ELSE, push it back for error handling
            Parser::PushBackToken(tok);
        }
    }

    return true; // returning true here means the StmtList parsed successfully
}


//Type ::= INTEGER | FLOAT | BOOLEAN | STRING | CHARACTER;
bool Type(istream& in, int& line){
	LexItem tok = Parser::GetNextToken(in, line);
	if(tok == INT || tok == FLOAT || tok == BOOL || tok == STRING || tok == CHAR || tok.GetLexeme() == "bool"){
		return true;
	}else{
		
		Parser::PushBackToken(tok);  
		ParseError(line, "Incorrect Declaration Type.");
		return false;
	}
	
}
//End of StmtList
//DeclStmt ::= IDENT {, IDENT } : [CONSTANT] Type [(Range)] [:= Expr] ;
bool DeclStmt(istream& in, int& line) {
    LexItem tok;
    tok = Parser::GetNextToken(in, line);
	
    if (tok != IDENT) {
        ParseError(line, "Incorrect identifiers list in Declaration Statement.");
        return false;
    }
	defVar[tok.GetLexeme()] = true;
	
    while (true) {
        tok = Parser::GetNextToken(in, line);
        if (tok == COLON){
            break;
		}
		if(id_or_kw(tok.GetLexeme(),line) != IDENT){
			string message = "Invalid name for an Identifier:(" + tok.GetLexeme()+")";
			ParseError(line,message);
			ParseError(line,"Incorrect identifiers list in Declaration Statement.");
			return false;
		}
		
        if (tok != COMMA) {
            ParseError(line, "Missing comma in declaration statement.");
			ParseError(line,"Incorrect identifiers list in Declaration Statement.");
            return false;
        }
        tok = Parser::GetNextToken(in, line);
        if (tok != IDENT) {
            ParseError(line, "Incorrect identifiers list in Declaration Statement.");
            return false;
        }
		if(defVar.count(tok.GetLexeme())){
			ParseError(line,"Variable Redefinition");
			ParseError(line,"Incorrect identifiers list in Declaration Statement.");
			return false;
		}
		defVar[tok.GetLexeme()] = true;
		
    }
	
	tok = Parser::GetNextToken(in,line);
	
	if(tok == CONST){
		tok = Parser::GetNextToken(in,line);
	}
	if(!(tok == INT || tok == STRING || tok == FLOAT || tok == CHAR || tok == BOOL)){
		
		ParseError(line,"Incorrect Declaration Type.");
		return false;
	}
	tok = Parser::GetNextToken(in,line);
	
    if (tok == LPAREN) {
        if (!Range(in, line)) {
            ParseError(line, "Incorrect or missing range statement.");
            return false;
        }
        tok = Parser::GetNextToken(in, line);
        if (tok != RPAREN) {
            ParseError(line, "Missing Right Parenthesis");
			
            return false;
        }
        tok = Parser::GetNextToken(in, line);
    }

    if (tok == ASSOP) {
        if(Expr(in,line)){
			tok = Parser::GetNextToken(in,line);
			
		}
		else{
			ParseError(line,"Missing expresion after :=.");
			return false;
		}
    }
	if(tok == SEMICOL){
		return true;
	}
    if (tok != SEMICOL) {
        Parser::PushBackToken(tok);
		
        ParseError(line, "Missing semicolon at end of statement");
        return false;
    }

    return true;
}

//DeclPart ::= DeclStmt { DeclStmt }
bool DeclPart(istream& in, int& line) {
	
	LexItem tok;
	if(!DeclStmt(in, line)){
		ParseError(line, "Non-recognizable Declaration Part.");
		return false;
	}
	
	while(true){
		tok = Parser::GetNextToken(in, line);
		if(tok == BEGIN){
			Parser::PushBackToken(tok);
			return true;
		}
		Parser::PushBackToken(tok); 
		if(!DeclStmt(in, line)){
			ParseError(line, "Non-recognizable Declaration Part.");
			return false;
		}
	}

	return true;
}//end of DeclPart function


bool ProcName(istream& in, int& line){
	LexItem tok = Parser::GetNextToken(in,line);
	// automatically return false
	if(tok != IDENT){
		return false;
	}
	if(tok.GetLexeme() != "prog1"){
		defVar[tok.GetLexeme()] = true;
	}
	
	return true;
}
//ProcBody ::= DeclPart BEGIN StmtList END ProcName ;
bool ProcBody(istream& in, int& line){
	LexItem tok;
	
	// parse the declaration part
	if(!DeclPart(in,line)){
		
		return false;
	}
	// check for the BEGIN keyword
	tok = Parser::GetNextToken(in,line);
	
	if(tok != BEGIN){
		ParseError(line,"Missing Begin.");
		return false;
	}
	// parse the statement list
	if(!StmtList(in,line)){
		return false;
	}
	// get the next token which should be END
	tok = Parser::GetNextToken(in,line);
	if(tok != END){
		ParseError(line,"Missing closing END IF for If-statement.");
		ParseError(line,"Invalid If statement.");
		line++;
		ParseError(line,"Syntactic error in statement list.");
		return false;
	}
	// ensure the Procedure name follows END
	if(!ProcName(in,line)){
		ParseError(line, "Missing Procedure Name.");
		return false;
	}
	return true;
}

//Prog ::= PROCEDURE ProcName IS ProcBody
bool Prog(istream& in, int& line){
	LexItem tok = Parser::GetNextToken(in,line);
	bool status = false;
	if(tok != PROCEDURE){
		ParseError(line,"Incorrect compilation file.");
		return false;
	}
	status = ProcName(in,line);
	if(!status){
		ParseError(line,"Missing Procedure Name.");
		return false;
	}
	tok = Parser::GetNextToken(in,line);
	if(tok != IS){
		ParseError(line,"Missing IS statement");
		return false;
	}
	if(!ProcBody(in,line)){
		ParseError(line,"Incorrect procedure body.");
		ParseError(line,"Incorrect Procedure Definition.");
		return false;
	}
	
	cout << "Declared Variables:"<<endl;
	bool first = true;
	for(auto it = defVar.begin(); it != defVar.end(); ++it){
    if(!first) {
        cout << ", ";
    }
    cout << it->first;
    first = false;
	}
	cout <<"\n";
	cout <<"\n";
	cout << "(DONE)"<<endl;
	return true;
}

// Stmt ::= AssignStmt | PrintStmts | GetStmt | IfStmt
bool Stmt(istream& in, int& line) {
    bool status = false;
    LexItem tok = Parser::GetNextToken(in, line);
	// push it back so we can inspect it without consuming it
    Parser::PushBackToken(tok); 
    // checking what kind of statement we are dealing with based on the token
    if (tok == GET) {
        status = GetStmt(in, line);
    }
    else if (tok == PUT || tok == PUTLN) {
        status = PrintStmts(in, line);
    }
    else if (tok == IF) {
        status = IfStmt(in, line);
    }
    else if (tok == IDENT) {
        status = AssignStmt(in, line);
    }
    else {
        return false;
    }
	if (status) {
        return true;
    }
    return false;
}


// PrintStmts ::= (PutLine | Put) ( Expr) ;
bool PrintStmts(istream& in, int& line){
	LexItem tok = Parser::GetNextToken(in,line);
	if(tok != PUT && tok != PUTLN){
		ParseError(line,"Missing Put or PutLine in PrintStmts.");
		return false;
	}
	tok = Parser::GetNextToken(in,line);
	if(tok == LPAREN){
		if(Expr(in,line)){
			tok = Parser::GetNextToken(in,line);
			
			if(tok == RPAREN){
				tok = Parser::GetNextToken(in,line);
				
				if(tok != SEMICOL){
					Parser::PushBackToken(tok);
					//this tok is on the next line.
					line--;
					ParseError(line,"Missing semicolon at end of statement");
					ParseError(line,"Invalid put statement.");
					ParseError(line,"Syntactic error in statement list.");
					return false;
				}
			}else{
				ParseError(line,"Missing Right Parenthesis");
				ParseError(line,"Invalid put statement.");
				line++;
				ParseError(line,"Syntactic error in statement list.");
				return false;
			}
		}
	}else{
		ParseError(line,"Missing Left Parenthesis");
		ParseError(line,"Invalid put statement.");
		ParseError(line,"Syntactic error in statement list.");

		return false;
	}
    return true;
}

// GetStmt := Get (Var) ;
bool GetStmt(istream& in, int& line) {
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok != GET) {
		// push it back since it's not a GET statement
		Parser::PushBackToken(tok);  
		ParseError(line,"Missing Get word");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	
	if (tok != LPAREN){
		ParseError(line,"Missing Left Parenthesis");
		return false;
	}

	if(Var(in,line)){
		tok = Parser::GetNextToken(in,line);
		
		if(tok != RPAREN){
			ParseError(line,"Missing Right Parenthesis");
			return false;
		}else{
			return true;
		}
	} else {
		ParseError(line,"Incorrect Var.");
    	return false;
	}
	return true;
}



// IfStmt ::= IF Expr THEN StmtList { ELSIF Expr THEN StmtList } [ ELSE StmtList ] END IF ;
bool IfStmt(istream& in, int& line) {
	LexItem tok;
	tok = Parser::GetNextToken(in,line);
	
	if(tok != IF){
		ParseError(line, "Missing IF in IfStmt");
		return false;
	}

	if(Expr(in,line)){
		tok = Parser::GetNextToken(in,line);
		tok = Parser::GetNextToken(in,line);
		if(tok == IDENT){
			Parser::PushBackToken(tok);
			return true;
		}
		if(tok != THEN ){
			ParseError(line,"Missing THEN in IfStmt");
			return false;
		}
	
	}else{
		return false;
	}
	if(!StmtList(in,line)){
		return false;
	}
	while(true){
		tok = Parser::GetNextToken(in,line);
		
		if(tok == ELSE || tok.GetLexeme()=="else"){
			Parser::PushBackToken(tok);
			break;
		}
		if(tok != ELSIF){
			Parser::PushBackToken(tok);
			return true;
		}
		
		
		if(Expr(in,line)){
			tok = Parser::GetNextToken(in,line);
			tok = Parser::GetNextToken(in,line);
			if(tok == THEN){
				if(!StmtList(in,line)){
					break;
				}
			}
		}

	}
	
	tok = Parser::GetNextToken(in,line);
	if(tok == PUT){
		Parser::PushBackToken(tok);
		return true;
	}
	
	if(tok == ELSE){
		if(!StmtList(in,line)){
			ParseError(line,"Missing StmtList after ELSE");
			return false;
		}
		
	}
	if(tok == ELSE){
		return true;
	}
	
	if(tok != END ){
		ParseError(line,"Missing closing END IF for If-statement.");
		ParseError(line,"Invalid If statement.");
		line++;
		ParseError(line,"Syntactic error in statement list.");
		return false;
	}
	tok = Parser::GetNextToken(in,line);
	if(tok != IF){
		ParseError(line,"Missing ending IF in IfStmt");
		return false;
	}
    return true;
}

// AssignStmt ::= Var := Expr ;
bool AssignStmt(istream& in, int& line) {
	LexItem tok;
	if(!Var(in,line)){
		ParseError(line,"Missing Var for AssignStmt.");
		return false;
	}
	tok = Parser::GetNextToken(in,line);
	if(tok != ASSOP){
		ParseError(line,"Missing Assignment Operator");
		ParseError(line,"Invalid assignment statement.");
		ParseError(line,"Syntactic error in statement list.");
		return false;
	}
	if(!Expr(in,line)){
		ParseError(line,"Missing Expression in Assignment Statement");
		ParseError(line,"Invalid assignment statement.");
		ParseError(line,"Syntactic error in statement list.");
		return false;
	}

    return true;
}

// Var ::= IDENT
bool Var(istream& in, int& line) {
	LexItem tok = Parser::GetNextToken(in,line);
	
	
	if(tok == IDENT){
		defVar[tok.GetLexeme()] = true;
		return true;
	}else{
		Parser::PushBackToken(tok);
		
		ParseError(line,"Incorrect Var");
    	return false;
	}
	return true;
	
}

// Expr ::= Relation {(AND | OR) Relation }
bool Expr(istream& in, int& line) {
    LexItem tok;
    if (!Relation(in, line)) {
        return false;
    }
    while(true) {
        tok = Parser::GetNextToken(in, line);
        if(tok == AND || tok == OR) {
            if(!Relation(in, line)){
                ParseError(line, "Missing Relation after logical operator in Expr.");
                return false;
            }
        } 
        else {
            Parser::PushBackToken(tok);
            break;
        }
    }
    return true;
}

// Relation ::= SimpleExpr [ ( = | /= | < | <= | > | >= ) SimpleExpr ]
bool Relation(istream& in, int& line) {
	if (!SimpleExpr(in, line)) {
		return false;
	}
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == EQ || tok == NEQ || tok == LTHAN || tok == LTE || tok == GTHAN || tok == GTE) {
		if (!SimpleExpr(in, line)) {
			ParseError(line, "Missing SimpleExpr after comparison operator.");
			return false;
		}
		tok = Parser::GetNextToken(in,line);
		
		if(tok == THEN){
			Parser::PushBackToken(tok);
			return true;
		}
		if(tok != RPAREN){
			ParseError(line,"Missing right parenthesis after expression");
			ParseError(line,"Incorrect operand");
			ParseError(line,"Missing if statement condition");
			ParseError(line,"Invalid If statement.");
			ParseError(line,"Syntactic error in statement list.");
			return false;
		}else{
			Parser::PushBackToken(tok);
		}
	} else {
		Parser::PushBackToken(tok);  
	}
	return true;
}


// SimpleExpr ::= STerm { ( + | - | & ) STerm }
bool SimpleExpr(istream& in, int& line) {
	LexItem tok;
	if (!STerm(in, line)) {
		return false;
	}
	while (true) {
		tok = Parser::GetNextToken(in, line);
		if (tok == PLUS || tok == MINUS || tok == CONCAT) {
			if (!STerm(in, line)) {
				ParseError(line, "Missing STerm after operator.");
				return false;
			}
		} else {
			Parser::PushBackToken(tok);
			break;  
		}
	}
	return true;
}


// STerm ::= [ ( + | - ) ] Term
bool STerm(istream& in, int& line) {
    LexItem tok;
    tok = Parser::GetNextToken(in, line);
	
	
    if (tok == PLUS || tok == MINUS) {
        if (!Term(in, line, 0)) {
            ParseError(line, "Missing Term after sign in STerm.");
            return false;
        }
    } else {
        Parser::PushBackToken(tok);
        if (!Term(in, line, 0)) {         
            return false;
        }
    }
    return true;  
}

// Term ::= Factor { ( * | / | MOD ) Factor }
bool Term(istream& in, int& line, int sign) {
	LexItem tok;
	if(!Factor(in,line,0)){		
		return false;
	}
	while(true){
		tok = Parser::GetNextToken(in,line);
		if(tok == MULT || tok == DIV || tok == MOD){
			if(!Factor(in,line,0)){
				ParseError(line,"Incorrect operand");
				ParseError(line,"Missing operand after operator");
				return false;
			}
		}else{
			Parser::PushBackToken(tok);
			break;
		}
	}

    return true;
}

// Factor ::= Primary [** [(+ | -)] Primary ] | NOT Primary
bool Factor(istream& in, int& line, int sign) {
    LexItem tok;
    tok = Parser::GetNextToken(in, line);
    if(tok == NOT){
        if(Primary(in, line, 0)){
            return true;
        } else {
            ParseError(line, "Expected Primary after NOT.");
            return false;
        }
    } 
    else {
    	Parser::PushBackToken(tok); 
    }
    if(!Primary(in, line, 0)){
        //ParseError(line, "Missing Primary in Factor.");
        return false;
    }
    tok = Parser::GetNextToken(in, line);
    if(tok == EXP){
		
        
        tok = Parser::GetNextToken(in, line);
        if(tok != PLUS && tok != MINUS){
            Parser::PushBackToken(tok); 
        }
       
        if(!Primary(in, line, 0)){
            ParseError(line, "Expected Primary after exponentiation operator '**'.");
            return false;
        }
    } 
    else {
        Parser::PushBackToken(tok); 
    }
    return true; 
}



// Primary ::= Name | ICONST | FCONST | SCONST | BCONST | CCONST | (Expr)
bool Primary(istream& in, int& line, int sign) {
    LexItem tok = Parser::GetNextToken(in,line); 
    if(tok == ICONST || tok == FCONST || tok == SCONST || tok == BCONST || tok == CCONST) {
        return true;
    }else{
		Parser::PushBackToken(tok);
	}
	if(Name(in,line)) {
        return true;
    }  
    if(tok == LPAREN){
        if(!Expr(in, line)){
            ParseError(line, "Incorrect Expression within parentheses.");
            return false;
        }
        
        tok = Parser::GetNextToken(in, line);
        if(tok == RPAREN){
			tok = Parser::GetNextToken(in,line);
            return true;
        } else {
            ParseError(line, "Missing right parenthesis after expression");
            return false;
        }
    }
    return false;
}



// Name ::= IDENT [ ( Range ) ]
bool Name(istream& in, int& line) {
    LexItem tok = Parser::GetNextToken(in, line);
	if(tok == LPAREN){
		tok = Parser::GetNextToken(in,line);
	}
	if(tok == SEMICOL){
		ParseError(line,"Invalid Expression");
		ParseError(line,"Incorrect operand");
		ParseError(line,"Missing if statement condition");
		ParseError(line,"Invalid If statement.");
		ParseError(line,"Syntactic error in statement list.");
		ParseError(line,"Missing Statement for If-Stmt Then-clause");
		ParseError(line,"Invalid If statement.");
		line++;
		ParseError(line,"Syntactic error in statement list.");
		return false;
	}
	if(!defVar.count(tok.GetLexeme())){
		ParseError(line,"Using Undefined Variable");
		ParseError(line,"Invalid reference to a variable.");
		return false;
	}
    if(tok == IDENT){
        tok = Parser::GetNextToken(in, line);
        if(tok == LPAREN) {
            if(!Range(in, line)) {  
                ParseError(line, "Invalid Range within parentheses.");
                return false;
            }
            tok = Parser::GetNextToken(in, line);
            if(tok != RPAREN) {
                ParseError(line, "Missing Right Parenthesis after Range.");
                return false;
            }
            return true;  
        } 
        else { 
            Parser::PushBackToken(tok);
            return true;  
        }
    }
    ParseError(line, "Missing IDENT in NAME.");
    return false;
}

// Range ::= SimpleExpr [. . SimpleExpr ]
bool Range(std::istream& in, int& line) {
    LexItem tok;
    if(!SimpleExpr(in, line)) {
        ParseError(line, "Invalid starting SimpleExpr in Range.");
        return false;
    }
	tok = Parser::GetNextToken(in,line);
	if(tok == DOT){
		tok = Parser::GetNextToken(in,line);
		if(tok == DOT){
			if(SimpleExpr(in,line)){
				return true;
			}
		}

	}else{
		Parser::PushBackToken(tok);
	}
    return true;
}







