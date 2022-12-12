// **************************************************************************
// File       [ pattern.h ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/09/13 created ]
// **************************************************************************

#ifndef _CORE_PATTERN_H_
#define _CORE_PATTERN_H_

#include <vector>
#include <map>
#include <utility>
#include "circuit.h"
#include "fault.h"

namespace CoreNs
{

	class Pattern;

	typedef std::vector<Pattern *> PatternVec;

	//  Not used now
	// class Vertex;
	// typedef std::vector<Vertex *> VertexVec;
	//  end of compatibility graph

	class Pattern
	{
	public:
		Pattern();
		std::vector<Value> pPI1_;
		std::vector<Value> pPI2_;
		std::vector<Value> pPPI_;
		std::vector<Value> pSI_;
		std::vector<Value> pPO1_;
		std::vector<Value> pPO2_;
		std::vector<Value> pPPO_;
	};

	// This class process the test pattern set
	//
	// it supports :
	//  X-filling
	//  static compression
	class PatternProcessor
	{
	public:
		// Three types of patterns
		// BASIC_SCAN is for single time frame, stuck-at fault
		// LAUNCH_CAPTURE is for 2-pattern test , launch-on-capture
		// LAUNCH_SHIFT is for 2-pattern test, launch-on-shift
		enum Type
		{
			BASIC_SCAN = 0,
			LAUNCH_CAPTURE,
			LAUNCH_SHIFT
		};

		PatternProcessor();
		// ~PatternProcessor();

		//  pattern set attribute
		//  dynamic compression should be moved to ATPG  (E.4)
		//  static compression and x fill should be rewritten as two independent methods (A.1)
		//
		enum State
		{
			OFF = 0,
			ON
		};
		State staticCompression_;	 // Flag; ON = need static compression
		State dynamicCompression_; // Flag; ON = need dynamic compression
		State XFill_;							 //  Flag;  On = need X-Filling

		Type type_;
		int numPI_;
		int numPPI_;
		int numSI_;
		int numPO_;
		PatternVec patternVector_;
		std::vector<int> pPIorder_;
		std::vector<int> pPPIorder_;
		std::vector<int> pPOorder_;

		void init(Circuit *pCircuit);
		// Fixed static compression
		void StaticCompression();
		bool updateTable(std::vector<bool> mergeRecord, std::vector<bool> patternTable);
		// bool updateTable2(std::vector<bool> mergeRecord, std::vector<bool> patternTable); not used, hence removed
	};

	inline Pattern::Pattern()
	{
	}

	// inline Pattern::~Pattern()
	// {
	// 	pPI1_.clear();
	// 	pPI2_.clear();
	// 	pPPI_.clear();
	// 	pSI_.clear();
	// 	pPO1_.clear();
	// 	pPO2_.clear();
	// 	pPPO_.clear();
	// }

	inline PatternProcessor::PatternProcessor()
	{

		staticCompression_ = OFF;	 
		dynamicCompression_ = OFF; // dynamic compression should be moved to ATPG
		XFill_ = OFF;							

		type_ = BASIC_SCAN;
		numPI_ = 0;
		numPPI_ = 0;
		numSI_ = 0;
		numPO_ = 0;
	}

	// inline PatternProcessor::~PatternProcessor()
	// {
	// 	patternVector_.clear();
	// 	pPIorder_.clear();
	// 	pPPIorder_.clear();
	// 	pPOorder_.clear();
	// }

