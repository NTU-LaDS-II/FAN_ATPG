// *************************************************************************
// File       [ atpg.h ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/11/01 created ]
// **************************************************************************

#ifndef _CORE_ATPG_H_
#define _CORE_ATPG_H_
#include <cstdlib>
#include <string>
#include "simulator.h"
#include "decision_tree.h"
#include <algorithm>
#include <cassert>

namespace CoreNs
{

	const unsigned MAX_LIST_SIZE = 1000;
	const unsigned MAX_BACKTRACK = 500;
	const unsigned INFINITE = 0x7fffffff;
	const int UNIQUE_PATH_SENSITIZE_FAIL = -2;

	class Atpg
	{
	public:
		Atpg( Circuit *cir, Simulator *sim );
		~Atpg();

		enum GENERATION_STATUS
		{
			TEST_FOUND = 0,
			UNTESTABLE,
			ABORT
		};
		enum LINE_TYPE
		{
			FREE_LINE = 0,
			HEAD_LINE,
			BOUND_LINE
		};
		enum XPATH_STATE
		{
			NO_X_EXIST = 0,
			UNKNOWN,
			X_EXIST
		};
		enum IMP_STATUS
		{
			FORWARD = 0,
			BACKWARD,
			CONFLICT
		};
		enum BACKTRACE_STATUS
		{
			INITIAL = 0,
			CHECK_AND_SELECT,
			CURROBJ_DETERMINE,
			FANOBJ_DETERMINE
		};
		enum BACKTRACE_RESULT
		{
			NO_CONTRADICTORY = 0,
			CONTRADICTORY
		};
		enum FIND_OBJECTIVE_STATUS
		{
			CHECK_UNJUSTIFIED = 0,
			BACKTRACE_CURR,
			BACKTRACE_FANOUT,
			CHECK_HEADOBJ
		};
		enum MAIN_STATUS
		{
			IMPLY_AND_CHECK = 0,
			DECISION,
			BACKTRACK,
			JUSTIFY_FREE
		};

		void generation( PatternProcessor *pcoll, FaultListExtract *fListExtract ); // BAD NAME, B.8
		void TransitionDelayFaultATPG( FaultList &faultListToGen, PatternProcessor *pcoll, int &untest );
		void StuckAtFaultATPG( FaultList &faultListToGen, PatternProcessor *pcoll, int &untest );
		void XFill( PatternProcessor *pcoll );
		GENERATION_STATUS patternGeneration( Fault fault, bool isDTC );

		void setupCircuitParameter();
		void identifyLineParameter();
		void identifyDominator();
		void identifyUniquePath();
		void assignPatternPiValue( Pattern *pat ); // write PI values to pattern
		void assignPatternPoValue( Pattern *pat ); // write PO values to pattern

	protected:
		Circuit *cir_;
		Simulator *sim_;

		Fault headLineFault_;
		Fault currentFault_;
		unsigned backtrackLimit_;

		bool *modify_; // indicate whether the gate has been backtraced or implied, true means the gate has been modified.
		unsigned *n0_;
		unsigned *n1_;
		int *headLines_;			// array of headlines
		int nHeadLine_;				// number of headlines
		int *faultReach_;			// TRUE means this fanout is in fanout cone of target fault;
		LINE_TYPE *lineType_; // array of line types for all gates: FREE HEAD or BOUND
		XPATH_STATE *xPathStatus_;
		std::vector<int> *uniquePath_; // list of gates on the unique path associated with a D-forontier.  when there is only one gate in D-frontier, xPathTracing will update this information.

		std::stack<int> *eventList_;
		std::vector<int> backtrackList_;
		std::vector<int> backtraceResetList_; // gates n0 and n1 have been changed. They need to be reset after backtrace.
		std::vector<int> unjustified_;				// J-frontier list
		std::vector<int> initObject_;					// Initial fault activation objectives or D-frontier propagation objectives.
		std::vector<int> currentObject_;			// The objective that is selected from final objectives.
		std::vector<int> fanoutObjective_;
		std::vector<int> headObject_;	 // objectives at the headlines
		std::vector<int> finalObject_; // final objectives include fanout objectives and headline objectives.
		std::vector<int> dFrontier_;	 // D-frontier list
		DecisionTree decisionTree_;

		int firstTimeFrameSetUp( Fault &fault ); // for multiple time frame
		Gate *firstTimeFrameHeadLine_;

