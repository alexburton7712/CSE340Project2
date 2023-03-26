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


//mainly for task 3
string fuckingBitch(string variable, string set) {
    //one last attempt at a recursive function
    //I believe this is my best attempt yet
    cout << "Set = " << set << endl;
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
// void FirstSet(string variable, string firstSetList[]) {
//     for(int i = 0; i < ruleList.size(); i++) {
//         if(ruleList[i].leftHand.lexeme == variable) {
//             if(isInTerminal(ruleList[i].rightHand[0].lexeme)) {
//                 firstSetList[index(variable)] += ruleList[i].rightHand[0].lexeme + " ";
//             }
//             else {
//                 firstSetList[index(variable)] += firstSetList[index(ruleList[i].rightHand[0].lexeme)];
//             }
//         }
//     }
// }

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

    // cout << "index list: \t\t";
    // for (int i = 0; i < indexList.size(); i++){
    //     cout << indexList[i] << " ";
    // }
    // cout << endl;

    // cout << "generating index: \t";
    // for (int i = 0; i < indexList.size(); i++){
    //     cout << generatingIndex[i] << " ";
    // }
    // cout << endl;
    // cout << "reachable index: \t";
    // for (int i = 0; i < indexList.size(); i++){
    //     cout << reachableIndex[i] << " ";
    // }
    // cout << endl;

    // cout << "generating rules: \t";
    // for (int i = 0; i < generatableList.size(); i++){
    //     cout << generatableList[i] << " ";
    // }
    // cout << endl;

    // cout << "reachable rules: \t";
    // for (int i = 0; i < reachableList.size(); i++){
    //     cout << reachableList[i] << " ";
    // }
    // cout << endl;

    

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

// Task 3
void CalculateFirstSets()
{
    //empty string vector to push into each slot of firstSets
    vector<string> set;
    //initialize the firstSets
    for(int i = 0; i < indexList.size(); i++) {
        firstSets.push_back(set);
    }


    //firstSets of all terminals should be the terminal
    for(int i = 0; i < indexList.size(); i++) {
        if(isInTerminal(indexList[i])) {
            firstSets[i].push_back(indexList[i]);
        }
    }

    bool changed = true;
    while(changed) {
        for(int i = 0; i < ruleList.size(); i++) {
            //rule III
            if(ruleList[i].rightHand.size() != 0) {
                for(int j = 0; j < firstSets[index(ruleList[i].rightHand[0].lexeme)].size(); j++) {
                    firstSets[index(ruleList[i].leftHand.lexeme)].push_back(firstSets[index(ruleList[i].rightHand[0].lexeme)][j]);
                    changed = true;
                }
            }
            //rule IV

            //rule V
            bool allHaveEpsilon = true;
            for(int j = 0; j < ruleList[i].rightHand.size(); j++) {
                if(!isElement("#", firstSets[index(ruleList[i].rightHand[j].lexeme)])) {
                    allHaveEpsilon = false;
                    break;
                }
            }
            if(allHaveEpsilon) {
                firstSets[index(ruleList[i].leftHand.lexeme)].push_back("#");
                changed = true;
            }
        }
    }

    for(int l = 0; l < ruleList.size(); l++) {
        for(int i = 2 ; i < indexList.size(); i++) {
            for(int j = 0; j < ruleList.size(); j++) {
                if(ruleList[j].leftHand.lexeme == indexList[i]) {
                    if(ruleList[j].rightHand.size() == 0) { //epsilon where the RHS is empty
                        firstSets[index(ruleList[j].leftHand.lexeme)].push_back("#");
                    }
                    else {// otherwise add the First Set of the RHS[0] to firstSet of LHS
                        for(int k = 0; k < firstSets[index(ruleList[j].rightHand[0].lexeme)].size(); k++) {
                            firstSets[index(ruleList[j].leftHand.lexeme)].push_back(firstSets[index(ruleList[j].rightHand[0].lexeme)][k]);
                        }
                    }
                }
            }
        }
    }

    //remove repeats
    for(int i = 0; i < firstSets.size(); i++) {
        for(int j = 0; j < firstSets[i].size(); j++) {
            firstSets[i] = removeRepeats(firstSets[i]);
        }
    }

    string str = "";

    for(int j = 0; j < firstSets.size(); j++) {
            for(int k = 0; k < firstSets[j].size() - 1; k++) {
                cout << "HERE" << endl;
                if(index(firstSets[j][k]) > index(firstSets[j][k + 1])) {
                    str = firstSets[j][k];
                    cout << str << endl;
                    firstSets[j][k] = firstSets[j][k + 1];
                    firstSets[j][k + 1] = str;
                }
            }
        }

    for(int i = 0; i < indexList.size(); i++) {
        if(isInNonterminals(indexList[i])) {
            cout << "FIRST(" << indexList[i] << ") = { ";
            for(int j = 0; j < firstSets[i].size(); j++) {
                if(j != firstSets[i].size() - 1) {
                    cout << firstSets[i][j] << ", ";
                }
                else {
                    cout << firstSets[i][j];
                }
            }
            cout << " }" << endl;
        }
    }



}




// void FollowSet(string variable, string set) {
//     for(int i = 0; i < ruleList.size(); i++) {
//         for(int j = 0; j < ruleList[i].rightHand.size(); j++) {
//             if(ruleList[i].rightHand[j].lexeme == variable && (j + 1) = ruleList[i].rightHand.size()) {
//                  set += firstSet(ruleList[i].rightHand[j])     
//             }           
//         }
//     }
// }

    // Task 4
void CalculateFollowSets()
{
    // 1) FOLLOW(S) = { $ }   // where S is the starting Non-Terminal

    // 2) If A -> pBq is a production, where p, B and q are any grammar symbols,
    // then everything in FIRST(q)  except Є is in FOLLOW(B).

    // 3) If A->pB is a production, then everything in FOLLOW(A) is in FOLLOW(B).

    // 4) If A->pBq is a production and FIRST(q) contains Є, 
    // then FOLLOW(B) contains { FIRST(q) – Є } U FOLLOW(A) 


    vector<vector<string> > followSets(Nonterminals.size(), vector<string>(1));

    // for (int i = 0; i < Nonterminals.size(); i++) {
    //     followSets[i] = std::vector<string>();
    // }

    //  ----Step 1: Initialize follow set of start symbol----
    followSets[0].push_back("$"); //this is for all nonterminals

    //  ----Step 2: if S-> A B, first of B into follow of A----
    //for each rule
    for (int i = 0; i < ruleList.size(); i++) {
        //iterator
        Rule rule = ruleList[i];
        //for each rhs - reverse
        for (int j = rule.rightHand.size() - 1; j >= 0; j--) {
            //if this rhs is nonterminal
            if (isInNonterminals(rule.rightHand[j].lexeme)) {
                int k = 1;
                //the rule number that this rhs is defined at
                int index = goToRuleLeft(rule.rightHand[j-k].lexeme);
                //thisRHS in LSH, and that rule's rhs == empty
                //while righthand[j-k] doesnt have epsilon and is a nonterminal
                while (isInNonterminals(rule.rightHand[j-k].lexeme) && 
                    ruleList[goToRuleLeft(rule.rightHand[j-k].lexeme)].rightHand.size() == 0){
                    //followSets[i].push_back() <-first set of rule.rightHand[j]
                    k++;
                }
                while (isInNonterminals(rule.rightHand[j-k].lexeme)){
                    int p = 1;
                    while (ruleList[goToRuleLeft(rule.rightHand[j-k].lexeme) + p].rightHand.size() != 0 
                    && (goToRuleLeft(rule.rightHand[j-k].lexeme) + p) > ruleList.size()){
                        //finding rule that has epsilon (make sure stay in bounds)
                        p++;
                    }
                    if (ruleList[goToRuleLeft(rule.rightHand[j-k].lexeme) + p].rightHand.size() == 0){
                        //if it rhs empty = epsilon, then add this first set into A
                        //followSets[i].push_back() <-first set of rule.rightHand[j]
                        k++;
                    }
                }
                    

            }
            if (isInTerminal(rule.rightHand[j].lexeme)) {
                //if a terminal add to previous RHS while it can generate epsilons
                int k = 1;
                //while the rhs is non terminal
                while (isInNonterminals(rule.rightHand[j-k].lexeme)){
                    int p = 1;
                    while (ruleList[goToRuleLeft(rule.rightHand[j-k].lexeme) + p].rightHand.size() != 0 
                    && (goToRuleLeft(rule.rightHand[j-k].lexeme) + p) > ruleList.size()){
                        //finding rule that has epsilon (make sure stay in bounds)
                        p++;
                    }
                    if (ruleList[goToRuleLeft(rule.rightHand[j-k].lexeme) + p].rightHand.size() == 0){
                        //if it rhs empty = epsilon, then add this terminal into A
                        followSets[i].push_back(rule.rightHand[j-k].lexeme);
                        k++;
                    }
                }
            }
        }
    }

    //  ----Step 3: if S-> A B, follow of S into follow of B----
    //for each rule
    for (int i = 0; i < ruleList.size(); i++) {
        //iterator
        Rule rule = ruleList[i];
        //for each rhs - reverse
        for (int j = rule.rightHand.size() - 1; j >= 0; j--) {
            //if this rhs is nonterminal
            if (isInNonterminals(rule.rightHand[j].lexeme)) {

                //last rhs is non terminal
                //while nonterminal and j-k has epsilon 


                int k = 1;
                //the rule number that this rhs is defined at
                int index = goToRuleLeft(rule.rightHand[j-k].lexeme);
                //thisRHS in LSH, and that rule's rhs != empty
                //while righthand[j-k] doesnt have epsilon and is a nonterminal
                while (isInNonterminals(rule.rightHand[j-k].lexeme)){
                    int p = 1;
                    while (ruleList[goToRuleLeft(rule.rightHand[j-k].lexeme) + p].rightHand.size() != 0 
                    && (goToRuleLeft(rule.rightHand[j-k].lexeme) + p) > ruleList.size()){
                        //finding rule that has epsilon (make sure stay in bounds)
                        p++;
                    }
                    if (ruleList[goToRuleLeft(rule.rightHand[j-k].lexeme) + p].rightHand.size() == 0){
                        //if it rhs empty = epsilon, then add this terminal into A
                        //followSets[j].push_back() <-follow set of rule.rightHand[j-k]
                        k++;
                    }
                }
                    

            }
            if (isInTerminal(rule.rightHand[j].lexeme)) {
                break;
            }
        }
    }

    for (int i = 0; i < followSets.size(); i++){
        for (int j = 0; j < followSets.size(); j++){
            cout << followSets[i][j] << " ";
        }
        cout << endl;
    }
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

