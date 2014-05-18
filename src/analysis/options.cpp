/*
 *
 *                This source code is part of
 *                    ******************
 *                    ***   Pteros   ***
 *                    ******************
 *                 molecular modeling library
 *
 * Copyright (c) 2009-2013, Semen Yesylevskyy
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of Artistic License:
 *
 * Please note, that Artistic License is slightly more restrictive
 * then GPL license in terms of distributing the modified versions
 * of this software (they should be approved first).
 * Read http://www.opensource.org/licenses/artistic-license-2.0.php
 * for details. Such license fits scientific software better then
 * GPL because it prevents the distribution of bugged derivatives.
 *
*/

#include "pteros/analysis/options.h"
#include "pteros/core/pteros_error.h"
#include "boost/lexical_cast.hpp"
#include <boost/algorithm/string.hpp> // For to_lower
#include <sstream>

using namespace std;
using namespace pteros;

namespace pteros {

void parse_command_line(int argc, char** argv,
                        Options& toplevel,
                        std::string task_tag,
                        std::vector<Options>& tasks){

    if(argc<2) return; // Command line is empty

    bool in_task = false; // start with toplevel
    bool has_key = false;
    // tmp option
    Option o;
    o.name = "";
    // tmp task
    Options tsk;
    tsk.task_name = "";

    for(int i=1;i<argc;++i){        
        string str(argv[i]);

        if(str=="-") throw Pteros_error("Lone '-'!");

        //See if token contains '-' as first symbol not followed by number
        if(str[0]=='-' && str.size()>1 && !isdigit(str[1]) ){
            // This is key
            has_key = true;
            // strip '-'
            str = str.substr(1);

            // If we have filled option already, add it
            if(o.name!=""){
                if(!in_task){
                    toplevel.data.push_back(o);
                } else {
                    tsk.data.push_back(o);
                }
            }            

            // See if we got task tag
            if(str==task_tag){
                if(i==argc-1) throw Pteros_error("Incomplete task at the end of command line!");
                // If we already been in task mode, then end old task
                if(in_task){
                    if(tsk.task_name=="") throw Pteros_error("Task without name after"+string(argv[i-1])+"!");
                    tasks.push_back(tsk);
                }
                in_task = true;
                has_key = false;
                o.name = "";
                o.data.clear();
                tsk.task_name = "";
                tsk.data.clear();
                continue;
            }

            // set name
            o.name = str;
            // clear values
            o.data.clear();
        } else {
            // This is value
            if(!has_key && !in_task) throw Pteros_error("Error! Found value '"+str+"' without option name!");
            if(!has_key && in_task){
                // This is task name
                if(tsk.task_name!="") throw Pteros_error("Error: double task name '"+str+"'");
                if(i==argc-1) throw Pteros_error("Incomplete task at the end of command line!");
                tsk.task_name = str;
                continue;
            }
            // Add it
            o.data.push_back(str);
        }
    }
    // At the end see where to put last option
    if(o.name!=""){
        if(!in_task){
            toplevel.data.push_back(o);
        } else {
            tsk.data.push_back(o);
            // Add task itself
            tasks.push_back(tsk);
        }
    }    
}

// without nested tasks
void parse_command_line(int argc, char** argv, Options& toplevel){
    vector<Options> dum;
    parse_command_line(argc,argv,toplevel,"",dum);
}

} // namespece


void Options::debug(){
    cout << "task_name = " << task_name << endl;
    for(auto& o: data){
        o.debug();
    }
}

void Option::debug(){
    cout << name << ": ";
    for(auto& o: data){
        cout << o << " ";
    }
    cout << endl;
}

const Option& Options::operator()(std::string key) const {
    int ind = -1;
    int found = 0;
    for(int i=0;i<data.size();++i){
        if(data[i].name==key){
            ind = i;
            ++found;
        }
    }
    if(found==0) throw Pteros_error("Key '"+key+"' not found!");
    if(found>1) throw Pteros_error("More than one key '"+key+"' found!");
    return data[ind];
}

