/*
 * Copyright (C) Mohsen Zohrevandi, 2017
 *               Rida Bazzi 2019
 * Do not share this file with anyone
 * Programmed by: Rohan Nair
 */
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "lexer.h"

#include <vector>
#include <set>
#include <map>
#include <unordered_set>
using namespace std;

bool syntaxError = false;

LexicalAnalyzer lexer = LexicalAnalyzer();


struct ruleSet {
    vector<string> terminal;
    vector<string> nont;

    bool canReach;
    bool canGen;

    string left;
    vector<string> right;
};

vector<ruleSet> inpGram;

vector<string> tokPrin;
vector<string> nonTPrin;

std::set<string> blanSet; //check if need
bool inLeft (string lexeme, vector<ruleSet> inpGramRule) {

    for (int i = 0; i < inpGramRule.size(); i++) {

            if (lexeme.compare(inpGramRule[i].left)== 0) {

                return true;
        }
    }

    return false;


}

void concatTerms(string lexeme) {
    for (int i = 0; i < tokPrin.size(); i++) {
        if (lexeme.compare(tokPrin[i])== 0) {

            return;
        }
    }

    tokPrin.push_back(lexeme);

}

void addToNonTerminals(string lexeme) {
    for (int i = 0; i < nonTPrin.size(); i++) {
        if (lexeme.compare(nonTPrin[i])== 0) {
            return;

        }
    }

    nonTPrin.push_back(lexeme);
}

void syntaxErrorVerif() {
    syntaxError = true;
    cout << "SYNTAX ERROR !!!\n";
}

void termDeterm() {
        for (int i = 0; i < inpGram.size(); i++) {
        addToNonTerminals(inpGram[i].left);
        for (int j = 0; j < inpGram[i].right.size(); j++) {
            if (inLeft(inpGram[i].right[j],inpGram)) {

                addToNonTerminals(inpGram[i].right[j]);

            } else {
                concatTerms(inpGram[i].right[j]);

            }
        }
    }

}


bool iterR(ruleSet *readingRuleule) {

    while (lexer.peek(1).token_type != STAR) {
        Token chec = lexer.GetToken();

        if (chec.token_type != ID) {
            return true;;

        } else {
            readingRuleule->right.push_back(chec.lexeme);
        }
    }
    return false;

}

// read inpGram
void ReadGrammar()
{
    Token readingTo = lexer.GetToken();
    while (readingTo.token_type!=HASH&&readingTo.token_type!=END_OF_FILE) {
        ruleSet *rule = new ruleSet; 
        if (readingTo.token_type == ID) {
            rule->left = readingTo.lexeme;
            rule->canGen = false;
            rule->canReach = false;
            readingTo = lexer.GetToken();

            if (readingTo.token_type == ARROW) {
                
                if (iterR(rule)) {
                    syntaxErrorVerif();
                    return;
                }
                readingTo = lexer.GetToken();
                if (readingTo.token_type == STAR) {

                    inpGram.push_back(*rule);

                } else {

                    delete rule;
                    syntaxErrorVerif();
                    return;

                }

            } else {
                delete rule;
                syntaxErrorVerif();
                return;

            }
        } else {

            syntaxErrorVerif();
            return;
        }

        readingTo = lexer.GetToken();
       
    }
    
    if (readingTo.token_type == END_OF_FILE) {
        syntaxErrorVerif();
        return;
    }

    if (readingTo.token_type == HASH) {
        readingTo = lexer.GetToken();
        if (readingTo.token_type!=END_OF_FILE) {
            syntaxErrorVerif();
            return;
        }
    }
    // cout << "0\n";
}

// Task 1
void printTerminalsAndNoneTerminals()
{
    termDeterm();
    for (int i = 0; i<tokPrin.size();i++) {
        cout<<tokPrin[i]<<" ";

    }
    for (int i = 0; i<nonTPrin.size();i++) {
        cout<<nonTPrin[i]<<" ";

    }

    cout<<endl;
    // cout << "1\n";
}

// Task 2
vector<ruleSet> redGram;


int verifReah = 0;
int genNumCheck = 0;


