/*
 * Copyright (C) Mohsen Zohrevandi, 2017
 *               Rida Bazzi 2019
 * Do not share this file with anyone
 */
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "lexer.h"

using namespace std;


LexicalAnalyzer* lexGrammar = new LexicalAnalyzer();
class Rule {
    public:
        Token leftHand;
        std::vector<Token> rightHand;
};

//
//  ----LISTS----
//
vector<Rule> ruleList;
vector<int> indexLHS;
vector<int> indexRHS;
vector<string> indexList; 
vector<string> Terminals;
vector<string> Nonterminals;
vector<string> sortedNonterminals;


//
//  ----HELPER FUNCTIONS----
//
int index(string target) {
    for(int i = 0; i < indexList.size(); i++) {
        if(indexList[i] == target) {
            return i;
        }
    }
    return -1;
}
int goToRuleLeft(string lhs){
    //given a string, find which index in ruleList it is 
    //defined (or on left side)
    for(int i = 0; i < ruleList.size(); i++) {
        if(ruleList[i].leftHand.lexeme == lhs) {
            return i;
        }
    }
    return -1;
}
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

bool rhsTrue(bool generating[], vector<Token> rhs) {
    for(int i = 0; i < rhs.size(); i++) {
        if(!generating[index(rhs[i].lexeme)]) {
            return false;
        }
    }
    return true;
}




//mainly for task 1
void makeTerminals(){
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
}
void makeNonterminals(){
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
      
}

//mainly for task 2
vector<bool> makeReachableList(){
    //make vector of bools, corresponds with rule list - for reachable
    vector<bool> reachable(ruleList.size(), false); 
    //starting will be the one to be reached
    reachable[0] = true;

    //for each rule
    for (int i = 0; i < ruleList.size(); i++) {

        Rule thisRule = ruleList[i];

        //if this rule is reachable 
        if (reachable[i]) {
            
            //make all other rules that start with thisRule.lhs reachable
            for (int j = 0; j < ruleList.size(); j++) {
                Rule iterating = ruleList[j];
                if (iterating.leftHand.lexeme == thisRule.leftHand.lexeme){
                    reachable[j] = true;
                }
            }

            //for each rhs
            for (int j = 0; j < thisRule.rightHand.size(); j++) {
                Token right = thisRule.rightHand[j];
                //if this rhs is a nonterminal and not already in reachable, make this rule reachable
                if (isInNonterminals(right.lexeme)) {
                    //go through list
                    for (int k = 0; k < ruleList.size(); k++){
                        Rule iterating = ruleList[k];
                        //if this rule's LHS is the right, reachable
                        if (right.lexeme == iterating.leftHand.lexeme){
                            reachable[k] = true;
                        }
                    }
                    
                }
            }
        }
    }
    return reachable;
}
vector<bool> makeGeneratableList(){
    //make vector of bools, corresponds with rule list - for generatable
    vector<bool> generatable(ruleList.size(), false);

    for (int n = 0; n < ruleList.size(); n++) {
        //for each rule
        for (int i = 0; i < ruleList.size(); i++) {

            Rule thisRule = ruleList[i];

            if (thisRule.rightHand.size() == 0){
                generatable[i] = true;
            }
            else{
                //bool to keep track that all are terminals
                bool allGeneratable = true;
                //for each rhs
                for (int j = 0; j < thisRule.rightHand.size(); j++) {
                    Token token = thisRule.rightHand[j];
                    //if this rhs is a nonterminal, and not generating, stop
                    if (isInNonterminals(token.lexeme) && !generatable[goToRuleLeft(token.lexeme)]) {
                        allGeneratable = false;
                        break;
                    }
                }
                //now done going through rhs
                if (allGeneratable == true) {
                    //if they are all terminals, make this rule generatable
                    generatable[i] = true;
                }
            }
        }
    }

    return generatable;
}
vector<bool> makeUsefulList(vector<bool> reachable, vector<bool> generatable){
    
    vector<bool> useful(ruleList.size(), false);
/*
    // for (int i = 0; i < ruleList.size(); i++){

    //     Rule thisRule = ruleList[i];

    //     if (reachable[goToRuleLeft(thisRule.leftHand.lexeme)] == 1){
    //         bool allGeneratable = true;

    //         for (int j = 0; j < thisRule.rightHand.size(); j++){

    //             Token thisRHS = thisRule.rightHand[j];
    //             //if this rhs is a nonterminal
    //             if (isInNonterminals(thisRHS.lexeme)){
    //                 //check if in generatable
    //                     //generatable at the index where this RHS is a LHS (rule starter), if false
    //                 if (generatable[goToRuleLeft(thisRHS.lexeme)] == 0){
    //                     allGeneratable = false;
    //                     break;
    //                 }
    //                 //else continue, its fine
    //             }
    //             //else is a terminal
    //         }

    //         if (allGeneratable == true){
    //             useful[i] = true;
    //         }
    //     }

        
    // }
*/
    //first get all generating rules
    bool allFalse = true;
    for (int i = 0; i < ruleList.size(); i++){
        if (generatable[i]){
            useful[i] = true;
            allFalse = false;
        }
    }
    if (allFalse == true){
        return useful;
    }


    //first get all reaching rules
    for (int i = 0; i < ruleList.size(); i++){
        if (reachable[i]){
            bool usefulBool = true;
            //for each rhs
            for (int j = 0; j < ruleList[i].rightHand.size(); j++){
                //if not generating nonterminal, stop - where this rhs is defined (on lhs) is it generating
                    //i realized there can be multiple rules for one id....
                if (isInNonterminals(ruleList[i].rightHand[j].lexeme) && generatable[goToRuleLeft(ruleList[i].rightHand[j].lexeme)] == 0){
                    usefulBool = false;
                    break;

                    // //get reapeats
                    // //if this rhs is the same a the next one and the next, make 
                    // for (int k = j + 1; k < ruleList[i].rightHand.size(); k++){
                    //     if (ruleList[i].rightHand[j].lexeme == ruleList[i].rightHand[k].lexeme)
                    // }

                }
                else if (isInTerminal(ruleList[i].rightHand[j].lexeme)){
                    continue;
                }
            }
            if (usefulBool){
                useful[i] = true;
            }
            //useful[i] = true;
        }
        else{
            useful[i] = false;
        }
    }

    /*
    //for each rule
    for (int i = 0; i < ruleList.size(); i++) {

        Rule thisRule = ruleList[i];
        
        //if this rule is generatable and reachable, mark it as useful
        if (reachable[i]) {
            bool allUseful = true;
            //for each rhs
            for (int j = 0; j < thisRule.rightHand.size(); j++) {
                Token right = thisRule.rightHand[j];

                //if this rhs is a nonterminal and not already generatable, check if it's useful
                if (isInNonterminals(right.lexeme) && !generatable[index(right.lexeme)]) {
                    
                    bool allGeneratable = true;
                    //go through rhs
                    for (int k = 0; k < thisRule.rightHand.size(); k++){
                        Token token = thisRule.rightHand[k];
                        //if token is nonterminal and not generatable
                        if (isInNonterminals(token.lexeme) && !generatable[index(token.lexeme)]) {
                            allGeneratable = false;
                            break;
                        }
                    }
                    //if all tokens are generatable, mark this rule as useful
                    if (allGeneratable) {
                        useful[i] = true;
                        generatable[index(right.lexeme)] = true;
                    }
                }
            }
        }
        
    }*/
    return useful;
}

