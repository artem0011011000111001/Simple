#include "Lexer.h"
#include "Simple_defines.h"
#include "Simple_Error.h"

using namespace Simple;

Lexer::Lexer(String& input) : input(input), tokens(), pos(0), length(input.length()) {}

std::list<Token> Lexer::tokenize() {
	while (pos < length) {
		char current = peek(0);
		if (std::isdigit(current))
			if (peek(1) == 'd') tokenizeDigit();
			else tokenizeNumber();

		else if (std::isalpha(current) || current == '_' || current == '$') tokenizeWord();
		else if (current == '\"') tokenizeText();
		else if (current == '\'') tokenizeChar();
		else if (current == '#') tokenizeHexNumber();
		else if (current == '\n') tokenizeNewLine();
		else if (indexOfUnMap(OPERATORS, current) != simple_npos) tokenizeOperator();
		else next();
	}
	return tokens;
}

void Lexer::tokenizeNumber() {
	String buffer;
	char current = peek(0);
	while (true) {
		if (current == '.') {
			if (indexOf(buffer, '.') != simple_npos) throw Simple_Error("Two dots in float number");
			if (!isdigit(peek(1))) throw Simple_Error("Expected digit after .");
		}
		else if (!std::isdigit(current)) 
			break;
		buffer.push_back(current);
		current = next();
	}
	addToken(TokenType::NUM, buffer);
}

void Lexer::tokenizeDigit() {
	addToken(TokenType::DIGIT_, String(1 ,peek(0)));
	next(); // skip digit
	next();	// skip d
}

void Lexer::tokenizeHexNumber() {
	next(); // skip #
	String buffer;
	char current = peek(0);
	while (IsHex(current)) {
		buffer.push_back(current);
		current = next();
	}
	addToken(TokenType::HEX_NUM, buffer);
}

void Lexer::tokenizeWord() {
	String buffer;
	char current = peek(0);
	while (IsWord(current)) {
		buffer.push_back(current);
		current = next();
	}

	if (buffer == "const")
		addToken(TokenType::CONST);

	else if (buffer == "if")
		addToken(TokenType::IF);

	else if (buffer == "else")
		addToken(TokenType::ELSE);

	else if (buffer == "do")
		addToken(TokenType::DO);

	else if (buffer == "while")
		addToken(TokenType::WHILE);

	else if (buffer == "for")
		addToken(TokenType::FOR);

	else if (buffer == "break")
		addToken(TokenType::BREAK);

	else if (buffer == "continue")
		addToken(TokenType::CONTINUE);

	else if (buffer == "import")
		addToken(TokenType::IMPORT);

	else if (buffer == "switch")
		addToken(TokenType::SWITCH);

	else if (buffer == "case")
		addToken(TokenType::CASE);

	else if (buffer == "default")
		addToken(TokenType::DEFAULT);

	else if (buffer == "func")
		addToken(TokenType::FUNC);

	else if (buffer == "return")
		addToken(TokenType::RETURN);

	else if (buffer == "struct")
		addToken(TokenType::STRUCT);

	else if (buffer == "field")
		addToken(TokenType::FIELD);

	else if (buffer == "try")
		addToken(TokenType::TRY);

	else if (buffer == "catch")
		addToken(TokenType::CATCH);

	else if (buffer == "throw")
		addToken(TokenType::THROW);

	else addToken(TokenType::WORD, buffer);
}

void Lexer::tokenizeText() {
	next(); // skip opening "
	String buffer;
	char current = peek(0);
	while (current != '\"') {
		if (current == '\0')
			throw Simple_Error("No closing found \"");
		if (current == '\\') {
			current = next();
			switch (current) {
			case '\"':
				current = next();
				buffer.push_back('\"');
				continue;
			case 'n':
				current = next();
				buffer.push_back('\n');
				continue;
			case 't':
				current = next();
				buffer.push_back('\t');
				continue;
			default:
				buffer.push_back('\\');
				continue;
			}
		}
		buffer.push_back(current);
		current = next();
	}
	next(); // skip closing "
	addToken(TokenType::TEXT, buffer);
}

void Lexer::tokenizeChar() {
	next(); // skip opening '
	addToken(TokenType::CHAR_, String(1, peek(0)));
	next(); // skip char
	next(); // skip closing '
}

void Lexer::tokenizeOperator() {
	char current = peek(0);
	if (current == '/') {
		if (peek(1) == '/') {
			next();
			next();
			tokenizeComment();
			return;
		}
		else if (peek(1) == '*') {
			next();
			next();
			tokenizeMultilineComment();
			return;
		}
	}
	String buffer;
	while (true) {
		String text = buffer;
		if (OPERATORS.find(text + current) == OPERATORS.end() && !OPERATORS.empty()) {
			addToken(OPERATORS.at(text));
			return;
		}
		buffer.push_back(current);
		current = next();
	}
}

void Lexer::tokenizeComment() {
	char current = peek(0);
	while (indexOf("\n\r\0", current) == simple_npos) {
		current = next();
	}
}

void Lexer::tokenizeMultilineComment() {
	char current = peek(0);
	while (!(current == '*' && peek(1) == '/')) {
		if (current == '\0') throw Simple_Error("Missing close tag");
		current = next();
	}
	next(); // skip *
	next(); // skip /
}

void Lexer::tokenizeNewLine() {
	addToken(TokenType::NEW_LINE);
	next(); // skip \n
}