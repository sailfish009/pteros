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


#include "bindings_util.h"
#include "pteros/extras/substructure_search.h"

namespace py = pybind11;
using namespace pteros;
using namespace pybind11::literals;



void make_bindings_substructure_search(py::module& m){
    m.def("find_equivalent_atoms", &find_equivalent_atoms,"sel"_a,"x_memory"_a=1);
    m.def("find_substructures", &find_substructures, "source"_a, "query"_a, "find_all"_a=false);
    m.def("make_equivalent_to_template", &make_equivalent_to_template);
}

