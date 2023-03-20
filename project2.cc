/*
 * Copyright (C) Mohsen Zohrevandi, 2017
 *               Rida Bazzi 2019
 * Do not share this file with anyone
 */
#include <iostream>
#include <cstdio>
#include <string>
#include <cstdlib>
#include "lexer.h"

using namespace std;

string readString;
string grammar;

InputBuffer* inputGrammar = new InputBuffer();
InputBuffer* inputRule = new InputBuffer();

LexicalAnalyzer* lexGrammar = new LexicalAnalyzer();
LexicalAnalyzer* lexRule = new LexicalAnalyzer();

class Rule {
    public:
        std::string leftHand;
        std::vector<string> rightHand;
};

vector<Rule> ruleList;

vector<string> Terminals;
vector<string> Nonterminals;


void parseGrammar() {
    //cuts off the HASH at the end of the grammar
    grammar = grammar.substr(0, grammar.length() - 2);
}

Rule parseRule(string ruleInput) {
    //add LHS and RHS onto respective vectors
    //read input up to ARROW and make that LHS
    Rule rule;

    //put this string rule into input buffer
    inputRule->UngetString(ruleInput);

    int count = 0;
    
    //lexical analyzer's token string = ruleInput
    lexRule->GetToken().lexeme = ruleInput;
    

    /*
    Rule structure:
       {LEFTHAND} {ARROW} {RIGHTHAND} {ENDOFFILE}
    */
    
    //get {LEFTHAND}
    rule.leftHand = lexRule->GetToken().lexeme;
    cout << rule.leftHand << endl;
    //add to nonterminal directly, anything in lefthand will be nonterminal
    Nonterminals.push_back(rule.leftHand);
    
    //get {ARROW}
    //inputRule->GetToken();

    //get {RIGHTHAND}
    while (!inputRule->EndOfInput()){
        rule.rightHand.push_back(lexRule->GetToken().lexeme);
    }
    
    //now return rule
    return rule;
}

void parseRuleList(){
    std::string tempGrammar = grammar;
    char star = '*';
    string ruleString = "";
    Rule thisRule;
    
    for (int i = 0; i < tempGrammar.length(); i++){
        if (tempGrammar[i] == star){

            //use parseRule to return one individual rule using ruleString
            thisRule = parseRule(ruleString);

            //add to rule list
            ruleList.push_back(thisRule);

            //reset
            ruleString = "";
        }
        else{
            //get the one rule as a string (delimiter is star)
            ruleString += tempGrammar[i];
        }
    }
}

// read grammar
void ReadGrammar()
{
    //read string into grammar string variable
    cout << "0\n";
    cin >> readString;

    cout << "readString: " << readString << endl;

    //get the grammar and store in grammar string variable
    parseGrammar();
    //put grammar string into input buffer to read through token by token
    inputGrammar->UngetString(grammar);
    cout << "inputGrammar->UngetString(readString): " << inputGrammar->UngetString(grammar) << endl;
    
    //get list of rules and store in ruleList variable (a vector of rules)
    parseRuleList(); //will have to get each string and store in a rule (obejct) then put into class
}


// Task 1
void printTerminalsAndNoneTerminals()
{
    cout << "1\n";

    /*
        string nonTerminalList = "";
        string terminalList = "";
        int count = 1;
        
        //get first nonterminal because
        nonTerminalList += lex->GetToken().lexeme;
        nonTerminalList += " ";
        //gets the nonterminals into a list
        while(lex->peek(count + 1).token_type != END_OF_FILE) {
            //find the first star in the grammar
            if(lex->peek(count).token_type == STAR) {
                //add the terminal after STAR to the nonTerminalList because that will be the nonTerminal of the next rule
                nonTerminalList += lex->peek(count++).lexeme;
                nonTerminalList += " ";
            }
            //move through the input token by token
            count++;
        }
        //while loop to add the terminals to terminal list
        count = 0;
        cout << terminalList << endl;
        cout << nonTerminalList << endl;
        //read through the input to find the terminals
        while(lex->peek(count + 1).token_type != END_OF_FILE) {
            //if the token peeked is not STAR and it is not ARROW and it is not in the nonTerminalList, it is a terminal
            if(lex->peek(count).token_type != STAR && lex->peek(count).token_type != ARROW &&
                !isElement(nonTerminalList, lex->peek(count).lexeme)) {
                    terminalList += lex->peek(count).lexeme + " ";
            }
            count++;
    }*/


    //print THISRULE leftHand
        //if THISRULE rightHand is in all of leftHands,
    

}

/*
bool isElement(string list, string find) {
    bool isElement = false;
    char delimiter = ' ';
    int count = 0;
    string arr[100];
    int lastIndex = 0;
    //make array
    for (int i = 0; i < list.length(); i++) {
        if (list[i] == delimiter) {
            string word = list.substr(lastIndex, i - lastIndex);
            arr[count] = word;
            count++;
            lastIndex = i + 1;
        }
    }
    //get last word
    string word = list.substr(lastIndex, list.length() - lastIndex);
    arr[count] = word;
    count++;
    //find
    for(int i = 0; i < count; i++){
        if (arr[i] == find){
            isElement = true;
        }
    }
    return isElement;
} */

// Task 2
void RemoveUselessSymbols()

    //remove any rules that are A->episolon
    //go through the grammar if there are any RHS that are *, that is an epsilon rule and needs to be removed

    //remove any unaccessible rules S->AB, C->c, A->a, B->b: remove C->c
{
    cout << "2\n";
}

// Task 3
void CalculateFirstSets()
{
    cout << "3\n";
}

// Task 4
void CalculateFollowSets()
{
    cout << "4\n";
}

// Task 5
void CheckIfGrammarHasPredictiveParser()
{
    cout << "5\n";
}
    
int main (int argc, char* argv[])
{
    int task;

    if (argc < 2)
    {
        cout << "Error: missing argument\n";
        return 1;
    }

    /*
       Note that by convention argv[0] is the name of your executable,
       and the first argument to your program is stored in argv[1]
     */

    task = atoi(argv[1]);
    
    ReadGrammar();  // Reads the input grammar from standard input
                    // and represent it internally in data structures
                    // ad described in project 2 presentation file

    switch (task) {
        case 1: printTerminalsAndNoneTerminals();
            break;

        case 2: RemoveUselessSymbols();
            break;

        case 3: CalculateFirstSets();
            break;

        case 4: CalculateFollowSets();
            break;

        case 5: CheckIfGrammarHasPredictiveParser();
            break;

        default:
            cout << "Error: unrecognized task number " << task << "\n";
            break;
    }
    return 0;
}
