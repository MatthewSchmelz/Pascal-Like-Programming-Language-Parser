#include <iostream>
#include <string>
#include <vector> 
using namespace std;
#include "val.h"


Value Value::operator/(const Value& op) const {
	//Create a VALUE to return
	Value back;
	//Error checking to make sure we dont divide by 0
	if (op.GetType() == VINT) {
		if (op.GetInt() == 0) return VERR;
	}
	else if (op.GetType() == VREAL) {
		if (op.GetReal() == 0.0) return VERR;
	}
	if (IsInt() && op.IsInt()) {

		return (GetInt() / op.GetInt());
	}
	else if (IsReal()) {
		//We know the first operand is real, so we just need to see if the other is real or not
		if (op.IsInt()) {
			return GetReal() / op.GetInt();
		}
		else if (op.IsReal()) {
			return GetReal() / op.GetReal();
		}
		//if the second operand is neither an int or real, its gonna produce an error.
	}
	else if (IsInt()) {
		//We know they both cant be int at this point, so no reason to check. We know that the first operand is an int, so lets check the second
		if (op.IsReal()) {
			return GetInt() / op.GetReal();
		}
		//if the second operand is neither an int nor Real, error
	}
	//Set op to an error value and toss it back
	back.SetType(VERR);
	return back;
}

Value Value::operator%(const Value& oper) const {
	//Create a VALUE to return
	Value back;
	//Both need to be INTs
	if (IsInt() && oper.IsInt()) {
		return (GetInt() % oper.GetInt());
	}
	//Set op to an error value and toss it back
	back.SetType(VERR);
	return back;
}

Value Value::operator==(const Value& op) const {
	Value back;
	if (GetType() == op.GetType()) {
		if (IsInt()) {
			return Value(GetInt() == op.GetInt());
		}
		else if (IsReal()) {
			return Value(GetReal() == op.GetReal());
		}
		else if (IsString()) {
			//Strings for some reason cant be compared to each other??
			return Value(GetString() == op.GetString());
			//back.SetType(VERR);
			//return back;
		}
		else if (IsBool()) {
			return Value(GetBool() == op.GetBool());
		}
		else {
			return Value(true); // Probably ERR = ERR, or something similar. 
		}
	}
	//Could also be Int/Real or Real/Int, need to check those too for some god forsaken reason
	else if (IsReal() && (op.IsInt() || op.IsReal())) {
		//We know the first operand is real, so we just need to see if the other is real or not
		if (op.IsInt()) {
			return Value(GetReal() == op.GetInt());
		}
		//if the second operand is neither an int or real, its gonna produce an error.
	}
	else if (IsInt() && (op.IsInt() || op.IsReal())) {
		//We know they both cant be int at this point, so no reason to check. We know that the first operand is an int, so lets check the second
		if (op.IsReal()) {
			return Value(GetInt() == op.GetReal());
		}
		//if the second operand is neither an int nor Real, error
	}
	back.SetType(VERR);
	return back; // Types aint the Same ypes aint equal

}

Value Value::operator&&(const Value& oper) const {
	//Both need to be Boolean
	Value back;
	if (IsBool() && oper.IsBool()) {
		return Value(GetBool() && oper.GetBool());
	}
	back.SetType(VERR);
	return back; // Types aint the Same ypes aint equal
}

//Extra ones added after the RA8 Assignment 

// numeric overloaded add this to op
Value Value::operator+(const Value& op) const {
	Value back;
	if (IsInt() && op.IsInt()) {

		return (GetInt() + op.GetInt());
	}
	else if (IsReal()) {
		//We know the first operand is real, so we just need to see if the other is real or not
		if (op.IsInt()) {
			return GetReal() + op.GetInt();
		}
		else if (op.IsReal()) {
			return GetReal() + op.GetReal();
		}
		//if the second operand is neither an int or real, its gonna produce an error.
	}
	else if (IsInt()) {
		//We know they both cant be int at this point, so no reason to check. We know that the first operand is an int, so lets check the second
		if (op.IsReal()) {
			return GetInt() + op.GetReal();
		}
		//if the second operand is neither an int nor Real, error
	}
	back.SetType(VERR);
	return back; // Types arent able to be added
}

// numeric overloaded subtract op from this
Value Value::operator-(const Value& op) const {
	Value back;
	if (IsInt() && op.IsInt()) {

		return (GetInt() - op.GetInt());
	}
	else if (IsReal()) {
		//We know the first operand is real, so we just need to see if the other is real or not
		if (op.IsInt()) {
			return GetReal() - op.GetInt();
		}
		else if (op.IsReal()) {
			return GetReal() - op.GetReal();
		}
		//if the second operand is neither an int or real, its gonna produce an error.
	}
	else if (IsInt()) {
		//We know they both cant be int at this point, so no reason to check. We know that the first operand is an int, so lets check the second
		if (op.IsReal()) {
			return GetInt() - op.GetReal();
		}
		//if the second operand is neither an int nor Real, error
	}
	back.SetType(VERR);
	return back; //Invalid Types
}
// numeric overloaded multiply this by op
Value Value::operator*(const Value& op) const {
	Value back;
	if (IsInt() && op.IsInt()) {

		return (GetInt() * op.GetInt());
	}
	else if (IsReal()) {
		//We know the first operand is real, so we just need to see if the other is real or not
		if (op.IsInt()) {
			return GetReal() * op.GetInt();
		}
		else if (op.IsReal()) {
			return GetReal() * op.GetReal();
		}
		//if the second operand is neither an int or real, its gonna produce an error.
	}
	else if (IsInt()) {
		//We know they both cant be int at this point, so no reason to check. We know that the first operand is an int, so lets check the second
		if (op.IsReal()) {
			return GetInt() * op.GetReal();
		}
		//if the second operand is neither an int nor Real, error
	}
	back.SetType(VERR);
	return back; //Invalid Types
}

