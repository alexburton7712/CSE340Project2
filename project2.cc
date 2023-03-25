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

//standard, using throughout 
vector<Rule> ruleList;
vector<int> indexLHS;
vector<int> indexRHS;
vector<string> indexList; 
vector<string> sortedNonterminals;

//mainly for task 1
vector<string> Terminals;
vector<string> Nonterminals;


//printing functions, for debugging
void printRule(Rule thisRule) {
    cout << thisRule.leftHand.lexeme  << " -> ";

    for (int k = 0; k < thisRule.rightHand.size(); k++) {
        cout << thisRule.rightHand[k].lexeme << " ";
    }
    cout << endl;
}
void printRules(vector<Rule> list) {

    cout << "   ----PRINTING RULES----  " << endl;

    for (int j = 0; j < list.size(); j++) {
        Rule thisRule = list[j];
        printRule(thisRule);
    }
    cout << "   ----------------------  " << endl;

}



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

//for task 2 and 5
bool HasUselessRules() {
    vector<bool> reachable(ruleList.size(), false);
    reachable[0] = true; // start symbol is always reachable

    // Compute reachable rules
    for (int i = 0; i < ruleList.size(); i++) {
        if (reachable[i]) {
            Rule thisRule = ruleList[i];
            for (int j = 0; j < thisRule.rightHand.size(); j++) {
                Token token = thisRule.rightHand[j];
                if (isInNonterminals(token.lexeme) && !reachable[index(token.lexeme)]) {
                    reachable[index(token.lexeme)] = true;
                }
            }
        }
    }

    bool hasUseless = false;
    // Check for unreachable rules
    for (int i = 0; i < ruleList.size(); i++) {
        if (!reachable[i]) {
            hasUseless = true;
        }
    }
    if (!hasUseless) {
        return false;
    }
    else{
        return true;
    }
}

bool HasLeftRecursion() {
    //for each nonterminal
    for (int i = 0; i < Nonterminals.size(); i++) {

        //for each rule
        for (int j = 0; j < ruleList.size(); j++) {
            //if this rule is for the current nonterminal and the first token
            //on the right hand side is also the current nonterminal
            if (ruleList[j].leftHand.lexeme == Nonterminals[i]
                && ruleList[j].rightHand[0].lexeme == Nonterminals[i]) {
                //left recursion found
                return true;
            }
        }
    }
    //no left recursion found
    return false;
}