void reachVerifMeth(string lexeme){
    for (int i = 0; i < inpGram.size(); i++){
        if (inpGram[i].left.compare(lexeme)== 0&&!inpGram[i].canReach) {
            inpGram[i].canReach=true;
            verifReah++;
        }
    }
}

bool generProc(string lexeme){
    for (int i = 0; i < inpGram.size(); i++) {
        if (inpGram[i].left.compare(lexeme)== 0&&inpGram[i].canGen == true) {
           return true;
        }
    }


    return false;
}

bool generSearch(int num){
    ruleSet mainRule = inpGram[num];
    for (int i = 0; i < mainRule.right.size(); i++) {
        if (inLeft(mainRule.right[i],inpGram)&&!generProc(mainRule.right[i])) {
            return false;
        }

    }
    return true;
}

void getredGram(){
    reachVerifMeth(inpGram[0].left);
    int prev_reachable = 0;
    int prev_generating = 0;
    while (prev_generating!=genNumCheck||prev_reachable!=verifReah) {
        
        prev_generating = genNumCheck;
        prev_reachable = verifReah;
        
        for (int i = 0; i < inpGram.size(); i++) {
            if(inpGram[i].canGen ==false && generSearch(i)){
                inpGram[i].canGen = true;
                genNumCheck++;
            }
        }
        for (int i = 0; i<inpGram.size();i++) {
            if (inpGram[i].canReach ==true) {
                bool status = true;
                for (int j = 0; j < inpGram[i].right.size(); j++) {
                    if(inLeft(inpGram[i].right[j],inpGram)&&!generProc(inpGram[i].right[j])){
                        status = false;
                        break;
                    }
                }
                if (status == true) {
                    if (!inpGram[i].canGen) {
                        inpGram[i].canGen=true;
                        genNumCheck++;

                    }

                    for (int j= 0; j<inpGram[i].right.size();j++) {
                        if (inLeft(inpGram[i].right[j],inpGram)&&generProc(inpGram[i].right[j])) {
                            reachVerifMeth(inpGram[i].right[j]);

                        }
                    }
             
                }
            }
       }
       
    }
    
    
    for(int i = 0; i < inpGram.size(); i++){
        if(inpGram[i].canGen==true&&inpGram[i].canReach == true){
            redGram.push_back(inpGram[i]);
        }
    }
}

void RemoveUselessSymbols()
{
    // cout << "2\n";
    termDeterm();

    //
    getredGram();
    
    for (int i = 0; i < redGram.size(); i++) {
        cout<<redGram[i].left<<" -> ";
        if (redGram[i].right.size() == 0){
            cout<<"#";
        } else{
            cout<<redGram[i].right[0];
        }

        for(int j=1;j<redGram[i].right.size();j++){
            cout<<" "<<redGram[i].right[j];
        }
        cout<<endl;


    }
}

// Task 3
struct newruleSet {
    string left;
    vector<vector<string>> right;
};

vector<newruleSet> definition;
map < string,set<string> > firstSetList;

void createNewRuleSet() {
    unordered_set<string> lefts;
    for (int i = 0; i < inpGram.size(); i++) {
        if (lefts.count(inpGram[i].left) == 0) {
            newruleSet nrs;
            nrs.left = inpGram[i].left;
            nrs.right.push_back(inpGram[i].right);
            definition.push_back(nrs);
            lefts.insert(inpGram[i].left);
        }
        else {
            for (int j = 0; j < definition.size(); j++) {
                if (definition[j].left == inpGram[i].left) {
                    definition[j].right.push_back(inpGram[i].right);
                    break;
                }
            }
        }
    }
    
}

bool isTerminal(string s) {
     for (int i = 0;i<tokPrin.size();i++){
        if(tokPrin[i].compare(s)== 0){
            return true;
        }
     }
     return false;
}
bool isLast(string s, vector<string> vec, int index) {
    if (index== vec.size()-1) {
        return true;
    }
    return false;
}

bool hasEpsilon(string key) {
    return firstSetList.find(key)->second.find("#")!=firstSetList.find(key)->second.end();
}

bool isSubset(set<string> set1, set<string> set2) {
    for( string curr  : set2){
        if(set1.count(curr)== 0){
            return false;
        }
    }
    return true;
}