		bool coninuationMeaningful( Gate *pLastDFrontier );
		int countNumGatesInDFrontier( Gate *pFaultyLine );
		bool checkFaultPropToPO( bool &faultPropToPO );
		bool checkUnjustifiedBoundLines();
		void assignValueToFinalObject();
		Gate *initialize( Fault &fault, int &BackImpLevel, IMP_STATUS &implyStatus, bool isDTC );
		void initialList( bool initFlag );
		void initialNetlist( Gate &gFaultyLine, bool isDTC );
		Fault setFreeFaultyGate( Gate &gate );
		int setFaultyGate( Fault &fault );
		int uniquePathSensitize( Gate &gate );
		void justifyFreeLines( Fault &fOriginalFault );
		void restoreFault( Fault &fOriginalFault );
		void fanoutFreeBacktrace( Gate *pObjectGate );
		void updateUnjustifiedLines();
		void updateDFrontier();
		bool backtrack( int &BackImpLevel );
		void initialObjectives();
		void findFinalObjective( BACKTRACE_STATUS &flag,
														 bool FaultPropPO, Gate *&pLastDFrontier );
		Gate *findEasiestInput( Gate *pGate, Value Val );
		Gate *findCloseToOutput( std::vector<int> &list, int &index );
		BACKTRACE_RESULT multipleBacktrace( BACKTRACE_STATUS atpgStatus, int &finalObjectiveId ); // WRONG should be multiBacktrace or multipleBacktrace
		bool xPathTracing( Gate *pGate );																													// WRONG should be XPathTracing or UnknownPathTracing
		bool Implication( IMP_STATUS atpgStatus, int StartLevel );
		IMP_STATUS faultyGateEvaluation( Gate *pGate );
		IMP_STATUS evaluation( Gate *pGate );
		IMP_STATUS backwardImplication( Gate *pGate );

		Value evaluationGood( Gate &g );
		Value evaluationFaulty( Gate &g );
		Value assignBacktraceValue( unsigned &n0, unsigned &n1, Gate &g );

		void randomFill( Pattern *pat );
		void setn0n1( int gate, unsigned n0, unsigned n1 );

		//
		void pushEvent( int gid ); // push events to the event list of corresponding level
		int popEvent( int depth );
		void pushOutputEvents( int gid );									// WRONG NAME; T.14 ; should be called pushOutputEvents ;  push all the gate output events to event list of the corresponding level
		void pushOutputEvents( int gid, int &gateCount ); // record how many gates are pushed
		void pushInputEvents( int gid, int index );
		int listPop( std::vector<int> &list );
		void listDelete( std::vector<int> &list, int index );
		void clearAllEvent();

		// 5-Value logic evaluation functions
		Value cINV( Value i1 );
		Value cAND2( Value i1, Value i2 );
		Value cAND3( Value i1, Value i2, Value i3 );
		Value cAND4( Value i1, Value i2, Value i3, Value i4 );
		Value cNAND2( Value i1, Value i2 );
		Value cNAND3( Value i1, Value i2, Value i3 );
		Value cNAND4( Value i1, Value i2, Value i3, Value i4 );
		Value cOR2( Value i1, Value i2 );
		Value cOR3( Value i1, Value i2, Value i3 );
		Value cOR4( Value i1, Value i2, Value i3, Value i4 );
		Value cNOR2( Value i1, Value i2 );
		Value cNOR3( Value i1, Value i2, Value i3 );
		Value cNOR4( Value i1, Value i2, Value i3, Value i4 );
		Value cXOR2( Value i1, Value i2 );
		Value cXOR3( Value i1, Value i2, Value i3 );
		Value cXNOR2( Value i1, Value i2 );
		Value cXNOR3( Value i1, Value i2, Value i3 );

		/* Added by Shi-Tang Liu */
		std::string getValStr( Value val );
		void clearOneGateFaultEffect( Gate &g );
		void clearAllFaultEffectBySimulation();
		void testClearFaultEffect( FaultList &faultListToTest );
		int storeCurrentGateValue();
		void calDepthFromPo();
		void resetPreValue();
		bool isExistXPath( Gate *pGate );
		std::vector<bool> isInEventList_;
		void clearEventList_( bool isDebug );
		void resetIsInEventList();
		void setValueAndRunImp( Gate &g, Value val );
		void checkLevelInfo();
		void StuckAtFaultATPGWithDTC( FaultList &faultListToGen, PatternProcessor *pcoll, int &untest );
		Gate *getWireForActivation( Fault &fault );
		void reverseFaultSimulation( PatternProcessor *pcoll, FaultList &originalFaultList );
	};

