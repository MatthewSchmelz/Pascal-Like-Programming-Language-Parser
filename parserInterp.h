/*
 * parser.h
 * Programming Assignment 2
 * Fall 2023
*/

#ifndef PARSER_H_
#define PARSER_H_

#include <iostream>

using namespace std;

#include "lex.h"
#include "val.h"


extern bool Prog(istream& in, int& line); //Got
extern bool DeclPart(istream& in, int& line); //Got
extern bool DeclStmt(istream& in, int& line); //Got
extern bool Stmt(istream& in, int& line); //Got
extern bool StructuredStmt(istream& in, int& line);
extern bool CompoundStmt(istream& in, int& line);
extern bool SimpleStmt(istream& in, int& line);
extern bool WriteLnStmt(istream& in, int& line); //Got
extern bool WriteStmt(istream& in, int& line);
extern bool IfStmt(istream& in, int& line);
extern bool AssignStmt(istream& in, int& line);

extern bool ExprList(istream& in, int& line); //Got

extern bool IdentList(istream& in, int& line);

//Actal Header functions when we get around to doing that
/*
extern bool Var(istream& in, int& line); //Got

extern bool Expr(istream& in, int& line);
extern bool LogANDExpr(istream& in, int& line);
extern bool RelExpr(istream& in, int& line);
extern bool SimpleExpr(istream& in, int& line);
extern bool Term(istream& in, int& line);
extern bool SFactor(istream& in, int& line);
extern bool Factor(istream& in, int& line, int sign);

*/
extern bool Var(istream& in, int& line, LexItem& idtok);
extern bool Expr(istream& in, int& line, Value& retVal);
extern bool LogANDExpr(istream& in, int& line, Value& retVal);
extern bool RelExpr(istream& in, int& line, Value& retVal);
extern bool SimpleExpr(istream& in, int& line, Value& retVal);
extern bool Term(istream& in, int& line, Value& retVal);
extern bool SFactor(istream& in, int& line, Value& retVal);
extern bool Factor(istream& in, int& line, Value& retVal, int sign);

extern int ErrCount();
#endif /* PARSE_H_ */