	inline void PatternProcessor::init(Circuit *pCircuit)
	{
		pPIorder_.clear();
		pPPIorder_.clear();
		pPOorder_.clear();

		numPI_ = pCircuit->npi_;
		numPO_ = pCircuit->npo_;
		numPPI_ = pCircuit->nppi_;

		pPIorder_.resize(numPI_);
		for (int i = 0; i < numPI_; ++i)
		{
			pPIorder_[i] = i;
		}

		pPOorder_.resize(numPO_);
		for (int i = 0; i < numPO_; ++i)
		{
			pPOorder_[i] = pCircuit->ngate_ - pCircuit->npo_ - pCircuit->nppi_ + i;
		}

		pPPIorder_.resize(numPPI_);
		for (int i = 0; i < numPPI_; ++i)
		{
			pPPIorder_[i] = pCircuit->npi_ + i;
		}
	}
	// **************************************************************************
	// Function   [ PatternProcessor::StaticCompression ]
	// Commentor  [ HKY CYW ]
	// Synopsis   [ usage: do static compression
	// 		We first compare each pair of the patterns and check whether they are compatible.
	// 		If so, we will merge these patterns bit by bit
	// 		Note that
	// 		Value L = 0
	// 		Value H = 1
	// 		Value X = 3
	// 		And the rule of compression
	// 		(X,L) -> L
	// 		(X,H) -> H
	//		So when we merge two patterns, always let the bit to be the smaller one.
	//		That is, always don't use X as a result if one bit is L or H.
	//              in:    Pattern list
	//              out:   void
	//            ]
	// Date       [ HKY Ver. 1.0 started 2014/09/01 ]
	// **************************************************************************
	inline void PatternProcessor::StaticCompression()
	{
		int size = (int)patternVector_.size();
		std::vector<bool> mergeRecord(size);
		for (int i = 0; i < size; ++i)
		{
			mergeRecord[i] = false;
		}
		// bool *patternTable = new bool[size * size]();
		std::vector<bool> patternTable(size * size);
		for (int i = 0; i < size; ++i)
		{
			patternTable[i * size + i] = true;
		}
		// for each pair of patterns, build pattern table
		for (int i = 0; i < size - 1; ++i)
		{
			if (mergeRecord[i] == true)
				continue; // If the pattern has been merged before, no need to merge again
			for (int j = i + 1; j < size; ++j)
			{
				if (mergeRecord[j] == true)
					continue;

				bool compatible = true; // Can be merged into one
				for (int k = 0; k < numPI_; ++k)
				{ // If any bit of the patterns has different values(one is high and one is low), the patterns are not compatible
					if (((patternVector_[i]->pPI1_[k] == L) && (patternVector_[j]->pPI1_[k] == H)) || ((patternVector_[i]->pPI1_[k] == H) && (patternVector_[j]->pPI1_[k] == L)))
					{
						compatible = false;
						break;
					}
				}
				if (!(patternVector_[i]->pPI2_.empty()) && (compatible == true))
				{ // If the pattern has second primary input, we have to check it too
					for (int k = 0; k < numPI_; ++k)
					{
						if (((patternVector_[i]->pPI2_[k] == L) && (patternVector_[j]->pPI2_[k] == H)) || ((patternVector_[i]->pPI2_[k] == H) && (patternVector_[j]->pPI2_[k] == L)))
						{
							compatible = false;
							break;
						}
					}
				}
				if (!(patternVector_[i]->pPPI_.empty()) && (compatible == true))
				{ // Check ppi
					for (int k = 0; k < numPPI_; ++k)
					{
						if (((patternVector_[i]->pPPI_[k] == L) && (patternVector_[j]->pPPI_[k] == H)) || ((patternVector_[i]->pPPI_[k] == H) && (patternVector_[j]->pPPI_[k] == L)))
						{
							compatible = false;
							break;
						}
					}
				}
				// if ((patternVector_[i]->pSI_ != NULL) && (compatible == true))
				if (!(patternVector_[i]->pSI_.empty()) && (compatible == true))
				{ // Check si
					for (int k = 0; k < numSI_; ++k)
					{
						if (((patternVector_[i]->pSI_[k] == L) && (patternVector_[j]->pSI_[k] == H)) || ((patternVector_[i]->pSI_[k] == H) && (patternVector_[j]->pSI_[k] == L)))
						{
							compatible = false;
							break;
						}
					}
				}
				// the patterns are compatible
				if (compatible == true)
				{
					patternTable[i * size + j] = true;
					patternTable[j * size + i] = true;
				}
				else
				{
					patternTable[i * size + j] = false;
					patternTable[j * size + i] = false;
				}
			}
		}
		// std::cout << "Finish build table" << std::endl;

		// show pattern table
		/*
		for (int i=0; i<size; ++i){
				for (int j=0; j<size; ++j){
						std::cout<<patternTable[i*size+j]<<" ";
				}
				std::cout<<std::endl;
		}
		*/
		// for each pair of patterns, try to merge them
		updateTable(mergeRecord, patternTable);
		// replace Pattern
		PatternVec compPattern;
		for (int i = 0; i < (int)patternVector_.size(); ++i)
		{
			if (mergeRecord[i] == false)
			{
				compPattern.push_back(patternVector_[i]);
			}
		}
		patternVector_ = compPattern;
	}