	inline Atpg::Atpg( Circuit *cir, Simulator *sim )
	{
		cir_ = cir;
		sim_ = sim;

		n0_ = new unsigned[ cir->tgate_ ];
		n1_ = new unsigned[ cir->tgate_ ];
		lineType_ = new LINE_TYPE[ cir->tgate_ ];
		xPathStatus_ = new XPATH_STATE[ cir->tgate_ ];
		faultReach_ = new int[ cir->tgate_ ];
		uniquePath_ = new std::vector<int>[ cir->tgate_ ];
		modify_ = new bool[ cir->tgate_ ];
		eventList_ = new std::stack<int>[ cir->tlvl_ ];
		headLines_ = NULL;
		firstTimeFrameHeadLine_ = NULL;

		backtrackList_.reserve( cir->tgate_ );
		backtrackLimit_ = MAX_BACKTRACK;

		dFrontier_.reserve( MAX_LIST_SIZE );
		fanoutObjective_.reserve( MAX_LIST_SIZE );
		initObject_.reserve( MAX_LIST_SIZE );
		headObject_.reserve( MAX_LIST_SIZE );
		finalObject_.reserve( MAX_LIST_SIZE );
		unjustified_.reserve( MAX_LIST_SIZE );
		currentObject_.reserve( MAX_LIST_SIZE );
		isInEventList_.resize( cir->tgate_ );
		std::fill( isInEventList_.begin(), isInEventList_.end(), false );
	}
	inline Atpg::~Atpg()
	{
		delete[] eventList_;
		delete[] headLines_;
		delete[] n0_;
		delete[] n1_;
		delete[] lineType_;
		delete[] xPathStatus_;
		delete[] faultReach_;
		delete[] uniquePath_;
		delete[] modify_;
	}

