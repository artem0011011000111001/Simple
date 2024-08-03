#include "Variables.h"
#include "Values.h"
#include "Simple_Error.h"
#include "Simple_defines.h"
#include "Utils.h"

using namespace Simple;

void Variables::SetAllVariables(std::unordered_map<String, Variable>&& variables) {

	/*for (auto& var : variables)
	{
		if (Variables::variables.find(var.first) == Variables::variables.end())
			Variables::variables[var.first] = std::move(var.second);
	}*/

	std::unordered_map<String, Variable> resultMap;
	for (auto& pair : variables) {
		if (Variables::variables.find(pair.first) != Variables::variables.end()) {
			resultMap[pair.first] = std::move(Variables::variables[pair.first]);
		}
		else {
			resultMap.operator[](pair.first) = std::move(pair.second);
		}
	}
	Variables::variables = std::move(resultMap);
}

bool Variables::IsExist(const String& key) {
	return variables.find(key) != variables.end();
}

ValuePtr Variables::Get(const String& key) {
	if (!IsExist(key)) return ZERO;
	return variables.at(key).value->clone();
}

Value& Variables::GetRef(const String& key) {
	if (!IsExist(key)) throw Simple_Error("Ref not send");
	return variables.at(key).value->get_ref();
}

const std::unordered_map<String, Variable>& Variables::GetAllVariables() {
	return variables;
}

void Variables::Set(const String& key, Variable value) {
	auto CheckInvalidName = [key](String invalidName) {
		if (key == invalidName)
			throw Simple_Error("\"" + invalidName + "\" invalid name");
		};

	CheckInvalidName("true");
	CheckInvalidName("false");
	CheckInvalidName("void");
	CheckInvalidName("this");

	bool IsIsExist = IsExist(key);
	if (IsIsExist && IsConstant(key)) throw Simple_Error("\"" + key + "\" is const");
	else if (IsIsExist) variables[key] = std::move(value);
	else variables.emplace(key, std::move(value));
}

void Variables::SetNew(const String& key, Variable value) {
	if (IsExist(key)) variables[key] = std::move(value);
	else variables.emplace(key, std::move(value));
}

std::unordered_map<String, Variable> Variables::CreateStandartVariables() {
	std::unordered_map<String, Variable> vars;

	//Math::InitVars();
	//Stream::InitVars();
	//vars.emplace("true",    Variable(CREATE_PTR<NumberValue>(1),   true));
	//vars.emplace("false",   Variable(CREATE_PTR<NumberValue>(0),   true));
	vars.emplace("__denys", Variable(CREATE_PTR<NumberValue>(666), false));
	return vars;
}

void Variables::PushState() {
	/*std::unordered_map<String, ValuePtr> currentState;
	copy_variables(Variables::GetAllVariables(), currentState);
	variablesState.push(std::move(currentState));*/

	std::unordered_map<String, Variable> currentState;
	copy_variables(Variables::GetAllVariables(), currentState);
	variablesState.push(std::move(currentState));
}

void Variables::PopState() {
	/*if (!variablesState.empty()) {
		Variables::SetAllVariables(std::move(variablesState.top()));
		variablesState.pop();
	}
	else {
		throw Simple_Error("No variable state to restore");
	}*/
	if (!variablesState.empty()) {
		Variables::SetAllVariables(std::move(variablesState.top()));
		variablesState.pop();
	}
	else {
		throw Simple_Error("No variable state to restore");
	}
}

bool Variables::IsConstant(const String& key) {
	for (auto& var : variables) {
		if (var.first == key && var.second.is_const)
			return true;
	}
	return false;
}

std::unordered_map<String, Variable> Variables::variables = CreateStandartVariables();

std::stack<std::unordered_map<String, Variable>> Variables::variablesState;