set<string> unify(set<string> set1, set<string> set2) {
    set<string> result = set1;
    result.insert(set2.begin(), set2.end());
    return result;
}

void printFirstSets(map<string,set<string> > currMap){
    for(string curr_nt : nonTPrin){
        if(currMap.find(curr_nt)!=currMap.end()){
            cout<<"FIRST"<<"("<<curr_nt<<") = { ";
            bool isFirst = true;
            set<string> currSet= currMap[curr_nt];
            if(currSet.find("#")!=currSet.end()){
                cout<<"#";
                isFirst = false;
            }
            for(string curr_t : tokPrin){
                if(currSet.find(curr_t)!=currSet.end()){
                    if(isFirst){
                        isFirst=false;
                        cout<<curr_t;
                    }
                    else{
                    cout<<", "<<curr_t;
                    }
                }
            }
            cout<<" }";

        }
        cout<<endl;
    }
}
void getFirstSets()
{

    
    termDeterm();
    createNewRuleSet();
    for (newruleSet n : definition) {
        set<string> empty_set = {};
        firstSetList.insert(pair<string, set<string> >(n.left, empty_set));
      
    }
    
    bool changed = true;
    
    while (changed) {
        changed =false;
        for (newruleSet rule : definition) {
            for (auto ors : rule.right) {
                
                if (ors.size() == 0) {
                    if (firstSetList[rule.left].count("#") == 0) {
                        changed = true;
                        firstSetList[rule.left].insert("#");
                    }
                    
                    continue;
                    }
                for (int i = 0; i < ors.size(); i++) {
                        if (isTerminal(ors[i])) {
                            if (firstSetList[rule.left].count(ors[i]) == 0) {
                                firstSetList[rule.left].insert(ors[i]);
                                changed = true;
                            }
                            break;
                        }

                    else {
                        set<string> old_set = firstSetList[rule.left];
                        set<string> newset = firstSetList[ors[i]];
                        newset.erase("#");
                        if(!isSubset(old_set, newset)){
                            changed = true;
                            set<string> tempfirst;
                            tempfirst = unify(newset, firstSetList[rule.left]);
                            firstSetList[rule.left] = tempfirst;
                        }

                        if (isLast(ors[i], ors, i)) {
                            if (hasEpsilon(ors[i])&&firstSetList[rule.left].count("#") == 0) {
                                    firstSetList[rule.left].insert("#");
                                    changed = true;
                            }
                        }
                        else {
                            if (!(hasEpsilon(ors[i]))) {
                                break;
                            }
                        }
                    }
                }
            }
            
        }
        
    }
   
}

void CalculateFirstSets()
{
    // cout << "3\n";
    getFirstSets();
    printFirstSets(firstSetList);
}