	inline bool PatternProcessor::updateTable(std::vector<bool> mergeRecord, std::vector<bool> patternTable)
	{
		int size = (int)patternVector_.size();
		typedef std::pair<int, int> Pattern_pair;
		typedef std::map<Pattern_pair, int> Pattern_table;
		Pattern_table patternCandidate;
		// find pattern pair candidate
		for (int i = 0; i < size - 1; ++i)
		{
			for (int j = i + 1; j < size; ++j)
			{
				if (patternTable[i * size + j])
				{
					Pattern_pair temp_pair(i, j);
					patternCandidate[temp_pair] = 0;
				}
			}
		}
		// update similarity pattern pair candidate
		// std::cout<<"FIRST similarity pattern pair candidate"<<std::endl;
		for (Pattern_table::iterator it = patternCandidate.begin(); it != patternCandidate.end(); ++it)
		{
			int i = it->first.first;
			int j = it->first.second;
			int similararityCount = 0;
			for (int k = 0; k < size; ++k)
			{
				// if(mergeRecord[k]){continue;}
				if (patternTable[i * size + k] && patternTable[i * size + k] == patternTable[j * size + k])
				{
					++similararityCount;
				}
			}
			it->second = similararityCount;
		}

		while (!patternCandidate.empty())
		{

			std::vector<bool> updateSimilarityPattern(size);
			for (int i = 0; i < size; ++i)
			{
				updateSimilarityPattern[i] = false;
			}
			int similarityPattern_first = 0;
			int similarityPattern_second = 0;
			int maxSimilarity = 0;
			for (Pattern_table::iterator it = patternCandidate.begin(); it != patternCandidate.end(); ++it)
			{
				if (it->second > maxSimilarity)
				{
					maxSimilarity = it->second;
					similarityPattern_first = it->first.first;
					similarityPattern_second = it->first.second;
				}
			}
			if (maxSimilarity == 0)
			{
				return false;
			}
			// std::cout<<"-----MERGE PAIR "<<similarityPattern_first<<" "<<similarityPattern_second<<std::endl;
			// del_first_pattern
			std::vector<int> del_first_pattern;
			if (maxSimilarity != 0)
			{
				int i = similarityPattern_first;
				int j = similarityPattern_second;
				for (int k = 0; k < size; ++k)
				{
					if (mergeRecord[k])
					{
						continue;
					}
					if (patternTable[i * size + k] == 1 && patternTable[j * size + k] == 0)
					{
						del_first_pattern.push_back(k);
					}
					updateSimilarityPattern[k] = patternTable[i * size + k] || patternTable[j * size + k];
					patternTable[i * size + k] = patternTable[i * size + k] && patternTable[j * size + k];
					patternTable[k * size + i] = patternTable[i * size + k];
				}
			}
			int del_first_pattern_size = (int)del_first_pattern.size();
			for (int i = 0; i < del_first_pattern_size; ++i)
			{
				if (del_first_pattern[i] < similarityPattern_first)
				{
					Pattern_pair temp_pair(del_first_pattern[i], similarityPattern_first);
					patternCandidate.erase(temp_pair);
				}
				else
				{
					Pattern_pair temp_pair(similarityPattern_first, del_first_pattern[i]);
					patternCandidate.erase(temp_pair);
				}
			}
			std::vector<int> del_second_pattern;
			for (int i = 0; i < size; ++i)
			{
				if (mergeRecord[i])
				{
					continue;
				}
				if (similarityPattern_second == i)
				{
					continue;
				}
				if (patternTable[similarityPattern_second * size + i])
				{
					del_second_pattern.push_back(i);
				}
			}
			int del_second_pattern_size = (int)del_second_pattern.size();
			for (int i = 0; i < del_second_pattern_size; ++i)
			{
				// std::cout<<del_second_pattern[i]<<"-"<<similarityPattern_second<<std::endl;
				if (del_second_pattern[i] < similarityPattern_second)
				{
					Pattern_pair temp_pair(del_second_pattern[i], similarityPattern_second);
					patternCandidate.erase(temp_pair);
				}
				else
				{
					Pattern_pair temp_pair(similarityPattern_second, del_second_pattern[i]);
					patternCandidate.erase(temp_pair);
				}
			}
			// merge pattern
			if (maxSimilarity != 0)
			{
				int i = similarityPattern_first;
				int j = similarityPattern_second;
				for (int k = 0; k < numPI_; ++k)
				{
					patternVector_[i]->pPI1_[k] = (patternVector_[i]->pPI1_[k] < patternVector_[j]->pPI1_[k]) ? patternVector_[i]->pPI1_[k] : patternVector_[j]->pPI1_[k];
				}
				if (!patternVector_[i]->pPI2_.empty())
				{
					for (int k = 0; k < numPI_; ++k)
					{
						patternVector_[i]->pPI2_[k] = (patternVector_[i]->pPI2_[k] < patternVector_[j]->pPI2_[k]) ? patternVector_[i]->pPI2_[k] : patternVector_[j]->pPI2_[k];
					}
				}
				if (!patternVector_[i]->pPPI_.empty())
				{
					for (int k = 0; k < numPPI_; ++k)
					{
						patternVector_[i]->pPPI_[k] = (patternVector_[i]->pPPI_[k] < patternVector_[j]->pPPI_[k]) ? patternVector_[i]->pPPI_[k] : patternVector_[j]->pPPI_[k];
					}
				}
				if (!patternVector_[i]->pSI_.empty())
				{
					for (int k = 0; k < numSI_; ++k)
					{
						patternVector_[i]->pSI_[k] = (patternVector_[i]->pSI_[k] < patternVector_[j]->pSI_[k]) ? patternVector_[i]->pSI_[k] : patternVector_[j]->pSI_[k];
					}
				}

				for (int k = 0; k < numPO_; ++k)
				{
					patternVector_[i]->pPO1_[k] = (patternVector_[i]->pPO1_[k] < patternVector_[j]->pPO1_[k]) ? patternVector_[i]->pPO1_[k] : patternVector_[j]->pPO1_[k];
				}
				if (!patternVector_[i]->pPO2_.empty())
				{
					for (int k = 0; k < numPO_; ++k)
					{
						patternVector_[i]->pPO2_[k] = (patternVector_[i]->pPO2_[k] < patternVector_[j]->pPO2_[k]) ? patternVector_[i]->pPO2_[k] : patternVector_[j]->pPO2_[k];
					}
				}
				if (!patternVector_[i]->pPPO_.empty())
				{
					for (int k = 0; k < numPPI_; ++k)
					{
						patternVector_[i]->pPPO_[k] = (patternVector_[i]->pPPO_[k] < patternVector_[j]->pPPO_[k]) ? patternVector_[i]->pPPO_[k] : patternVector_[j]->pPPO_[k];
					}
				}
				mergeRecord[j] = true;
			}
			// update similarity pattern pair candidate
			// std::cout<<"--AFTER-------------------"<<std::endl;
			/*
			for(Pattern_table::iterator it = patternCandidate.begin(); it !=patternCandidate.end(); ++it){
					int i = it->first.first;
					int j = it->first.second;
					int similararityCount = 0;
					for(int k = 0; k<size;++k){
							if(mergeRecord[k]){continue;}
							if(patternTable[i*size+k] && patternTable[i*size+k]==patternTable[j*size+k]){++similararityCount;}
					}
					it->second = similararityCount;
					//std::cout<<"PAT "<<i<<" "<<j<<" "<<it->second<<std::endl;
			}
			*/
			// update similarity pattern pair candidate
			for (int i = 0; i < size; ++i)
			{
				if (mergeRecord[i])
				{
					continue;
				}
				if (updateSimilarityPattern[i])
				{
					for (int j = 0; j < size; ++j)
					{
						if (i == j)
						{
							continue;
						}
						if (mergeRecord[j])
						{
							continue;
						}
						if (patternTable[i * size + j])
						{
							Pattern_pair temp_pair;
							if (i < j)
							{
								temp_pair.first = i;
								temp_pair.second = j;
							}
							else
							{
								temp_pair.first = j;
								temp_pair.second = i;
							}
							Pattern_table::iterator it = patternCandidate.find(temp_pair);
							if (it != patternCandidate.end())
							{
								int similararityCount = 0;
								for (int b = 0; b < size; ++b)
								{
									if (mergeRecord[b])
									{
										continue;
									}
									if (patternTable[i * size + b] && patternTable[i * size + b] == patternTable[j * size + b])
									{
										++similararityCount;
									}
								}
								it->second = similararityCount;
							}
						}
					}
				}
			}
		}

		return true;
	}
	// This is for static compression using compatibility graph
	//
	// new static compression, Not In Used

