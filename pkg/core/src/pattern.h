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

	//  this is for static compression using compatibility graph
	class Vertex;
	typedef std::vector<Vertex *> VertexVec;
	//  end of compatibility graph

	class Pattern
	{
	public:
		Pattern();
		~Pattern();
		Value *pi1_;
		Value *pi2_;
		Value *ppi_;
		Value *si_;
		Value *po1_;
		Value *po2_;
		Value *ppo_;
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
		~PatternProcessor();

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
		int npi_;
		int nppi_;
		int nsi_;
		int npo_;
		PatternVec pats_;
		int *piOrder_;
		int *ppiOrder_;
		int *poOrder_;

		void init( Circuit *cir );
		// Fixed static compression
		void StaticCompression();
		bool updatetable( bool *mergeRecord, bool *patternTable );
		bool updatetable2( bool *mergeRecord, bool *patternTable );
	};

	inline Pattern::Pattern()
	{
		pi1_ = NULL;
		pi2_ = NULL;
		ppi_ = NULL;
		si_ = NULL;
		po1_ = NULL;
		po2_ = NULL;
		ppo_ = NULL;
	}

	inline Pattern::~Pattern()
	{
		delete[] pi1_;
		delete[] pi2_;
		delete[] ppi_;
		delete[] si_;
		delete[] po1_;
		delete[] po2_;
		delete[] ppo_;
	}

	inline PatternProcessor::PatternProcessor()
	{

		staticCompression_ = OFF;	 //
		dynamicCompression_ = OFF; // dynamic compression should be moved to ATPG
		XFill_ = OFF;							 //

		type_ = BASIC_SCAN;
		npi_ = 0;
		nppi_ = 0;
		nsi_ = 0;
		npo_ = 0;
		piOrder_ = NULL;
		ppiOrder_ = NULL;
		poOrder_ = NULL;
	}

	inline PatternProcessor::~PatternProcessor()
	{
		if ( pats_.size() > 0 )
			delete[] pats_[ 0 ];
		pats_.clear();
		delete[] piOrder_;
		delete[] ppiOrder_;
		delete[] poOrder_;
	}

	inline void PatternProcessor::init( Circuit *cir )
	{
		delete[] piOrder_;
		delete[] ppiOrder_;
		delete[] poOrder_;
		piOrder_ = NULL;
		ppiOrder_ = NULL;
		poOrder_ = NULL;

		npi_ = cir->npi_;
		npo_ = cir->npo_;
		nppi_ = cir->nppi_;

		piOrder_ = new int[ npi_ ];
		for ( int i = 0; i < npi_; ++i )
			piOrder_[ i ] = i;

		poOrder_ = new int[ npo_ ];
		for ( int i = 0; i < npo_; ++i )
			poOrder_[ i ] = cir->ngate_ - cir->npo_ - cir->nppi_ + i;

		ppiOrder_ = new int[ nppi_ ];
		for ( int i = 0; i < nppi_; ++i )
			ppiOrder_[ i ] = cir->npi_ + i;
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
		int size = (int)pats_.size();
		bool mergeRecord[ size ];
		for ( int i = 0; i < size; ++i )
		{
			mergeRecord[ i ] = false;
		}
		bool *patternTable = new bool[ size * size ]();
		for ( int i = 0; i < size; ++i )
		{
			patternTable[ i * size + i ] = true;
		}
		// for each pair of patterns, build pattern table
		std::cout << "Build table" << std::endl;
		for ( int i = 0; i < size - 1; ++i )
		{
			if ( mergeRecord[ i ] == true )
				continue; // If the pattern has been merged before, no need to merge again
			for ( int j = i + 1; j < size; ++j )
			{
				if ( mergeRecord[ j ] == true )
					continue;

				bool compatible = true;
				for ( int k = 0; k < npi_; ++k )
				{ // If any bit of the patterns has different values(one is high and one is low), the patterns are not compatible
					if ( ( ( pats_[ i ]->pi1_[ k ] == L ) && ( pats_[ j ]->pi1_[ k ] == H ) ) || ( ( pats_[ i ]->pi1_[ k ] == H ) && ( pats_[ j ]->pi1_[ k ] == L ) ) )
					{
						compatible = false;
						break;
					}
				}
				if ( ( pats_[ i ]->pi2_ != NULL ) && ( compatible == true ) )
				{ // If the pattern has second primary input, we have to check it too
					for ( int k = 0; k < npi_; ++k )
					{
						if ( ( ( pats_[ i ]->pi2_[ k ] == L ) && ( pats_[ j ]->pi2_[ k ] == H ) ) || ( ( pats_[ i ]->pi2_[ k ] == H ) && ( pats_[ j ]->pi2_[ k ] == L ) ) )
						{
							compatible = false;
							break;
						}
					}
				}
				if ( ( pats_[ i ]->ppi_ != NULL ) && ( compatible == true ) )
				{ // Check ppi
					for ( int k = 0; k < nppi_; ++k )
					{
						if ( ( ( pats_[ i ]->ppi_[ k ] == L ) && ( pats_[ j ]->ppi_[ k ] == H ) ) || ( ( pats_[ i ]->ppi_[ k ] == H ) && ( pats_[ j ]->ppi_[ k ] == L ) ) )
						{
							compatible = false;
							break;
						}
					}
				}
				if ( ( pats_[ i ]->si_ != NULL ) && ( compatible == true ) )
				{ // Check si
					for ( int k = 0; k < nsi_; ++k )
					{
						if ( ( ( pats_[ i ]->si_[ k ] == L ) && ( pats_[ j ]->si_[ k ] == H ) ) || ( ( pats_[ i ]->si_[ k ] == H ) && ( pats_[ j ]->si_[ k ] == L ) ) )
						{
							compatible = false;
							break;
						}
					}
				}
				// the patterns are compatible
				if ( compatible == true )
				{
					patternTable[ i * size + j ] = true;
					patternTable[ j * size + i ] = true;
				}
				else
				{
					patternTable[ i * size + j ] = false;
					patternTable[ j * size + i ] = false;
				}
			}
		}
		std::cout << "Finish build table" << std::endl;

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
		updatetable( mergeRecord, patternTable );

		delete[] patternTable;
		// replace Pattern
		PatternVec compPattern;
		for ( int i = 0; i < (int)pats_.size(); ++i )
		{
			if ( mergeRecord[ i ] == false )
			{
				compPattern.push_back( pats_[ i ] );
			}
		}
		pats_ = compPattern;
	}

	inline bool PatternProcessor::updatetable( bool *mergeRecord, bool *patternTable )
	{
		int size = (int)pats_.size();
		typedef std::pair<int, int> Pattern_pair;
		typedef std::map<Pattern_pair, int> Pattern_table;
		Pattern_table pattern_candidate;
		// find pattern pair candidate
		for ( int i = 0; i < size - 1; ++i )
		{
			for ( int j = i + 1; j < size; ++j )
			{
				if ( patternTable[ i * size + j ] )
				{
					Pattern_pair temp_pair( i, j );
					pattern_candidate[ temp_pair ] = 0;
				}
			}
		}
		// update similarity pattern pair candidate
		// std::cout<<"FIRST similarity pattern pair candidate"<<std::endl;
		for ( Pattern_table::iterator it = pattern_candidate.begin(); it != pattern_candidate.end(); ++it )
		{
			int i = it->first.first;
			int j = it->first.second;
			int count_similarity = 0;
			for ( int k = 0; k < size; ++k )
			{
				// if(mergeRecord[k]){continue;}
				if ( patternTable[ i * size + k ] && patternTable[ i * size + k ] == patternTable[ j * size + k ] )
				{
					++count_similarity;
				}
			}
			it->second = count_similarity;
			// std::cout<<"PAT "<<i<<" "<<j<<" "<<it->second<<std::endl;
		}

		while ( pattern_candidate.size() != 0 )
		{

			bool update_similarity_pattern[ size ];
			for ( int i = 0; i < size; ++i )
			{
				update_similarity_pattern[ i ] = false;
			}
			int similarity_pattern_first = 0;
			int similarity_pattern_second = 0;
			int max_similarity = 0;
			for ( Pattern_table::iterator it = pattern_candidate.begin(); it != pattern_candidate.end(); ++it )
			{
				if ( it->second > max_similarity )
				{
					max_similarity = it->second;
					similarity_pattern_first = it->first.first;
					similarity_pattern_second = it->first.second;
				}
			}
			if ( max_similarity == 0 )
			{
				return false;
			}
			// std::cout<<"-----MERGE PAIR "<<similarity_pattern_first<<" "<<similarity_pattern_second<<std::endl;
			// del_first_pattern
			std::vector<int> del_first_pattern;
			if ( max_similarity != 0 )
			{
				int i = similarity_pattern_first;
				int j = similarity_pattern_second;
				for ( int k = 0; k < size; ++k )
				{
					if ( mergeRecord[ k ] )
					{
						continue;
					}
					if ( patternTable[ i * size + k ] == 1 && patternTable[ j * size + k ] == 0 )
					{
						// std::cout<<"FUCK "<<i<<" "<<k<<std::endl;
						del_first_pattern.push_back( k );
					}
					update_similarity_pattern[ k ] = patternTable[ i * size + k ] || patternTable[ j * size + k ];
					patternTable[ i * size + k ] = patternTable[ i * size + k ] && patternTable[ j * size + k ];
					patternTable[ k * size + i ] = patternTable[ i * size + k ];
				}
			}
			int del_first_pattern_size = (int)del_first_pattern.size();
			for ( int i = 0; i < del_first_pattern_size; ++i )
			{
				// std::cout<<del_first_pattern[i]<<"-"<<similarity_pattern_first<<std::endl;
				if ( del_first_pattern[ i ] < similarity_pattern_first )
				{
					Pattern_pair temp_pair( del_first_pattern[ i ], similarity_pattern_first );
					pattern_candidate.erase( temp_pair );
				}
				else
				{
					Pattern_pair temp_pair( similarity_pattern_first, del_first_pattern[ i ] );
					pattern_candidate.erase( temp_pair );
				}
			}
			std::vector<int> del_second_pattern;
			for ( int i = 0; i < size; ++i )
			{
				if ( mergeRecord[ i ] )
				{
					continue;
				}
				if ( similarity_pattern_second == i )
				{
					continue;
				}
				if ( patternTable[ similarity_pattern_second * size + i ] )
				{
					del_second_pattern.push_back( i );
				}
			}
			int del_second_pattern_size = (int)del_second_pattern.size();
			for ( int i = 0; i < del_second_pattern_size; ++i )
			{
				// std::cout<<del_second_pattern[i]<<"-"<<similarity_pattern_second<<std::endl;
				if ( del_second_pattern[ i ] < similarity_pattern_second )
				{
					Pattern_pair temp_pair( del_second_pattern[ i ], similarity_pattern_second );
					pattern_candidate.erase( temp_pair );
				}
				else
				{
					Pattern_pair temp_pair( similarity_pattern_second, del_second_pattern[ i ] );
					pattern_candidate.erase( temp_pair );
				}
			}
			// merge pattern
			if ( max_similarity != 0 )
			{
				// std::cout<<"MERGE???"<<std::endl;
				int i = similarity_pattern_first;
				int j = similarity_pattern_second;
				for ( int k = 0; k < npi_; ++k )
				{
					pats_[ i ]->pi1_[ k ] = ( pats_[ i ]->pi1_[ k ] < pats_[ j ]->pi1_[ k ] ) ? pats_[ i ]->pi1_[ k ] : pats_[ j ]->pi1_[ k ];
				}
				if ( pats_[ i ]->pi2_ != NULL )
				{
					for ( int k = 0; k < npi_; ++k )
					{
						pats_[ i ]->pi2_[ k ] = ( pats_[ i ]->pi2_[ k ] < pats_[ j ]->pi2_[ k ] ) ? pats_[ i ]->pi2_[ k ] : pats_[ j ]->pi2_[ k ];
					}
				}
				if ( pats_[ i ]->ppi_ != NULL )
				{
					for ( int k = 0; k < nppi_; ++k )
					{
						pats_[ i ]->ppi_[ k ] = ( pats_[ i ]->ppi_[ k ] < pats_[ j ]->ppi_[ k ] ) ? pats_[ i ]->ppi_[ k ] : pats_[ j ]->ppi_[ k ];
					}
				}
				if ( pats_[ i ]->si_ != NULL )
				{
					for ( int k = 0; k < nsi_; ++k )
					{
						pats_[ i ]->si_[ k ] = ( pats_[ i ]->si_[ k ] < pats_[ j ]->si_[ k ] ) ? pats_[ i ]->si_[ k ] : pats_[ j ]->si_[ k ];
					}
				}

				for ( int k = 0; k < npo_; ++k )
				{
					pats_[ i ]->po1_[ k ] = ( pats_[ i ]->po1_[ k ] < pats_[ j ]->po1_[ k ] ) ? pats_[ i ]->po1_[ k ] : pats_[ j ]->po1_[ k ];
				}
				if ( pats_[ i ]->po2_ != NULL )
				{
					for ( int k = 0; k < npo_; ++k )
					{
						pats_[ i ]->po2_[ k ] = ( pats_[ i ]->po2_[ k ] < pats_[ j ]->po2_[ k ] ) ? pats_[ i ]->po2_[ k ] : pats_[ j ]->po2_[ k ];
					}
				}
				if ( pats_[ i ]->ppo_ != NULL )
				{
					for ( int k = 0; k < nppi_; ++k )
					{
						pats_[ i ]->ppo_[ k ] = ( pats_[ i ]->ppo_[ k ] < pats_[ j ]->ppo_[ k ] ) ? pats_[ i ]->ppo_[ k ] : pats_[ j ]->ppo_[ k ];
					}
				}
				mergeRecord[ j ] = true;
			}
			// update similarity pattern pair candidate
			// std::cout<<"--AFTER-------------------"<<std::endl;
			/*
			for(Pattern_table::iterator it = pattern_candidate.begin(); it !=pattern_candidate.end(); ++it){
					int i = it->first.first;
					int j = it->first.second;
					int count_similarity = 0;
					for(int k = 0; k<size;++k){
							if(mergeRecord[k]){continue;}
							if(patternTable[i*size+k] && patternTable[i*size+k]==patternTable[j*size+k]){++count_similarity;}
					}
					it->second = count_similarity;
					//std::cout<<"PAT "<<i<<" "<<j<<" "<<it->second<<std::endl;
			}
			*/
			// update similarity pattern pair candidate
			for ( int i = 0; i < size; ++i )
			{
				if ( mergeRecord[ i ] )
				{
					continue;
				}
				if ( update_similarity_pattern[ i ] )
				{
					for ( int j = 0; j < size; ++j )
					{
						if ( i == j )
						{
							continue;
						}
						if ( mergeRecord[ j ] )
						{
							continue;
						}
						if ( patternTable[ i * size + j ] )
						{
							Pattern_pair temp_pair;
							if ( i < j )
							{
								temp_pair.first = i;
								temp_pair.second = j;
							}
							else
							{
								temp_pair.first = j;
								temp_pair.second = i;
							}
							Pattern_table::iterator it = pattern_candidate.find( temp_pair );
							if ( it != pattern_candidate.end() )
							{
								int count_similarity = 0;
								for ( int b = 0; b < size; ++b )
								{
									if ( mergeRecord[ b ] )
									{
										continue;
									}
									if ( patternTable[ i * size + b ] && patternTable[ i * size + b ] == patternTable[ j * size + b ] )
									{
										++count_similarity;
									}
								}
								it->second = count_similarity;
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
	// new static compression

	inline bool PatternProcessor::updatetable2( bool *mergeRecord, bool *patternTable )
	{
		int size = (int)pats_.size();
		typedef std::pair<int, int> Pattern_pair;
		typedef std::map<Pattern_pair, int> Pattern_table;
		Pattern_table pattern_candidate;
		std::map<int, int> Node_size;

		// find pattern pair candidate
		for ( int i = 0; i < size - 1; ++i )
		{
			for ( int j = i + 1; j < size; ++j )
			{
				if ( patternTable[ i * size + j ] )
				{
					Pattern_pair temp_pair( i, j );
					pattern_candidate[ temp_pair ] = 0;
				}
			}
			Node_size[ i ] = 1;
		}
		// update similarity pattern pair candidate
		// std::cout<<"FIRST similarity pattern pair candidate"<<std::endl;
		for ( Pattern_table::iterator it = pattern_candidate.begin(); it != pattern_candidate.end(); ++it )
		{
			int i = it->first.first;
			int j = it->first.second;
			int count_similarity = 0;
			for ( int k = 0; k < size; ++k )
			{
				// if(mergeRecord[k]){continue;}
				if ( patternTable[ i * size + k ] && patternTable[ i * size + k ] == patternTable[ j * size + k ] )
				{
					++count_similarity;
				}
			}
			it->second = count_similarity;
			// std::cout<<"PAT "<<i<<" "<<j<<" "<<it->second<<std::endl;
		}

		while ( pattern_candidate.size() != 0 )
		{

			bool update_similarity_pattern[ size ];
			for ( int i = 0; i < size; ++i )
			{
				update_similarity_pattern[ i ] = false;
			}
			int similarity_pattern_first = 0;
			int similarity_pattern_second = 0;
			int max_similarity = 0;
			// martan
			int max_pair_size = 0;
			// std::cout<<"--------------begin----------------"<<std::endl;//

			for ( Pattern_table::iterator it = pattern_candidate.begin(); it != pattern_candidate.end(); ++it )
			{

				// std::cout<<"similarity:"<<it->second<<" pair size:"<<(Node_size[it->first.first]+Node_size[it->first.second])<<std::endl;//

				if ( it->second > max_similarity )
				{
					max_pair_size = 0;
				}

				if ( it->second >= max_similarity )
				{
					max_similarity = it->second;
					// similarity_pattern_first=it->first.first;
					// similarity_pattern_second=it->first.second;

					if ( ( Node_size[ it->first.first ] + Node_size[ it->first.second ] ) > max_pair_size )
					{
						max_pair_size = Node_size[ it->first.first ] + Node_size[ it->first.second ];

						similarity_pattern_first = it->first.first;
						similarity_pattern_second = it->first.second;

						// std::cout<<"-->update"<<std::endl;//
						// std::cout<<"similarity:"<<it->second<<" pair size:"<<(Node_size[it->first.first]+Node_size[it->first.second])<<std::endl;//
						// std::cout<<"-->update"<<std::endl;//
					}
				}
			}
			// std::cout<<" pair size:"<<(Node_size[similarity_pattern_first]+Node_size[similarity_pattern_second])<<std::endl;//
			// std::cout<<"--------------end----------------"<<std::endl;//

			if ( max_similarity == 0 )
			{
				return false;
			}
			// std::cout<<"-----MERGE PAIR "<<similarity_pattern_first<<" "<<similarity_pattern_second<<std::endl;
			// del_first_pattern
			std::vector<int> del_first_pattern;
			if ( max_similarity != 0 )
			{
				int i = similarity_pattern_first;
				int j = similarity_pattern_second;
				for ( int k = 0; k < size; ++k )
				{
					if ( mergeRecord[ k ] )
					{
						continue;
					}
					if ( patternTable[ i * size + k ] == 1 && patternTable[ j * size + k ] == 0 )
					{
						// std::cout<<"FUCK "<<i<<" "<<k<<std::endl;
						del_first_pattern.push_back( k );
					}
					update_similarity_pattern[ k ] = patternTable[ i * size + k ] || patternTable[ j * size + k ];
					patternTable[ i * size + k ] = patternTable[ i * size + k ] && patternTable[ j * size + k ];
					patternTable[ k * size + i ] = patternTable[ i * size + k ];
				}
			}
			int del_first_pattern_size = (int)del_first_pattern.size();
			for ( int i = 0; i < del_first_pattern_size; ++i )
			{
				// std::cout<<del_first_pattern[i]<<"-"<<similarity_pattern_first<<std::endl;
				if ( del_first_pattern[ i ] < similarity_pattern_first )
				{
					Pattern_pair temp_pair( del_first_pattern[ i ], similarity_pattern_first );
					pattern_candidate.erase( temp_pair );
				}
				else
				{
					Pattern_pair temp_pair( similarity_pattern_first, del_first_pattern[ i ] );
					pattern_candidate.erase( temp_pair );
				}
			}
			std::vector<int> del_second_pattern;
			for ( int i = 0; i < size; ++i )
			{
				if ( mergeRecord[ i ] )
				{
					continue;
				}
				if ( similarity_pattern_second == i )
				{
					continue;
				}
				if ( patternTable[ similarity_pattern_second * size + i ] )
				{
					del_second_pattern.push_back( i );
				}
			}
			int del_second_pattern_size = (int)del_second_pattern.size();
			for ( int i = 0; i < del_second_pattern_size; ++i )
			{
				// std::cout<<del_second_pattern[i]<<"-"<<similarity_pattern_second<<std::endl;
				if ( del_second_pattern[ i ] < similarity_pattern_second )
				{
					Pattern_pair temp_pair( del_second_pattern[ i ], similarity_pattern_second );
					pattern_candidate.erase( temp_pair );
				}
				else
				{
					Pattern_pair temp_pair( similarity_pattern_second, del_second_pattern[ i ] );
					pattern_candidate.erase( temp_pair );
				}
			}
			// merge pattern
			if ( max_similarity != 0 )
			{
				// std::cout<<"MERGE???"<<std::endl;
				int i = similarity_pattern_first;
				int j = similarity_pattern_second;
				for ( int k = 0; k < npi_; ++k )
				{
					pats_[ i ]->pi1_[ k ] = ( pats_[ i ]->pi1_[ k ] < pats_[ j ]->pi1_[ k ] ) ? pats_[ i ]->pi1_[ k ] : pats_[ j ]->pi1_[ k ];
				}
				if ( pats_[ i ]->pi2_ != NULL )
				{
					for ( int k = 0; k < npi_; ++k )
					{
						pats_[ i ]->pi2_[ k ] = ( pats_[ i ]->pi2_[ k ] < pats_[ j ]->pi2_[ k ] ) ? pats_[ i ]->pi2_[ k ] : pats_[ j ]->pi2_[ k ];
					}
				}
				if ( pats_[ i ]->ppi_ != NULL )
				{
					for ( int k = 0; k < nppi_; ++k )
					{
						pats_[ i ]->ppi_[ k ] = ( pats_[ i ]->ppi_[ k ] < pats_[ j ]->ppi_[ k ] ) ? pats_[ i ]->ppi_[ k ] : pats_[ j ]->ppi_[ k ];
					}
				}
				if ( pats_[ i ]->si_ != NULL )
				{
					for ( int k = 0; k < nsi_; ++k )
					{
						pats_[ i ]->si_[ k ] = ( pats_[ i ]->si_[ k ] < pats_[ j ]->si_[ k ] ) ? pats_[ i ]->si_[ k ] : pats_[ j ]->si_[ k ];
					}
				}

				for ( int k = 0; k < npo_; ++k )
				{
					pats_[ i ]->po1_[ k ] = ( pats_[ i ]->po1_[ k ] < pats_[ j ]->po1_[ k ] ) ? pats_[ i ]->po1_[ k ] : pats_[ j ]->po1_[ k ];
				}
				if ( pats_[ i ]->po2_ != NULL )
				{
					for ( int k = 0; k < npo_; ++k )
					{
						pats_[ i ]->po2_[ k ] = ( pats_[ i ]->po2_[ k ] < pats_[ j ]->po2_[ k ] ) ? pats_[ i ]->po2_[ k ] : pats_[ j ]->po2_[ k ];
					}
				}
				if ( pats_[ i ]->ppo_ != NULL )
				{
					for ( int k = 0; k < nppi_; ++k )
					{
						pats_[ i ]->ppo_[ k ] = ( pats_[ i ]->ppo_[ k ] < pats_[ j ]->ppo_[ k ] ) ? pats_[ i ]->ppo_[ k ] : pats_[ j ]->ppo_[ k ];
					}
				}
				mergeRecord[ j ] = true;

				Node_size[ i ] += 1;
				Node_size[ j ] = 0;
			}
			// update similarity pattern pair candidate
			// std::cout<<"--AFTER-------------------"<<std::endl;
			/*
			for(Pattern_table::iterator it = pattern_candidate.begin(); it !=pattern_candidate.end(); ++it){
					int i = it->first.first;
					int j = it->first.second;
					int count_similarity = 0;
					for(int k = 0; k<size;++k){
							if(mergeRecord[k]){continue;}
							if(patternTable[i*size+k] && patternTable[i*size+k]==patternTable[j*size+k]){++count_similarity;}
					}
					it->second = count_similarity;
					//std::cout<<"PAT "<<i<<" "<<j<<" "<<it->second<<std::endl;
			}
			*/
			// update similarity pattern pair candidate
			for ( int i = 0; i < size; ++i )
			{
				if ( mergeRecord[ i ] )
				{
					continue;
				}
				if ( update_similarity_pattern[ i ] )
				{
					for ( int j = 0; j < size; ++j )
					{
						if ( i == j )
						{
							continue;
						}
						if ( mergeRecord[ j ] )
						{
							continue;
						}
						if ( patternTable[ i * size + j ] )
						{
							Pattern_pair temp_pair;
							if ( i < j )
							{
								temp_pair.first = i;
								temp_pair.second = j;
							}
							else
							{
								temp_pair.first = j;
								temp_pair.second = i;
							}
							Pattern_table::iterator it = pattern_candidate.find( temp_pair );
							if ( it != pattern_candidate.end() )
							{
								int count_similarity = 0;
								for ( int b = 0; b < size; ++b )
								{
									if ( mergeRecord[ b ] )
									{
										continue;
									}
									if ( patternTable[ i * size + b ] && patternTable[ i * size + b ] == patternTable[ j * size + b ] )
									{
										++count_similarity;
									}
								}
								it->second = count_similarity;
							}
						}
					}
				}
			}
			// std::cout<<"PCSIZE "<<pattern_candidate.size()<<std::endl;
		}

		return true;
	}

	//

	class Vertex
	{
	public:
		Vertex();
		Vertex( Value *data, Fault *fault );
		~Vertex();
		Value *data_;
		FaultList fault_;
	};

	inline Vertex::Vertex()
	{
		data_ = NULL;
	}

	inline Vertex::Vertex( Value *data, Fault *fault )
	{
		data_ = data;
		fault_.push_back( fault );
	}

	inline Vertex::~Vertex()
	{
		delete[] data_;
		fault_.clear();
	}
	// end of Vertex

};

#endif