	inline void Atpg::setn0n1( int gate, unsigned n0, unsigned n1 )
	{
		n0_[ gate ] = n0;
		n1_[ gate ] = n1;
	}
	//
	inline void Atpg::pushEvent( int gid )
	{
		Gate &g = cir_->gates_[ gid ];
		if ( !modify_[ gid ] )
		{
			eventList_[ g.lvl_ ].push( gid );
			modify_[ gid ] = true;
		}
	}
	inline int Atpg::popEvent( int depth )
	{
		int gid = eventList_[ depth ].top();
		eventList_[ depth ].pop();
		return gid;
	}
	inline int Atpg::listPop( std::vector<int> &list )
	{
		int num = list.back();
		list.pop_back();
		return num;
	}
	inline void Atpg::pushOutputEvents( int gid )
	{
		Gate &g = cir_->gates_[ gid ];
		for ( int i = 0; i < g.nfo_; i++ )
			pushEvent( g.fos_[ i ] );
	}
	inline void Atpg::pushOutputEvents( int gid, int &gateCount )
	{
		Gate &g = cir_->gates_[ gid ];
		for ( int i = 0; i < g.nfo_; i++ )
		{
			Gate &gOut = cir_->gates_[ g.fos_[ i ] ];
			if ( !modify_[ g.fos_[ i ] ] )
			{
				eventList_[ gOut.lvl_ ].push( g.fos_[ i ] );
				modify_[ g.fos_[ i ] ] = true;
				gateCount++;
			}
		}
	}
	inline void Atpg::pushInputEvents( int gid, int index )
	{
		Gate &g = cir_->gates_[ gid ];
		pushEvent( g.fis_[ index ] );
		pushOutputEvents( g.fis_[ index ] );
	}
	inline void Atpg::listDelete( std::vector<int> &list, int index )
	{
		list[ index ] = list.back();
		list.pop_back();
	}
	inline void Atpg::clearAllEvent()
	{
		int gid;
		for ( int i = 0; i < cir_->tlvl_; i++ )
			while ( !eventList_[ i ].empty() )
			{
				gid = popEvent( i );
				modify_[ gid ] = false;
			}
	}
	//}}}
	//{{{ 5-value logic evaluation functions
	inline Value Atpg::cINV( Value i1 )
	{
		const static Value map[ 5 ] = { H, L, X, B, D };
		if ( i1 >= Z )
			return Z;
		return map[ i1 ];
	}
	inline Value Atpg::cAND2( Value i1, Value i2 )
	{
		const static Value map[ 5 ][ 5 ] = {
				{ L, L, L, L, L },
				{ L, H, X, D, B },
				{ L, X, X, X, X },
				{ L, D, X, D, L },
				{ L, B, X, L, B } };
		if ( i1 >= Z || i2 >= Z )
			return Z;
		return map[ i1 ][ i2 ];
	}
	inline Value Atpg::cAND3( Value i1, Value i2, Value i3 )
	{
		return cAND2( i1, cAND2( i2, i3 ) );
	}
	inline Value Atpg::cAND4( Value i1, Value i2, Value i3, Value i4 )
	{
		return cAND2( cAND2( i1, i2 ), cAND2( i3, i4 ) );
	}
	inline Value Atpg::cNAND2( Value i1, Value i2 )
	{
		return cINV( cAND2( i1, i2 ) );
	}
	inline Value Atpg::cNAND3( Value i1, Value i2, Value i3 )
	{
		return cINV( cAND3( i1, i2, i3 ) );
	}
	inline Value Atpg::cNAND4( Value i1, Value i2, Value i3, Value i4 )
	{
		return cINV( cAND4( i1, i2, i3, i4 ) );
	}
	inline Value Atpg::cOR2( Value i1, Value i2 )
	{
		const static Value map[ 5 ][ 5 ] = {
				{ L, H, X, D, B },
				{ H, H, H, H, H },
				{ X, H, X, X, X },
				{ D, H, X, D, H },
				{ B, H, X, H, B } };
		if ( i1 >= Z || i2 >= Z )
			return Z;
		return map[ i1 ][ i2 ];
	}
	inline Value Atpg::cOR3( Value i1, Value i2, Value i3 )
	{
		return cOR2( i1, cOR2( i2, i3 ) );
	}
	inline Value Atpg::cOR4( Value i1, Value i2, Value i3, Value i4 )
	{
		return cOR2( cOR2( i1, i2 ), cOR2( i3, i4 ) );
	}
	inline Value Atpg::cNOR2( Value i1, Value i2 )
	{
		return cINV( cOR2( i1, i2 ) );
	}
	inline Value Atpg::cNOR3( Value i1, Value i2, Value i3 )
	{
		return cINV( cOR3( i1, i2, i3 ) );
	}
	inline Value Atpg::cNOR4( Value i1, Value i2, Value i3, Value i4 )
	{
		return cINV( cOR4( i1, i2, i3, i4 ) );
	}
	inline Value Atpg::cXOR2( Value i1, Value i2 )
	{
		const static Value map[ 5 ][ 5 ] = {
				{ L, H, X, D, B },
				{ H, L, X, B, D },
				{ X, X, X, X, X },
				{ D, B, X, L, H },
				{ B, D, X, H, L } };
		if ( i1 >= Z || i2 >= Z )
			return Z;
		return map[ i1 ][ i2 ];
	}
	inline Value Atpg::cXOR3( Value i1, Value i2, Value i3 )
	{
		return cXOR2( i1, cXOR2( i2, i3 ) );
	}
	inline Value Atpg::cXNOR2( Value i1, Value i2 )
	{
		return cINV( cXOR2( i1, i2 ) );
	}
	inline Value Atpg::cXNOR3( Value i1, Value i2, Value i3 )
	{
		return cINV( cXOR3( i1, i2, i3 ) );
	}
	//}}}