	// inline bool PatternProcessor::updateTable2(std::vector<bool> mergeRecord, std::vector<bool> patternTable)
	// {
	// 	int size = (int)patternVector_.size();
	// 	typedef std::pair<int, int> Pattern_pair;
	// 	typedef std::map<Pattern_pair, int> Pattern_table;
	// 	Pattern_table patternCandidate;
	// 	std::map<int, int> Node_size;

	// 	// find pattern pair candidate
	// 	for (int i = 0; i < size - 1; ++i)
	// 	{
	// 		for (int j = i + 1; j < size; ++j)
	// 		{
	// 			if (patternTable[i * size + j])
	// 			{
	// 				Pattern_pair temp_pair(i, j);
	// 				patternCandidate[temp_pair] = 0;
	// 			}
	// 		}
	// 		Node_size[i] = 1;
	// 	}
	// 	// update similarity pattern pair candidate
	// 	// std::cout<<"FIRST similarity pattern pair candidate"<<std::endl;
	// 	for (Pattern_table::iterator it = patternCandidate.begin(); it != patternCandidate.end(); ++it)
	// 	{
	// 		int i = it->first.first;
	// 		int j = it->first.second;
	// 		int similararityCount = 0;
	// 		for (int k = 0; k < size; ++k)
	// 		{
	// 			// if(mergeRecord[k]){continue;}
	// 			if (patternTable[i * size + k] && patternTable[i * size + k] == patternTable[j * size + k])
	// 			{
	// 				++similararityCount;
	// 			}
	// 		}
	// 		it->second = similararityCount;
	// 		// std::cout<<"PAT "<<i<<" "<<j<<" "<<it->second<<std::endl;
	// 	}

