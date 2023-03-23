/*
 * Copyright (C) Mohsen Zohrevandi, 2017
 *               Rida Bazzi 2019
 * Do not share this file with anyone
 */
#include <iostream>
#include <cstdio>
#include <string>
#include <cstdlib>
#include <fstream>
#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string readString;
string grammar;

LexicalAnalyzer* lexGrammar = new LexicalAnalyzer();

class Rule {
    public:
        Token leftHand;
        std::vector<Token> rightHand;
};

vector<Rule> ruleList;

vector<string> Terminals;
vector<string> Nonterminals;

vector<int> indexLHS;
vector<int> indexRHS;
vector<string> indexList; 
vector<string> sortedNonterminals;

int index(string target) {
    for(int i = 0; i < indexList.size(); i++) {
        if(indexList[i] == target) {
            return i;
        }
    }
    return -1;
}

//tells if it is an element in given list
bool isInNonterminals(string target) {
    for(int i = 0; i < Nonterminals.size(); i++) {
        if(Nonterminals[i] == target) {
            return true;
        }
    }
    return false;
}

bool isElement(string target, vector<Rule> list) {
    for(int i = 0; i < list.size(); i ++) {
        if(list[i].leftHand.lexeme == target) {
            return true;
        }
    }
    return false;
}

bool isElement(string target, vector<string> list) {
    for(int i = 0; i < list.size(); i++) {
        if(target == list[i]) {
            return true;
        }
    }
    return false;
}

bool isInTerminal(string target) {
    for(int i = 0; i < Terminals.size(); i++) {
        if(Terminals[i] == target) {
            return true;
        }
    }
    return false;
}


// read grammar
void ReadGrammar()
{
    indexList.push_back("#");
    indexList.push_back("$");

    Rule thisRule;

    bool endStar = false;
    //read grammar token by token
    while(true){ //for each rule in the set of rules

        //first get token to analyze it
        Token iterator = lexGrammar->GetToken();

        //if hash, end of grammar
        if (iterator.token_type == HASH){
            break;
        }
        //if arrow, skip
        else if (iterator.token_type == ARROW){
            continue;
        }
        //if id, add to left hand side
        else if (iterator.token_type == ID){

            //index
            if(index(iterator.lexeme) == -1) {
                indexList.push_back(iterator.lexeme);    
            }

            //get left hand side
            thisRule.leftHand = iterator;

            //now get right hand side (a list)
            while (true) { //for each RHS in rule

                //get token to analyze it
                Token token = lexGrammar->GetToken();
                //if  star, end of rule
                if (token.token_type == STAR) {
                    break;
                }
                //if id, add to RHS
                else if (token.token_type == ID) {

                    //index
                    if(index(token.lexeme) == -1) {
                        indexList.push_back(token.lexeme);    
                    }
                    thisRule.rightHand.push_back(token);
                }
            }
            //reset
            endStar = false;
        }
        ruleList.push_back(thisRule);
    }

    //making terminals and nonterminals 
        //{NONTERMINAL} --> {TERMINAL}*
        //----formatting
        //Terminals first, then nontemrinals are printed all in one line
        //  ----make nonterminal list - easy just make sure no repeats----

    //first add the first LHS into non terminals
    Nonterminals.push_back(ruleList[0].leftHand.lexeme);

    //now add the rest
    for(int i = 1; i < ruleList.size(); i++) { //for each rule's left hand
        //if not in nonterminals already, add
        if (!isInNonterminals(ruleList[i].leftHand.lexeme)){
            Nonterminals.push_back(ruleList[i].leftHand.lexeme);
        }//if in nonterminals, dont add, its a repeat
    }  
    //  ----we now have complete nonterminals list (no repeats)---- 
      
    //iterate through full ruleList
    for(int i = 0; i < ruleList.size(); i++) { //for each rule's right hand

        for(int j = 0; j < ruleList[i].rightHand.size(); j++) { //for each right hand id
            
            //if this RHS is not a nonterminal, add to terminals and make sure no repeat
            if(!isInNonterminals(ruleList[i].rightHand[j].lexeme) && !isInTerminal(ruleList[i].rightHand[j].lexeme)) {
                Terminals.push_back(ruleList[i].rightHand[j].lexeme);
            }
            //if in nonterminals, is a nonterminal
        }
    }
    //  ----we now have complete terminals list (no repeats)----

    //INDEX METHOD for sorting
    for(int i = 0; i < indexList.size(); i++) {
        //if index[i] is a nonTerminal and not in the sortedTerminals, add it to the sortedNonTerminals
        if(!(isElement(indexList[i], sortedNonterminals)) && isInNonterminals(indexList[i])) {
            sortedNonterminals.push_back(indexList[i]);
        }
    }
    
}

