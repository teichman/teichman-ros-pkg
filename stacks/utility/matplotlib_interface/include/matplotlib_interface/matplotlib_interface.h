#ifndef MATPLOTLIB_INTERFACE_H
#define MATPLOTLIB_INTERFACE_H

#include <Python.h>
#include <Eigen/Eigen>
#include <sstream>
#include <cstdarg>
#include <iostream>
#include <vector>
#include <boost/algorithm/string.hpp>

//! Export data to python with the same name as in the c++ program.
//! Works with anything you can call mpliToPython on.
#define mpliExport(name) PyRun_SimpleString((std::string(#name) + std::string(" = ") + mpliToPython(name)).c_str())

//! Export data to python with a specified name.
//! Works with anything you can call mpliToPython on.
template<typename T>
void mpliNamedExport(const std::string& name, const T& data)
{
  PyRun_SimpleString((name + std::string(" = ") + mpliToPython(data)).c_str());
}

//! Call before running anything mpli-related.  Can only be called once because of a numpy bug.
void mpliBegin();
//! Call after done with everything mpli-related to clean up python.
void mpliEnd();
//! Executes the contents of a python file.
void mpliExecuteFile(const std::string& filename);
//! Executes str as a python command.
void mpli(const std::string& str);

std::string mpliToPython(const std::string& str); 
std::string mpliToPython(int val);
std::string mpliToPython(size_t val);
std::string mpliToPython(double val);
std::string mpliToPython(const std::vector<double>& vec);
std::string mpliToPython(const Eigen::VectorXd& vec);
std::string mpliToPython(const Eigen::MatrixXd& mat);
std::string mpliToPython(const Eigen::ArrayXXd& arr);
std::string mpliToPython(const Eigen::ArrayXd& arr);

void mpliPrintSize();

#endif // MATPLOTLIB_INTERFACE_H