	// 	while (!patternCandidate.empty())
	// 	{

	// 		// bool updateSimilarityPattern[size];
	// 		std::vector<bool> updateSimilarityPattern(size);
	// 		for (int i = 0; i < size; ++i)
	// 		{
	// 			updateSimilarityPattern[i] = false;
	// 		}
	// 		int similarityPattern_first = 0;
	// 		int similarityPattern_second = 0;
	// 		int maxSimilarity = 0;
	// 		// martan
	// 		int max_pair_size = 0;
	// 		// std::cout<<"--------------begin----------------"<<std::endl;//

	// 		for (Pattern_table::iterator it = patternCandidate.begin(); it != patternCandidate.end(); ++it)
	// 		{

	// 			// std::cout<<"similarity:"<<it->second<<" pair size:"<<(Node_size[it->first.first]+Node_size[it->first.second])<<std::endl;//

	// 			if (it->second > maxSimilarity)
	// 			{
	// 				max_pair_size = 0;
	// 			}

	// 			if (it->second >= maxSimilarity)
	// 			{
	// 				maxSimilarity = it->second;
	// 				// similarityPattern_first=it->first.first;
	// 				// similarityPattern_second=it->first.second;

	// 				if ((Node_size[it->first.first] + Node_size[it->first.second]) > max_pair_size)
	// 				{
	// 					max_pair_size = Node_size[it->first.first] + Node_size[it->first.second];

	// 					similarityPattern_first = it->first.first;
	// 					similarityPattern_second = it->first.second;

	// 					// std::cout<<"-->update"<<std::endl;//
	// 					// std::cout<<"similarity:"<<it->second<<" pair size:"<<(Node_size[it->first.first]+Node_size[it->first.second])<<std::endl;//
	// 					// std::cout<<"-->update"<<std::endl;//
	// 				}
	// 			}
	// 		}
	// 		// std::cout<<" pair size:"<<(Node_size[similarityPattern_first]+Node_size[similarityPattern_second])<<std::endl;//
	// 		// std::cout<<"--------------end----------------"<<std::endl;//