// Task 1
void printTerminalsAndNoneTerminals()
{
    for(int i = 0; i < Terminals.size(); i++) {
        cout << Terminals[i] << " ";
    }

    for(int i = 0; i < sortedNonterminals.size(); i++) {
        cout << sortedNonterminals[i] << " ";
    }
}

bool rhsTrue(bool generating[], vector<Token> rhs) {
    for(int i = 0; i < rhs.size(); i++) {
        if(!generating[index(rhs[i].lexeme)]) {
            return false;
        }
    }
    return true;
}

// Task 2
void RemoveUselessSymbols()
{
    //remove any rules that are A->episolon
    //go through the grammar if there are any RHS that are *, that is an epsilon rule and needs to be removed

    //remove any unaccessible rules S->AB, C->c, A->a, B->b: remove C->c

    bool generating[indexList.size()];

    //set all terminals to generating
    for(int i = 0; i < indexList.size(); i++) {
        if(isInTerminal(indexList[i]) || indexList[i] == "#" || indexList[i] == "$") {
            generating[i] = true;
        }
    }

    bool change = true;

    while(change) {
        //go through each rule and check if the RHS is all generating
        //if it is change the LHS variable to True
        for(int i = 0; i < ruleList.size(); i++) {//goes through each rule
            for(int j = 0; j < ruleList[i].rightHand.size(); j++) { //go through the RHS of each rule
                if(rhsTrue(generating, ruleList[i].rightHand)) { //if the full RHS is true set the left hadn side variable to true
                    generating[index(ruleList[i].leftHand.lexeme)] = true;
                }
                else{
                    change = false;
                }
            }
        }
    }

    //remove any rules with a non generating symbol
    //create a generating vector
    vector<Rule> generatingRules;
    //go through the rule list and add any rules that are generating to the vector
    for(int i = 0; i < ruleList.size(); i++) {
        if(generating[index(ruleList[i].leftHand.lexeme)]) {
            cout << "HERE1" << endl;
        }
        if(isElement(ruleList[i].leftHand.lexeme, generatingRules)) {
            cout << "HERE2" << endl;
        }
        //if it is a generating rule, add it to the generatingRuleList && check for repeats
        if(generating[index(ruleList[i].leftHand.lexeme)] == true && 
            isElement(ruleList[i].leftHand.lexeme, generatingRules)) {
                generatingRules.push_back(ruleList[i]);
        }
    }

    //print out generating rules
    for(int i = 0; i < generatingRules.size(); i ++) {
        cout << i << " " << generatingRules[i].leftHand.lexeme << endl;
    }

    //caluclate reachable symbols
    //vector to hold all the reachable rules
    vector<Rule> reachableRules;
    //boolean array to hold which symbols are reachable, all should be false
    bool reachable[indexList.size()];

    //save start symbol
    string startSymbol = ruleList[0].leftHand.lexeme;
    //start symbol will always be reachable
    reachable[index(startSymbol)] = true;

    //go through the rest of the rules and any RHS with a reachable LHS will become reachable
    for(int i = 0; i < generatingRules.size(); i++) {
        if(reachable[index(generatingRules[i].leftHand.lexeme)] == true) {
            for(int j = 0; j < generatingRules[i].rightHand.size(); j++) {
                reachable[index(generatingRules[i].rightHand[j].lexeme)] = true;
            }
            //add the reachable rule to the reachable rule vector
            reachableRules.push_back(generatingRules[i]);
        }
    }


    cout << "going to print" << endl;
    //print out the reachableRule vector
    for(int i = 0; i < reachableRules.size(); i++) {
        cout << "forloop" << endl;
        //print LHS
        cout << reachableRules[i].leftHand.lexeme << " "; 
        //print ARROW
        cout << "-> ";
        //print RHS
        for(int j = 0; j < reachableRules[i].rightHand.size(); j++) {
            cout << reachableRules[i].rightHand[j].lexeme << " ";
        }
        //print new line
        cout << endl;
    }
    
    
}


// Task 3
void CalculateFirstSets()
{
    std::cout << "3\n";

    //S->ABC, FIRST(S) = FIRST(A)
    //if FIRST(A) contains epsilon, FIRST(S) = FIRST(A) - epislon U FIRST(B)

    vector<string>  firstSets;


}

// Task 4
void CalculateFollowSets()
{
    std::cout << "4\n";
}

// Task 5
void CheckIfGrammarHasPredictiveParser()
{
    std::cout << "5\n";
    //if has useless rules, not predictive parser?
}
    
int main (int argc, char* argv[])
{
    int task;

    if (argc < 2)
    {
        std::cout << "Error: missing argument\n";
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
            std::cout << "Error: unrecognized task number " << task << "\n";
            break;
    }
    return 0;
}