//numeric integer division this by oper
Value Value::div(const Value& oper) const {
	Value back;
	//Error checking to make sure we dont divide by 0
	if (oper.GetType() == VINT) {
		if (oper.GetInt() == 0) return VERR;
	}
	else if (oper.GetType() == VREAL) {
		if (oper.GetReal() == 0.0) return VERR;
	}
	if (IsInt() && oper.IsInt()) {

		return  static_cast<int>(GetInt() / oper.GetInt());
	}
	else if (IsReal()) {
		//We know the first operand is real, so we just need to see if the other is real or not
		if (oper.IsInt()) {
			return  static_cast<int>(GetReal() / oper.GetInt());
		}
		else if (oper.IsReal()) {
			return  static_cast<int>(GetReal() / oper.GetReal());
		}
		//if the second operand is neither an int or real, its gonna produce an error.
	}
	else if (IsInt()) {
		//We know they both cant be int at this point, so no reason to check. We know that the first operand is an int, so lets check the second
		if (oper.IsReal()) {
			return  static_cast<int>(GetInt() / oper.GetReal());
		}
		//if the second operand is neither an int nor Real, error
	}
	//Set op to an error value and toss it back
	back.SetType(VERR);
	return back; //Invalid Types
}

//overloaded greater than operator of this with op
Value Value::operator>(const Value& op) const {
	Value back;
	if (GetType() == op.GetType()) {
		if (IsInt()) {
			return Value(GetInt() > op.GetInt());
		}
		else if (IsReal()) {
			return Value(GetReal() > op.GetReal());
		}
		else if (IsString()) {
			//Strings for some reason cant be compared to each other??
			return Value(GetString() > op.GetString());
			//back.SetType(VERR);
			//return back;
		}
		else if (IsBool()) {
			return Value(GetBool() > op.GetBool());
		}
		else {
			return Value(true); // Probably ERR = ERR, or something similar. 
		}
	}
	//Could also be Int/Real or Real/Int, need to check those too for some god forsaken reason
	else if (IsReal() && (op.IsInt() || op.IsReal())) {
		//We know the first operand is real, so we just need to see if the other is real or not
		if (op.IsInt()) {
			return Value(GetReal() > op.GetInt());
		}
		//if the second operand is neither an int or real, its gonna produce an error.
	}
	else if (IsInt() && (op.IsInt() || op.IsReal())) {
		//We know they both cant be int at this point, so no reason to check. We know that the first operand is an int, so lets check the second
		if (op.IsReal()) {
			return Value(GetInt() > op.GetReal());
		}
		//if the second operand is neither an int nor Real, error
	}
	back.SetType(VERR);
	return back; //Invalid Types
}
//overloaded less than operator of this with op
Value Value::operator<(const Value& op) const {
	Value back;
	if (GetType() == op.GetType()) {
		if (IsInt()) {
			return Value(GetInt() < op.GetInt());
		}
		else if (IsReal()) {
			return Value(GetReal() < op.GetReal());
		}
		else if (IsString()) {
			//Strings for some reason cant be compared to each other??
			return Value(GetString() < op.GetString());
			//back.SetType(VERR);
			//return back;
		}
		else if (IsBool()) {
			return Value(GetBool() < op.GetBool());
		}
		else {
			return Value(true); // Probably ERR = ERR, or something similar. 
		}
	}
	//Could also be Int/Real or Real/Int, need to check those too for some god forsaken reason
	else if (IsReal() && (op.IsInt() || op.IsReal())) {
		//We know the first operand is real, so we just need to see if the other is real or not
		if (op.IsInt()) {
			return Value(GetReal() < op.GetInt());
		}
		//if the second operand is neither an int or real, its gonna produce an error.
	}
	else if (IsInt() && (op.IsInt() || op.IsReal())) {
		//We know they both cant be int at this point, so no reason to check. We know that the first operand is an int, so lets check the second
		if (op.IsReal()) {
			return Value(GetInt() < op.GetReal());
		}
		//if the second operand is neither an int nor Real, error
	}
	back.SetType(VERR);
	return back; //Invalid Types
}



//Logic operations  

//Overloaded Oring operator
Value Value::operator||(const Value& oper) const {
	Value back;
	if (IsBool() && oper.IsBool()) {
		return Value(GetBool() || oper.GetBool());
	}
	back.SetType(VERR);
	return back;
}
//Overloaded Not/complement operator
Value Value::operator!() const {
	Value back;
	if (IsBool()) {
		return Value(!GetBool());
	}
	back.SetType(VERR);
	return back;
}