	// 		if (maxSimilarity == 0)
	// 		{
	// 			return false;
	// 		}
	// 		// std::cout<<"-----MERGE PAIR "<<similarityPattern_first<<" "<<similarityPattern_second<<std::endl;
	// 		// del_first_pattern
	// 		std::vector<int> del_first_pattern;
	// 		if (maxSimilarity != 0)
	// 		{
	// 			int i = similarityPattern_first;
	// 			int j = similarityPattern_second;
	// 			for (int k = 0; k < size; ++k)
	// 			{
	// 				if (mergeRecord[k])
	// 				{
	// 					continue;
	// 				}
	// 				if (patternTable[i * size + k] == 1 && patternTable[j * size + k] == 0)
	// 				{
	// 					// std::cout<<"FUCK "<<i<<" "<<k<<std::endl;
	// 					del_first_pattern.push_back(k);
	// 				}
	// 				updateSimilarityPattern[k] = patternTable[i * size + k] || patternTable[j * size + k];
	// 				patternTable[i * size + k] = patternTable[i * size + k] && patternTable[j * size + k];
	// 				patternTable[k * size + i] = patternTable[i * size + k];
	// 			}
	// 		}
	// 		int del_first_pattern_size = (int)del_first_pattern.size();
	// 		for (int i = 0; i < del_first_pattern_size; ++i)
	// 		{
	// 			// std::cout<<del_first_pattern[i]<<"-"<<similarityPattern_first<<std::endl;
	// 			if (del_first_pattern[i] < similarityPattern_first)
	// 			{
	// 				Pattern_pair temp_pair(del_first_pattern[i], similarityPattern_first);
	// 				patternCandidate.erase(temp_pair);
	// 			}
	// 			else
	// 			{
	// 				Pattern_pair temp_pair(similarityPattern_first, del_first_pattern[i]);
	// 				patternCandidate.erase(temp_pair);
	// 			}
	// 		}
	// 		std::vector<int> del_second_pattern;
	// 		for (int i = 0; i < size; ++i)
	// 		{
	// 			if (mergeRecord[i])
	// 			{
	// 				continue;
	// 			}
	// 			if (similarityPattern_second == i)
	// 			{
	// 				continue;
	// 			}
	// 			if (patternTable[similarityPattern_second * size + i])
	// 			{
	// 				del_second_pattern.push_back(i);
	// 			}
	// 		}
	// 		int del_second_pattern_size = (int)del_second_pattern.size();
	// 		for (int i = 0; i < del_second_pattern_size; ++i)
	// 		{
	// 			// std::cout<<del_second_pattern[i]<<"-"<<similarityPattern_second<<std::endl;
	// 			if (del_second_pattern[i] < similarityPattern_second)
	// 			{
	// 				Pattern_pair temp_pair(del_second_pattern[i], similarityPattern_second);
	// 				patternCandidate.erase(temp_pair);
	// 			}
	// 			else
	// 			{
	// 				Pattern_pair temp_pair(similarityPattern_second, del_second_pattern[i]);
	// 				patternCandidate.erase(temp_pair);
	// 			}
	// 		}
	// 		// merge pattern
	// 		if (maxSimilarity != 0)
	// 		{
	// 			// std::cout<<"MERGE???"<<std::endl;
	// 			int i = similarityPattern_first;
	// 			int j = similarityPattern_second;
	// 			for (int k = 0; k < numPI_; ++k)
	// 			{
	// 				patternVector_[i]->pPI1_[k] = (patternVector_[i]->pPI1_[k] < patternVector_[j]->pPI1_[k]) ? patternVector_[i]->pPI1_[k] : patternVector_[j]->pPI1_[k];
	// 			}
	// 			if (patternVector_[i]->pPI2_ != NULL)
	// 			{
	// 				for (int k = 0; k < numPI_; ++k)
	// 				{
	// 					patternVector_[i]->pPI2_[k] = (patternVector_[i]->pPI2_[k] < patternVector_[j]->pPI2_[k]) ? patternVector_[i]->pPI2_[k] : patternVector_[j]->pPI2_[k];
	// 				}
	// 			}
	// 			if (patternVector_[i]->pPPI_ != NULL)
	// 			{
	// 				for (int k = 0; k < numPPI_; ++k)
	// 				{
	// 					patternVector_[i]->pPPI_[k] = (patternVector_[i]->pPPI_[k] < patternVector_[j]->pPPI_[k]) ? patternVector_[i]->pPPI_[k] : patternVector_[j]->pPPI_[k];
	// 				}
	// 			}
	// 			if (patternVector_[i]->pSI_ != NULL)
	// 			{
	// 				for (int k = 0; k < numSI_; ++k)
	// 				{
	// 					patternVector_[i]->pSI_[k] = (patternVector_[i]->pSI_[k] < patternVector_[j]->pSI_[k]) ? patternVector_[i]->pSI_[k] : patternVector_[j]->pSI_[k];
	// 				}
	// 			}