	// **************************************************************************
	// Function   [ Atpg::evaluationGood ]
	// Commentor  [ WYH ]
	// Synopsis   [ usage: Given the gate without falut, and generate the output,
	//                     and return.
	//              in:    Gate& g
	//              out:   Value
	//            ]
	// Date       [ WYH Ver. 1.0 started 2013/08/15]
	// **************************************************************************
	//{{{ Value Atpg::evaluationGood(Gate& g)
	inline Value Atpg::evaluationGood( Gate &g )
	{
		if ( g.type_ == Gate::PI || g.type_ == Gate::PPI )
			return g.v_;
		Value v[ 4 ];
		for ( int i = 0; i < g.nfi_; i++ )
			v[ i ] = cir_->gates_[ g.fis_[ i ] ].v_;

		switch ( g.type_ )
		{
			case Gate::PO:
			case Gate::PPO:
			case Gate::BUF:
				return v[ 0 ];
			case Gate::INV:
				return cINV( v[ 0 ] );
			case Gate::AND2:
				return cAND2( v[ 0 ], v[ 1 ] );
			case Gate::AND3:
				return cAND3( v[ 0 ], v[ 1 ], v[ 2 ] );
			case Gate::AND4:
				return cAND4( v[ 0 ], v[ 1 ], v[ 2 ], v[ 3 ] );

			case Gate::NAND2:
				return cNAND2( v[ 0 ], v[ 1 ] );
			case Gate::NAND3:
				return cNAND3( v[ 0 ], v[ 1 ], v[ 2 ] );
			case Gate::NAND4:
				return cNAND4( v[ 0 ], v[ 1 ], v[ 2 ], v[ 3 ] );

			case Gate::OR2:
				return cOR2( v[ 0 ], v[ 1 ] );
			case Gate::OR3:
				return cOR3( v[ 0 ], v[ 1 ], v[ 2 ] );
			case Gate::OR4:
				return cOR4( v[ 0 ], v[ 1 ], v[ 2 ], v[ 3 ] );

			case Gate::NOR2:
				return cNOR2( v[ 0 ], v[ 1 ] );
			case Gate::NOR3:
				return cNOR3( v[ 0 ], v[ 1 ], v[ 2 ] );
			case Gate::NOR4:
				return cNOR4( v[ 0 ], v[ 1 ], v[ 2 ], v[ 3 ] );

			case Gate::XOR2:
				return cXOR2( v[ 0 ], v[ 1 ] );
			case Gate::XOR3:
				return cXOR3( v[ 0 ], v[ 1 ], v[ 2 ] );

			case Gate::XNOR2:
				return cXNOR2( v[ 0 ], v[ 1 ] );
			case Gate::XNOR3:
				return cXNOR3( v[ 0 ], v[ 1 ], v[ 2 ] );
			default:
				return v[ 0 ];
		}
	}
	//}}}

