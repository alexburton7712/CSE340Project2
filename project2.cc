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
vector<vector<string> > firstSets;
vector<vector<string> > followSets;



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
int findSortedNonterminalIndex(vector<string> sorted, string target){
    for(int i = 0; i < sorted.size(); i++) {
        if(sorted[i] == target) {
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


void addToSet(vector<string> &existing, vector<string> toInput){
    //add each string in toInput one by one into this existing 2d string vector
    //insert at exising's given index, add all strings to that vector

    //for example, add each string from toInput (rhs first set)
    //into this lhs first set and check for repeats

    //add rhs into lhs --> (lhs, rhs)
    
    for (int i = 0; i < toInput.size(); i++){
        //if this string not already in this existing set, add
        // if(allEpsilon && !isElement("#", existing)) {
        //     existing.push_back("#");
        // }
        if (!isElement(toInput[i], existing) && toInput[i] != "#"){
            existing.push_back(toInput[i]);
        }
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

vector<string> removeRepeats(vector<string> list) {
    vector<string> newList;
    if(list.size() != 0) {
        for(int i = 0; i < list.size(); i++) {
            if(!isElement(list[i], newList)) {
                newList.push_back(list[i]);
            }
        }
    }
    return newList;
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

bool hasEpsilon(Rule rule) {
    //checks the first set of each variable in the RHS
    //if they all have epsilon, return true, else return false
    for(int i = 0; i < rule.rightHand.size(); i++) { 
        if(!isElement("#", firstSets[index(rule.rightHand[i].lexeme)])) {
            return false;
        }
    }
    return true;
}

void CalculatingFirst(){

    //empty string vector to push into each slot of firstSets
    vector<string> set;
    //initialize the firstSets
    for(int i = 0; i < indexList.size(); i++) {
        firstSets.push_back(set);
    }

    for (int n = 0; n < ruleList.size(); n++){
    for (int i = 0; i < ruleList.size(); i++){
        //for ease
        Rule thisRule = ruleList[i];
        //to help find firstSet of this LHS (firstSet is corresponding with indexList)
        int indexThisLHS = index(thisRule.leftHand.lexeme);

        //if empty, add # if not already there
        if (thisRule.rightHand.size() == 0){
            if (!isElement("#", firstSets[indexThisLHS])){
                firstSets[indexThisLHS].push_back("#");
            } 
        }
        //else not empty, go through rhs
        else { 
            //get first rhs and determine
            Token thisRight = thisRule.rightHand[0];
            int indexStartRHS = index(thisRight.lexeme);

            bool hasAllEpsilon = hasEpsilon(thisRule);

            //if a terminal, just add and stop
            if (isInTerminal(thisRight.lexeme)){
                //if not already in this rule's (lhs) firstset, add this terminal to it
                if (!isElement(thisRight.lexeme, firstSets[indexThisLHS])){
                    firstSets[indexThisLHS].push_back(thisRight.lexeme);
                    //add then stop
                } 
            }
            else if(hasAllEpsilon && !isElement("#", firstSets[indexThisLHS])) {
                firstSets[indexThisLHS].push_back("#");
            }
            //else is nonterminal, will have a first set and has #, add and loop
            else if(isInNonterminals(thisRight.lexeme) && isElement("#", firstSets[indexStartRHS])){
                // //first add this nonterminal's first set into this rule's (lhs) first set
                // addToSet(firstSets[indexThisLHS], firstSets[indexStartRHS]);
                int k = 0; 

                //then we loop through rhs
                //while in bounds, starting from first one
                while (k < thisRule.rightHand.size()){ //for the rest of rsh

                    if(isInTerminal(thisRule.rightHand[k].lexeme)) {
                        if(!isElement(thisRule.rightHand[k].lexeme, firstSets[indexThisLHS])) {
                            firstSets[indexThisLHS].push_back(thisRule.rightHand[k].lexeme);
                        }
                        break;
                    }                
                    
                    //get index of this RHS in indexList - for ease
                    int indexThisRHS = index(thisRule.rightHand[k].lexeme);

                    //if non terminal first set has #, add nonterminal first set to this rule(lhs)'s first set
                    if (isInNonterminals(thisRule.rightHand[k].lexeme) && isElement("#", firstSets[indexThisRHS])){
                        //add thisRHSNonTerminal first set to thisRule'sLHS first set
                        addToSet(firstSets[indexThisLHS], firstSets[indexThisRHS]);
                    }
                    else if (isInTerminal(thisRule.rightHand[k].lexeme)){
                        //if not already, add
                        if (!isElement(thisRule.rightHand[k].lexeme, firstSets[indexThisLHS])){
                            firstSets[indexThisLHS].push_back(thisRule.rightHand[k].lexeme);
                            break;
                            //add then stop
                        } 
                    }
                    else if (isInNonterminals(thisRule.rightHand[k].lexeme) && !isElement("#", firstSets[indexThisRHS])){
                        //add thisRHSNonTerminal first set to thisRule'sLHS first set
                        addToSet(firstSets[indexThisLHS], firstSets[indexThisRHS]);
                        break;
                    }
                    k++;
                }
            }
            //else nonterminal with no empty, just add and end
            else if (isInNonterminals(thisRight.lexeme) && !isElement("#", firstSets[indexStartRHS])){
                //if this rhs's first set not empty, continue
                if (firstSets[indexStartRHS].size() != 0){
                    //first add this nonterminal's first set into this rule's (lhs) first set
                    //add first set of this rhs into lhs
                    addToSet(firstSets[indexThisLHS], firstSets[indexStartRHS]);
                    //not adding here

                }   
                //if empty, just skip
            }
        }

    }

    }

}


//mainly for task 5
bool HasUselessRules() {
    int size  = indexList.size();

    //
    //  ----GENERATING RULES----
    //

    //  ----STEP 1: SET ALL TERMINALS TO GENERATING----
    bool generatingIndex[indexList.size()];
    //initialize to false first
    for (int i = 0; i < indexList.size(); i++) {
        generatingIndex[i] = false;
    }
    for(int i = 0; i < indexList.size(); i++) {
        if(isInTerminal(indexList[i]) || indexList[i] == "#" || indexList[i] == "$") {
            generatingIndex[i] = true;
        }
    }
    
    //  ---STEP 2: GO THRU EACH RULE----
    for(int n = 0; n < ruleList.size(); n++) {
        //for each rule
        for(int i = 0; i < ruleList.size(); i++) {

            //if epislon, then generating 
            if(ruleList[i].rightHand.size() == 0) {
                generatingIndex[index(ruleList[i].leftHand.lexeme)] = true;
            }
            else{
                //if all of rhs is generating, make this rule (this lhs) generating
                if(rhsTrue(generatingIndex, ruleList[i].rightHand) == true) {
                    generatingIndex[index(ruleList[i].leftHand.lexeme)] = true;
                }
            }
        }
    }

    //  ---STEP 3: ADD GENERATING LHS TO GENERATING RULES----
    vector<bool> generatableList(ruleList.size(), false); //use this for cross reference
    //go through the rule list and add any rules that are generating to the vector
    for(int i = 0; i < ruleList.size(); i++) {
        //if it is a generating rule's lhs, add it to the generatingRuleList
        if(generatingIndex[index(ruleList[i].leftHand.lexeme)] && rhsTrue(generatingIndex, ruleList[i].rightHand)) {
            generatableList[i] = true;
        }
    }

    vector<Rule> generatingRules;
    for(int i = 0; i < ruleList.size(); i++) {
        //if it is a generating rule's lhs, add it to the generatingRuleList
        if(generatableList[i]) {
            generatingRules.push_back(ruleList[i]);
        }
    }
    
    //
    //  ----REACHABLE RULES----
    //

    //using generatble rules (filtered rule list)

    //  ---STEP 4: MAKE FIRST SYMBOL REACHABLE----

    //if starting symbol is not generating, then cannot generate anything
    //if !generating(index[ruleList[0].leftHand.lexeme])

    bool reachableIndex[indexList.size()];
    //first make all false
    for (int i = 0; i < indexList.size(); i++) {
        reachableIndex[i] = false;
    }


    //if the starting symbol is generating, continue to make reachable list
    if (generatingIndex[index(ruleList[0].leftHand.lexeme)]){
        
        //save start symbol - if we have some generating rules, set the first one to reaching
        if (generatingRules.size() != 0){
            reachableIndex[index(generatingRules[0].leftHand.lexeme)] = true;
        }

        //  ---STEP 5: FIND REACHABLE RULES----
        for(int n = 0; n < generatingRules.size(); n++) {
            for(int i = 0; i < generatingRules.size(); i++) {
                
                // if this rule's lhs is reachable
                // go through the rest of the rules and any RHS with a reachable LHS will become reachable
                                
                if(reachableIndex[index(generatingRules[i].leftHand.lexeme)]) {
                    if (generatingRules[i].rightHand.size() == 0){
                        reachableIndex[index("#")] = true;
                    }
                    else{
                        //for each rhs, make reachable
                        for(int j = 0; j < generatingRules[i].rightHand.size(); j++) {
                            reachableIndex[index(generatingRules[i].rightHand[j].lexeme)] = true;
                        }
                    }
                }
        
            }
        }
    }

    

    //  ---STEP 6: ADD REACHABLE LHS TO REACHABLE RULES----
    vector<bool> reachableList(generatingRules.size(), false); //use this for cross reference
    //go through the rule list and add any rules that are generating to the vector
    for(int i = 0; i < generatingRules.size(); i++) {
        //for each generating rule, if its a reachable symbol, make this rule reachable
        if(reachableIndex[index(generatingRules[i].leftHand.lexeme)]) {
            reachableList[i] = true;
        }
    }

    bool hasUseless = false;
    // Check for unreachable rules
    for (int i = 0; i < generatingRules.size(); i++) {
        if (!reachableList[i]) {
            hasUseless = true;
            break;
        }
    }
    if (hasUseless) {
        return true;
    }
    else{
        return false;
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
    int size  = indexList.size();

    //
    //  ----GENERATING RULES----
    //

    //  ----STEP 1: SET ALL TERMINALS TO GENERATING----
    bool generatingIndex[indexList.size()];
    //initialize to false first
    for (int i = 0; i < indexList.size(); i++) {
        generatingIndex[i] = false;
    }
    for(int i = 0; i < indexList.size(); i++) {
        if(isInTerminal(indexList[i]) || indexList[i] == "#" || indexList[i] == "$") {
            generatingIndex[i] = true;
        }
    }
    
    //  ---STEP 2: GO THRU EACH RULE----
    for(int n = 0; n < ruleList.size(); n++) {
        //for each rule
        for(int i = 0; i < ruleList.size(); i++) {

            //if epislon, then generating 
            if(ruleList[i].rightHand.size() == 0) {
                generatingIndex[index(ruleList[i].leftHand.lexeme)] = true;
            }
            else{
                //if all of rhs is generating, make this rule (this lhs) generating
                if(rhsTrue(generatingIndex, ruleList[i].rightHand) == true) {
                    generatingIndex[index(ruleList[i].leftHand.lexeme)] = true;
                }
            }
        }
    }

    //  ---STEP 3: ADD GENERATING LHS TO GENERATING RULES----
    vector<bool> generatableList(ruleList.size(), false); //use this for cross reference
    //go through the rule list and add any rules that are generating to the vector
    for(int i = 0; i < ruleList.size(); i++) {
        //if it is a generating rule's lhs, add it to the generatingRuleList
        if(generatingIndex[index(ruleList[i].leftHand.lexeme)] && rhsTrue(generatingIndex, ruleList[i].rightHand)) {
            generatableList[i] = true;
        }
    }

    vector<Rule> generatingRules;
    for(int i = 0; i < ruleList.size(); i++) {
        //if it is a generating rule's lhs, add it to the generatingRuleList
        if(generatableList[i]) {
            generatingRules.push_back(ruleList[i]);
        }
    }
    
    //
    //  ----REACHABLE RULES----
    //
    //using generatble rules (filtered rule list)

    //  ---STEP 4: MAKE FIRST SYMBOL REACHABLE----
    //if starting symbol is not generating, then cannot generate anything
    //if !generating(index[ruleList[0].leftHand.lexeme])
    bool reachableIndex[indexList.size()];
    //first make all false
    for (int i = 0; i < indexList.size(); i++) {
        reachableIndex[i] = false;
    }
    //if the starting symbol is generating, continue to make reachable list
    if (generatingIndex[index(ruleList[0].leftHand.lexeme)]){

        //save start symbol - if we have some generating rules, set the first one to reaching
        if (generatingRules.size() != 0){
            reachableIndex[index(generatingRules[0].leftHand.lexeme)] = true;
        }

        //  ---STEP 5: FIND REACHABLE RULES----
        for(int n = 0; n < generatingRules.size(); n++) {
            for(int i = 0; i < generatingRules.size(); i++) {
                
                // if this rule's lhs is reachable
                // go through the rest of the rules and any RHS with a reachable LHS will become reachable
                                
                if(reachableIndex[index(generatingRules[i].leftHand.lexeme)]) {
                    if (generatingRules[i].rightHand.size() == 0){
                        reachableIndex[index("#")] = true;
                    }
                    else{
                        //for each rhs, make reachable
                        for(int j = 0; j < generatingRules[i].rightHand.size(); j++) {
                            reachableIndex[index(generatingRules[i].rightHand[j].lexeme)] = true;
                        }
                    }
                }
        
            }
        }
    }

    //  ---STEP 6: ADD REACHABLE LHS TO REACHABLE RULES----
    vector<bool> reachableList(generatingRules.size(), false); //use this for cross reference
    //go through the rule list and add any rules that are generating to the vector
    for(int i = 0; i < generatingRules.size(); i++) {
        //for each generating rule, if its a reachable symbol, make this rule reachable
        if(reachableIndex[index(generatingRules[i].leftHand.lexeme)]) {
            reachableList[i] = true;
        }
    }

    //  ----STEP 7: PRINT----
    for (int i = 0; i < generatingRules.size(); i++) {
        //if usefule, print
        if (reachableList[i]) {

            Rule thisRule = generatingRules[i];
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

//bubble sort algorithm to sort the list in order that the terminals appear in the indexList
void reOrder(vector<string> &list) {

    string str = "";

    for(int i = 0; i < list.size(); i++) {
        for(int j = 0; j < list.size() - 1; j++) {
            if(index(list[j]) > index(list[j + 1])) {
                //switch the two entries
                //cout << list[j] << " " <<  list[j + 1] << endl;
                str = list[j];
                list[j] = list[j + 1];
                list[j + 1] = str;
                //cout << list[j] << " " <<  list[j + 1] << endl;
            }
        }
    }
}

// Task 3
void CalculateFirstSets()
{
    CalculatingFirst();

    //make sure the firstSets are in order as they appear in the grammar
    for(int i = 0; i < firstSets.size(); i++) {
        reOrder(firstSets[i]);
    }

    for (int i = 0; i < indexList.size(); i++){
        if (isInNonterminals(indexList[i])){
            cout << "FIRST(" << indexList[i] << ") = { ";
            for (int j = 0; j < firstSets[i].size(); j++){
                cout << firstSets[i][j];
                if (j != firstSets[i].size() - 1){
                    cout <<  ", ";
                }
            }
            cout << " }";
            cout << endl;
        }
    }

}

// // Task 4
// void CalculateFollowSets(){

//     CalculatingFirst();

//     // 1) FOLLOW(S) = { $ }   // where S is the starting Non-Terminal

//     // 2) If A -> pBq is a production, where p, B and q are any grammar symbols,
//     // then everything in FIRST(q)  except Є is in FOLLOW(B).

//     // 3) If A->pB is a production, then everything in FOLLOW(A) is in FOLLOW(B).

//     // 4) If A->pBq is a production and FIRST(q) contains Є, 
//     // then FOLLOW(B) contains { FIRST(q) – Є } U FOLLOW(A) 


//     //empty string vector to push into each slot of followSets
//     vector<string> set;
//     //initialize the followSets, corresponds with indexList
//     for(int i = 0; i < indexList.size(); i++) {
//         followSets.push_back(set);
//     }
//     //now we have a vector of vectorofstrings for each nonterminal, how they appear in the grammar


//     //  ----Step 1: Initialize follow set of start symbol----
//     //this is fine
//     for(int i = 0; i < indexList.size(); i++) {
//         if (isInNonterminals(indexList[i])){
//             followSets[i].push_back("$"); //should be third symbol... after # and $
//             break;
//         }
//     }


//     ///printing first sets jsut to reference 
//     for (int i = 0; i < indexList.size(); i++){
//         if (isInNonterminals(indexList[i])){
//             cout << "FIRST(" << indexList[i] << ") = { ";
//             for (int j = 0; j < firstSets[i].size(); j++){
//                 cout << firstSets[i][j];
//                 if (j != firstSets[i].size() - 1){
//                     cout <<  ", ";
//                 }
//             }
//             cout << " }";
//             cout << endl;
//         }
//     }

//     //  ----Step 2: if S-> A B, first of B into follow of A----
//     //for each rule
//     for (int i = 0; i < ruleList.size(); i++) {
//         //iterator
//         Rule rule = ruleList[i];
//         //get the last rhs symbol
//         string lastRHS = ruleList[i].rightHand[rule.rightHand.size() - 1].lexeme;

//         //if this lastrhs is a terminal, loop through nonterminalrhs that have # in first sets
//         //and add to their follows
//         if (isInTerminal(lastRHS)){ //if this rhs is terminal - keep adding to nonterminals that have # in first set
//             //the terminal for ease
//             string thisTerminal = rule.rightHand[j].lexeme;
//             //get rhs indexList index for ease
//             int indexLastRHS = index(lastRHS);

//             int k = 1; //since terminal, start with second to last one
//             //while in bounds, is a nonterminal, and has # in first set
//             while((rule.rightHand.size()-k) >= 0 && isInNonterminals(rule.rightHand[j-k].lexeme) && isElement("#", firstSets[index(rule.rightHand[j-k].lexeme)])){
//                 //the previous nonterminal index
//                 int indexPreviousRHS = index(rule.rightHand[rule.rightHand.size()-k].lexeme);

//                 //add this RHS terminal into previous RHS nonterminal if not already in there
//                 if (!isElement(thisTerminal, followSets[indexPreviousRHS])){
//                     followSets[indexPreviousRHS].push_back(thisTerminal);
//                 }
//                 k++;
//             }
//         }
//         //else it is a nonterminal
//         else if (isInNonterminals(lastRHS)){

//             //for each rhs starting from the back, 
//             for (int j = rule.rightHand.size() - 1; j >= 0; j--) {
                
//                 //going back counter
//                 int k = 1;
//                 //this rhs' first
//                 int indexFirst = index(rule.rightHand[j].lexeme);
//                 //while in bounds, and previous is nontemrminal, and this RHS's first set has #
//                 while((j-k) >= 0 && isInNonterminals(rule.rightHand[j-k].lexeme) && isElement("#", firstSets[index(rule.rightHand[j-k].lexeme)])){


//             }
            
//         }

//         //for each rhs - starting from back
//         for (int j = rule.rightHand.size() - 1; j >= 0; j--) {

//             //if this rhs is nonterminal, put this nonterminal into previous nonterminal
//             if (isInNonterminals(rule.rightHand[j].lexeme)) {
//                 //rightHand[j] is THIS rhs
//                 //rightHand[j-k] is PREVIOUS rhs rule, in this case it is a nonterminal

//                 //going back counter
//                 int k = 1;
//                 //this rhs' first
//                 int indexFirst = index(rule.rightHand[j].lexeme);
//                 //while in bounds, and previous is nontemrminal, and this RHS's first set has #
//                 while((j-k) >= 0 && isInNonterminals(rule.rightHand[j-k].lexeme) && isElement("#", firstSets[index(rule.rightHand[j-k].lexeme)])){
//                     //the previous one's follow set index
//                     int indexFollow = index(rule.rightHand[j-k].lexeme);
//                     //put entire first of this nonterminal, into this follow of previous nonterminals(s)
//                     addToSet(followSets[indexFollow], firstSets[indexFirst]);     //already checks for repeats            
//                     k++;
//                 }
//                 // //thisRHS in LSH, and that rule's rhs == empty
//                 // //while righthand[j-k] doesnt have epsilon and is a nonterminal
//                 // while (isInNonterminals(rule.rightHand[j-k].lexeme) && 
//                 //     ruleList[goToRuleLeft(rule.rightHand[j-k].lexeme)].rightHand.size() == 0){ //the nonterminal(s) part
//                 //     //the previous one's follow set index
//                 //     int indexFollow = index(rule.rightHand[j-k].lexeme);
//                 //     //put first of this nonterminal, into follow of previous nonterminals(s)
//                 //     followSets[indexFollow].push_back(firstSets[indexFirst]);                    
//                 //     k++;
//                 // }
//                 // while (isInNonterminals(rule.rightHand[j-k].lexeme)){
//                 //     int p = 1;
//                 //     while (ruleList[goToRuleLeft(rule.rightHand[j-k].lexeme) + p].rightHand.size() != 0 
//                 //     && (goToRuleLeft(rule.rightHand[j-k].lexeme) + p) > ruleList.size()){
//                 //         //the previous one's follow set index
//                 //         int indexFollow = index(rule.rightHand[j-k].lexeme);
//                 //         //finding rule that has epsilon (make sure stay in bounds)
//                 //         p++;
//                 //     }
//                 //     if (ruleList[goToRuleLeft(rule.rightHand[j-k].lexeme) + p].rightHand.size() == 0){
//                 //         //if it rhs empty = epsilon, then add this first set into A
//                 //         //followSets[i].push_back() <-first set of rule.rightHand[j]
//                 //         k++;
//                 //     }
//                 // }
//             }

//             //if this rhs is terminal - keep adding to nonterminals that have # in first set
//             // else if (isInTerminal(rule.rightHand[j].lexeme)) {
//             //     string thisTerminal = rule.rightHand[j].lexeme;
//             //     //rightHand[j] is THIS rhs
//             //     //rightHand[j-k] is PREVIOUS rhs rule, in this case it is a terminal

//             //     //going back counter
//             //     int k = 1;
//             //     //while in bounds, PREVIOUS rhs is a nonterminal, and this RHS's first set has #

//             //     //put B into A (B is terminal and A is nonterminal)
//             //     while((j-k) >= 0 && isInNonterminals(rule.rightHand[j-k].lexeme) && isElement("#", firstSets[index(rule.rightHand[j-k].lexeme)])){
//             //         //the previous one's follow set index
//             //         int indexPreviousRHS = index(rule.rightHand[j-k].lexeme);
//             //         //add this RHS terminal into previous RHS nonterminal if not already in there
//             //         if (!isElement(thisTerminal, followSets[indexPreviousRHS])){
//             //             followSets[indexPreviousRHS].push_back(thisTerminal);
//             //         }
//             //         k++;
//             //     }
//             //     // while (isInNonterminals(rule.rightHand[j-k].lexeme)){
//             //     //     int p = 1;
//             //     //     while (ruleList[goToRuleLeft(rule.rightHand[j-k].lexeme) + p].rightHand.size() != 0 
//             //     //     && (goToRuleLeft(rule.rightHand[j-k].lexeme) + p) > ruleList.size()){
//             //     //         //finding rule that has epsilon (make sure stay in bounds)
//             //     //         p++;
//             //     //     }
//             //     //     if (ruleList[goToRuleLeft(rule.rightHand[j-k].lexeme) + p].rightHand.size() == 0){
//             //     //         //if it rhs empty = epsilon, then add this terminal into A
//             //     //         followSets[i].push_back(rule.rightHand[j-k].lexeme);
//             //     //         k++;
//             //     //     }
//             //     // }
//             // }
//         }
//     }

//     //  ----Step 3: if S-> A B, follow of S into follow of B----
//     //for each rule
//     for (int i = 0; i < ruleList.size(); i++) {
//         //iterator
//         Rule rule = ruleList[i];
//         //for each rhs - reverse
//         for (int j = rule.rightHand.size() - 1; j >= 0; j--) {
//             //if this rhs is nonterminal
//             if (isInNonterminals(rule.rightHand[j].lexeme)) {
//                 //rightHand[j] is THIS rhs
//                 //rightHand[j-k] is PREVIOUS rhs rule, in this case it is a nonterminal
//                 //going back counter
//                 int k = 0; //start from very last rhs first
//                 //thisRHS in LSH, and that rule's rhs != empty
//                 //while righthand[j-k] doesnt have epsilon and is a nonterminal
//                 while((j-k) >= 0 && isInNonterminals(rule.rightHand[j-k].lexeme) && ruleList[goToRuleLeft(rule.rightHand[j-k].lexeme)].rightHand.size() == 0){
//                     //index for this lhs
//                     int indexFollowLHS = index(rule.leftHand.lexeme);
//                     //index for this iterating rhs
//                     int indexFollowRHS = index(rule.rightHand[j-k].lexeme);
//                     //add follow of LHS into follow of RHS
//                     addToSet(followSets[indexFollowRHS], followSets[indexFollowLHS]);
//                     k++;
//                 }
//             }
//             if (isInTerminal(rule.rightHand[j].lexeme)) {
//                 break;
//             }
//         }
//     }

//     cout << "   ----END FOLLOW SETS----" << endl;
//     for (int i = 0; i < indexList.size(); i++){
//         if (isInNonterminals(indexList[i])){
//             cout << "FOLLOW(" << indexList[i] << ") = "; // = { 
//             for (int j = 0; j < followSets[i].size(); j++){
//                 cout << followSets[i][j];
//                 if (j != followSets[i].size() - 1){
//                     cout <<  ", ";
//                 }
//             }
//             //cout << " }";
//             cout << endl;
//         }
//     }
// }

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
        std::cout << "NO\n";
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

        // case 4: CalculateFollowSets();
        //     break;

        case 5: CheckIfGrammarHasPredictiveParser();
            break;

        default:
            cout << "Error: unrecognized task number " << task << "\n";
            break;
    }
    return 0;
}