	// 			for (int k = 0; k < numPO_; ++k)
	// 			{
	// 				patternVector_[i]->pPO1_[k] = (patternVector_[i]->pPO1_[k] < patternVector_[j]->pPO1_[k]) ? patternVector_[i]->pPO1_[k] : patternVector_[j]->pPO1_[k];
	// 			}
	// 			if (patternVector_[i]->pPO2_ != NULL)
	// 			{
	// 				for (int k = 0; k < numPO_; ++k)
	// 				{
	// 					patternVector_[i]->pPO2_[k] = (patternVector_[i]->pPO2_[k] < patternVector_[j]->pPO2_[k]) ? patternVector_[i]->pPO2_[k] : patternVector_[j]->pPO2_[k];
	// 				}
	// 			}
	// 			if (patternVector_[i]->pPPO_ != NULL)
	// 			{
	// 				for (int k = 0; k < numPPI_; ++k)
	// 				{
	// 					patternVector_[i]->pPPO_[k] = (patternVector_[i]->pPPO_[k] < patternVector_[j]->pPPO_[k]) ? patternVector_[i]->pPPO_[k] : patternVector_[j]->pPPO_[k];
	// 				}
	// 			}
	// 			mergeRecord[j] = true;

	// 			Node_size[i] += 1;
	// 			Node_size[j] = 0;
	// 		}
	// 		// update similarity pattern pair candidate
	// 		// std::cout<<"--AFTER-------------------"<<std::endl;
	// 		/*
	// 		for(Pattern_table::iterator it = patternCandidate.begin(); it !=patternCandidate.end(); ++it){
	// 				int i = it->first.first;
	// 				int j = it->first.second;
	// 				int similararityCount = 0;
	// 				for(int k = 0; k<size;++k){
	// 						if(mergeRecord[k]){continue;}
	// 						if(patternTable[i*size+k] && patternTable[i*size+k]==patternTable[j*size+k]){++similararityCount;}
	// 				}
	// 				it->second = similararityCount;
	// 				//std::cout<<"PAT "<<i<<" "<<j<<" "<<it->second<<std::endl;
	// 		}
	// 		*/
	// 		// update similarity pattern pair candidate
	// 		for (int i = 0; i < size; ++i)
	// 		{
	// 			if (mergeRecord[i])
	// 			{
	// 				continue;
	// 			}
	// 			if (updateSimilarityPattern[i])
	// 			{
	// 				for (int j = 0; j < size; ++j)
	// 				{
	// 					if (i == j)
	// 					{
	// 						continue;
	// 					}
	// 					if (mergeRecord[j])
	// 					{
	// 						continue;
	// 					}
	// 					if (patternTable[i * size + j])
	// 					{
	// 						Pattern_pair temp_pair;
	// 						if (i < j)
	// 						{
	// 							temp_pair.first = i;
	// 							temp_pair.second = j;
	// 						}
	// 						else
	// 						{
	// 							temp_pair.first = j;
	// 							temp_pair.second = i;
	// 						}
	// 						Pattern_table::iterator it = patternCandidate.find(temp_pair);
	// 						if (it != patternCandidate.end())
	// 						{
	// 							int similararityCount = 0;
	// 							for (int b = 0; b < size; ++b)
	// 							{
	// 								if (mergeRecord[b])
	// 								{
	// 									continue;
	// 								}
	// 								if (patternTable[i * size + b] && patternTable[i * size + b] == patternTable[j * size + b])
	// 								{
	// 									++similararityCount;
	// 								}
	// 							}
	// 							it->second = similararityCount;
	// 						}
	// 					}
	// 				}
	// 			}
	// 		}
	// 		// std::cout<<"PCSIZE "<<patternCandidate.size()<<std::endl;
	// 	}

	// 	return true;
	// }

	//

	// 	class Vertex
	// 	{
	// 	public:
	// 		Vertex();
	// 		Vertex(std::vector<Value> data, Fault *fault);
	// 		~Vertex();
	// 		std::vector<Value> data_;
	// 		FaultList fault_;
	// 	};

	// 	inline Vertex::Vertex()
	// 	{
	// 		data_ = NULL;
	// 	}

	// 	inline Vertex::Vertex(std::vector<Value> data, Fault *fault)
	// 	{
	// 		data_ = data;
	// 		fault_.push_back(fault);
	// 	}

	// 	inline Vertex::~Vertex()
	// 	{
	// 		delete[] data_;
	// 		fault_.clear();
	// 	}
	// 	// end of Vertex

};

#endif
