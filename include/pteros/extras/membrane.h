/*
 *
 *                This source code is part of
 *                    ******************
 *                    ***   Pteros   ***
 *                    ******************
 *                 molecular modeling library
 *
 * Copyright (c) 2009-2017, Semen Yesylevskyy
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

#pragma once
#include "pteros/core/selection.h"
#include "pteros/core/logging.h"
#include <Eigen/Core>

namespace pteros {


struct Lipid_descr {
    std::string name;
    std::string whole_sel_str;
    std::string head_sel_str;
    std::string tail_sel_str;
    std::string mid_sel_str;
};

class Lipid {
    friend class Membrane;
public:
    Lipid(const Selection& sel, const Lipid_descr& descr);   

    Eigen::Vector3f get_mid_xyz() const {return mid_sel.XYZ(0);}
    Eigen::Vector3f get_head_xyz() const {return head_sel.XYZ(0);}
    Eigen::Vector3f get_tail_xyz() const {return tail_sel.XYZ(0);}

    Selection whole_sel;
    Selection head_sel;
    Selection tail_sel;
    Selection mid_sel;

    std::string name;
    Eigen::Vector3f normal;
    Eigen::Vector3f smoothed_mid_xyz;
    float tilt;
    float splay;
    float quad_fit_rms;
    float area;
    int leaflet;
    float gaussian_curvature;
    float mean_curvature;
    int coord_number;

private:    
    // Set current COM coordinates of seletions to their first atoms used as markers
    void set_markers();
    // Restore atomic coords of markers
    void unset_markers();

    Eigen::Vector3f saved_head0, saved_tail0, saved_mid0;

    Selection local_sel;
};


struct Splay_pair {
    int lip1;
    int lip2;
    float splay;
};

class Membrane {
public:
    Membrane(System *sys, const std::vector<Lipid_descr>& species);
    void compute_properties(float d,
                            Vector3f_const_ref external_normal = Eigen::Vector3f::Zero());
    void write_vmd_arrows(const std::string& fname);
    void write_smoothed(const std::string &fname);

    const Lipid& get_lipid(int i){ return lipids[i]; }

    std::vector<Lipid> lipids;

    std::vector<Splay_pair> splay;

    std::vector<std::vector<int>> neighbors;

    std::vector<std::vector<int>> leaflets;

private:
    System* system;
    std::vector<Lipid_descr> lipid_species;


    std::vector<Selection> leaflets_sel;

    std::shared_ptr<spdlog::logger> log;
    std::unordered_map<int,int> index_map;

    // Dynamic properties
    std::vector<Eigen::Vector2i> neighbor_pairs;
};

}