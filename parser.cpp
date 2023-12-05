/* Implementation of Recursive-Descent Parser
	for a Simple Pasacal-Like Language
 * parser.cpp
 * Programming Assignment 2
 * Fall 2023
*/


#include "lex.h"
#include "parserInterp.h"
#include <iostream>
#include <string> // exit prototype
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cstdlib> // exit prototype
#include <map>
#include <list>

/*
map<string, bool> defVar;
map<string, Token> SymTable;

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if (pushed_back) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem& t) {
		if (pushed_back) {
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
/*

3.
4. CANT FIND TYPE
5.
6.
7.
8. CompoundStmt ::= BEGIN Stmt {; Stmt } END
10. WriteStmt ::= WRITE (ExprList)
11. IfStmt ::= IF Expr THEN Stmt [ ELSE Stmt ]
12.
15.
16.
17.
18.
19. Term ::= SFactor { ( * | / | DIV | MOD ) SFactor }
20. SFactor ::= [( - | + | NOT )] Factor
21. Factor ::= IDENT | ICONST | RCONST | SCONST | BCONST | (Expr)

Current Errors
-Needs to check when variables are redeclared
-Needs to check when variables are used and the variable doesn't exist.



//Prog ::= PROGRAM IDENT ; DeclPart CompoundStmt .
bool Prog(istream& in, int& line) {
	//Alright, so we 
	LexItem t;
	//make sure the program starts with 'program'
	t = Parser::GetNextToken(in, line);
	if (t != PROGRAM) {
		ParseError(line, "Missing PROGRAM Keyword.");
		return false;
	}
	//Check to see if the program has an identifier
	t = Parser::GetNextToken(in, line);
	if (t != IDENT) {
		ParseError(line, "Missing Program Name.");
		return false;
	}
	//Check semicolon
	t = Parser::GetNextToken(in, line);
	if (t != SEMICOL) {
		ParseError(line, "No Program Semicol");
		return false;
	}

	//Check DeclPart
	bool DeclP = DeclPart(in, line);
	if (!DeclP) {
		ParseError(line, "Incorrect Declaration Section.");
		return false;
	}

	bool CompS = CompoundStmt(in, line);
	if (!CompS) {
		ParseError(line, "Incorrect Program Body.");
		return false;
	}

	//Check Dot
	t = Parser::GetNextToken(in, line);
	if (t != DOT) {
		ParseError(line, "No Program Dot");
		return false;
	}

	//If everything goes through, the Program has no errors. so we can return true and get the DONE statement
	return true;
}

//WriteLnStmt ::= writeln (ExprList) 
//Double check this, im not sure if the professor put this completely there
bool WriteLnStmt(istream& in, int& line) {
	LexItem t;
	//cout << "in WriteStmt" << endl;

	t = Parser::GetNextToken(in, line);
	if (t != LPAREN) {

		ParseError(line, "Missing Left Parenthesis");
		return false;
	}

	bool ex = ExprList(in, line);

	if (!ex) {
		ParseError(line, "Missing expression list for WriteLn statement");
		return false;
	}

	t = Parser::GetNextToken(in, line);
	if (t != RPAREN) {

		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	//Evaluate: print out the list of expressions values

	return ex;
}//End of WriteLnStmt


//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
	//cout << "in ExprList and before calling Expr" << endl;
	status = Expr(in, line);
	if (!status) {
		ParseError(line, "Missing Expression");
		return false;
	}

	LexItem tok = Parser::GetNextToken(in, line);

	if (tok == COMMA) {
		//cout << "before calling ExprList" << endl;
		status = ExprList(in, line);
		//cout << "after calling ExprList" << endl;
	}
	else if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else {
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}//ExprList

//Var
//Var ::= IDENT
bool Var(istream& in, int& line) {
	LexItem tok = Parser::GetNextToken(in, line);
	if (tok != IDENT) {
		ParseError(line, "Variable not IDENT");
		return false;
	}
	//Variable is an IDENT, all good.
	return true;
}

//DeclPart :: = VAR DeclStmt; { DeclStmt; }
bool DeclPart(istream& in, int& line) {
	// Grab the first token, ensure it's VAR
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok != VAR) {
		ParseError(line, "Non-recognizable Declaration Part.");
		return false;
	}

	bool status = DeclStmt(in, line);
	if (!status) {
		ParseError(line, "Syntactic error in Declaration Block.");
		return false;
	}

	// Continue looping through until something either messes up or we finish the statements.
	while (true) {
		LexItem nextToken = Parser::GetNextToken(in, line);

		if (nextToken == SEMICOL) {
			// Continue parsing more DeclStmt statements
			//Check if the next token is BEGIN, if so, abort it, and we're done here. if not, abort it and keep going.
			nextToken = Parser::GetNextToken(in, line);
			if (nextToken == BEGIN) {
				Parser::PushBackToken(nextToken);
				break;
			}
			else {
				Parser::PushBackToken(nextToken);
			}
			status &= DeclStmt(in, line);
			if (!status) {
				ParseError(line, "Syntactic error in Declaration Block.");
				return false;
			}
		}
		else if (nextToken != VAR) {
			// We're done with DeclPart
			Parser::PushBackToken(nextToken);
			break;
		}
		else {
			// Missing semicolon
			ParseError(line, "Expected ';' in DeclPart");
			status = false;
			break;
		}
	}

	return status;
}

// DeclStmt :: = IDENT{ , IDENT } : Type[:= Expr]
//Type ::= INTEGER | REAL | BOOLEAN | STRING
bool DeclStmt(istream& in, int& line) {
	bool status = true;
	while (true) {
		LexItem ident = Parser::GetNextToken(in, line);
		if (ident != IDENT) {
			//ParseError(line, "Expected IDENT in DeclStmt");
			status = false;
			break;
		}

		// Ensure the Ident hasn't already been seen. If it has, throw an error; if it hasn't, put it into the list
		if (defVar.find(ident.GetLexeme()) != defVar.end()) {
			ParseError(line, "Variable Redefinition");
			ParseError(line, "Incorrect identifiers list in Declaration Statement.");
			return false;
		}
		else {
			defVar[ident.GetLexeme()] = true;
		}

		// Check for a comma, colon, or assignment operator (ASSOP)
		LexItem t = Parser::GetNextToken(in, line);
		if (t == COMMA) {
			continue;  // Continue for multiple identifiers
		}
		else if (t == COLON) {
			// Check for the type (INTEGER, REAL, BOOLEAN, STRING)
			t = Parser::GetNextToken(in, line);
			if (t != INTEGER && t != REAL && t != BOOLEAN && t != STRING) {
				ParseError(line, "Incorrect Declaration Type.");
				return false;
			}

			// Check for the assignment operator (ASSOP)
			t = Parser::GetNextToken(in, line);
			if (t == ASSOP) {
				// Parse the expression after the assignment operator
				status = Expr(in, line);
				if (!status) {
					ParseError(line, "Incorrect initialization expression.");
					return false;
				}
				return true;  // Declaration with assignment
			}
			else {
				// No assignment, just the declaration
				Parser::PushBackToken(t);
				return true;
			}
		}
		else {
			ParseError(line, "Missing comma in declaration statement.");
			ParseError(line, "Incorrect identifiers list in Declaration Statement.");
			status = false;
			break;
		}
	}
	return status;
}


//CANT FIND THIS IN PARSER, WHAT IS THIS?
//Type ::= INTEGER | REAL | BOOLEAN | STRING
bool Type(istream& in, int& line) {
	LexItem t = Parser::GetNextToken(in, line);

	if (t == INTEGER || t == REAL || t == BOOLEAN || t == STRING) {
		// It's a valid type
		return true;
	}
	else {
		ParseError(line, "Expected valid type (INTEGER, REAL, BOOLEAN, STRING)");
		return false;
	}
	//The return here is just for Vocareum
	return false;
}

//Stmt ::= SimpleStmt | StructuredStmt
bool Stmt(istream& in, int& line) {
	bool status = false;
	LexItem nextToken = Parser::GetNextToken(in, line);

	if (nextToken == BEGIN || nextToken == IF) {
		// It's a StructuredStmt
		Parser::PushBackToken(nextToken);  // Push back the token to be handled by StructuredStmt
		status = StructuredStmt(in, line);
		if (!status) {
			ParseError(line, "Incorrect Structured Statement.");
			return false;
		}
	}
	else {
		// It's a SimpleStmt
		Parser::PushBackToken(nextToken);  // Push back the token to be handled by SimpleStmt
		status = SimpleStmt(in, line);
		if (!status) {
			ParseError(line, "Incorrect Simple Statement.");
			return false;
		}
	}

	//Adding for Vocaerum
	return true;
}

//StructuredStmt :: = IfStmt | CompoundStmt
bool StructuredStmt(istream& in, int& line) {
	//Check the next token, depending on what it is, send it to Compound or Ifstmt
	LexItem nextToken = Parser::GetNextToken(in, line);

	if (nextToken == BEGIN) {
		// It's a A compoundStmt
		Parser::PushBackToken(nextToken);  // Push back the token to be handled by CompoundStmt
		return CompoundStmt(in, line);
	}
	else if (nextToken == IF) {
		// It's a Ifstatmnt
		Parser::PushBackToken(nextToken);  // Push back the token to be handled by Ifstatmt
		return IfStmt(in, line);
	}
	else {
		//We should not have gotten here. Abort and send an error.
		ParseError(line, "Recieved an invalid symbol in StructuredStmt, somehow.");
		return false;
	}
	return true;
}

// CompoundStmt :: = BEGIN Stmt{ ; Stmt } END
bool CompoundStmt(istream& in, int& line) {
	bool status;
	LexItem t = Parser::GetNextToken(in, line);

	if (t != BEGIN) {
		ParseError(line, "Expected 'BEGIN' in CompoundStmt");
		return false;
	}

	status = Stmt(in, line);
	if (!status) {
		ParseError(line, "Missing Stmt in CmpdStmt");
		return false;
	}

	while (true) {
		t = Parser::GetNextToken(in, line);

		if (t == SEMICOL) {
			// Continue parsing more statements
			status = Stmt(in, line);
			if (!status) {
				ParseError(line, "Syntactic error in the statement.");
				return false;
			}
		}
		else if (t == END) {
			// We found End, Wrap it up
			break;

		}
		else {
			//Didnt find end.
			ParseError(line, "Missing end of compound statement.");
			return false;
		}

	}

	return status;
}

//IfStmt :: = IF Expr THEN Stmt[ELSE Stmt]
bool IfStmt(istream& in, int& line) {
	bool status;
	LexItem ifToken = Parser::GetNextToken(in, line);

	if (ifToken != IF) {
		ParseError(line, "Expected 'IF' in IfStmt");
		return false;
	}

	// Parse the condition expression
	status = Expr(in, line);
	if (!status) {
		ParseError(line, "Missing if statement Logic Expression");
		return false;
	}

	LexItem thenToken = Parser::GetNextToken(in, line);

	if (thenToken != THEN) {
		ParseError(line, "If-Stmt Syntax Error");
		return false;
	}

	// Parse the statement for the "THEN" branch
	status = Stmt(in, line);
	if (!status) {
		ParseError(line, "Missing Statement for If-Stmt Then-Part");
		return false;
	}

	LexItem elseToken = Parser::GetNextToken(in, line);

	if (elseToken == ELSE) {
		// Parse the statement for the "ELSE" branch

		status = Stmt(in, line);
		if (!status) {
			ParseError(line, "Missing Stmt in IfStmt");
			return false;
		}
		return true;
	}
	else {
		// No "ELSE" branch, we made it through safely
		Parser::PushBackToken(elseToken);
		return true;
	}
	//Added a return for Vocareum

	return false;
}

//SimpleStmt ::= AssignStmt | WriteLnStmt | WriteStmt
bool SimpleStmt(istream& in, int& line) {
	bool status;
	//Write starts with WRITE, WriteLN starts with WRITELN, Assign starts with VAR
	//Once we check them, we dont need to abort them, taking that idea from the professor.
	LexItem t = Parser::GetNextToken(in, line);
	if (t == WRITE) {
		status = WriteStmt(in, line);
		if (!status) {

			return false;
		}
	}
	else if (t == WRITELN) {
		status = WriteLnStmt(in, line);
		if (!status) {

			return false;
		}
	}
	//Var should be checking for variables we know
	else if (defVar.find(t.GetLexeme()) != defVar.end()) {
		status = AssignStmt(in, line);
		if (!status) {
			//needs Incorrect Structured Stmt
			return false;
		}
	}
	else if (t == DOT) {
		//Looks like we've reached the end of the program, but there should have been an 'end' before this. 
		//Send this back as true and it'll catch on, Why the error comes after the dot instead of just at the dot, Ill never know.
		return true;
	}
	else {
		//Found an Invalid Variable
		ParseError(line, "Undeclared Variable");
		ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
		return false;
	}

	return true;
}


//WriteStmt :: = WRITE(ExprList)
//Stolen from Professor, same thing, really.
bool WriteStmt(istream& in, int& line) {
	LexItem t;
	//cout << "in WriteStmt" << endl;

	t = Parser::GetNextToken(in, line);
	if (t != LPAREN) {

		ParseError(line, "Missing Left Parenthesis");
		return false;
	}

	bool ex = ExprList(in, line);

	if (!ex) {
		ParseError(line, "Missing expression list for WriteLn statement");
		return false;
	}

	t = Parser::GetNextToken(in, line);
	if (t != RPAREN) {

		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	//Evaluate: print out the list of expressions values

	return ex;
}




//AssignStmt :: = Var : = Expr
bool AssignStmt(istream& in, int& line) {
	//Already checked the Variable in SimpleStmt.
	LexItem t;

	t = Parser::GetNextToken(in, line);
	if (t != ASSOP) {
		ParseError(line, "Missing Assignment Operator");
		return false;
	}
	//Check Expr
	bool status = Expr(in, line);
	if (!status) {
		ParseError(line, "Missing Expression in Assignment Statment");
		return false;
	}
	return status;
}

// Expr :: = LogAndExpr{ OR LogAndExpr }
//Revaluate, LogOrExpr is just a nonterminal name and doesnt need to be checked as such.
bool Expr(istream& in, int& line) {
	bool status = true;
	LexItem op;
	/*
		LexItem op = Parser::GetNextToken(in, line);
	if (op !=IDENT) {
		ParseError(line, "Missing Identifier for Expr");
		return false;
	}
	op = Parser::GetNextToken(in, line);
	if (op != ASSOP) {
		ParseError(line, "Missing ASSOP for Expr");
		return false;
	}
	


	//Check to see if we have a LogAndExpr

	status = LogANDExpr(in, line);
	if (!status) {
		//ParseError(line, "Missing LogAndExpr");
		return false;
	}
	//Check for Additional LogAndExpr
	while (true) {
		op = Parser::GetNextToken(in, line);

		if (op == OR) {
			// Continue parsing the expression
			status = LogANDExpr(in, line);
			if (!status) {
				//ParseError(line, "Missing LogAndExpr");
				return false;
			}
		}
		//Dumping lexemes it should not know here. Easier to tailor make right now.
		else if (op == ERR) {
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" + op.GetLexeme() + ")" << endl;
			return false;
		}
		else {
			// Push back the token if it's not an "OR" operator
			Parser::PushBackToken(op);
			break;
		}
	}

	return status;
}

//LogAndExpr :: = RelExpr{ AND RelExpr }
bool LogANDExpr(istream& in, int& line) {
	bool status = RelExpr(in, line);
	if (!status) {
		//ParseError(line, "Missing RealExpr in LogANDExpr");
		return false;
	}

	while (true) {
		LexItem op = Parser::GetNextToken(in, line);

		if (op == AND) {
			// Continue parsing the expression
			status &= RelExpr(in, line);
		}
		else {
			// Push back the token if it's not an "AND" operator
			Parser::PushBackToken(op);
			break;
		}
	}

	return status;
}

//RelExpr :: = SimpleExpr[(= | < | >) SimpleExpr]
bool RelExpr(istream& in, int& line) {
	bool status = SimpleExpr(in, line);
	if (!status) {
		//ParseError(line, "Missing SimpleExpr in RelExpr");
		return false;
	}
	//Check for = < or >

	LexItem op = Parser::GetNextToken(in, line);
	if (op != EQ && op != LTHAN && op != GTHAN) {
		//Not something we want, abort it and leave the juice
		Parser::PushBackToken(op);
		return status;
	}
	//We have noted that we have another SimpleExpr
	status = SimpleExpr(in, line);
	if (!status) {
		//ParseError(line, "Missing SimpleExpr in RelExpr");
		return false;
	}

	return status;
}

//SimpleExpr :: Term { ( + | - ) Term }
bool SimpleExpr(istream& in, int& line) {
	LexItem tok;
	bool status = Term(in, line);
	if (!status) {

		return false;
	}
	//Now we check for repeating terms
	while (true) {
		tok = Parser::GetNextToken(in, line);
		if (tok == PLUS || tok == MINUS) {
			//We got what we want, check next term
			status = Term(in, line);
			if (!status) {
				ParseError(line, "Missing Term in RealExpr");
				return false;
			}
		}
		else {
			//Its something else, push it back and lets get out of here.
			Parser::PushBackToken(tok);
			break;
		}

	}
	return true;
}

//Term :: = SFactor{ (*| / | DIV | MOD) SFactor }
bool Term(istream& in, int& line) {
	LexItem tok;
	//Bool Op helps us understand if we just passed a boolean, if we did and theres not an operand next, we throw an error
	//False = No operator
	//True = Found Operator, NEED operand
	bool status = SFactor(in, line);
	if (!status) {
		//ParseError(line, "Missing SFactor in Term");
		return false;
	}
	//Now we check for repeating terms
	while (true) {
		tok = Parser::GetNextToken(in, line);
		// DIV and '/' are the same thing, as far as I can tell. Could bring some errors in the future but honestly I dont see it
		if (tok == MULT || tok == DIV || tok == MOD) {
			//We got what we want, check next SFactor
			//Change operator satus to true
			status = SFactor(in, line);
			if (!status) {
				ParseError(line, "Missing operand after operator");
				return false;
			}
		}
		else {
			//Its something else, push it back and lets get out of here.
			Parser::PushBackToken(tok);
			break;
		}

	}
	return true;

}

//SFactor :: = [(-| +| NOT)] Factor
bool SFactor(istream& in, int& line) {
	bool status = true;
	int sign = 1;
	LexItem tok;
	tok = Parser::GetNextToken(in, line);
	if (tok != MINUS || tok != PLUS || tok != NOT) {
		//Factor does not have a opperand beforehand, push it back.
		Parser::PushBackToken(tok);
	}
	//If it did have one, we just consumed it, and now we can move onto factor.
	status = Factor(in, line, sign);
	if (!status) {
		//Wasnt asked for in testprog, so whatev.
		//ParseError(line, "Missing Factor in SFactor");
		return false;
	}

	return status;
}
//Factor :: = IDENT | ICONST | RCONST | SCONST | BCONST | (Expr)
bool Factor(istream& in, int& line, int sign) {
	LexItem nextToken = Parser::GetNextToken(in, line);
	if (nextToken == MINUS) {
		//its a negative, ignore it
		nextToken = Parser::GetNextToken(in, line);
	}

	if (nextToken == IDENT || nextToken == ICONST || nextToken == RCONST || nextToken == SCONST || nextToken == BCONST) {
		// It's a valid identifier or constant
		return true;
	}
	else if (nextToken == LPAREN) {
		// It's an opening parenthesis, parse the sub-expression
		bool status = Expr(in, line);
		if (!status) {
			ParseError(line, "Missing Expr in Factor");
			return false;
		}
		//Easier to Tailor Make, though I can look at this later if PA 3 Requires it
		LexItem closeParen = Parser::GetNextToken(in, line);
		if (closeParen == LTHAN) {
			ParseError(line, "Illegal Relational Expression.");
			//ParseError(line, "Missing expression after (");
			status = false;
		}
		if (closeParen != RPAREN) {
			//ParseError(line, "Illegal Relational Expression.");
			ParseError(line, "Missing expression after (");
			status = false;
		}

		return status;
	}
	else {
		//ParseError(line, "Expected identifier, constant, or opening parenthesis in Factor");
		return false;
	}

	//Adding a return for Vocareum
	return false;
}

*/