const Option& Options::operator()(std::string key, std::string default_val) {
    int ind = -1;
    int found = 0;
    for(int i=0;i<data.size();++i){
        if(data[i].name==key){
            ind = i;
            ++found;
        }
    }
    if(found>1) throw Pteros_error("More than one key '"+key+"' found!");
    if(found==0 || data[ind].data.empty()){
        // Default value case
        Option tmp;
        tmp.name = key;        
        // Split default_val by " "
        stringstream ss(default_val);
        string buf;
        while(ss >> buf) tmp.data.push_back(buf);        
        // If tmp.data is still empty here, than default_val is empty string or a space(s)
        // In this case we add it as is
        if(tmp.data.empty()) tmp.data.push_back(default_val);

        // Default value is added to data and returned as usually
        data.push_back(tmp);
        return data.back();
    }
    // Normal case
    return data[ind];
}

bool pteros::Options::has(string key)
{
    int ind = -1;
    int found = 0;
    for(int i=0;i<data.size();++i){
        if(data[i].name==key){
            ind = i;
            ++found;
        }
    }
    if(found>1) throw Pteros_error("More than one key '"+key+"' found!");
    if(found==0)
        return false;
    else
        return true;
}


//----------------------------------------------------------------

int Option::as_int() const {
    if(data.size()!=1) throw Pteros_error("Only one INT value expected for key '"+name+"'!");
    try {
        return boost::lexical_cast<int>(data[0]);
    } catch(boost::bad_lexical_cast){
        throw Pteros_error("Value '"+data[0]+"' is not INT!");
    }
}

float Option::as_float() const {
    if(data.size()!=1) throw Pteros_error("Only one FLOAT value expected for key '"+name+"'!");
    try {
        return boost::lexical_cast<float>(data[0]);
    } catch(boost::bad_lexical_cast){
        throw Pteros_error("Value '"+data[0]+"' is not FLOAT!");
    }
}

bool Option::as_bool() const {
    if(data.size()!=1) throw Pteros_error("Only one BOOL value expected for key '"+name+"'!");
    string s(data[0]);
    boost::algorithm::to_lower(s);
    if(s=="true"){
        return true;
    } else if(s=="false"){
        return false;
    } else {
        throw Pteros_error("Value '"+s+"' is not BOOL!");
    }

}

string Option::as_string() const {        
    if(data.size()!=1) throw Pteros_error("Only one STRING value expected for key '"+name+"'!");
    return data[0];
}

vector<int> Option::as_ints() const {
    if(data.empty()) throw Pteros_error("One or more INT values are expected for key '"+name+"'!");
    vector<int> res;
    for(auto& str: data){
        try {
            res.push_back( boost::lexical_cast<int>(str) );
        } catch(boost::bad_lexical_cast){
            throw Pteros_error("Value '"+str+"' is not INT!");
        }
    }
    return res;
}

vector<float> Option::as_floats() const {
    if(data.empty()) throw Pteros_error("One or more FLOAT values are expected for key '"+name+"'!");
    vector<float> res;
    for(auto& str: data){
        try {
            res.push_back( boost::lexical_cast<float>(str) );
        } catch(boost::bad_lexical_cast){
            throw Pteros_error("Value '"+str+"' is not FLOAT!");
        }
    }
    return res;
}

vector<bool> Option::as_bools() const {
    if(data.empty()) throw Pteros_error("One or more BOOL values are expected for key '"+name+"'!");
    vector<bool> res;
    for(auto s: data){
        boost::algorithm::to_lower(s);
        if(s=="true"){
            res.push_back(true);
        } else if(s=="false"){
            res.push_back(false);
        } else {
            throw Pteros_error("Value '"+s+"' is not BOOL!");
        }
    }
    return res;
}

vector<string> Option::as_strings() const {
    if(data.empty()) throw Pteros_error("One or more STRING values are expected for key '"+name+"'!");
    return data;
}