	//{{{ Value Atpg::evaluationFaulty(Gate& g)
	// **************************************************************************
	// Function   [ Atpg::evaluationFaulty ]
	// Commentor  [ CAL ]
	// Synopsis   [ usage: deal with 2 frame PPI, check it's D or D' logic
	//              in:    gate
	//              out:   value
	//            ]
	// Date       [ Ver. 1.0 started 2013/08/13 ]
	// **************************************************************************
	inline Value Atpg::evaluationFaulty( Gate &g )
	{
		Value val;
		int i, FaultyLine;
		switch ( g.type_ )
		{
			case Gate::PI:
			case Gate::PPI:
				return g.v_;
			case Gate::PO:
			case Gate::PPO:
			case Gate::BUF:
				val = cir_->gates_[ g.fis_[ 0 ] ].v_;
				if ( val == L && ( currentFault_.type_ == Fault::SA1 || currentFault_.type_ == Fault::STF ) )
					val = B; // logic D' (0/1)
				if ( val == H && ( currentFault_.type_ == Fault::SA0 || currentFault_.type_ == Fault::STR ) )
					val = D; // logic D  (1/0)
				return val;
			case Gate::INV:
				val = cir_->gates_[ g.fis_[ 0 ] ].v_;
				if ( currentFault_.line_ == 0 )
				{
					val = cINV( val );
					if ( val == L && ( currentFault_.type_ == Fault::SA1 || currentFault_.type_ == Fault::STF ) )
						val = B;
					if ( val == H && ( currentFault_.type_ == Fault::SA0 || currentFault_.type_ == Fault::STR ) )
						val = D;
					return val;
				}
				else
				{
					if ( val == L && ( currentFault_.type_ == Fault::SA1 || currentFault_.type_ == Fault::STF ) )
						val = B;
					if ( val == H && ( currentFault_.type_ == Fault::SA0 || currentFault_.type_ == Fault::STR ) )
						val = D;
					return cINV( val );
				}
			case Gate::AND2:
			case Gate::AND3:
			case Gate::AND4:
				FaultyLine = currentFault_.line_;
				if ( currentFault_.line_ == 0 )
				{
					val = cir_->gates_[ g.fis_[ 0 ] ].v_;
					for ( i = 1; i < g.nfi_; i++ )
						val = cAND2( val, cir_->gates_[ g.fis_[ i ] ].v_ );
					if ( val == L && ( currentFault_.type_ == Fault::SA1 || currentFault_.type_ == Fault::STF ) )
						val = B;
					if ( val == H && ( currentFault_.type_ == Fault::SA0 || currentFault_.type_ == Fault::STR ) )
						val = D;
				}
				else
				{
					val = cir_->gates_[ g.fis_[ FaultyLine - 1 ] ].v_;
					if ( val == L && ( currentFault_.type_ == Fault::SA1 || currentFault_.type_ == Fault::STF ) )
						val = B;
					if ( val == H && ( currentFault_.type_ == Fault::SA0 || currentFault_.type_ == Fault::STR ) )
						val = D;
					for ( i = 0; i < g.nfi_; i++ )
						if ( i != FaultyLine - 1 )
							val = cAND2( val, cir_->gates_[ g.fis_[ i ] ].v_ );
				}
				return val;
			case Gate::NAND2:
			case Gate::NAND3:
			case Gate::NAND4:
				FaultyLine = currentFault_.line_;
				if ( currentFault_.line_ == 0 )
				{
					val = cir_->gates_[ g.fis_[ 0 ] ].v_;
					for ( i = 1; i < g.nfi_; i++ )
						val = cAND2( val, cir_->gates_[ g.fis_[ i ] ].v_ );

					val = cINV( val );

					if ( val == L && ( currentFault_.type_ == Fault::SA1 || currentFault_.type_ == Fault::STF ) )
						val = B;
					if ( val == H && ( currentFault_.type_ == Fault::SA0 || currentFault_.type_ == Fault::STR ) )
						val = D;
				}
				else
				{
					val = cir_->gates_[ g.fis_[ FaultyLine - 1 ] ].v_;
					if ( val == L && ( currentFault_.type_ == Fault::SA1 || currentFault_.type_ == Fault::STF ) )
						val = B;
					if ( val == H && ( currentFault_.type_ == Fault::SA0 || currentFault_.type_ == Fault::STR ) )
						val = D;
					for ( i = 0; i < g.nfi_; i++ )
						if ( i != FaultyLine - 1 )
							val = cAND2( val, cir_->gates_[ g.fis_[ i ] ].v_ );
					val = cINV( val );
				}
				return val;
			case Gate::OR2:
			case Gate::OR3:
			case Gate::OR4:
				FaultyLine = currentFault_.line_;
				if ( currentFault_.line_ == 0 )
				{
					val = cir_->gates_[ g.fis_[ 0 ] ].v_;
					for ( i = 1; i < g.nfi_; i++ )
						val = cOR2( val, cir_->gates_[ g.fis_[ i ] ].v_ );
					if ( val == L && ( currentFault_.type_ == Fault::SA1 || currentFault_.type_ == Fault::STF ) )
						val = B;
					if ( val == H && ( currentFault_.type_ == Fault::SA0 || currentFault_.type_ == Fault::STR ) )
						val = D;
				}
				else
				{
					val = cir_->gates_[ g.fis_[ FaultyLine - 1 ] ].v_;
					if ( val == L && ( currentFault_.type_ == Fault::SA1 || currentFault_.type_ == Fault::STF ) )
						val = B;
					if ( val == H && ( currentFault_.type_ == Fault::SA0 || currentFault_.type_ == Fault::STR ) )
						val = D;
					for ( i = 0; i < g.nfi_; i++ )
						if ( i != FaultyLine - 1 )
							val = cOR2( val, cir_->gates_[ g.fis_[ i ] ].v_ );
				}
				return val;
			case Gate::NOR2:
			case Gate::NOR3:
			case Gate::NOR4:
				FaultyLine = currentFault_.line_;
				if ( currentFault_.line_ == 0 )
				{
					val = cir_->gates_[ g.fis_[ 0 ] ].v_;
					for ( i = 1; i < g.nfi_; i++ )
						val = cOR2( val, cir_->gates_[ g.fis_[ i ] ].v_ );

					val = cINV( val );

					if ( val == L && ( currentFault_.type_ == Fault::SA1 || currentFault_.type_ == Fault::STF ) )
						val = B;
					if ( val == H && ( currentFault_.type_ == Fault::SA0 || currentFault_.type_ == Fault::STR ) )
						val = D;
				}
				else
				{
					val = cir_->gates_[ g.fis_[ FaultyLine - 1 ] ].v_;
					if ( val == L && ( currentFault_.type_ == Fault::SA1 || currentFault_.type_ == Fault::STF ) )
						val = B;
					if ( val == H && ( currentFault_.type_ == Fault::SA0 || currentFault_.type_ == Fault::STR ) )
						val = D;
					for ( i = 0; i < g.nfi_; i++ )
						if ( i != FaultyLine - 1 )
							val = cOR2( val, cir_->gates_[ g.fis_[ i ] ].v_ );
					val = cINV( val );
				}
				return val;
			case Gate::XOR2:
			case Gate::XOR3:
				FaultyLine = currentFault_.line_;
				if ( currentFault_.line_ == 0 )
				{

					if ( g.type_ == Gate::XOR2 )
						val = cXOR2( cir_->gates_[ g.fis_[ 0 ] ].v_, cir_->gates_[ g.fis_[ 1 ] ].v_ );
					else
						val = cXOR3( cir_->gates_[ g.fis_[ 0 ] ].v_, cir_->gates_[ g.fis_[ 1 ] ].v_, cir_->gates_[ g.fis_[ 2 ] ].v_ );

					if ( val == L && ( currentFault_.type_ == Fault::SA1 || currentFault_.type_ == Fault::STF ) )
						val = B;
					if ( val == H && ( currentFault_.type_ == Fault::SA0 || currentFault_.type_ == Fault::STR ) )
						val = D;
				}
				else
				{
					val = cir_->gates_[ g.fis_[ FaultyLine - 1 ] ].v_;
					if ( val == L && ( currentFault_.type_ == Fault::SA1 || currentFault_.type_ == Fault::STF ) )
						val = B;
					if ( val == H && ( currentFault_.type_ == Fault::SA0 || currentFault_.type_ == Fault::STR ) )
						val = D;

					if ( g.type_ == Gate::XOR2 )
					{
						if ( FaultyLine - 1 == 0 )
							val = cXOR2( cir_->gates_[ g.fis_[ 1 ] ].v_, val );
						else
							val = cXOR2( cir_->gates_[ g.fis_[ 0 ] ].v_, val );
					}
					else
					{ // XOR3
						if ( FaultyLine - 1 == 0 )
							val = cXOR3( cir_->gates_[ g.fis_[ 1 ] ].v_, cir_->gates_[ g.fis_[ 2 ] ].v_, val );
						else if ( FaultyLine - 1 == 1 )
							val = cXOR3( cir_->gates_[ g.fis_[ 0 ] ].v_, cir_->gates_[ g.fis_[ 2 ] ].v_, val );
						else
							val = cXOR3( cir_->gates_[ g.fis_[ 1 ] ].v_, cir_->gates_[ g.fis_[ 0 ] ].v_, val );
					}
				}
				return val;
			case Gate::XNOR2:
			case Gate::XNOR3:
				FaultyLine = currentFault_.line_;
				if ( currentFault_.line_ == 0 )
				{

					if ( g.type_ == Gate::XNOR2 )
						val = cXNOR2( cir_->gates_[ g.fis_[ 0 ] ].v_, cir_->gates_[ g.fis_[ 1 ] ].v_ );
					else
						val = cXNOR3( cir_->gates_[ g.fis_[ 0 ] ].v_, cir_->gates_[ g.fis_[ 1 ] ].v_, cir_->gates_[ g.fis_[ 2 ] ].v_ );

					if ( val == L && ( currentFault_.type_ == Fault::SA1 || currentFault_.type_ == Fault::STF ) )
						val = B;
					if ( val == H && ( currentFault_.type_ == Fault::SA0 || currentFault_.type_ == Fault::STR ) )
						val = D;
				}
				else
				{
					val = cir_->gates_[ g.fis_[ FaultyLine - 1 ] ].v_;
					if ( val == L && ( currentFault_.type_ == Fault::SA1 || currentFault_.type_ == Fault::STF ) )
						val = B;
					if ( val == H && ( currentFault_.type_ == Fault::SA0 || currentFault_.type_ == Fault::STR ) )
						val = D;

					if ( g.type_ == Gate::XNOR2 )
					{
						if ( FaultyLine - 1 == 0 )
							val = cXNOR2( cir_->gates_[ g.fis_[ 1 ] ].v_, val );
						else
							val = cXNOR2( cir_->gates_[ g.fis_[ 0 ] ].v_, val );
					}
					else
					{ // XOR3
						if ( FaultyLine - 1 == 0 )
							val = cXNOR3( cir_->gates_[ g.fis_[ 1 ] ].v_, cir_->gates_[ g.fis_[ 2 ] ].v_, val );
						else if ( FaultyLine - 1 == 1 )
							val = cXNOR3( cir_->gates_[ g.fis_[ 0 ] ].v_, cir_->gates_[ g.fis_[ 2 ] ].v_, val );
						else
							val = cXNOR3( cir_->gates_[ g.fis_[ 1 ] ].v_, cir_->gates_[ g.fis_[ 0 ] ].v_, val );
					}
				}
				return val;
			default:
				return g.v_;
		}
	}
	//}}}

