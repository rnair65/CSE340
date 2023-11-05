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

void concatNonTerms(string lexeme) {
    for (int i = 0; i < nonTPrin.size(); i++) {
        if (lexeme.compare(nonTPrin[i]) == 0) {

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
        concatNonTerms(inpGram[i].left);
        for (int j = 0; j < inpGram[i].right.size(); j++) {


            if (inLeft(inpGram[i].right[j],inpGram)) {

                concatNonTerms(inpGram[i].right[j]);

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
    int lastreachable = 0;
    int lastgenerating = 0;
    while (lastgenerating!=genNumCheck||lastreachable!=verifReah) {
        
        lastgenerating = genNumCheck;
        lastreachable = verifReah;
        
        for (int i = 0; i < inpGram.size(); i++) {
            if (inpGram[i].canGen == false && generSearch(i)) {

                inpGram[i].canGen = true;
                genNumCheck++;

            }
        }

        for (int i = 0; i < inpGram.size(); i++) {

            if (inpGram[i].canReach == true) {
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

                    for (int j = 0; j < inpGram[i].right.size(); j++) {


                        if (inLeft(inpGram[i].right[j],inpGram)&&generProc(inpGram[i].right[j])) {

                            reachVerifMeth(inpGram[i].right[j]);

                        }
                    }
             
                }
            }
       }
       
    }
    
    
    for (int i = 0; i < inpGram.size(); i++) {

        if (inpGram[i].canGen==true&&inpGram[i].canReach == true) {

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


        } else {

            cout<<redGram[i].right[0];

        }

        for (int j = 1; j < redGram[i].right.size(); j++) {
            cout<<" "<<redGram[i].right[j];

        }

        cout<<endl;


    }
}

// Task 3
struct newruleSet {
    vector<vector<string>> right;

    string left;

};

vector<newruleSet> def;

map < string,set<string> > firstSetList;

void devnooRS() {

    unordered_set<string> lefts;

    for (int i = 0; i < inpGram.size(); i++) {

        if (lefts.count(inpGram[i].left) == 0) {

            newruleSet diffRuleSet;
            diffRuleSet.left = inpGram[i].left;
            diffRuleSet.right.push_back(inpGram[i].right);

            def.push_back(diffRuleSet);
            lefts.insert(inpGram[i].left);

        } else {

            for (int j = 0; j < def.size(); j++) {

                if (def[j].left == inpGram[i].left) {

                    def[j].right.push_back(inpGram[i].right);
                    break;


                }
            }
        }
    }
    
}

bool termCheckBool(string s) {

     for (int i = 0; i < tokPrin.size();i++) {

        if (tokPrin[i].compare(s) == 0) {

            return true;
        }
     }

     return false;

}




bool lastCheckBool(string s, vector<string> vec, int index) {

    if (index == vec.size()-1) {
        return true;


    }

    return false;
}

bool epsiCheckBool(string key) {

    return firstSetList.find(key)->second.find("#")!=firstSetList.find(key)->second.end();

}

bool subsetCheckBool(set<string> initSet, set<string> secSet) {

    for (string pres : secSet) {

        if (initSet.count(pres) == 0) {

            return false;

        }
    }
    return true;

}

set<string> unify(set<string> initSet, set<string> secSet) {

    set<string> result = initSet;

    result.insert(secSet.begin(), secSet.end());

    return result;
}

void firSetListOutput(map<string,set<string> > presMap){
    for(string pres_nt : nonTPrin){

        if(presMap.find(pres_nt)!=presMap.end()){

            cout<<"FIRST"<<"("<<pres_nt<<") = { ";
            bool isFirst = true;

            set<string> presSet= presMap[pres_nt];

            if( presSet.find("#")!=presSet.end()) {
                cout<<"#";
                isFirst = false;

            }

            for (string pres_t : tokPrin) {

                if (presSet.find(pres_t)!=presSet.end()) {

                    if (isFirst) {

                        isFirst=false;
                        cout<<pres_t;

                    } else{

                        cout<<", "<<pres_t;

                    }
                }
            }

            cout<<" }";

        }
        cout<<endl;
    }
}


void firSetListRetr()
{

    
    termDeterm();

    devnooRS();
    for (newruleSet n : def) {
        set<string> empty_set = {};
        firstSetList.insert(pair<string, set<string> >(n.left, empty_set));
      
    }
    
    bool modif = true;
    
    while (modif) {

        modif = false;

        for (newruleSet rule : def) {
            for (auto ors : rule.right) {
                
                if (ors.size() == 0) {
                    if (firstSetList[rule.left].count("#") == 0) {
                        modif = true;
                        firstSetList[rule.left].insert("#");
                    }
                    
                    continue;

                    }
                for (int i = 0; i < ors.size(); i++) {
                        if (termCheckBool(ors[i])) {
                            if (firstSetList[rule.left].count(ors[i]) == 0) {
                                firstSetList[rule.left].insert(ors[i]);
                                modif = true;
                            }
                            break;
                        } else {

                        set<string> old_set = firstSetList[rule.left];
                        set<string> newset = firstSetList[ors[i]];

                        newset.erase("#");

                        if(!subsetCheckBool(old_set, newset)){

                            modif = true;
                            set<string> tempfirst;
                            tempfirst = unify(newset, firstSetList[rule.left]);
                            firstSetList[rule.left] = tempfirst;


                        }

                        if (lastCheckBool(ors[i], ors, i)) {

                            if (epsiCheckBool(ors[i])&&firstSetList[rule.left].count("#") == 0) {

                                    firstSetList[rule.left].insert("#");
                                    modif = true;

                            }
                        } else {
                            if (!(epsiCheckBool(ors[i]))) {

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
    firSetListRetr();

    firSetListOutput(firstSetList);


}

// Task 4
void folSetOutp(map<string,set<string> > presMap){

    for (string pres_nt : nonTPrin) {

        if (presMap.find(pres_nt)!=presMap.end()) {

            cout<<"FOLLOW"<<"("<<pres_nt<<") = { ";
            bool isFirst = true;

            set<string> presSet= presMap[pres_nt];

            if (presSet.find("$")!=presSet.end()) {

                cout<<"$";
                isFirst = false;

            }
            for (string pres_t : tokPrin) {

                if (presSet.find(pres_t)!=presSet.end()) {

                    if (isFirst) {
                        isFirst=false;
                        cout<<pres_t;

                    } else{
                        cout<<", "<<pres_t;

                    }
                }
            }
            cout<<" } \n ";

        }
    }
}
map <string,set<string> > followSetList;

void folSetRetr(){

    firSetListRetr();

    for (newruleSet n : def) {

        set<string> empty_set;
        followSetList.insert(pair<string, set<string> >(n.left, empty_set));

    }

    followSetList[nonTPrin[0]].insert("$");

    cout<<endl;
    for (newruleSet presSet: def) {
        
        for (int j= 0; j < presSet.right.size(); j++) {

            if (presSet.right[j].size() > 0) {

                for (int k = 0; k < presSet.right[j].size()-1; k++) {

                    string pres_nt = presSet.right[j][k] ;
                    
                    if (inLeft(pres_nt,inpGram)){

                            for(int t = k+1;t<presSet.right[j].size();t++) {

                                string check_nt =presSet.right[j][t];

                                if (inLeft(presSet.right[j][t],inpGram)) {

                                    bool epsiCheckBool = firstSetList.find(check_nt)->second.find("#")!=firstSetList.find(check_nt)->second.end();
                                    set<string> newset = firstSetList[check_nt];

                                    newset.erase("#");
                                    set<string> tempfirst;

                                    tempfirst = unify(newset,followSetList[pres_nt]);

                                    followSetList[pres_nt] = tempfirst;

                                    if(!epsiCheckBool){
                                        break;
                                    }
                                } else {
                                    followSetList.find(pres_nt)->second.insert(check_nt);

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
    //folSetOutp(followSetList);
    
    bool modif = true;
    //while change is true : apply rules 2 and 3 for every rule
    while (modif == true) {
        modif = false;

        for (newruleSet presSet : def) {

            string left_nt = presSet.left;

            for (int j = 0; j < presSet.right.size();j++) {
                
                if(presSet.right[j].size() > 0) {

                    for(int k = presSet.right[j].size()-1;k>= 0;k--) {
                        string pres_nt = presSet.right[j][k];
                        
                        if(inLeft(pres_nt,inpGram)){
                    
                            if (!subsetCheckBool(followSetList[pres_nt],followSetList[left_nt])) {
                                set<string> newset = followSetList[left_nt];
                                set<string> tempfollow;

                                tempfollow = unify(newset,followSetList[pres_nt]);
                                followSetList[pres_nt] = tempfollow;

                                modif = true;

                            }

                            bool epsiCheckBool = firstSetList[pres_nt].count("#") == 1;

                            if(!epsiCheckBool) {
                                break;
                            }
                        } else {

                            break;


                        }
                        
                    }
                //cout<<endl;
                }
            }
        }
       
    }
    
}

bool nullCheckIntersecBool(set<string> initSet, set<string> secSet){
    for (string pres : initSet) {

        if(secSet.count(pres) == 1){

            return false;

        }
    }


    return true;
}

void CalculateFollowSets()
{
    // cout << "4\n";
    folSetRetr();


    folSetOutp(followSetList);

}

// Task 5
void CheckIfGrammarHasPredictiveParser()
{
    // cout << "5\n";
    folSetRetr();

    firSetListRetr();
    
    for(newruleSet presRule :def){

        set<string> major_comp; 

        if (presRule.right.size() >= 1) {
            int it = 0;
            for(int i = 0;i<presRule.right.size()-1;i++){
                vector<string> rule = presRule.right[i];
                it++;
                set<string> presStrings;
                if(rule.size()== 0){
                    presStrings.insert("#");
                }
                else{
                for(int j = 0; j < rule.size(); j++){

                    string pres = rule[j];
                    if(inLeft(pres,inpGram)){

                        set<string> newset = firstSetList[pres];

                        if (j!=rule.size()-1&&newset.count("#") == 1) {
                            newset.erase("#");


                        }
                        presStrings = unify(presStrings,newset);
                        if (firstSetList[pres].count("#") == 0) {
                            break;
                        }

                    } else {
                        presStrings.insert(pres);
                        break;

                        
                    }
                }
            }
                if (!nullCheckIntersecBool(presStrings,major_comp)) {
                    
                    for(string t : presStrings){
                        //cout<< t;
                    }
                    //cout<<endl;
                    for(string t : major_comp){
                        //cout<< t;
                    }        
                    //cout<<endl;        
                    cout<<"NO";
                
                    return;
            
                } else {
                    major_comp = unify(major_comp,presStrings);

                }
        }
    }
    } 
        for (int i = 0; i < nonTPrin.size(); i++) {

            if (firstSetList[nonTPrin[i]].count("#") == 1){

                if(!nullCheckIntersecBool(firstSetList[nonTPrin[i]],followSetList[nonTPrin[i]])) {
                    
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

