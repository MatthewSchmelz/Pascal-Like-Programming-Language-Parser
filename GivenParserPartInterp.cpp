
#include "parserInterp.h"
#include "val.h"


map<string, bool> defVar;
map<string, Token> SymTable;

map<string, Value> TempsResults; //Container of temporary locations of Value objects for results of expressions, variables values and constants 
queue <Value>* ValQue; //declare a pointer variable to a queue of Value objects

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
bool WriteLnStmt(istream& in, int& line) {
	LexItem t;
	//cout << "in WriteStmt" << endl;
	ValQue = new queue<Value>;

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

	//Evaluate: print out the list of expressions' values
	while (!(*ValQue).empty())
	{
		Value nextVal = (*ValQue).front();
		cout << nextVal;
		ValQue->pop();
	}
	cout << endl;
	return ex;
}//End of WriteLnStmt

//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
	Value retVal;
	//cout << "in ExprList and before calling Expr" << endl;
	status = Expr(in, line, retVal);
	if (!status) {
		ParseError(line, "Missing Expression");
		return false;
	}
	
	ValQue->push(retVal);
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

//Var ::= IDENT
bool Var(istream& in, int& line, LexItem& idtok) {
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
	Value val;
	//Setting up a temporary new queue to store all variables in, we'll use this for defining them if there is an expr
	queue <string> VarQueue, VarQueue2;
	//It is a string and a token
	

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
			//new variable! send it to the queue.
			VarQueue.push(ident.GetLexeme());
			VarQueue2.push(ident.GetLexeme());
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
			//We need to mark down what kind of variables each of them are, so that only correct values can be assigned to them
			
			while (!VarQueue2.empty()) {
		
				SymTable[VarQueue2.front()] = t.GetToken();
				//Add a new TempsResults profile for each of the types.
				VarQueue2.pop();
			}



			// Check for the assignment operator (ASSOP)
			t = Parser::GetNextToken(in, line);
			if (t == ASSOP) {
				// Parse the expression after the assignment operator
				status = Expr(in, line,val);
				if (!status) {
					ParseError(line, "Incorrect initialization expression.");
					return false;
				}
				//We have successfully parsed a function, so val should contain the final result, allowing us to give the value to all variables
				while (!VarQueue.empty()) {
					TempsResults.insert(make_pair(VarQueue.front(),val));
					//Add a new TempsResults profile for each of the types.
					VarQueue.pop();
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
	Value val;
	LexItem ifToken = Parser::GetNextToken(in, line);

	if (ifToken != IF) {
		ParseError(line, "Expected 'IF' in IfStmt");
		return false;
	}

	// Parse the condition expression
	status = Expr(in, line, val);
	if (!status) {
		ParseError(line, "Missing if statement Logic Expression");
		return false;
	}
	//We need to check the evaluation for the expr that just came back, if true, then we only do what happens after THEN, if false
	//Than we do ELSE

	LexItem thenToken = Parser::GetNextToken(in, line);

	if (thenToken != THEN) {
		ParseError(line, "If-Stmt Syntax Error");
		return false;
	}

	// Parse the statement for the "THEN" branch if statement satisfied
	if (val.GetBool()) {
		status = Stmt(in, line);
		if (!status) {
			ParseError(line, "Missing Statement for If-Stmt Then-Part");
			return false;
		}
	}
	else {
		//Start skipping tokens until the next stmt is done
		//Check if next token is BEGIN, if so, dont stop skipping until END, else just skip till ;
		LexItem next = Parser::GetNextToken(in, line);
		if (next == BEGIN) {
			while (true) {
				next = Parser::GetNextToken(in, line);
				if (next == END) {
					break;
				}
			}
		}
		else {
			while (true) {
				if (next == SEMICOL) {
					break;
				}
			}
		}

	}
	

	LexItem elseToken = Parser::GetNextToken(in, line);

	if (elseToken == ELSE) {
		// Parse the statement for the "ELSE" branch if if not satisfied
		if (!val.GetBool()) {
			status = Stmt(in, line);
			if (!status) {
				ParseError(line, "Missing Stmt in IfStmt");
				return false;
			}
		}
		else {
			//Start skipping tokens until the next stmt is done
			//Check if next token is BEGIN, if so, dont stop skipping until END, else just skip till ;
			LexItem next = Parser::GetNextToken(in, line);
			if (next == BEGIN) {
				while (true) {
					next = Parser::GetNextToken(in, line);
					if (next == END) {
						break;
					}
				}
			}
			else {
				while (true) {
					if (next == SEMICOL) {
						break;
					}
				}
			}
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
		//We found it? good, abort it, we need it for the variable assignment.
		Parser::PushBackToken(t);


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
	ValQue = new queue<Value>;
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
	while (!(*ValQue).empty())
	{
		Value nextVal = (*ValQue).front();
		cout << nextVal;
		ValQue->pop();
	}
	//Evaluate: print out the list of expressions values

	return ex;
}




//AssignStmt :: = Var : = Expr
bool AssignStmt(istream& in, int& line) {
	//Already checked the Variable in SimpleStmt.
	LexItem var, t;
	Value val;
	Token type;
	//Var is our variable, we must assign it, later
	var = Parser::GetNextToken(in, line);

	t = Parser::GetNextToken(in, line);
	if (t != ASSOP) {
		ParseError(line, "Missing Assignment Operator");
		return false;
	}
	//Check Expr
	bool status = Expr(in, line, val);
	if (!status) {
		ParseError(line, "Missing Expression in Assignment Statment");
		return false;
	}
	//Send var,val1 to the Listings.
	//cout << val;
	//Ensure whatever is getting assigned is getting assigned correctly.
	auto typeIt = SymTable.find(var.GetLexeme());
	type = typeIt->second;

	if (type == INTEGER) {
		//Check to make sure that the val we're about to throw in is an integer, or real
		if (val.GetType() ==VINT ) {
			TempsResults[var.GetLexeme()] = val;
		}
		else if (val.GetType() == VREAL) {
			TempsResults[var.GetLexeme()] = int(val.GetReal());
		}
		else {
			ParseError(line, "Non compatible value being assigned to a variable");
			return false;
		}

		
	}
	else if (type == REAL) {
		//Check to make sure that the val we're about to throw in is an integer, or real
		if (val.GetType() == VINT) {
			TempsResults[var.GetLexeme()] = double(val.GetInt());
		}
		else if (val.GetType() == VREAL) {
			TempsResults[var.GetLexeme()] = val.GetReal();
		}
		else {
			ParseError(line, "Non compatible value being assigned to a variable");
			return false;
		}
	}
	else if (type == BOOLEAN) {
		TempsResults[var.GetLexeme()] = val.GetBool();
	}
	else if (type == VSTRING) {
		TempsResults[var.GetLexeme()] = val.GetString();
	}
	else {
		TempsResults[var.GetLexeme()] = val;
		cout << "Havent applied proper type for " + var.GetLexeme() + "Proper type is " + to_string(val.GetType())  << endl;
	}

	

	return status;
}

// Expr :: = LogAndExpr{ OR LogAndExpr }
//Revaluate, LogOrExpr is just a nonterminal name and doesnt need to be checked as such.
bool Expr(istream& in, int& line, Value& retVal) {
	bool status = true;
	LexItem op;



	//Create 2 value objects, these will store the results of two possible logAndExpr. 
	//At the end, we complete the or statement 
	Value val1, val2;


	//Check to see if we have a LogAndExpr
	status = LogANDExpr(in, line,val1);
	retVal = val1;
	if (!status) {
		//ParseError(line, "Missing LogAndExpr");
		return false;
	}
	//Check for Additional LogAndExpr
	while (true) {
		op = Parser::GetNextToken(in, line);

		if (op == OR) {
			// Continue parsing the expression
			status = LogANDExpr(in, line, val2);
			if (!status) {
				//ParseError(line, "Missing LogAndExpr");
				return false;
			}
			retVal = val1 || val2;
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
bool LogANDExpr(istream& in, int& line, Value&retVal) {
	//Same as before, 2 vars, check at end
	Value val1, val2;


	bool status = RelExpr(in, line, val1);
	retVal = val1;
	if (!status) {
		//ParseError(line, "Missing RealExpr in LogANDExpr");
		return false;
	}

	while (true) {
		LexItem op = Parser::GetNextToken(in, line);

		if (op == AND) {
			// Continue parsing the expression
			status = RelExpr(in, line, val2);
			retVal = val1 && val2;
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
bool RelExpr(istream& in, int& line, Value& retVal) {
	// make 2 values, compare at the end,
	Value val2;
	bool status = SimpleExpr(in, line, retVal);
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
	status = SimpleExpr(in, line, val2);
	if (!status) {
		//ParseError(line, "Missing SimpleExpr in RelExpr");
		return false;
	}
	//We have two simple expressions, and a comparator, do that now and modify retVal
	if (op == EQ) {
		retVal = (retVal == val2);
	}
	else if (op == LTHAN) {
		retVal = (retVal < val2);
	}
	else if (op == GTHAN) {
		retVal = (retVal > val2);
	}
	return status;
}

//SimpleExpr :: Term { ( + | - ) Term }
bool SimpleExpr(istream& in, int& line, Value& retVal) {
	LexItem tok;
	//Same as before

	Value val1, val2;
	bool status = Term(in, line, val1);
	if (!status) {

		return false;
	}
	//Now we check for repeating terms
	while (true) {
		tok = Parser::GetNextToken(in, line);
		if (tok == PLUS || tok == MINUS) {
			//We got what we want, check next term
			//clean out val2
			val2 = 0;
			status = Term(in, line, val2);
			if (!status) {
				ParseError(line, "Missing Term in RealExpr");
				return false;
			}
			//Add/Subtract them, and move onto the next
			if (tok == PLUS) {
				val1 = val1 + val2;
			}
			else {
				val1 = val1 - val2;
			}
		}
		else {
			//Its something else, push it back and lets get out of here.
			Parser::PushBackToken(tok);

			break;
		}

	}

	//set val1 to retval after we computed everything
	retVal = val1;
	return true;
}

//Term :: = SFactor{ (*| / | DIV | MOD) SFactor }
bool Term(istream& in, int& line, Value& retVal) {
	LexItem tok;
	Value val1, val2;
	//Bool Op helps us understand if we just passed a boolean, if we did and theres not an operand next, we throw an error
	//False = No operator
	//True = Found Operator, NEED operand
	bool status = SFactor(in, line, val1);
	if (!status) {
		//ParseError(line, "Missing SFactor in Term");
		return false;
	}
	//Now we check for repeating terms
	while (true) {
		tok = Parser::GetNextToken(in, line);
		// DIV and '/' are the same thing, as far as I can tell. Could bring some errors in the future but honestly I dont see it
		if (tok == MULT || tok == DIV || tok == MOD || tok == IDIV) {
			//We got what we want, check next SFactor
			//Change operator satus to true
			status = SFactor(in, line, val2);
			if (!status) {
				ParseError(line, "Missing operand after operator");
				return false;
			}
			//We have val1 and val2, compute em
			if (tok == MULT) {
				val1 = val1 * val2;
			}
			else if(tok == DIV ||tok == IDIV) {
				val1 = val1 / val2;
			}
			else if (tok == MOD) {
				val1 = val1 % val2;
				val1 = double(val1.GetInt());
			}
		}
		else {
			//Its something else, push it back and lets get out of here.
			Parser::PushBackToken(tok);
			break;
		}

	}
	retVal = val1;
	return true;

}

//SFactor :: = [(-| +| NOT)] Factor
bool SFactor(istream& in, int& line, Value& retVal) {
	bool status = true;
	LexItem t = Parser::GetNextToken(in, line);
	int sign = 0;
	if (t == MINUS)
	{
		sign = -1;
	}
	else if (t == PLUS)
	{
		sign = 1;
	}
	else if (t == NOT)
	{
		sign = -2;//sign is a NOT op for logic operands
	}
	else
		Parser::PushBackToken(t);

	//I think we can just send retVal down the line?? 
	status = Factor(in, line,retVal ,sign);
	return status;
}
//Factor :: = IDENT | ICONST | RCONST | SCONST | BCONST | (Expr)
bool Factor(istream& in, int& line,Value& retVal, int sign) {
	LexItem nextToken = Parser::GetNextToken(in, line);

	Value val;

	if (nextToken == IDENT || nextToken == ICONST || nextToken == RCONST || nextToken == SCONST || nextToken == BCONST) {
		// It's a valid identifier or constant
		//We need to attach the factor to a value, according to the sign
		//IDENTs need to grabbed from TempResults
		//SCONTS are strings, they can just go through
		//ICONST and RCONSTS are just numbers, they can go through
		//Same with BOOL.
		if(nextToken == IDENT) {
		//Grab the variable from TempList, if its not there, scream at the world
			auto it = TempsResults.find(nextToken.GetLexeme());

			if (it != TempsResults.end()) {
				retVal = it->second; // Access the value associated with the key
			}
			else {
				//not in map, theyre using an unitialized variable
				ParseError(line, "Using Uninitialized Variable");
				return false;
			}

		}
		else if (nextToken == ICONST) {
			retVal = stoi(nextToken.GetLexeme());
		}
		else if (nextToken == RCONST) {
			retVal = stod(nextToken.GetLexeme());
		}
		else if (nextToken == SCONST) {
			retVal = nextToken.GetLexeme();
		}
		else if (nextToken == BCONST) {
			if (nextToken.GetLexeme() == "true")
				retVal = true;
			else {
				retVal = false;
			}

		}
		// || nextToken == RCONST || nextToken == SCONST || nextToken == BCONST
		//Handle Signs
		if (sign ==1) {
			//All Good
		}
		else if (sign == -1) {
			retVal = retVal * -1;
		}
		else if (sign == -2) {
			
			retVal = !retVal;
		}
		else if (sign == 0) {
		}


		return true;
	}
	else if (nextToken == LPAREN) {
		// It's an opening parenthesis, parse the sub-expression
		bool status = Expr(in, line,retVal);
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