//mainly for task 3
string fuckingBitch(string variable, string set) {
    //one last attempt at a recursive function
    //I believe this is my best attempt yet
    if(isInTerminal(variable)) {
        set += variable;
    }
    else {
        for(int i = 0; i < ruleList.size(); i++) {
            if(ruleList[i].leftHand.lexeme == variable) {
                set += fuckingBitch(ruleList[i].rightHand[0].lexeme, set);
            }
        }
    }
    return set;
}
void FirstSet(string variable, string firstSetList[]) {
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



//mainly for task 5
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

    makeNonterminals();
    makeTerminals();


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

// Task 2
void RemoveUselessSymbols()
{
    vector<bool> reachableList = makeReachableList();
    vector<bool> generatableList = makeGeneratableList();
    vector<bool> usefulList = makeUsefulList(reachableList, generatableList);

    cout << "reachableList: " << endl;
    for (int i = 0; i < reachableList.size(); i++){
        cout << reachableList[i] << " ";
    }
    cout << endl;
    cout << "generatableList: " << endl;
    for (int i = 0; i < generatableList.size(); i++){
        cout << generatableList[i] << " ";
    }
    cout << endl;
    cout << "usefulList: " << endl;
    for (int i = 0; i < usefulList.size(); i++){
        cout << usefulList[i] << " ";
    }
    cout << endl;

    //printing, for each rule, 
    for (int i = 0; i < ruleList.size(); i++) {
        //if usefule, print
        if (usefulList[i]) {
            Rule thisRule = ruleList[i];
            cout << thisRule.leftHand.lexeme << " -> ";
            if (thisRule.rightHand.size() == 0){
                cout << "#";
            }
            else{
                for (int j = 0; j < thisRule.rightHand.size(); j++) {
                    Token token = thisRule.rightHand[j];
                    cout << token.lexeme << " ";
                }
            }
            cout << endl;
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


    //loop to run the recursive function, assuming firstSet is all null again
    for(int i = 0; i < indexList.size(); i++) {
        fuckingBitch(indexList[i], firstSet[i]);
    }

    for (int i = 0; i < firstSet->size(); i++){
        cout << firstSet[i] << endl;
    }
}

// Task 4
void CalculateFollowSets()
{
    cout << "4\n";
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

