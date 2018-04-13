/*
 * This file is a part of
 *
 * ============================================
 * ###   Pteros molecular modeling library  ###
 * ============================================
 *
 * (C) 2009-2018, Semen Yesylevskyy
 *
 * All works, which use Pteros, should cite the following papers:
 *  
 *  1.  Semen O. Yesylevskyy, "Pteros 2.0: Evolution of the fast parallel
 *      molecular analysis library for C++ and python",
 *      Journal of Computational Chemistry, 2015, 36(19), 1480–1488.
 *      doi: 10.1002/jcc.23943.
 *
 *  2.  Semen O. Yesylevskyy, "Pteros: Fast and easy to use open-source C++
 *      library for molecular analysis",
 *      Journal of Computational Chemistry, 2012, 33(19), 1632–1636.
 *      doi: 10.1002/jcc.22989.
 *
 * This is free software distributed under Artistic License:
 * http://www.opensource.org/licenses/artistic-license-2.0.php
 *
*/

#include "pteros/python/compiled_plugin.h"
#include <fstream>

using namespace std;
using namespace pteros;

TASK_SERIAL(secondary)
public:    

    string help(){
        return  "Purpose:\n"
                "\tComputes DSSP secondary structure for the system\n"
                "Output:\n"
                "\tFile <label>.dat containing the following columns:\n"
                "\ttime,DSSP\n"
                "\tThere is no space after ','! Spaces are DSSP codes themselves.\n"
                "Options:\n"
                "\tNone";
    }

protected:
    void pre_process() override {
        sel.modify(system, options("sel","protein").as_string());

        jump_remover.add_atoms(sel);


        onlynum = options("onlynum","false").as_bool();

        // Output        
        f.open(options("out",fmt::format("dssp_{}.dat",get_id())).as_string());
        f << "#frame N :DSSP_code_string. NO space after ':'!" << endl;
    }

    void process_frame(const Frame_info &info) override {
        string s = sel.dssp();
        // Count all structured residues
        int N = std::count_if(s.begin(), s.end(), [](char c){return c!='T' && c!='S' && c!=' ';});
        f << info.valid_frame << " " << N;
        if(onlynum){
            f << endl;
        } else {
            f << " :" << s << endl;
        }
    }

    void post_process(const Frame_info &info) override {
        f.close();
    }    

private:
    bool onlynum;
    ofstream f;
    Selection sel;
};

CREATE_COMPILED_PLUGIN(secondary)