	// **************************************************************************
	// Function   [ Atpg::assignPatternPiValue ]
	// Commentor  [ CAL ]
	// Synopsis   [ usage: assign primary input pattern value
	//              in:    Pattern list
	//              out:   void
	//              pointer modification: Pattern
	//            ]
	// Date       [ Ver. 1.0 started 2013/08/13 ]
	// **************************************************************************
	inline void Atpg::assignPatternPiValue( Pattern *pat )
	{
		for ( int i = 0; i < cir_->npi_; i++ )
			pat->pi1_[ i ] = cir_->gates_[ i ].v_;
		if ( pat->pi2_ != NULL && cir_->nframe_ > 1 )
			for ( int i = 0; i < cir_->npi_; i++ )
				pat->pi2_[ i ] = cir_->gates_[ i + cir_->ngate_ ].v_;
		for ( int i = 0; i < cir_->nppi_; i++ )
			pat->ppi_[ i ] = cir_->gates_[ cir_->npi_ + i ].v_;
		if ( pat->si_ != NULL && cir_->nframe_ > 1 )
			pat->si_[ 0 ] = cir_->connType_ == Circuit::SHIFT ? cir_->gates_[ cir_->ngate_ + cir_->npi_ ].v_ : X;
	}

	// **************************************************************************
	// Function   [ Atpg::assignPatternPoValue ]
	// Commentor  [ CAL ]
	// Synopsis   [ usage: assign primary output pattern value
	//              in:    Pattern list
	//              out:   void
	//              pointer modification: Pattern
	//            ]
	// Date       [ Ver. 1.0 started 2013/08/13 ]
	// **************************************************************************
	inline void Atpg::assignPatternPoValue( Pattern *pat )
	{
		sim_->goodSim();
		int offset = cir_->ngate_ - cir_->npo_ - cir_->nppi_;
		for ( int i = 0; i < cir_->npo_; i++ )
		{
			if ( cir_->gates_[ offset + i ].gl_ == PARA_H )
				pat->po1_[ i ] = L;
			else if ( cir_->gates_[ offset + i ].gh_ == PARA_H )
				pat->po1_[ i ] = H;
			else
				pat->po1_[ i ] = X;
		}
		if ( pat->po2_ != NULL && cir_->nframe_ > 1 )
			for ( int i = 0; i < cir_->npo_; i++ )
			{
				if ( cir_->gates_[ offset + i + cir_->ngate_ ].gl_ == PARA_H )
					pat->po2_[ i ] = L;
				else if ( cir_->gates_[ offset + i + cir_->ngate_ ].gh_ == PARA_H )
					pat->po2_[ i ] = H;
				else
					pat->po2_[ i ] = X;
			}

		offset = cir_->ngate_ - cir_->nppi_;
		if ( cir_->nframe_ > 1 )
			offset += cir_->ngate_;
		for ( int i = 0; i < cir_->nppi_; i++ )
		{
			if ( cir_->gates_[ offset + i ].gl_ == PARA_H )
				pat->ppo_[ i ] = L;
			else if ( cir_->gates_[ offset + i ].gh_ == PARA_H )
				pat->ppo_[ i ] = H;
			else
				pat->ppo_[ i ] = X;
		}
	}

	// should be moved to pattern.h  ; job A.1
	// **************************************************************************
	// Function   [ Atpg::randomFill ]
	// Commentor  [ CAL ]
	// Synopsis   [ usage: random to set the don't care pattern 1 or 0
	//              in:    Pattern
	//              out:   void
	//              pointer modification: Pattern
	//            ]
	// Date       [ Ver. 1.0 started 2013/08/13 ]
	// **************************************************************************
	inline void Atpg::randomFill( Pattern *pat )
	{
		srand( 0 );
		for ( int i = 0; i < cir_->npi_; i++ )
			if ( pat->pi1_[ i ] == X )
				pat->pi1_[ i ] = rand() % 2;
		for ( int i = 0; i < cir_->nppi_; i++ )
			if ( pat->ppi_[ i ] == X )
				pat->ppi_[ i ] = rand() % 2;
		if ( pat->pi2_ != NULL )
			for ( int i = 0; i < cir_->npi_; i++ )
				if ( pat->pi2_[ i ] == X )
					pat->pi2_[ i ] = rand() % 2;
		if ( pat->si_ != NULL )
			if ( pat->si_[ 0 ] == X )
				pat->si_[ 0 ] = rand() % 2;
	}

};

#endif
