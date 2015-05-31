/*
 * stats.h
 * Author: Aven Bross
 * Date: 5/20/2015
 * 
 * Description:
 * Little stats library to do 1 var stats on input vectors
*/

#ifndef STATS_H
#define STATS_H

#include <vector>
#include <map>
#include <iostream>
#include <cmath>

template<typename T>
void oneVarStats(std::vector<T> records);

#include "stats.cpp"

#endif