// read grammar
void ReadGrammar()
{
    indexList.push_back("#");
    indexList.push_back("$");

    //read grammar token by token
    while(true){ //for each rule in the set of rules
        Rule thisRule;
        bool endStar = false;

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
            while (endStar == false) { //for each RHS in rule

                //get next token to analyze it
                Token token = lexGrammar->GetToken();
                //if  star, end of rule
                if (token.token_type == STAR) {
                    endStar = true;
                }
                else if (iterator.token_type == ARROW){
                    continue;
                }
                //if id, add to RHS
                else if (token.token_type == ID) {

                    //index
                    if(index(token.lexeme) == -1) {
                        indexList.push_back(token.lexeme);    
                    }
                    thisRule.rightHand.push_back(token);
                }
                else{
                    continue;
                }
            }
        }
        ruleList.push_back(thisRule);

        //now have complete rule list 
        
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
    

    //now have complete terminals and nonterminals list
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

    //
    //  ----REACHABLE TERMINALS----
    //

    /*
    vector<Rule> reachableRules;
    vector<string> reachableNonterminals;
    //  ---------------------------
    cout << "----STARTING RULES----" << endl;
    printRules(ruleList);
    //
    //  ----get first rule in reachable----
    //this is final list we use to print
    Rule firstRule = ruleList[0];
    //add first rule
    reachableRules.push_back(firstRule);
    //
    reachableNonterminals.push_back(firstRule.leftHand.lexeme);
    //add all of rhs (nonterminals)
    for (int i = 0; i < firstRule.rightHand.size(); i++){
        //add to reachableList
        if (find(Nonterminals.begin(), Nonterminals.end(), firstRule.rightHand[i].lexeme) != Nonterminals.end()){
            reachableNonterminals.push_back(firstRule.rightHand[i].lexeme);
        }
    }
    //
    //  ----look at rest of rules----
    //for each rule
    for (int j = 1; j < ruleList.size(); j++) {
        Rule iterator = ruleList[j];
        string left = iterator.leftHand.lexeme;
        vector<Token> right = iterator.rightHand;
        //if lhs is in reachable nonterminals
        if (find(reachableNonterminals.begin(), reachableNonterminals.end(), left) != reachableNonterminals.end()) {
            //  ----add rule----
            reachableRules.push_back(iterator);
            //  dont add lhs again, would be repeated
            //  ----add rhs----
            //add all of rhs (nonterminals)
            for (int i = 0; i < right.size(); i++){
                //if this rhs is a nonterminal and not already in reachable, add
                if (find(Nonterminals.begin(), Nonterminals.end(), firstRule.rightHand[i].lexeme) != Nonterminals.end()
                    && find(reachableNonterminals.begin(), reachableNonterminals.end(), right[i].lexeme) == reachableNonterminals.end()){
                    reachableNonterminals.push_back(right[i].lexeme);
                }
            }
        }
    }
    
    */

    //
    //  ----REACHABLE TERMINALS----
    //

    //make vector of bools, corresponds with rule list - for reachable
    vector<bool> reachable(ruleList.size(), false); 
    //starting will be the one to be reached
    reachable[0] = true;

    //for each rule
    for (int i = 0; i < ruleList.size(); i++) {

        Rule thisRule = ruleList[i];
        //if this rule is reachable 
        if (reachable[i]) {
            //for each rhs
            for (int j = 0; j < thisRule.rightHand.size(); j++) {
                Token token = thisRule.rightHand[j];
                //if this rhs is a nonterminal and not already in reachable, make this rule reachable
                if (isInNonterminals(token.lexeme) && !reachable[index(token.lexeme)]) {
                    reachable[index(token.lexeme)] = true;
                }
            }
        }
    }

    //make vector of bools, corresponds with rule list - for generatable
    vector<bool> generatable(Nonterminals.size(), false);

    //for each rule
    for (int i = 0; i < ruleList.size(); i++) {

        Rule thisRule = ruleList[i];
        //bool to keep track that all are terminals
        bool allTerminals = true;
        //for each rhs
        for (int j = 0; j < thisRule.rightHand.size(); j++) {
            Token token = thisRule.rightHand[j];
            //if this rhs is a nonterminal, and not terminals
            if (isInNonterminals(token.lexeme)) {
                //set to false, end
                allTerminals = false;
                break;
            }
        }
        //now done going through rhs
        if (allTerminals) {
            //if they are all terminals, make this rule generatable
            generatable[index(thisRule.leftHand.lexeme)] = true;
        }
    }


    //vector of bools for usefule rules - corresponds with rule list, now we get both
    vector<bool> useful(ruleList.size(), false);
    //for each rule
    for (int i = 0; i < ruleList.size(); i++) {
        Rule thisRule = ruleList[i];

        //if its reachable
        if (reachable[i]) {
            //keep track all usefule
            bool allUseful = true;

            //for each rhs
            for (int j = 0; j < thisRule.rightHand.size(); j++) {
                Token token = thisRule.rightHand[j];
                //if it is a nonterminal and not generatable, it is not useful
                if (isInNonterminals(token.lexeme) && !generatable[index(token.lexeme)]) {
                    allUseful = false;
                    break;
                }
            }

            //if all usefule stands, make this a useful rule
            if (allUseful) {
                useful[i] = true;
            }
        }
    }

    //printing, for each rule, 
    for (int i = 0; i < ruleList.size(); i++) {
        //if usefule, print
        if (useful[i]) {
            Rule thisRule = ruleList[i];
            cout << thisRule.leftHand.lexeme << " -> ";
            for (int j = 0; j < thisRule.rightHand.size(); j++) {
                Token token = thisRule.rightHand[j];
                cout << token.lexeme << " ";
            }
            cout << endl;
        }
    }

            
         /*   
    //
    //  ----GENERATING RULES----
    //
    //for each rule not in reaching
    for (int i = 0; i < ruleList.size(); i++){
        const Rule thisRule = ruleList[i];
        //if this rule is not in reachable
        //errors here, comparing pointer == nonpointer
        if (find(reachableRules.begin(), reachableRules.end(), thisRule) != reachableRules.end()){
            cout << "rule is not in reachable yet: " << endl;
            printRule(thisRule);
            //rule is not in reachable yet
            //check if rhs is only terminals
            for (int j = 0; j < thisRule.rightHand.size(); j++){
                //if in terminals
                if (find(Terminals.begin(), Terminals.end(), thisRule.rightHand[j].lexeme) == Terminals.end()){
                    cout << "this is not a terminal, end: " << thisRule.rightHand[j].lexeme << endl;
                    break;
                }
            }
        }
    }
    bool generating[indexList.size()];
    //set all terminals to generating
    for(int i = 0; i < indexList.size(); i++) {
        if(isInTerminal(indexList[i]) || indexList[i] == "#" || indexList[i] == "$") {
            generating[i] = true;
        }
    }
    bool change = true;
    while(change) {
        //go through each rule and check if the RHS is all generating all terminals
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
        printRules(generatingRules);
    }
    */



    
}

//use ruleList
string FirstSet(string variable, string firstSetList[]) {
    for(int i = 0; i < ruleList.size(); i++) {
        if(ruleList[i].leftHand.lexeme == variable) {
            if(isInTerminal(ruleList[i].rightHand[0].lexeme)) {
                firstSetList[index(variable)] += ruleList[i].rightHand[0].lexeme + " ";
            }
            else {
                firstSetList[index(variable)] += firstSetList[index(ruleList[i].rightHand[0].lexeme)];
            }
        }
    }
}

// Task 3
void CalculateFirstSets()
{
    string firstSet[indexList.size()];
    //S->ABC, FIRST(S) = FIRST(A)
    //if FIRST(A) contains epsilon, FIRST(S) = FIRST(A) - epislon U FIRST(B)

    //firstSets of all terminals should be the terminal
    for(int i = 0; i < indexList.size(); i++) {
        if(isInTerminal(indexList[i])) {
            firstSet[i] = indexList[i];
        }
    }

    //go through the full index list, in order to make sure there is full coverage, 
    //you need to go through the indexList NonTerminalLsit.size() times
    for(int j = 0; j < Nonterminals.size(); j++) {
        for(int i = 0; i < indexList.size(); i++) {
            FirstSet(indexList[i], firstSet);
        }
    }


}

// Task 4
void CalculateFollowSets()
{
    std::cout << "4\n";
}

// Task 5
void CheckIfGrammarHasPredictiveParser()
{
    //if has useless rules, not predictive parser?
    if (HasUselessRules()) {
        std::cout << "NO\n";
        return;
    }
    if (HasLeftRecursion()){
        std::cout << "NO\n";
        return;
    }
    else{
        std::cout << "YES\n";
        return;
    }
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