// Task 4
void printFollowSets(map<string,set<string> > currMap){
    for(string curr_nt : nonTPrin){
        if(currMap.find(curr_nt)!=currMap.end()){
            cout<<"FOLLOW"<<"("<<curr_nt<<") = { ";
            bool isFirst = true;
            set<string> currSet= currMap[curr_nt];
            if(currSet.find("$")!=currSet.end()){
                cout<<"$";
                isFirst = false;
            }
            for(string curr_t : tokPrin){
                if(currSet.find(curr_t)!=currSet.end()){
                    if(isFirst){
                        isFirst=false;
                        cout<<curr_t;
                    }
                    else{
                    cout<<", "<<curr_t;
                    }
                }
            }
            cout<<" } \n ";

        }
    }
}
map <string,set<string> > followSetList;
void getFollowSets(){
    getFirstSets();
    for (newruleSet n : definition) {
        set<string> empty_set;
        followSetList.insert(pair<string, set<string> >(n.left, empty_set));
    }
    followSetList[nonTPrin[0]].insert("$");

    cout<<endl;
    for(newruleSet currSet: definition){
        
        for(int j= 0;j<currSet.right.size();j++){
            if (currSet.right[j].size()>0) {
            for (int k= 0;k<currSet.right[j].size()-1;k++) {
                string curr_nt =currSet.right[j][k] ;
                
                if(inLeft(curr_nt,inpGram)){
                        for(int t = k+1;t<currSet.right[j].size();t++){
                            string check_nt =currSet.right[j][t];
                            if(inLeft(currSet.right[j][t],inpGram)){
                                bool hasEpsilon = firstSetList.find(check_nt)->second.find("#")!=firstSetList.find(check_nt)->second.end();
                                set<string> newset = firstSetList[check_nt];
                                newset.erase("#");
                                set<string> tempfirst;
                                tempfirst = unify(newset,followSetList[curr_nt]);
                                followSetList[curr_nt] = tempfirst;
                                if(!hasEpsilon){
                                    break;
                                }
                            } else {
                                followSetList.find(curr_nt)->second.insert(check_nt);
                                break;
                            }
                        }
                }
                
                
            }
            }
            //cout<<" , ";
            
        }
        //cout<<endl;
    }
    //printFollowSets(followSetList);
    
    bool changed = true;
    //while change is true : apply rules 2 and 3 for every rule
    while (changed == true) {
        changed = false;
        for(newruleSet currSet : definition){
            string left_nt = currSet.left;
            for (int j= 0;j<currSet.right.size();j++){
                
                if(currSet.right[j].size()>0){
                for(int k = currSet.right[j].size()-1;k>= 0;k--){
                    string curr_nt = currSet.right[j][k];
                    
                    if(inLeft(curr_nt,inpGram)){
                
                        if (!isSubset(followSetList[curr_nt],followSetList[left_nt])) {
                            set<string> newset = followSetList[left_nt];
                            set<string> tempfollow;
                            tempfollow = unify(newset,followSetList[curr_nt]);
                            followSetList[curr_nt] = tempfollow;
                            changed = true;
                        }
                        bool hasEpsilon = firstSetList[curr_nt].count("#")==1;
                        if(!hasEpsilon){
                            break;
                        }
                    }
                    else{
                        //stop iterating through if the curr_nt is terminal
                        break;
                }
                    
                }
                //cout<<endl;
            }
            }
        }
       
    }
    
}
bool intersectionIsNull(set<string> set1, set<string> set2){
    for (string curr : set1 ){
        if(set2.count(curr) == 1){
            return false;
        }
    }
    return true;
}

void CalculateFollowSets()
{
    // cout << "4\n";
    getFollowSets();
    printFollowSets(followSetList);

}

// Task 5
void CheckIfGrammarHasPredictiveParser()
{
    // cout << "5\n";
    getFollowSets();

    getFirstSets();
    
    for(newruleSet currRule :definition){
        set<string> major_comp; //idList1
        if(currRule.right.size()>=1){
        int it = 0;
        for(int i = 0;i<currRule.right.size()-1;i++){
            vector<string> rule = currRule.right[i];
            it++;
            set<string> currStrings;
            if(rule.size()== 0){
                currStrings.insert("#");
            }
            else{
            for(int j = 0;j<rule.size();j++){
                string curr = rule[j];
                if(inLeft(curr,inpGram)){
                     set<string> newset = firstSetList[curr];
                    if(j!=rule.size()-1&&newset.count("#")==1){
                        newset.erase("#");
                    }
                    currStrings = unify(currStrings,newset);
                    if(firstSetList[curr].count("#")== 0){
                        break;
                    }

                }
                else{
                    currStrings.insert(curr);
                    break;
                }
            }
        }
            if(!intersectionIsNull(currStrings,major_comp)){
                
                for(string t : currStrings){
                    //cout<< t;
                }
                //cout<<endl;
                for(string t : major_comp){
                    //cout<< t;
                }        
                //cout<<endl;        
                cout<<"NO";
               
                return;
        
            }
            else{
                major_comp = unify(major_comp,currStrings);
            }
        }
    }
    } 
        for (int i = 0; i < nonTPrin.size(); i++) {
            if (firstSetList[nonTPrin[i]].count("#") == 1){
                if(!intersectionIsNull(firstSetList[nonTPrin[i]],followSetList[nonTPrin[i]])) {
                    cout<<"NO"<<endl;
                    return;

                }
            }
        }
    
    cout<<"YES"<<endl;
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
    
    ReadGrammar();  // Reads the input inpGram from standard input
                    // and represent it internally in data structures
                    // ad described in project 2 presentation file
    if (syntaxError == false){

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
    }
    return 0;
}

