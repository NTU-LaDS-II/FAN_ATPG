// **************************************************************************
// File       [ atpg.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/11/01 created ]
// **************************************************************************

#include "atpg.h"

using namespace std;
using namespace CoreNs;

// **************************************************************************
// Function   [ Atpg::generation ]
// Commentor  [ CAL ]
// Synopsis   [ usage: generate patterns for faults
//              in:    Pattern list, Fault list
//              out:   void //TODO not void
//            ]
// Date       [ Ver. 1.0 started 2013/08/13 ]
// **************************************************************************
void Atpg::generation( PatternProcessor *pcoll, FaultListExtract *fListExtract )
{
	setupCircuitParameter();
	pcoll->init( cir_ );
	checkLevelInfo();
	// setting faults for running ATPG
	FaultList faultListToGen;
	FaultListIter it = fListExtract->current_.begin();
	int untest = 0;
	for ( ; it != fListExtract->current_.end(); ++it )
		if ( ( *it )->state_ != Fault::DT && ( *it )->state_ != Fault::RE && ( *it )->line_ >= 0 )
			faultListToGen.push_back( *it );
	// ori fault copy
	FaultList faultOri = faultListToGen; //

	Fault *currF = NULL;
	// start ATPG
	// take one undetected fault from the faultListToGen
	// if the fault is undetected, run ATPG on it

	// To test clearAllFaultEffectBySimulation
	testClearFaultEffect( faultListToGen );

	while ( faultListToGen.begin() != faultListToGen.end() )
	{
		if ( faultListToGen.front()->state_ == Fault::AB )
			break;

		if ( currF == faultListToGen.front() )
		{
			// cout << "Bug!!!" <<endl;
			// cout << faultListToGen.size() << endl;
			// cout << faultListToGen.front()->gate_ << ' ' << faultListToGen.front()->line_ << ' ' << faultListToGen.front()->type_ << ' '
			// 	 << ((faultListToGen.front()->line_==0)?(0):(cir_->gates_[faultListToGen.front()->gate_].fis_[faultListToGen.front()->line_-1])) << endl;
			faultListToGen.front()->state_ = Fault::DT;
			faultListToGen.pop_front();
			continue;
		}
		currF = faultListToGen.front();

		// Transition delay fault ATPG
		if ( ( *faultListToGen.front() ).type_ == Fault::STR || ( *faultListToGen.front() ).type_ == Fault::STF )
			TransitionDelayFaultATPG( faultListToGen, pcoll, untest );

		// stuck-at fault ATPG
		else
			StuckAtFaultATPGWithDTC( faultListToGen, pcoll, untest );
		// StuckAtFaultATPG( faultListToGen, pcoll, untest );
	}

	// do static compression using compability graph
	if ( pcoll->staticCompression_ == PatternProcessor::ON )
	{
		// pcoll->StaticCompression();
		reverseFaultSimulation( pcoll, faultOri );
	}
	// do XFill
	if ( pcoll->XFill_ == PatternProcessor::ON )
	{
		XFill( pcoll );
	}
}

// **************************************************************************
// Function   [ Atpg::TransitionDelayFaultATPG ]
// Commentor  [ HKY CYW ]
// Synopsis   [ usage: do transition delay fault model ATPG
//              in:    Pattern list, Fault list, int untest
//              out:   void//TODO
//            ]
// Date       [ HKY Ver. 1.0 started 2014/09/01 ]
// **************************************************************************
void Atpg::TransitionDelayFaultATPG( FaultList &faultListToGen, PatternProcessor *pcoll, int &untest )
{
	Fault fTDF = *faultListToGen.front();

	GENERATION_STATUS result = patternGeneration( Fault( fTDF.gate_ + cir_->ngate_, fTDF.type_, fTDF.line_, fTDF.aggr_ ), false );
	if ( result == TEST_FOUND )
	{
		Pattern *p = new Pattern;
		p->pi1_ = new Value[ cir_->npi_ ];
		p->pi2_ = new Value[ cir_->npi_ ];
		p->ppi_ = new Value[ cir_->nppi_ ];
		p->si_ = new Value[ 1 ];
		p->po1_ = new Value[ cir_->npo_ ];
		p->po2_ = new Value[ cir_->npo_ ];
		p->ppo_ = new Value[ cir_->nppi_ ];
		pcoll->pats_.push_back( p );
		assignPatternPiValue( pcoll->pats_.back() );

		if ( ( pcoll->staticCompression_ == PatternProcessor::OFF ) && ( pcoll->XFill_ == PatternProcessor::ON ) )
		{
			randomFill( pcoll->pats_.back() );
		}

		sim_->pfFaultSim( pcoll->pats_.back(), faultListToGen );
		assignPatternPoValue( pcoll->pats_.back() );
	}
	else if ( result == UNTESTABLE )
	{
		faultListToGen.front()->state_ = Fault::AU;
		faultListToGen.pop_front();
		untest++;
	}
	else
	{
		faultListToGen.front()->state_ = Fault::AB;
		faultListToGen.push_back( faultListToGen.front() );
		faultListToGen.pop_front();
	}
}

// **************************************************************************
// Function   [ Atpg::StuckAtFaultATPG ]
// Commentor  [ HKY CYW ]
// Synopsis   [ usage: do stuck at fault model ATPG
//              in:    Pattern list, Fault list, int untest
//              out:   void //TODO
//            ]
// Date       [ HKY Ver. 1.0 started 2014/09/01 ]
// **************************************************************************
void Atpg::StuckAtFaultATPG( FaultList &faultListToGen, PatternProcessor *pcoll, int &untest )
{
	GENERATION_STATUS result = patternGeneration( *faultListToGen.front(), false );
	if ( result == TEST_FOUND )
	{
		Pattern *p = new Pattern;
		p->pi1_ = new Value[ cir_->npi_ ];
		p->ppi_ = new Value[ cir_->nppi_ ];
		p->po1_ = new Value[ cir_->npo_ ];
		p->ppo_ = new Value[ cir_->nppi_ ];
		pcoll->pats_.push_back( p );
		assignPatternPiValue( pcoll->pats_.back() );

		// if static compression is OFF and random fill in on
		// do random x-fill
		if ( ( pcoll->staticCompression_ == PatternProcessor::OFF ) && ( pcoll->XFill_ == PatternProcessor::ON ) )
		{
			randomFill( pcoll->pats_.back() );
		}

		sim_->pfFaultSim( pcoll->pats_.back(), faultListToGen );
		assignPatternPoValue( pcoll->pats_.back() );
	}
	else if ( result == UNTESTABLE )
	{
		faultListToGen.front()->state_ = Fault::AU;
		faultListToGen.pop_front();
		untest++;
	}
	else
	{
		faultListToGen.front()->state_ = Fault::AB;
		faultListToGen.push_back( faultListToGen.front() );
		faultListToGen.pop_front();
	}
}

// **************************************************************************
// Function   [ Atpg::XFill ]
// Commentor  [ HKY CYW ]
// Synopsis   [ usage: do X-Fill on generated pattern
//              in:    Pattern list
//              out:   void //TODO
//            ]
// Date       [ HKY Ver. 1.0 started 2014/09/01 ]
// **************************************************************************
void Atpg::XFill( PatternProcessor *pcoll )
{
	for ( int i = 0; i < (int)pcoll->pats_.size(); ++i )
	{
		randomFill( pcoll->pats_[ i ] );
		sim_->assignPatternToPi( pcoll->pats_.at( i ) );
		sim_->goodSim();
		assignPatternPoValue( pcoll->pats_.at( i ) );
	}
}

// **************************************************************************
// Function   [ Atpg::setupCircuitParameter ]
// Commentor  [ KOREAL ]
// Synopsis   [ usage: initialize gate's data including
//					    numOfZero = {0}
//						numOfOne  = {0}
//						modify_
//						eventList_
//				//TODO in out
//            ]
// Date       [ KOREAL Ver. 1.0 started 2013/08/10 ]
// **************************************************************************
void Atpg::setupCircuitParameter()
{

	// initialize numOfZero, numOfOne and modify_ of a gate
	for ( int i = 0; i < cir_->tgate_; i++ )
	{
		n0_[ i ] = 0; // BAD NAME; should be n0
		n1_[ i ] = 0; // BAD NAME; should be n1
		modify_[ i ] = false;
	}

	// set depthFromPo_
	calDepthFromPo();

	// Determine the lineType of a gate is FREE_LINE, BOUND_LINE or HEAD_LINE.
	identifyLineParameter();

	// see identifyDominator()
	identifyDominator();

	// see identifyUniquePath()
	identifyUniquePath();
}

// **************************************************************************
// Function   [ Atpg::identifyLineParameter ]
// Commentor  [ CKY ]
// Synopsis   [ usage: fill in LineParameter(FREE or HEAD or BOUND)
//              in:    void //TODO
//              out:   void //TODO
//            ]
// Date       [ CKY Ver. 1.0 commented and finished 2013/08/17 ]
// **************************************************************************
void Atpg::identifyLineParameter()
{
	nHeadLine_ = 0; // number of head line

	// go through all the gates
	for ( int i = 0; i < cir_->tgate_; i++ )
	{

		// get ith gate from gate array in cir_
		Gate &g = cir_->gates_[ i ];

		// assign as FREE_LINE first
		lineType_[ g.id_ ] = FREE_LINE;

		// check it is BOUND_LINE or not
		if ( g.type_ != Gate::PI && g.type_ != Gate::PPI )
			for ( int j = 0; j < g.nfi_; j++ ) // g.nfi_  number of fanin

				// if one of fanin is not FREE_LINE, then set lineType as BOUND_LINE
				if ( lineType_[ g.fis_[ j ] ] != FREE_LINE )
				{
					lineType_[ g.id_ ] = BOUND_LINE;
					break;
				}
		// check it is HEAD_LINE or not(rule 1)
		if ( lineType_[ g.id_ ] == FREE_LINE && g.nfo_ != 1 )
		{ // g.nfo_  number of fanout
			lineType_[ g.id_ ] = HEAD_LINE;
			nHeadLine_++; // number of head line + 1
		}

		// check it is HEAD_LINE or not(rule 2)
		if ( lineType_[ g.id_ ] == BOUND_LINE )
			for ( int j = 0; j < g.nfi_; j++ ) // g.nfi_  number of fanin
				if ( lineType_[ g.fis_[ j ] ] == FREE_LINE )
				{
					lineType_[ g.fis_[ j ] ] = HEAD_LINE;
					nHeadLine_++;
				}
	}

	// store all head lines to array  headLines_
	headLines_ = new int[ nHeadLine_ ];
	int inHead = 0;
	for ( int i = 0; i < cir_->tgate_; i++ )
		if ( lineType_[ i ] == HEAD_LINE )
		{
			headLines_[ inHead++ ] = i;
			if ( inHead == nHeadLine_ )
				break;
		}
}

// **************************************************************************
// Function   [ Atpg::identifyDominator ]
// Commentor  [ CAL ]
// Synopsis   [ usage: identify Dominator of the output gate
//                     for unique sensitization. After this function, each
//                     gate have one or zero Dominator in its uniquePath_ vector
//              in:    gate list
//              out:   void //TODO
//            ]
// Date       [ Ver. 1.0 started 2013/08/13   last modified 2020/07/09 ]
// **************************************************************************
void Atpg::identifyDominator()
{
	for ( int i = cir_->tgate_ - 1; i >= 0; i-- )
	{
		Gate &g = cir_->gates_[ i ];
		if ( g.nfo_ <= 1 ) // if gate has only 1 or 0 output skip this gate
			continue;

		int gateCount = 0;
		pushOutputEvents( i, gateCount );

		for ( int j = g.lvl_ + 1; j < cir_->tlvl_; j++ )
			while ( !eventList_[ j ].empty() )
			{ // if next level's output isn't empty
				Gate &gDom = cir_->gates_[ eventList_[ j ].top() ];
				eventList_[ j ].pop();
				modify_[ gDom.id_ ] = false; // set the gDom is not handle
				/*
				 * Because the gateCount is zero while the eventList_ is not zero, we
				 * only need to pop without other operations. That is, continue operation.
				 */
				if ( gateCount <= 0 )
					continue;
				gateCount--;
				if ( gateCount == 0 )
				{																							// when all the fanout gate has been calculated
					uniquePath_[ g.id_ ].push_back( gDom.id_ ); // push the last calculate fanout gate into uniquePath_
					break;
				}
				/*
				 * If there is a gate without gates on its output, then we suppose that this
				 * gate is PO/PPO.  Because there are other gates still inside the
				 * event queue (gateCount larger than 1), it means that gate g does
				 * not have any dominator.  Hence, we will set gateCount to zero as a
				 * signal to clear the gates left in eventList_.  While the eventList_ is
				 * not empty but gateCount is zero, we will continue.
				 */
				if ( gDom.nfo_ == 0 )
					gateCount = 0;
				else if ( gDom.nfo_ > 1 )
				{
					if ( uniquePath_[ gDom.id_ ].size() == 0 )
						gateCount = 0;
					else
					{
						/*
						 * Because the first gate in uniquePath_ is the closest Dominator, we just
						 * push it to the eventList_. Then, we can skip the operation we have done
						 * for gates with higher lvl_ than gate g.
						 */
						Gate &gTmp = cir_->gates_[ uniquePath_[ gDom.id_ ][ 0 ] ];
						if ( !modify_[ gTmp.id_ ] )
						{
							eventList_[ gTmp.lvl_ ].push( gTmp.id_ );
							modify_[ gTmp.id_ ] = true;
							gateCount++;
						}
					}
				}
				else if ( !modify_[ gDom.fos_[ 0 ] ] )
				{
					Gate &gTmp = cir_->gates_[ gDom.fos_[ 0 ] ];
					eventList_[ gTmp.lvl_ ].push( gTmp.id_ );
					modify_[ gTmp.id_ ] = true;
					gateCount++;
				}
			}
	}
}

// **************************************************************************
// Function   [ Atpg::identifyUniquePath ]
// Commentor  [ CAL ]
// Synopsis   [ usage: compute the uniquePath_
//                In unique path sensitizatoin phase, we will need to know
//                if the inputs of a gate is fault reachable. Then, we can
//                prevent to assign non-controlling value to them.
//
//                We find the dominator, then we push_back the input gate
//                which is fault reachable from gate g.
//
//                After identifyUniquePath, If a gate with dominator, then
//                the uniquePath_ vector of this gate will contains the
//                following gate id:
//
//                  [dominator_id   fault-reachable_input_gate_1_id   fault-reachable_input_gate_2_id  ......]
//
//              in:    gate list
//              out:   void //TODO
//            ]
// Date       [ Ver. 1.0 started 2013/08/13   last modified 2020/07/09 ]
// **************************************************************************
void Atpg::identifyUniquePath()
{
	for ( int i = cir_->tgate_ - 1; i >= 0; i-- )
	{
		Gate &g = cir_->gates_[ i ];
		int count;
		/*
		 * Because we will call identifyDominator before entering this function,
		 * a gate with uniquePath_ will contain one Dominator.  Hence, we can
		 * skip the gates while the sizes of their uniquePath_ is zero.
		 * */
		if ( uniquePath_[ g.id_ ].size() == 0 )
			continue;
		faultReach_[ g.id_ ] = i;
		count = 0;
		pushOutputEvents( i, count );

		for ( int j = g.lvl_ + 1; j < cir_->tlvl_; j++ )
			while ( !eventList_[ j ].empty() )
			{ // if fanout gate has not empty
				Gate &gTmp = cir_->gates_[ eventList_[ j ].top() ];
				eventList_[ j ].pop();
				modify_[ gTmp.id_ ] = false;
				faultReach_[ gTmp.id_ ] = i;
				count--;
				if ( count == 0 )
				{
					for ( int k = 0; k < gTmp.nfi_; k++ )
					{
						Gate &gReach = cir_->gates_[ gTmp.fis_[ k ] ];
						if ( faultReach_[ gReach.id_ ] == i )						// if it is UniquePath
							uniquePath_[ g.id_ ].push_back( gReach.id_ ); // save gate in uniquePath_ list
					}
					break;
				}
				pushOutputEvents( gTmp.id_, count );
			}
	}
}

// **************************************************************************
// Function   [ Atpg::PatternGeneration ]
// Commentor  [ KOREAL ]
// Synopsis   [ usage: Given a fault, generate the pattern
//              in:    fault isDTC
//              out:   GENERATION_STATUS(TEST_FOUND = 0, UNTESTABLE, ABORT) //TODO
//				There are four main atpgStatus while generate the pattern:
//
//				IMPLY_AND_CHECK: 	Determine as many signal values as possible then check if the backtrace is meaningful or not.
//				DECISION:			Using the multiple backtrace procedure to determine a final objective.
//				BACKTRACK:			If the values are incompatible during propagation or implications, backtracking is necessary.
//				JUSTIFY_FREE:		at the end of the process. Finding values on the primary inputs which justify all the values on the head lines
//            ]
// Date       [ KOREAL Ver. 1.0 started 2013/08/10 ]
// **************************************************************************
Atpg::GENERATION_STATUS Atpg::patternGeneration( Fault fault, bool isDTC )
{

	Gate *pFaultyLine = NULL;									// the gate pointer, whose fanOut is the target fault
	Gate *pLastDFrontier = NULL;							// the D-frontier gate which has the highest level of the D-frontier
	IMP_STATUS implyStatus;										// decide implication to go forward or backward
	BACKTRACE_STATUS backtraceFlag;						// backtrace flag including    { INITIAL, CHECK_AND_SELECT, CURROBJ_DETERMINE, FANOBJ_DETERMINE }
	GENERATION_STATUS genStatus = TEST_FOUND; // the atpgStatus that will be return, including		{ TEST_FOUND, UNTESTABLE, ABORT }
	int BackImpLevel = 0;											// backward imply level
	int numOfBacktrack = 0;										// backtrack times
	bool Finish = false;											// Finish is true when whole pattern generation process is done
	bool faultPropToPO = false;								// faultPropToPO is true when the fault is propagate to the PO

	// Get the gate whose output is fault line and set the BackImpLevel
	// SET A FAULT SIGNAL
	pFaultyLine = initialize( fault, BackImpLevel, implyStatus, isDTC );
	// If there's no such gate, return UNTESTABLE
	if ( pFaultyLine == NULL )
		return UNTESTABLE;

	// SET BACKTRACE FLAG
	backtraceFlag = INITIAL;

	while ( !Finish )
	{
		// Implication
		if ( !Implication( implyStatus, BackImpLevel ) )
		{
			// implication INCONSISTENCY
			// record the number of backtrack
			if ( decisionTree_.lastNodeMark() )
				numOfBacktrack++;
			// Abort if numOfBacktrack reaching the backtrackLimit_
			if ( numOfBacktrack > (int)backtrackLimit_ )
			{
				genStatus = ABORT;
				Finish = true;
			}

			clearAllEvent();

			// IS THERE AN UNTRIED COMBINATION OF VALUES ON ASSIGNED HEAD LINES OR FANOOUT POINTS?
			// If yes, SET UNTRIED COMBINATION OF VALUES in the function backtrack
			if ( backtrack( BackImpLevel ) )
			{
				// backtrack success and initialize the data
				// SET BACKTRACE FLAG
				backtraceFlag = INITIAL;
				implyStatus = BackImpLevel > 0 ? BACKWARD : FORWARD;
				pLastDFrontier = NULL;
			}
			else
			{
				// backtrack fail
				// EXIT: UNTESTABLE FAULT
				genStatus = UNTESTABLE;
				Finish = true;
			}
			continue;
		}

		// IS CONTINUATION OF BACKTRACE MEANINGFUL?
		if ( !coninuationMeaningful( pLastDFrontier ) )
			// SET BACKTRACE FLAG
			backtraceFlag = INITIAL;

		// FAULT SIGNAL PROPAGATED TO A PRIMARY OUTPUT?
		if ( checkFaultPropToPO( faultPropToPO ) )
		{
			// IS THERE ANY UNJUSTIFIED BOUND LINE?
			if ( checkUnjustifiedBoundLines() )
			{
				// DETERMINE A FINAL OBJECTIVE TO ASSIGN A VALUE
				findFinalObjective( backtraceFlag, faultPropToPO, pLastDFrontier );
				// ASSIGN A VALUE TO THE FINAL OBJECTIVE LINE
				assignValueToFinalObject();
				implyStatus = FORWARD;
				continue;
			}
			else
			{
				// Finding values on the primary inputs which justify all the values on the head lines
				// LINE JUSTIFICATION OF FREE LINES
				justifyFreeLines( fault );
				// EXIT: TEST GENERATED
				genStatus = TEST_FOUND;
				Finish = true;
			}
		}
		else
		{
			// not propagate to PO
			// THE NUMBER OF GATES IN D-FRONTIER?
			int numGatesInDFrontier = countNumGatesInDFrontier( pFaultyLine );

			// ZERO
			if ( numGatesInDFrontier == 0 )
			{
				// no frontier can propagate to the PO
				// record the number of backtrack
				if ( decisionTree_.lastNodeMark() )
					numOfBacktrack++;
				// Abort if numOfBacktrack reaching the backtrackLimit_
				if ( numOfBacktrack > (int)backtrackLimit_ )
				{
					genStatus = ABORT;
					Finish = true;
				}

				clearAllEvent();

				// IS THERE AN UNTRIED COMBINATION OF VALUES ON ASSIGNED HEAD LINES OR FANOOUT POINTS?
				// If yes, SET UNTRIED COMBINATION OF VALUES in the function backtrack
				if ( backtrack( BackImpLevel ) )
				{
					// backtrack success and initialize the data
					// SET BACKTRACE FLAG
					backtraceFlag = INITIAL;
					implyStatus = BackImpLevel > 0 ? BACKWARD : FORWARD;
					pLastDFrontier = NULL;
				}
				else
				{
					// backtrack fail
					// EXIT: UNTESTABLE FAULT
					genStatus = UNTESTABLE;
					Finish = true;
				}
			}
			// ONE
			else if ( numGatesInDFrontier == 1 )
			{
				// There exist just one path to the PO
				// UNIQUE SENSITIZATION
				BackImpLevel = uniquePathSensitize( cir_->gates_[ dFrontier_[ 0 ] ] );
				// Unique Sensitization fail
				if ( BackImpLevel == UNIQUE_PATH_SENSITIZE_FAIL )
				{
					// If UNIQUE_PATH_SENSITIZE_FAIL, the number of gates in d-frontier in the next while loop
					// and will backtrack
					continue;
				}
				// Unique Sensitization success
				if ( BackImpLevel > 0 )
				{
					implyStatus = BACKWARD;
					continue;
				}
				else if ( BackImpLevel == 0 )
				{
					continue;
				}
				else
				{
					// BackImpLevel < 0
					// find an objective and set backtraceFlag and pLastDFrontier
					findFinalObjective( backtraceFlag, faultPropToPO, pLastDFrontier );
					assignValueToFinalObject();
					implyStatus = FORWARD;
					continue;
				}
			}
			// PLURAL
			else
			{
				// DETERMINE A FINAL OBJECTIVE TO ASSIGN A VALUE
				findFinalObjective( backtraceFlag, faultPropToPO, pLastDFrontier );
				// ASSIGN A VALUE TO THE FINAL OBJECTIVE LINE
				assignValueToFinalObject();
				implyStatus = FORWARD;
				continue;
			}
		}
	}
	return genStatus;
}

// TODO comment
bool Atpg::coninuationMeaningful( Gate *pLastDFrontier )
{
	bool fDFrontierChanged; // fDFrontierChanged is true when D-frontier must to change
	// update unjustified lines
	updateUnjustifiedLines();

	// if the initial object is modified, delete it from initObject_ list
	for ( int k = initObject_.size() - 1; k >= 0; k-- )
	{
		if ( modify_[ initObject_[ k ] ] )
			listDelete( initObject_, k );
	}

	// determine the pLastDFrontier should be changed or not
	if ( pLastDFrontier != NULL )
	{
		if ( pLastDFrontier->v_ == X )
			fDFrontierChanged = false;
		else
			fDFrontierChanged = true;
	}
	else
		fDFrontierChanged = true;

	// If all init. objectives have been implied or the last D-frontier has changed, reset backtrace atpgStatus
	return !( initObject_.empty() || fDFrontierChanged );
}

// TODO comment not good name
int Atpg::countNumGatesInDFrontier( Gate *pFaultyLine )
{
	// update the frontier
	updateDFrontier();

	// Change the xPathStatus from X_EXIST to UNKNOWN of a gate
	// which has equal or higher level than the faulty gate
	//(the gate array has been levelized already)
	// This is to clear the xPathStatus of previous xPathTracing
	for ( int i = pFaultyLine->id_; i < cir_->tgate_; i++ )
	{
		Gate *pGate = &cir_->gates_[ i ];
		if ( xPathStatus_[ pGate->id_ ] == X_EXIST )
			xPathStatus_[ pGate->id_ ] = UNKNOWN;
	}

	// if D-frontier can't propagate to the PO, erase it
	for ( int k = dFrontier_.size() - 1; k >= 0; k-- )
	{
		if ( !xPathTracing( &cir_->gates_[ dFrontier_[ k ] ] ) )
			listDelete( dFrontier_, k );
	}

	return dFrontier_.size();
}

// TODO
bool Atpg::checkFaultPropToPO( bool &faultPropToPO )
{
	// find out is there a D or B at on any PO?(i.e. The fault is propagate to the PO)
	for ( int i = 0; i < cir_->npo_ + cir_->nppi_; i++ )
	{
		Value v = cir_->gates_[ cir_->tgate_ - i - 1 ].v_;
		if ( v == D || v == B )
		{
			faultPropToPO = true;
			return true;
		}
	}
	faultPropToPO = false;
	return false;
}

// TODO
bool Atpg::checkUnjustifiedBoundLines()
{
	// Find if there exists any unjustified bound line
	for ( int i = 0; i < (int)unjustified_.size(); i++ )
	{
		Gate *pGate = &cir_->gates_[ unjustified_[ i ] ];
		if ( pGate->v_ != X && !modify_[ pGate->id_ ] && lineType_[ pGate->id_ ] == BOUND_LINE )
		{ // unjustified bound line
			return true;
		}
	}
	return false;
}
// TODO
void Atpg::assignValueToFinalObject()
{
	while ( !finalObject_.empty() )
	{ // while exist any finalObject
		Gate *pGate = &cir_->gates_[ listPop( finalObject_ ) ];

		// judge the value by numOfZero and numOfOne
		if ( n0_[ pGate->id_ ] > n1_[ pGate->id_ ] )
			pGate->v_ = L;
		else
			pGate->v_ = H;

		// put decision of the finalObjective into decisionTree
		decisionTree_.put( pGate->id_, backtrackList_.size() );
		// record this gate and backtrace later
		backtrackList_.push_back( pGate->id_ );

		if ( lineType_[ pGate->id_ ] == HEAD_LINE )
			modify_[ pGate->id_ ] = true;
		else
			pushEvent( pGate->id_ );
		pushOutputEvents( pGate->id_ );
	}
}
// TODO
Gate *Atpg::initialize( Fault &fault, int &BackImpLevel, IMP_STATUS &implyStatus, bool isDTC )
{
	currentFault_ = fault;

	Gate *gFaultyLine = &cir_->gates_[ fault.gate_ ];
	int line = fault.line_;
	if ( line )
		gFaultyLine = &cir_->gates_[ gFaultyLine->fis_[ line - 1 ] ];
	initialList( true );
	initialNetlist( *gFaultyLine, isDTC );
	int fGate_id = fault.gate_;
	// headLineFault_.gate_ = -1; //bug report
	if ( lineType_[ gFaultyLine->id_ ] == FREE_LINE )
	{
		if ( ( fault.type_ == Fault::SA0 || fault.type_ == Fault::STR ) && gFaultyLine->v_ != L )
			gFaultyLine->v_ = D;
		if ( ( fault.type_ == Fault::SA1 || fault.type_ == Fault::STF ) && gFaultyLine->v_ != H )
			gFaultyLine->v_ = B;
		backtrackList_.push_back( gFaultyLine->id_ );

		headLineFault_ = setFreeFaultyGate( *gFaultyLine );
		currentFault_ = headLineFault_;
		BackImpLevel = 0;
		implyStatus = FORWARD;
		fGate_id = headLineFault_.gate_;
	}

	else
		BackImpLevel = setFaultyGate( fault );

	if ( BackImpLevel < 0 )
		return NULL;

	dFrontier_.push_back( fGate_id );

	int Level = uniquePathSensitize( cir_->gates_[ fGate_id ] );
	if ( Level == UNIQUE_PATH_SENSITIZE_FAIL )
		return NULL;
	if ( Level > BackImpLevel )
	{
		BackImpLevel = Level;
		implyStatus = BACKWARD;
	}
	if ( fault.type_ == Fault::STR || fault.type_ == Fault::STF )
	{
		Level = firstTimeFrameSetUp( fault );
		if ( Level < 0 )
			return NULL;
		if ( Level > BackImpLevel )
		{
			BackImpLevel = Level;
			implyStatus = BACKWARD;
		}
	}

	return &cir_->gates_[ fGate_id ];
}
void Atpg::initialList( bool initFlag )
{
	initObject_.clear();
	currentObject_.clear();
	fanoutObjective_.clear();
	headObject_.clear();
	finalObject_.clear();

	if ( initFlag )
	{
		unjustified_.clear();
		dFrontier_.clear();
		backtrackList_.clear();
		decisionTree_.clear();
		headLineFault_ = Fault(); // NE
		firstTimeFrameHeadLine_ = NULL;
	}
}
void Atpg::initialNetlist( Gate &gFaultyLine, bool isDTC )
{
	for ( int i = 0; i < cir_->tgate_; i++ )
	{
		Gate &g = cir_->gates_[ i ];

		if ( lineType_[ g.id_ ] == FREE_LINE )
			modify_[ g.id_ ] = true;
		else
			modify_[ g.id_ ] = false;

		faultReach_[ g.id_ ] = 0;
		/*
		 * I will assign value outside the patternGeneration for DTC, so
		 * I only need to initialize it for primary fault.
		 * */
		if ( isDTC == false )
		{
			g.v_ = X;
		}
		xPathStatus_[ g.id_ ] = UNKNOWN;
	}

	pushEvent( gFaultyLine.id_ );

	for ( int i = gFaultyLine.lvl_; i < cir_->tlvl_; i++ )
	{
		while ( !eventList_[ i ].empty() )
		{
			int gid = popEvent( i );
			Gate &rCurrentGate = cir_->gates_[ gid ];
			modify_[ gid ] = false;
			faultReach_[ gid ] = 1;

			for ( int j = 0; j < rCurrentGate.nfo_; j++ )
				pushEvent( rCurrentGate.fos_[ j ] );
		}
	}
}

// **************************************************************************
// Function   [ Atpg::setFaultyGate ]
// Commentor  [ WYH ]
// Synopsis   [ usage: Initial assignment of fault signal.
//                     There are two situations :
//                     1. Fault is on the input line of pFaultyGate, and
//                        pFaultyLine is the fanin gate of pFaultyGate
//                       (1) Activate the fault, and set value of pFaultyLine
//                           according to fault type.
//                       (2) According to the type of pFaultyGate, set other fanin
//                           gate of pFaultyGate to NoneControlling value of
//                           pFaultyGate, and set value of pFaultyGate.
//                       (3) Schedule all fanout gates of fanin gates of pFaultyGate,
//                           and schedule fanout gates of pFaultyGate.
//                       (4) Update BackImpLevel to be maximum level of
//                           fanin gates of pFaultyGate.
//                     2. Fault is on the ouput line of pFaultyGate, and
//                        pFaultyLine is pFaultyGate.
//                       (1) Activate the fault, and set value of pFaultyLine
//                           according to fault type.
//                       (2) Schedule fanout gates of pFaultyGate.
//                       (3) If pFaultyGate is a HEADLINE, all it's fanin gates are
//                           FREELINE, no need to set value.
//                           Else, set the value of it's fanin gates, and schedule
//                           all fanout gates of fanin gates of pFaultyGate.
//                       (4) Update BackImpLevel to be maximum level of
//                           fanin gates of pFaultyGate.
//              in:    Fault& fault
//              out:   int BackImpLevel: The BackImpLevel indicates the backward
//                     imply level return -1 when fault UNTESTABLE
//            ]
// Date       [ WYH Ver. 1.0 started 2013/08/16 ]
// **************************************************************************
int Atpg::setFaultyGate( Fault &fault )
{
	Gate *pFaultyGate;
	// pFaultyLine is the gate before the fault (fanin gate of pFaultyGate)
	Gate *pFaultyLine;
	int BackImpLevel = 0;
	bool isOutputFault = ( fault.line_ == 0 );
	// if the fault is SA0 or STR, then set faulty value to D (1/0)
	Value FaultyValue = ( fault.type_ == Fault::SA0 || fault.type_ == Fault::STR ) ? D : B;
	Value vtemp;
	int i;

	pFaultyGate = &cir_->gates_[ fault.gate_ ];
	// if the fault is input fault, the pFaultyLine is decided by fault.line_
	// else pFaultyLine is pFaultyGate
	if ( !isOutputFault )
		pFaultyLine = &cir_->gates_[ pFaultyGate->fis_[ fault.line_ - 1 ] ];
	else
		pFaultyLine = pFaultyGate;

	if ( !isOutputFault )
	{
		if ( FaultyValue == D && pFaultyLine->v_ != L )
			pFaultyLine->v_ = H;
		else if ( FaultyValue == B && pFaultyLine->v_ != H )
			pFaultyLine->v_ = L;
		else
			// The fault can not propagate to PO
			return -1;

		backtrackList_.push_back( pFaultyLine->id_ );

		if ( pFaultyGate->type_ == Gate::AND2 || pFaultyGate->type_ == Gate::AND3 || pFaultyGate->type_ == Gate::AND4 || pFaultyGate->type_ == Gate::NAND2 || pFaultyGate->type_ == Gate::NAND3 || pFaultyGate->type_ == Gate::NAND4 || pFaultyGate->type_ == Gate::NOR2 || pFaultyGate->type_ == Gate::NOR3 || pFaultyGate->type_ == Gate::NOR4 || pFaultyGate->type_ == Gate::OR2 || pFaultyGate->type_ == Gate::OR3 || pFaultyGate->type_ == Gate::OR4 )
		{
			// scan all fanin gate of pFaultyGate
			bool isFaultyGateScanned = false;
			for ( i = 0; i < pFaultyGate->nfi_; i++ )
			{
				Gate *pFaninGate = &cir_->gates_[ pFaultyGate->fis_[ i ] ];
				if ( pFaninGate != pFaultyLine )
				{
					if ( pFaninGate->v_ == X )
					{
						pFaninGate->v_ = pFaultyGate->getInputNonCtrlValue();
						backtrackList_.push_back( pFaninGate->id_ );
					}
					// If the value has already been set, it should be
					// noncontrol value, otherwise the fault can't propagate
					else if ( pFaninGate->v_ != pFaultyGate->getInputNonCtrlValue() )
						return -1;
				}
				else
				{
					if ( isFaultyGateScanned == false )
						isFaultyGateScanned = true;
					else
						return -1; // UNTESTABLE FAULT
				}
			}
			vtemp = pFaultyGate->isInverse();
			// find the pFaultyGate output value
			pFaultyGate->v_ = cXOR2( vtemp, FaultyValue );
			backtrackList_.push_back( pFaultyGate->id_ );
		}
		else if ( pFaultyGate->type_ == Gate::INV || pFaultyGate->type_ == Gate::BUF || pFaultyGate->type_ == Gate::PO || pFaultyGate->type_ == Gate::PPO )
		{
			vtemp = pFaultyGate->isInverse();
			pFaultyGate->v_ = cXOR2( vtemp, FaultyValue );
			backtrackList_.push_back( pFaultyGate->id_ );
		}

		if ( pFaultyGate->v_ != X )
		{
			// schedule all the fanout gate of pFaultyGate
			pushOutputEvents( pFaultyGate->id_ );
			modify_[ pFaultyGate->id_ ] = true;
		}

		for ( i = 0; i < pFaultyGate->nfi_; i++ )
		{
			Gate *pFaninGate = &cir_->gates_[ pFaultyGate->fis_[ i ] ];
			if ( pFaninGate->v_ != X )
			{
				// set the BackImpLevel to be maximum of fanin gate's level
				if ( BackImpLevel < pFaninGate->lvl_ )
					BackImpLevel = pFaninGate->lvl_;
				// shedule all fanout gates of the fanin gate
				pushInputEvents( pFaultyGate->id_, i );
			}
		}
	}
	else
	{ // output fault
		if ( ( FaultyValue == D && pFaultyGate->v_ == L ) || ( FaultyValue == B && pFaultyGate->v_ == H ) )
			return -1;

		pFaultyGate->v_ = FaultyValue;
		backtrackList_.push_back( pFaultyGate->id_ );
		// schedule all of fanout gate of the pFaultyGate
		pushOutputEvents( pFaultyGate->id_ );
		// backtrace stops at HEAD LINE
		if ( lineType_[ pFaultyGate->id_ ] == HEAD_LINE )
			modify_[ pFaultyGate->id_ ] = true;

		else if ( pFaultyGate->type_ == Gate::INV || pFaultyGate->type_ == Gate::BUF || pFaultyGate->type_ == Gate::PO || pFaultyGate->type_ == Gate::PPO )
		{

			Gate *pFaninGate = &cir_->gates_[ pFaultyGate->fis_[ 0 ] ];
			modify_[ pFaultyGate->id_ ] = true;

			Value Val = FaultyValue == D ? H : L;
			vtemp = pFaultyGate->isInverse();
			pFaninGate->v_ = cXOR2( vtemp, Val );
			backtrackList_.push_back( pFaninGate->id_ );
			pushInputEvents( pFaultyGate->id_, 0 );

			if ( BackImpLevel < pFaninGate->lvl_ )
				BackImpLevel = pFaninGate->lvl_;
		}
		else if ( ( FaultyValue == D && pFaultyGate->getOutputCtrlValue() == H ) || ( FaultyValue == B && pFaultyGate->getOutputCtrlValue() == L ) )
		{

			modify_[ pFaultyGate->id_ ] = true;
			// scan all fanin gate of pFaultyGate
			for ( i = 0; i < pFaultyGate->nfi_; i++ )
			{
				Gate *pFaninGate = &cir_->gates_[ pFaultyGate->fis_[ i ] ];
				if ( pFaninGate->v_ == X )
				{
					pFaninGate->v_ = pFaultyGate->getInputNonCtrlValue();
					backtrackList_.push_back( pFaninGate->id_ );
					// shedule all fanout gate of the pFaninGate
					pushInputEvents( pFaultyGate->id_, i );
					if ( BackImpLevel < pFaninGate->lvl_ )
						BackImpLevel = pFaninGate->lvl_;
				}
			}
		}
		else
		{
			pushEvent( pFaultyGate->id_ );
			if ( BackImpLevel < pFaultyGate->lvl_ )
				BackImpLevel = pFaultyGate->lvl_;
		}
	}
	return BackImpLevel;
}

// **************************************************************************
// Function   [ Atpg::firstTimeFrameSetUp ]
// Commentor  [ WYH ]
// Synopsis   [ usage: Initial assignment of fault signal, and set the first time
//                     meet HEAD LINE gate.
//                     There are two situations :
//                     1.Faulty gate is FREE LINE
//                      (1)Activate the fault, and set value of pFaultyLine
//                         according to fault type.
//                      (2)Do while loop until pFaultyLine becomes HEAD LINE
//                      (3)Schedule fanout gates of pFaultyLine.
//                     2.Fault is HEAD LINE or BOUND LINE
//                      (1)Activate the fault, and set value of pFaultyLine
//                         according to fault type.
//                      (2)Schedule fanout gates of pFaultyLine.
//                      (3) Update BackImpLevel to be maximum level of
//                          fanin gates of pFaultyLine.
//              in:    Fault& fault
//              out:   int BackImpLevel: The BackImpLevel indicates the backward
//                     imply level return -1 when fault UNTESTABLE
//            ]
// Date       [ WYH Ver. 1.0 started 2013/08/17 ]
// **************************************************************************
int Atpg::firstTimeFrameSetUp( Fault &fault )
{ // NE
	Gate *pFaultyGate;
	Gate *pFaultyLine;
	int BackImpLevel = 0;
	bool isOutputFault = ( fault.line_ == 0 );
	Value FaultyValue = ( fault.type_ == Fault::STR ) ? L : H;
	Value vtemp;
	int i;

	pFaultyGate = &cir_->gates_[ fault.gate_ - cir_->ngate_ ];

	if ( !isOutputFault )
		pFaultyLine = &cir_->gates_[ pFaultyGate->fis_[ fault.line_ - 1 ] ];
	else
		pFaultyLine = pFaultyGate;

	if ( lineType_[ pFaultyLine->id_ ] == FREE_LINE )
	{
		if ( ( FaultyValue == H && pFaultyLine->v_ == L ) || ( FaultyValue == L && pFaultyLine->v_ == H ) )
			return -1;

		pFaultyLine->v_ = FaultyValue;
		backtrackList_.push_back( pFaultyLine->id_ );
		fanoutFreeBacktrace( pFaultyLine );
		Gate *gTemp = pFaultyLine;
		Gate *gNext;
		// propagate until meet the other LINE
		do
		{
			gNext = &cir_->gates_[ gTemp->fos_[ 0 ] ];
			if ( !gNext->isUnary() && ( gNext->getOutputCtrlValue() == X || gTemp->v_ != gNext->getInputCtrlValue() ) )
				break;
			gNext->v_ = cXOR2( gNext->isInverse(), gTemp->v_ );
			gTemp = gNext;
		} while ( lineType_[ gTemp->id_ ] == FREE_LINE );

		if ( lineType_[ gTemp->id_ ] == HEAD_LINE )
		{
			modify_[ gTemp->id_ ] = true;
			backtrackList_.push_back( gTemp->id_ );
			pushOutputEvents( gTemp->id_ );
		}
		firstTimeFrameHeadLine_ = gTemp;
	}
	else
	{
		if ( ( FaultyValue == H && pFaultyLine->v_ == L ) || ( FaultyValue == L && pFaultyLine->v_ == H ) )
			return -1;

		pFaultyLine->v_ = FaultyValue;
		backtrackList_.push_back( pFaultyLine->id_ );
		pushOutputEvents( pFaultyLine->id_ );

		if ( lineType_[ pFaultyLine->id_ ] == HEAD_LINE )
			modify_[ pFaultyLine->id_ ] = true;

		else if ( pFaultyLine->type_ == Gate::INV || pFaultyLine->type_ == Gate::BUF || pFaultyLine->type_ == Gate::PO || pFaultyLine->type_ == Gate::PPO )
		{

			Gate *pFaninGate = &cir_->gates_[ pFaultyLine->fis_[ 0 ] ];
			modify_[ pFaultyLine->id_ ] = true;

			Value Val = FaultyValue == H ? H : L;
			vtemp = pFaultyLine->isInverse();
			pFaninGate->v_ = cXOR2( vtemp, Val );
			backtrackList_.push_back( pFaninGate->id_ );
			pushInputEvents( pFaultyLine->id_, 0 );
			if ( BackImpLevel < pFaninGate->lvl_ )
				BackImpLevel = pFaninGate->lvl_;
		}
		else if ( ( FaultyValue == H && pFaultyLine->getOutputCtrlValue() == H ) || ( FaultyValue == L && pFaultyLine->getOutputCtrlValue() == L ) )
		{

			modify_[ pFaultyLine->id_ ] = true;
			for ( i = 0; i < pFaultyLine->nfi_; i++ )
			{
				Gate *pFaninGate = &cir_->gates_[ pFaultyLine->fis_[ i ] ];
				// if the value has not been set, then set it to noncontrol value
				if ( pFaninGate->v_ == X )
				{
					pFaninGate->v_ = pFaultyLine->getInputNonCtrlValue();
					backtrackList_.push_back( pFaninGate->id_ );
					pushInputEvents( pFaultyLine->id_, i );
					// set the BackImpLevel to be maximum of fanin gate's level
					if ( BackImpLevel < pFaninGate->lvl_ )
						BackImpLevel = pFaninGate->lvl_;
				}
			}
		}
		else
		{
			pushEvent( pFaultyLine->id_ );
			if ( BackImpLevel < pFaultyLine->lvl_ )
				BackImpLevel = pFaultyLine->lvl_;
		}
	}
	return BackImpLevel;
}

// **************************************************************************
// Function   [ Atpg::setFreeFaultyGate ]
// Commentor  [ WYH ]
// Synopsis   [ usage: This function is called when pFaultyLine is FREELINE.
//                     That means it has only one output gate.
//                     The fault must be on the output line of pFaultyGate.
//                     This function first sets the value of pFaultyGate, and
//                     pushes output gate pFaultyGate to BacktraceList.
//                     In the while loop, sets unknown fanin gate of pFaultyGate
//                     to noncontrol value of pFaultyGate and sets the value of
//                     pFaultyGate.
//                     The loop breaks when pFaultyGate becomes a HEADLINE.
//                     When pFaultyGate is a HEADLINE, this function schedules
//                     all fanout gate of pFaultyGate, and decides the new
//                     fault type according to the value of pFaultyGate
//                     and returns the new fault.
//              in:    Gate& gate
//              out:   Fault
//            ]
// Date       [ WYH Ver. 1.0 started 2013/08/17 ]
// **************************************************************************
Fault Atpg::setFreeFaultyGate( Gate &gate )
{
	int i;
	int gid( 0 );
	Gate *pCurrentGate = NULL;
	vector<int> sStack;
	sStack.push_back( gate.fos_[ 0 ] );

	while ( !sStack.empty() )
	{
		gid = listPop( sStack );
		pCurrentGate = &cir_->gates_[ gid ];
		Value Val = pCurrentGate->getInputNonCtrlValue();
		if ( Val == X )
			Val = L;
		for ( i = 0; i < pCurrentGate->nfi_; i++ )
		{
			Gate &gFanin = cir_->gates_[ pCurrentGate->fis_[ i ] ];
			// set pCurrentGate unknown fanin gate to noncontrol value
			if ( gFanin.v_ == X )
			{
				gFanin.v_ = Val;
				backtrackList_.push_back( gFanin.id_ );
			}
		}
		if ( pCurrentGate->v_ == X )
		{
			// set the value of pCurrentGate by evaluationGood
			pCurrentGate->v_ = evaluationGood( *pCurrentGate );
			backtrackList_.push_back( pCurrentGate->id_ );
		}
		// if the pCurrentGate is FREE LINE, pCurrentGate output gate becomes a new pCurrentGate
		// until pCurrentGate becomes HEAD LINE
		if ( lineType_[ gid ] == FREE_LINE )
			sStack.push_back( pCurrentGate->fos_[ 0 ] );
	}

	modify_[ gid ] = true;

	pushOutputEvents( gid );
	// decide the new fault type according to pCurrentGate value
	return Fault( gid, pCurrentGate->v_ == D ? Fault::SA0 : Fault::SA1, 0 );
}

// **************************************************************************
// Function   [ Atpg::uniquePathSensitize ]
// Commentor  [ CLT ]
// Synopsis   [ usage: Finds the last gate(pnextgate) in the uniquepath starts from pgate,
//                     return BackImpLevel which is the max of the pnextgate's input level.
//                     BackImpLevel is -1 if no uniquepath.
//              in:    gate
//              out:   int(BackImpLevel)
//            ]
// Date       [ Ver. 1.0 started 2013/08/13 ]
// **************************************************************************
int Atpg::uniquePathSensitize( Gate &gate )
{
	int i, j;
	int BackImpLevel = -1;
	Gate *pFaultyGate = &cir_->gates_[ currentFault_.gate_ ];

	if ( pFaultyGate->id_ != gate.id_ )
	{ // if gate is not the FaultyGate
		Value NonControlVal = gate.getInputNonCtrlValue();
		if ( NonControlVal != X )
		{ // if gate has an NonControlVal (1 or 0)
			for ( i = 0; i < gate.nfi_; i++ )
			{
				Gate *pFaninGate = &cir_->gates_[ gate.fis_[ i ] ];
				if ( pFaninGate->v_ == X )
				{
					pFaninGate->v_ = NonControlVal;
					if ( BackImpLevel < pFaninGate->lvl_ ) // BackImpLevel becomes MAX of fan in level
						BackImpLevel = pFaninGate->lvl_;

					backtrackList_.push_back( pFaninGate->id_ );
					pushInputEvents( gate.id_, i );
				}
				else if ( pFaninGate->v_ == gate.getInputCtrlValue() )
					return UNIQUE_PATH_SENSITIZE_FAIL;
			}
		}
	}

	Gate *pCurrGate = &gate;
	Gate *pNextGate;

	while ( true )
	{
		vector<int> &UniquePathList = uniquePath_[ pCurrGate->id_ ];

		// If pCurrGate is PO or PPO, break.
		if ( pCurrGate->type_ == Gate::PO || pCurrGate->type_ == Gate::PPO )
			break;

		// If pCurrGate is fanout free, set pNextGate to its output gate.
		else if ( pCurrGate->nfo_ == 1 )
			pNextGate = &cir_->gates_[ pCurrGate->fos_[ 0 ] ];

		// If pCurrGate no UniquePath, break.
		else if ( UniquePathList.size() == 0 )
			break;
		else
			pNextGate = &cir_->gates_[ UniquePathList[ 0 ] ]; // set pNextGate to UniquePathList[0].

		Value NonControlVal = pNextGate->getInputNonCtrlValue();

		if ( NonControlVal != X && !pNextGate->isUnary() )
		{ // If pNextGate is Unary and its NonControlVal is not unknown.
			if ( pCurrGate->nfo_ == 1 )
			{
				// If gCurrGate(pGate) is fanout free, pNextGate is gCurrGate's output gate.
				// Go through all pNextGate's input.
				for ( i = 0; i < pNextGate->nfi_; i++ )
				{
					Gate *pFaninGate = &cir_->gates_[ pNextGate->fis_[ i ] ];

					if ( pFaninGate != pCurrGate && pNextGate->getInputCtrlValue() != X && pFaninGate->v_ == pNextGate->getInputCtrlValue() )
						return UNIQUE_PATH_SENSITIZE_FAIL;

					if ( pFaninGate != pCurrGate && pFaninGate->v_ == X )
					{
						pFaninGate->v_ = NonControlVal; // Set input gate of pNextGate to pNextGate's NonControlVal
						if ( BackImpLevel < pFaninGate->lvl_ )
							BackImpLevel = pFaninGate->lvl_;
						// BackImpLevel becomes MAX of pNextGate's fanin level
						backtrackList_.push_back( pFaninGate->id_ );
						pushInputEvents( pNextGate->id_, i );
					}
				}
			}
			else
			{
				// gCurrGate(pGate) is not fanout free, pNextGate is UniquePathList[0].
				bool DependOnCurrent;
				for ( i = 0; i < pNextGate->nfi_; i++ )
				{
					// Go through all pNextGate's input.
					Gate *pFaninGate = &cir_->gates_[ pNextGate->fis_[ i ] ];
					DependOnCurrent = false;

					for ( j = 1; j < (int)UniquePathList.size(); j++ )
					{
						if ( UniquePathList[ j ] == pFaninGate->id_ )
						{
							DependOnCurrent = true;
							break;
						}
					}

					if ( !DependOnCurrent )
					{
						if ( pFaninGate->v_ != X && pFaninGate->v_ == pNextGate->getInputCtrlValue() && pNextGate->getInputCtrlValue() != X )
							return UNIQUE_PATH_SENSITIZE_FAIL;

						if ( pFaninGate->v_ != X )
							continue;

						pFaninGate->v_ = NonControlVal; // set to NonControlVal

						if ( BackImpLevel < pFaninGate->lvl_ )
							BackImpLevel = pFaninGate->lvl_; // BackImpLevel becomes MAX of pNextGate's fanin level
						backtrackList_.push_back( pFaninGate->id_ );
						pushInputEvents( pNextGate->id_, i );
					}
				}
			}
		}
		pCurrGate = pNextGate; // move to last gate in uniquePath_
	}
	return BackImpLevel;
}

// **************************************************************************
// Function   [ Atpg::justifyFreeLines ]
// Commentor  [ CLT ]
// Synopsis   [ usage: Justify free lines before terminating this test pattern generation.
//              in:    original fault
//              out:   void
//            ]
// Date       [ Ver. 1.0 started 2013/08/13 ]
// **************************************************************************
void Atpg::justifyFreeLines( Fault &fOriginalFault )
{
	int i;
	Gate *pHeadLineFaultGate = NULL;

	int gid = headLineFault_.gate_;
	if ( gid != -1 )
		pHeadLineFaultGate = &cir_->gates_[ gid ];

	// scan each HEADLINE
	for ( i = 0; i < nHeadLine_; i++ )
	{
		Gate *pGate = &cir_->gates_[ headLines_[ i ] ];
		if ( pGate->preV_ == pGate->v_ )
		{
			continue;
		}
		if ( pHeadLineFaultGate == pGate )
		{ // if the HEADLINE scaned now(pGate) is the new faulty gate
			// If the Original Fault is on a FREE LINE
			// map it back
			restoreFault( fOriginalFault );
			continue;
		}
		// for other HEADLINE, set D or D' to H or L respectively,
		if ( pGate->v_ == D )
			pGate->v_ = H;
		else if ( pGate->v_ == B )
			pGate->v_ = L;

		if ( !( pGate->type_ == Gate::PI || pGate->type_ == Gate::PPI || pGate->v_ == X ) )
			fanoutFreeBacktrace( pGate ); // do fanoutFreeBacktrace()
	}
}

// **************************************************************************
// Function   [ Atpg::fanoutFreeBacktrace ]
// Commentor  [ CLT ]
// Synopsis   [ usage: Backtrace in Fanout Free situation
//              in:    the gate to start performing
//              out:   void
//            ]
// Date       [ Ver. 1.0 started 2013/08/13 ]
// **************************************************************************
void Atpg::fanoutFreeBacktrace( Gate *pObjectGate )
{
	int i;
	currentObject_.clear();
	currentObject_.push_back( pObjectGate->id_ );
	// clear the currentObject_ list, push pObjectGate in currentObject_

	while ( !currentObject_.empty() )
	{
		Gate *pGate = &cir_->gates_[ listPop( currentObject_ ) ];

		if ( pGate->type_ == Gate::PI || pGate->type_ == Gate::PPI || pGate == firstTimeFrameHeadLine_ )
			continue;
		Value vinv = pGate->isInverse();

		if ( pGate->type_ == Gate::XOR2 || pGate->type_ == Gate::XNOR2 )
		{
			if ( &cir_->gates_[ pGate->fis_[ 1 ] ] != firstTimeFrameHeadLine_ )
			{
				if ( cir_->gates_[ pGate->fis_[ 0 ] ].v_ == X )
					cir_->gates_[ pGate->fis_[ 0 ] ].v_ = L;
				cir_->gates_[ pGate->fis_[ 1 ] ].v_ = cXOR3( pGate->v_, pGate->isInverse(), cir_->gates_[ pGate->fis_[ 0 ] ].v_ );
			}
			else
			{
				cir_->gates_[ pGate->fis_[ 0 ] ].v_ = cXOR3( pGate->v_, pGate->isInverse(), cir_->gates_[ pGate->fis_[ 1 ] ].v_ );
			}
			currentObject_.push_back( pGate->fis_[ 0 ] );
			currentObject_.push_back( pGate->fis_[ 1 ] ); // push both input gates into currentObject_ list
		}
		else if ( pGate->type_ == Gate::XOR3 && pGate->type_ == Gate::XNOR3 )
		{
			if ( &cir_->gates_[ pGate->fis_[ 1 ] ] != firstTimeFrameHeadLine_ )
			{
				if ( cir_->gates_[ pGate->fis_[ 0 ] ].v_ == X )
					cir_->gates_[ pGate->fis_[ 0 ] ].v_ = L;
				if ( cir_->gates_[ pGate->fis_[ 2 ] ].v_ == X )
					cir_->gates_[ pGate->fis_[ 2 ] ].v_ = L;
				cir_->gates_[ pGate->fis_[ 1 ] ].v_ = cXOR3( cXOR2( pGate->v_, pGate->isInverse() ), cir_->gates_[ pGate->fis_[ 0 ] ].v_, cir_->gates_[ pGate->fis_[ 2 ] ].v_ );
			}
			else
			{
				cir_->gates_[ pGate->fis_[ 0 ] ].v_ = L;
				cir_->gates_[ pGate->fis_[ 2 ] ].v_ = cXOR3( cXOR2( pGate->v_, pGate->isInverse() ), cir_->gates_[ pGate->fis_[ 0 ] ].v_, cir_->gates_[ pGate->fis_[ 1 ] ].v_ );
			}
			currentObject_.push_back( pGate->fis_[ 0 ] );
			currentObject_.push_back( pGate->fis_[ 1 ] );
			currentObject_.push_back( pGate->fis_[ 2 ] );
		}
		// if pGate only have one input gate
		else if ( pGate->isUnary() )
		{
			if ( &cir_->gates_[ pGate->fis_[ 0 ] ] != firstTimeFrameHeadLine_ )
				cir_->gates_[ pGate->fis_[ 0 ] ].v_ = cXOR2( pGate->v_, vinv );
			currentObject_.push_back( pGate->fis_[ 0 ] ); // add input gate into currentObject_ list
		}
		else
		{
			Value Val = cXOR2( pGate->v_, vinv );
			if ( Val == pGate->getInputCtrlValue() )
			{
				Gate *pMinLevelGate = &cir_->gates_[ pGate->fiMinLvl_ ];
				if ( pMinLevelGate != firstTimeFrameHeadLine_ )
				{
					pMinLevelGate->v_ = Val;
					currentObject_.push_back( pMinLevelGate->id_ );
				}
				else
				{
					Gate *pFaninGate( NULL );
					for ( i = 0; i < pGate->nfi_; i++ )
					{
						pFaninGate = &cir_->gates_[ pGate->fis_[ i ] ];
						if ( pFaninGate != firstTimeFrameHeadLine_ )
							break;
					}
					pFaninGate->v_ = Val;
					currentObject_.push_back( pFaninGate->id_ );
				}
			}
			else
			{
				for ( i = 0; i < pGate->nfi_; i++ )
				{
					Gate *pFaninGate = &cir_->gates_[ pGate->fis_[ i ] ];
					if ( pFaninGate->v_ == X )
						pFaninGate->v_ = Val;
					currentObject_.push_back( pFaninGate->id_ );
				}
			}
		}
	}
}

// **************************************************************************
// Function   [ Atpg::restoreFault ]
// Commentor  [ CLT ]
// Synopsis   [ usage: restore fault
//              in:    Fault list
//              out:   void
//            ]
// Date       [ Ver. 1.0 started 2013/08/13 ]
// **************************************************************************
void Atpg::restoreFault( Fault &fOriginalFault )
{
	Gate *pFaultPropGate;
	int i;

	fanoutObjective_.clear();
	pFaultPropGate = &cir_->gates_[ fOriginalFault.gate_ ];
	if ( fOriginalFault.line_ == 0 )
	{
		fanoutObjective_.push_back( pFaultPropGate->id_ );
	}

	// To fix bugs of free line fault which is at gate input
	for ( i = 0; i < pFaultPropGate->nfi_; i++ )
	{
		Gate *pFaninGate = &cir_->gates_[ pFaultPropGate->fis_[ i ] ];
		if ( pFaninGate->v_ == D )
			pFaninGate->v_ = H;
		if ( pFaninGate->v_ == B )
			pFaninGate->v_ = L;
		if ( pFaninGate->v_ == L || pFaninGate->v_ == H )
			fanoutObjective_.push_back( pFaninGate->id_ );
	}

	// push original fault gate into fanoutObjective_ list

	// for each loop, scan all fanin gates of pFaultPropGate (initial to be original fault gate)
	// if fanin gates' value == 0 or 1, add it into fanoutObjective_ list iteratively
	// let pFaultPropGate's output gate (FREELINE only have one output gate) be new pFaultPropGate
	// and perform the procedure of each loop till pFaultPropGate is HEADLINE
	if ( lineType_[ pFaultPropGate->id_ ] == FREE_LINE )
	{
		while ( pFaultPropGate->nfo_ > 0 )
		{
			pFaultPropGate = &cir_->gates_[ pFaultPropGate->fos_[ 0 ] ];
			for ( i = 0; i < pFaultPropGate->nfi_; i++ )
			{
				Gate *pFaninGate = &cir_->gates_[ pFaultPropGate->fis_[ i ] ];
				if ( pFaninGate->v_ == L || pFaninGate->v_ == H )
					fanoutObjective_.push_back( pFaninGate->id_ );
			}
			if ( lineType_[ pFaultPropGate->id_ ] == HEAD_LINE )
				break;
		}
	}
	// each loop pop out an element in the fanoutObjective_ list
	while ( !fanoutObjective_.empty() )
	{
		Gate *pGate = &cir_->gates_[ listPop( fanoutObjective_ ) ];
		// if the gate's value is D set to H, D' set to L
		if ( pGate->v_ == D )
			pGate->v_ = H;
		else if ( pGate->v_ == B )
			pGate->v_ = L;
		if ( !( pGate->type_ == Gate::PI || pGate->type_ == Gate::PPI || pGate->v_ == X ) ) // if the gate's value not unknown and the gates type not PI or PPI
			fanoutFreeBacktrace( pGate );
	}
}

// **************************************************************************
// Function   [ Atpg::updateUnjustifiedLines ]
// Commentor  [ CAL ]
// Synopsis   [ usage: update unjustified_ list
//              in:    void
//              out:   void
//            ]
// Date       [ Ver. 1.0 started 2013/08/13 ]
// **************************************************************************
void Atpg::updateUnjustifiedLines()
{
	int i, gid;

	// scan all gates in unjustified_ List, if some gates were put into unjustified list but were implied afterwards by other gates, remove those gates from the unjustified list.
	// if modify_[mGate.id_]  == true, delete it from unjustified_ List
	// else push mGate into finalObject_ List
	for ( i = unjustified_.size() - 1; i >= 0; i-- )
	{
		Gate &mGate = cir_->gates_[ unjustified_[ i ] ];
		if ( modify_[ mGate.id_ ] )
			listDelete( unjustified_, i );
		else
		{
			modify_[ mGate.id_ ] = false;
			finalObject_.push_back( mGate.id_ );
		}
	}

	// pop all element from finalObject_ and set it's modify_ to false till finalObject_ is empty
	while ( !finalObject_.empty() )
	{
		gid = listPop( finalObject_ );
		modify_[ gid ] = false;
	}
}

// **************************************************************************
// Function   [ Atpg::updateDFrontier ]
// Commentor  [ CAL ]
// Synopsis   [ usage: update DFrontier
//              in:    void
//              out:   void
//            ]
// Date       [ Ver. 1.0 started 2013/08/13 ]
// **************************************************************************
void Atpg::updateDFrontier()
{
	unsigned i;
	int j;
	for ( i = 0; i < dFrontier_.size(); )
	{
		Gate &mGate = cir_->gates_[ dFrontier_[ i ] ];
		if ( mGate.v_ == D || mGate.v_ == B )
		{
			for ( j = 0; j < mGate.nfo_; j++ )
				dFrontier_.push_back( mGate.fos_[ j ] );
			listDelete( dFrontier_, i );
		}
		else if ( mGate.v_ == X )
			i++;
		else
			listDelete( dFrontier_, i );
	}
}

// **************************************************************************
// Function   [ Atpg::backtrack ]
// Commentor  [ CLT ]
// Synopsis   [ usage: If decisionTree_ is not empty, update BackImpLevel.
//                     If decisionTree_ is empty, return false.
//              in:    BackImpLevel
//              out:   bool
//            ]
// Date       [ Ver. 1.0 started 2013/08/13 ]
// **************************************************************************
//
//
// those decision gates are put in the decisionTree
// the associated implication are put in the backtrackList
//
// when we backtrack a single gate in decision tree,
// we need to recover all the associated implications
// starting from the startPoint of backtrakList
//
//                       +------+------+
//  decision tree nodes: | G0=1 | G1=0 |...
//                       +------+------+
//       startPoint of G0=1 |        \ startPoint of G1=0
//                          V         \						.
//                       +----+----++----+----+----+
//  backtracklist:       |G4=0|G7=1||G6=1|G9=0|G8=0|...
//                       +----+----++----+----+----+
//                       \         /\______ _______/
//                        \___ ___/        V
//                            V            associated implications of G1=0
//         associated implications of G0=1
//
bool Atpg::backtrack( int &BackImpLevel )
{
	Gate *pDecisionGate = NULL;
	int mDecisionGateID;
	unsigned BacktrackPoint = 0;
	Value Val;
	int i, j;
	// backtrackList_ is for backtrack

	while ( !decisionTree_.empty() )
	{
		if ( decisionTree_.get( mDecisionGateID, BacktrackPoint ) )
			continue;
		// pDecisionGate is the bottom node of decisionTree_

		updateUnjustifiedLines();
		pDecisionGate = &cir_->gates_[ mDecisionGateID ];
		Val = cINV( pDecisionGate->v_ );

		for ( i = BacktrackPoint; i < (int)backtrackList_.size(); i++ )
		{
			// Reset gates and their ouput in backtrackList_, starts from its backtrack point.
			Gate *pGate = &cir_->gates_[ backtrackList_[ i ] ];

			pGate->v_ = X;
			modify_[ pGate->id_ ] = false;

			for ( j = 0; j < pGate->nfo_; j++ )
			{
				Gate *pFanoutGate = &cir_->gates_[ pGate->fos_[ j ] ];
				modify_[ pFanoutGate->id_ ] = false;
			}
		}

		BackImpLevel = 0;

		for ( i = BacktrackPoint + 1; i < (int)backtrackList_.size(); i++ )
		{
			// Find MAX level output in backtrackList_ and save it to BackImpLevel.
			Gate *pGate = &cir_->gates_[ backtrackList_[ i ] ];

			for ( j = 0; j < pGate->nfo_; j++ )
			{
				Gate *pFanoutGate = &cir_->gates_[ pGate->fos_[ j ] ];

				if ( pFanoutGate->v_ != X )
				{
					if ( !modify_[ pFanoutGate->id_ ] )
						unjustified_.push_back( pFanoutGate->id_ );
					pushEvent( pFanoutGate->id_ );
				}

				if ( pFanoutGate->lvl_ > BackImpLevel )
					BackImpLevel = pFanoutGate->lvl_;
			}
		}

		backtrackList_.resize( BacktrackPoint + 1 );
		pDecisionGate->v_ = Val; // Reverse its value, do backtrack.

		if ( lineType_[ pDecisionGate->id_ ] == HEAD_LINE )
			modify_[ pDecisionGate->id_ ] = false;
		else
			pushEvent( pDecisionGate->id_ );

		pushOutputEvents( pDecisionGate->id_ );

		Gate *pFaultyGate = &cir_->gates_[ currentFault_.gate_ ];

		dFrontier_.clear();
		dFrontier_.push_back( pFaultyGate->id_ );
		updateDFrontier();

		for ( int k = unjustified_.size() - 1; k >= 0; k-- )
			// Update unjustified_ list.
			if ( cir_->gates_[ unjustified_[ k ] ].v_ == X )
				listDelete( unjustified_, k );

		for ( i = pFaultyGate->id_; i < cir_->tgate_; i++ )
			xPathStatus_[ i ] = UNKNOWN;
		// Reset xPathStatus.

		return true;
	}

	return false;
}

// **************************************************************************
// Function   [ Atpg::Evaluation ]
// Commentor  [ KOREAL ]
// Synopsis   [ IN:  Gate *pGate
//              OUT: IMP_STATUS (FORWARD, BACKWARD, CONFLICT)
//
//              If pGate is the faulty gate, return FaultEvaluation(pGate);
//              else check the relationships between pGate's evaluated value
//              and current value.
//
//              If they are the same, set pGate to be modefied, return
//              FORWARD,
//              else if current value is unknown, set it to the evaluated
//              value and return FORWARD,
//              else if the evaluated value is different from current value,
//              return CONFLICT,
//              else (only know current value)return
//              BackwardImplication(pGate).
//            ]
// Date       [ KOREAL Ver. 1.0 started 2013/08/15 ]
// **************************************************************************
Atpg::IMP_STATUS Atpg::evaluation( Gate *pGate )
{
	modify_[ pGate->id_ ] = false;

	if ( lineType_[ pGate->id_ ] == HEAD_LINE )
	{
		// pGate is head line, set modify and return FORWARD
		modify_[ pGate->id_ ] = true;
		return FORWARD;
	}

	// pGate is the faulty gate, see FaultEvaluation();
	if ( pGate->id_ == currentFault_.gate_ )
		return faultyGateEvaluation( pGate );

	// pGate is not the faulty gate, see evaluationGood(*pGate)
	Value Val = evaluationGood( *pGate );

	if ( Val == pGate->v_ )
	{
		if ( Val != X )
			// Good value is equal to the gate output, return FORWARD
			modify_[ pGate->id_ ] = true;
		return FORWARD;
	}
	else if ( pGate->v_ == X )
	{
		// set it to the evaluated value.
		pGate->v_ = Val;
		backtrackList_.push_back( pGate->id_ );
		modify_[ pGate->id_ ] = true;
		pushOutputEvents( pGate->id_ );
		return FORWARD;
	}
	else if ( Val != X )
		////Good value is different to the gate output, return CONFLICT
		return CONFLICT;

	return backwardImplication( pGate );
}

// **************************************************************************
// Function   [ Atpg::FaultEvaluation ]
// Commentor  [ KOREAL ]
// Synopsis   [ IN:  Gate *pGate
//              OUT: IMP_STATUS (FORWARD, BACKWARD, CONFLICT)
//
//              Check the relationships between pGate's current value and the
//              evaluated value of pGate.
//
//              If current value of pGate is unknown, set it to the
//              evaluated value, return FORWARD.
//
//              If they are the same, set pGate to be modified, return FORWARD.
//
//              If pGate has current value, if only one input has an unknown
//              value, set the input to proper value and return BACKWARD; if
//              pGate has more than one input with unknown value, push pGate
//              into unjustified_ list.
//
//              If the evaluated value is different from current value, return
//              CONFLICT.
//
//            ]
// Date       [ KOREAL Ver. 1.0 started 2013/08/15 ]
// **************************************************************************
Atpg::IMP_STATUS Atpg::faultyGateEvaluation( Gate *pGate )
{

	// get the evaluated value of pGate.
	Value Val = evaluationFaulty( *pGate );
	int ImpPtr = 0;
	int i;
	if ( Val == X )
	{ // The evaluated value is X, means the init faulty objective has not achieved yet.
		if ( pGate->v_ != X )
		{
			int NumOfX = 0;

			// Count NumOfX, the amount of fanIn whose value is X.
			// set ImpPtr to be one of the fanIn whose value is X.
			for ( i = 0; i < pGate->nfi_; i++ )
			{
				Gate *pFaninGate = &cir_->gates_[ pGate->fis_[ i ] ];
				if ( pFaninGate->v_ == X )
				{
					NumOfX++;
					ImpPtr = i;
				}
			}
			if ( NumOfX == 1 )
			{ // The fanin has X value can be set to impl. value

				// ImpVal is the pGate pImpGate value
				Value ImpVal;

				// pImpGate is the pGate's fanIn whose value is X.
				Gate *pImpGate = &cir_->gates_[ pGate->fis_[ ImpPtr ] ];

				// set ImpVal if pGate is not XOR or XNOR
				if ( pGate->v_ == D )
					ImpVal = H;
				else if ( pGate->v_ == B )
					ImpVal = L;
				else
					ImpVal = pGate->v_;

				// set ImpVal if pGate is XOR2 or XNOR2
				if ( pGate->type_ == Gate::XOR2 || pGate->type_ == Gate::XNOR2 )
				{
					Value temp = ( ImpPtr == 0 ) ? cir_->gates_[ pGate->fis_[ 1 ] ].v_ : cir_->gates_[ pGate->fis_[ 0 ] ].v_;
					ImpVal = cXOR2( ImpVal, temp );
				}
				// set ImpVal if pGate is XOR3 or XNOR3
				if ( pGate->type_ == Gate::XOR3 || pGate->type_ == Gate::XNOR3 )
				{
					Value temp;
					if ( ImpPtr == 0 )
						temp = cXOR2( cir_->gates_[ pGate->fis_[ 1 ] ].v_,
													cir_->gates_[ pGate->fis_[ 2 ] ].v_ );
					else if ( ImpPtr == 1 )
						temp = cXOR2( cir_->gates_[ pGate->fis_[ 0 ] ].v_,
													cir_->gates_[ pGate->fis_[ 2 ] ].v_ );
					else
						temp = cXOR2( cir_->gates_[ pGate->fis_[ 1 ] ].v_,
													cir_->gates_[ pGate->fis_[ 0 ] ].v_ );
					ImpVal = cXOR2( ImpVal, temp );
				}

				// if pGate is an inverse function, ImpVal = NOT(ImpVal)
				Value isInv = pGate->isInverse();
				ImpVal = cXOR2( ImpVal, isInv );

				// set modify and the final value of pImpGate
				modify_[ pGate->id_ ] = true;
				pImpGate->v_ = ImpVal;

				// backward setting
				pushInputEvents( pGate->id_, ImpPtr );
				backtrackList_.push_back( pImpGate->id_ );

				return BACKWARD;
			}
			else

				unjustified_.push_back( pGate->id_ );
		}
	}
	else if ( Val == pGate->v_ )
	{ // The init. faulty objective has already been achieved
		modify_[ pGate->id_ ] = true;
	}
	else if ( pGate->v_ == X )
	{
		// if pGate's value is unknown, set pGate's value
		modify_[ pGate->id_ ] = true;
		pGate->v_ = Val;
		// forward setting
		pushOutputEvents( pGate->id_ );
		backtrackList_.push_back( pGate->id_ );
	}
	else
		// If the evaluated value is different from current value, return CONFLICT.
		return CONFLICT;

	return FORWARD;
}

Atpg::IMP_STATUS Atpg::backwardImplication( Gate *pGate )
{
	IMP_STATUS implyStatus = FORWARD;
	int i;
	if ( pGate->type_ == Gate::PI || pGate->type_ == Gate::PPI )
		return FORWARD;

	if ( pGate->type_ == Gate::BUF || pGate->type_ == Gate::INV || pGate->type_ == Gate::PO || pGate->type_ == Gate::PPO )
	{
		Gate *pImpGate = &cir_->gates_[ pGate->fis_[ 0 ] ];
		modify_[ pGate->id_ ] = true;

		Value isINV = pGate->type_ == Gate::INV ? H : L;
		pImpGate->v_ = cXOR2( pGate->v_, isINV );

		backtrackList_.push_back( pImpGate->id_ );
		pushInputEvents( pGate->id_, 0 );
		implyStatus = BACKWARD;
	}
	else if ( pGate->type_ == Gate::XOR2 || pGate->type_ == Gate::XNOR2 )
	{
		Gate *pInputGate0 = &cir_->gates_[ pGate->fis_[ 0 ] ];
		Gate *pInputGate1 = &cir_->gates_[ pGate->fis_[ 1 ] ];

		implyStatus = BACKWARD;

		if ( pInputGate0->v_ == X && pInputGate1->v_ != X )
		{
			if ( pGate->type_ == Gate::XOR2 )
				pInputGate0->v_ = cXOR2( pGate->v_, pInputGate1->v_ );
			else
				pInputGate0->v_ = cXNOR2( pGate->v_, pInputGate1->v_ );
			modify_[ pGate->id_ ] = true;
			backtrackList_.push_back( pInputGate0->id_ );
			pushInputEvents( pGate->id_, 0 );
		}
		else if ( pInputGate1->v_ == X && pInputGate0->v_ != X )
		{
			if ( pGate->type_ == Gate::XOR2 )
				pInputGate1->v_ = cXOR2( pGate->v_, pInputGate0->v_ );
			else
				pInputGate1->v_ = cXNOR2( pGate->v_, pInputGate0->v_ );
			modify_[ pGate->id_ ] = true;
			backtrackList_.push_back( pInputGate1->id_ );
			pushInputEvents( pGate->id_, 1 );
		}
		else
		{
			implyStatus = FORWARD;
			unjustified_.push_back( pGate->id_ );
		}
	}
	else if ( pGate->type_ == Gate::XOR3 || pGate->type_ == Gate::XNOR3 )
	{
		Gate *pInputGate0 = &cir_->gates_[ pGate->fis_[ 0 ] ];
		Gate *pInputGate1 = &cir_->gates_[ pGate->fis_[ 1 ] ];
		Gate *pInputGate2 = &cir_->gates_[ pGate->fis_[ 2 ] ];
		unsigned NumOfX = 0;
		unsigned ImpPtr = 0;
		if ( pInputGate0->v_ == X )
		{
			NumOfX++;
			ImpPtr = 0;
		}
		if ( pInputGate1->v_ == X )
		{
			NumOfX++;
			ImpPtr = 1;
		}
		if ( pInputGate2->v_ == X )
		{
			NumOfX++;
			ImpPtr = 2;
		}
		if ( NumOfX == 1 )
		{
			Gate *pImpGate = &cir_->gates_[ pGate->fis_[ ImpPtr ] ];
			Value temp;
			if ( ImpPtr == 0 )
				temp = cXOR3( pGate->v_, pInputGate1->v_, pInputGate2->v_ );
			else if ( ImpPtr == 1 )
				temp = cXOR3( pGate->v_, pInputGate0->v_, pInputGate2->v_ );
			else
				temp = cXOR3( pGate->v_, pInputGate1->v_, pInputGate0->v_ );

			if ( pGate->type_ == Gate::XNOR3 )
				temp = cINV( temp );
			pImpGate->v_ = temp;
			modify_[ pGate->id_ ] = true;
			backtrackList_.push_back( pImpGate->id_ );
			pushInputEvents( pGate->id_, ImpPtr );
			implyStatus = BACKWARD;
		}
		else
		{
			unjustified_.push_back( pGate->id_ );
			implyStatus = FORWARD;
		}
	}
	else
	{

		Value OutputControlVal = pGate->getOutputCtrlValue();
		Value InputControlVal = pGate->getInputCtrlValue();

		if ( pGate->v_ == OutputControlVal )
		{
			modify_[ pGate->id_ ] = true;
			Value InputNonControlVal = pGate->getInputNonCtrlValue();

			for ( i = 0; i < pGate->nfi_; i++ )
			{
				Gate *pFaninGate = &cir_->gates_[ pGate->fis_[ i ] ];

				if ( pFaninGate->v_ == X )
				{
					pFaninGate->v_ = InputNonControlVal;
					backtrackList_.push_back( pFaninGate->id_ );
					pushInputEvents( pGate->id_, i );
				}
			}

			implyStatus = BACKWARD;
		}
		else
		{
			int NumOfX = 0;
			int ImpPtr = 0;

			for ( i = 0; i < pGate->nfi_; i++ )
			{
				Gate *pFaninGate = &cir_->gates_[ pGate->fis_[ i ] ];
				if ( pFaninGate->v_ == X )
				{
					NumOfX++;
					ImpPtr = i;
				}
			}

			if ( NumOfX == 1 )
			{
				Gate *pImpGate = &cir_->gates_[ pGate->fis_[ ImpPtr ] ];
				pImpGate->v_ = InputControlVal;
				modify_[ pGate->id_ ] = true;
				backtrackList_.push_back( pImpGate->id_ );
				pushInputEvents( pGate->id_, ImpPtr );
				implyStatus = BACKWARD;
			}
			else
			{
				unjustified_.push_back( pGate->id_ );
				implyStatus = FORWARD;
			}
		}
	}

	return implyStatus;
}

// **************************************************************************
// Function   [ Atpg::Implication ]
// Commentor  [ CLT ]
// Synopsis   [ usage: Do BACKWARD and FORWARD implication to gates in eventList_,
//                     also put gates which can't be implied into unjustified_ list.
//              in:    atpgStatus(BACKWARD or FORWARD), StartLevel
//              out:   bool
//            ]
// Date       [ Ver. 1.0 started 2013/08/13 ]
// **************************************************************************
bool Atpg::Implication( IMP_STATUS atpgStatus, int StartLevel )
{
	IMP_STATUS impRet;
	int i;

	if ( atpgStatus != BACKWARD )
		StartLevel = 0;
	do
	{
		if ( atpgStatus == BACKWARD )
		{
			// BACKWARD loop: Do evaluation() to gates in eventList_ in BACKWARD order from StartLevel.
			// If one of them returns CONFLICT, Implication() returns false.
			for ( i = StartLevel; i >= 0; i-- )
				while ( !eventList_[ i ].empty() )
				{
					Gate *pGate = &cir_->gates_[ popEvent( i ) ];
					impRet = evaluation( pGate );
					if ( impRet == CONFLICT )
						return false;
				}
		}

		atpgStatus = FORWARD;
		for ( i = 0; i < cir_->tlvl_; i++ )
		{
			// FORWARD loop: Do evaluation() to gates in eventList_ in FORWARD order till it gets to MaxLevel.
			// If one of them returns CONFLICT, Implication() returns false.
			// If one of them returns BACKWARD, set StartLevel to current level - 1, goto BACKWARD loop.
			while ( !eventList_[ i ].empty() )
			{
				Gate *pGate = &cir_->gates_[ popEvent( i ) ];
				impRet = evaluation( pGate );
				if ( impRet == CONFLICT )
					return false;
				else if ( impRet == BACKWARD )
				{
					StartLevel = i - 1;
					atpgStatus = BACKWARD;
					break;
				}
			}

			if ( atpgStatus == BACKWARD )
				break;
		}
	} while ( atpgStatus == BACKWARD );
	return true;
}

// **************************************************************************
// Function   [ Atpg::initialObjectives ]
// Commentor  [ CKY ]
// Synopsis   [ usage: initial all objects of LineNum(n0_  n1_)
//              in:    void
//              out:   void
//            ]
// Date       [ CKY Ver. 1.0 commented and finished 2013/08/17 ]
// **************************************************************************
void Atpg::initialObjectives()
{
	currentObject_ = initObject_; // vector assignment

	// go through all current Object size
	for ( unsigned i = 0; i < currentObject_.size(); i++ )
	{
		// get currentObject gate in cir_ to pGate
		Gate *pGate = &cir_->gates_[ currentObject_[ i ] ];

		// if single value of the gate is Low or D', numOfZero=1, numOfOne=0
		if ( pGate->v_ == L || pGate->v_ == B )
			setn0n1( pGate->id_, 1, 0 );

		// if single value of the gate is High or D, numOfZero=0, numOfOne=1
		else if ( pGate->v_ == H || pGate->v_ == D )
			setn0n1( pGate->id_, 0, 1 );

		// if single value of the gate is X or Z or I
		// set line number depend on gate type
		else
		{
			switch ( pGate->type_ )
			{
				case Gate::AND2:
				case Gate::AND3:
				case Gate::AND4:
				case Gate::NOR2:
				case Gate::NOR3:
				case Gate::NOR4:
				case Gate::XNOR2:
				case Gate::XNOR3:
					setn0n1( pGate->id_, 0, 1 );
					break;
				case Gate::OR2:
				case Gate::OR3:
				case Gate::OR4:
				case Gate::NAND2:
				case Gate::NAND3:
				case Gate::NAND4:
				case Gate::XOR2:
				case Gate::XOR3:
					setn0n1( pGate->id_, 1, 0 );
					break;
				default:
					break;
			}
		}
		// record reset list
		backtraceResetList_.push_back( pGate->id_ );
	}
}

// **************************************************************************
// Function   [ Atpg::findEasiestInput ]
// Commentor  [ KOREAL ]
// Synopsis   [ usage: find the EasiestInput by gate::cc0_ or gate::cc1_
//              in:    1. Target gate (Gate* pGate)
//					   2. FanOut value of target gate (Value Val)
//              out:   FanIn of a target gate which is the easiestInput
//            ]
// Date       [ CPJ Ver. 1.0 started 2013/08/10 ]
// **************************************************************************
Gate *Atpg::findEasiestInput( Gate *pGate, Value Val )
{

	Gate *pRetGate = NULL;							// declaration of the return gate pointer
	unsigned easyControlVal = INFINITE; // easiest input gate's scope(non-select yet)
	int i;

	// if the fanIn amount is 1, just return the only fanIn

	if ( pGate->type_ == Gate::PO || pGate->type_ == Gate::PPO ||
			 pGate->type_ == Gate::BUF || pGate->type_ == Gate::INV )
		return &cir_->gates_[ pGate->fis_[ 0 ] ];

	if ( Val == L )
	{
		// choose the value-undetermined faninGate which has smallest cc0_
		for ( i = 0; i < pGate->nfi_; i++ )
		{
			Gate *pFaninGate = &cir_->gates_[ pGate->fis_[ i ] ];
			if ( pFaninGate->v_ != X )
				continue;
			if ( (unsigned)pFaninGate->cc0_ < easyControlVal )
			{
				easyControlVal = pFaninGate->cc0_;
				pRetGate = pFaninGate;
			}
		}
	}
	else
	{
		// choose the value-undetermined faninGate which has smallest cc1_
		for ( i = 0; i < pGate->nfi_; i++ )
		{
			Gate *pFaninGate = &cir_->gates_[ pGate->fis_[ i ] ];
			if ( pFaninGate->v_ != X )
				continue;
			if ( (unsigned)pFaninGate->cc1_ < easyControlVal )
			{
				easyControlVal = pFaninGate->cc1_;
				pRetGate = pFaninGate;
			}
		}
	}
	return pRetGate;
}

// **************************************************************************
// Function   [ Atpg::findCloseToOutput ]
// Commentor  [ CLT ]
// Synopsis   [ usage: find the gate which close to output
//              in:    the list to search, the index of the gate
//              out:   return the gate which is closest to output
//            ]
// Date       [ Ver. 1.0 started 2013/08/13 ]
// **************************************************************************
Gate *Atpg::findCloseToOutput( vector<int> &list, int &index )
{
	Gate *pCloseGate;
	int i;

	if ( list.empty() )
		return NULL;

	pCloseGate = &cir_->gates_[ list.back() ];
	index = list.size() - 1;
	for ( i = list.size() - 2; i >= 0; i-- )
	{
		if ( cir_->gates_[ list[ i ] ].depthFromPo_ < pCloseGate->depthFromPo_ )
		{
			index = i;
			pCloseGate = &cir_->gates_[ list[ i ] ];
		}
	}

	return pCloseGate;
}

// **************************************************************************
// Function   [ Atpg::multipleBacktrace ]
// Commentor  [ CKY ]
// Synopsis   [ usage: return NO_CONTRADICTORY  or  CONTRADICTORY after backtrace
//                     see paper P.4 P.5 and Fig.8 for detail information
//              in:    BACKTRACE_STATUS
//                       it have 2 possibilities,
//                       atpgStatus == INITIAL means Multiple Backtrace from the
//                        set of initial objectives
//                       atpgStatus == FANOBJ_DETERMINE means Multiple Backtrace
//                        from the set of Fanout-Point Objectives
//              out:   BACKTRACE_RESULT
//                       return CONTRADICTORY when find a
//                        Fanout-Point Objective is not reachable from the fault
//                        line and n0, n1 of it are both nonezero;
//                        otherwise, return NO_CONTRADICTORY
//                       n0 is the number of times objective 0 is required,
//                       n1 is the number of times objective 1 is required
//            ]
// Date       [ CKY Ver. 1.0 commented and finished 2013/08/17 ]
// **************************************************************************
Atpg::BACKTRACE_RESULT Atpg::multipleBacktrace( BACKTRACE_STATUS atpgStatus, int &finalObjectiveId )
{
	// atpgStatus may be INITIAL, CHECK_AND_SELECT, CURROBJ_DETERMINE, FANOBJ_DETERMINE

	// A
	if ( atpgStatus == INITIAL )
	{
		// LET THE SET OF INITIAL OBJECTIVES BE THE SET OF CURRENT OBJECTIVES
		initialObjectives();
		atpgStatus = CHECK_AND_SELECT; // ready to get into while loop
	}

	Gate *pCurrentObj = NULL;
	unsigned n0, n1, nn1, nn0;
	int index;

	while ( true )
	{
		Gate *pEasiestInput;
		switch ( atpgStatus )
		{
			case CHECK_AND_SELECT:
				// IS THE SET OF CURRENT OBJECTIVES EMPTY?
				if ( currentObject_.empty() )
				{ // YES
					// IS THE SET OF FANOUT-POINT OBJECTIVES EMPTY?
					if ( fanoutObjective_.empty() ) // YES
						// C, NO_CONTRADICTORY
						return NO_CONTRADICTORY;
					else // NO
						atpgStatus = FANOBJ_DETERMINE;
				}
				else
				{ // NO
					// TAKE OUT A CURRENT OBJECTIVE
					pCurrentObj = &cir_->gates_[ listPop( currentObject_ ) ];
					atpgStatus = CURROBJ_DETERMINE;
				}
				break; // switch break

			case CURROBJ_DETERMINE:
				// IS OBJECTIVE LINE A HEAD LINE?
				if ( lineType_[ pCurrentObj->id_ ] == HEAD_LINE )
				{ // YES
					// ADD THE CURRENT OBJECTIVE TO THE SET OF HEAD OBJECTIVES
					headObject_.push_back( pCurrentObj->id_ );
				}
				else
				{ // NO
					// assignBacktraceValue() declared in <Gate.h>, defined in <atpg.cpp>
					// store m_NumOfZero, m_NumOfOne in n0 or n1, depend on
					// different Gate Type
					// different Gate return different value, the value used in FindEasiestInput()
					Value Val = assignBacktraceValue( n0, n1, *pCurrentObj ); // get Val,no and n1 of pCurrentObj

					// FindEasiestInput() in <atpg.cpp>,
					// return Gate* of the pCurrentObj's fanin gate that is
					// the easiest gate to control value to Val
					pEasiestInput = findEasiestInput( pCurrentObj, Val );

					// add next objectives to current objectives and
					// calculate n0, n1 by rule1~rule6
					// rule1~rule6:[Reference paper p4, p5 Fig6
					for ( int i = 0; i < pCurrentObj->nfi_; i++ )
					{
						// go through all fan in gate of pCurrentObj
						Gate *pFaninGate = &cir_->gates_[ pCurrentObj->fis_[ i ] ];

						// ignore the fanin gate that already set value
						// (not unknown)
						if ( pFaninGate->v_ != X )
							continue;

						// DOES OBJECTIVE LINE FOLLOW A FANOUT-POINT?
						if ( pFaninGate->nfo_ > 1 )
						{ // YES

							// rule 1 ~ rule 5
							if ( pFaninGate == pEasiestInput )
							{
								nn0 = n0;
								nn1 = n1;
							}
							else if ( Val == L )
							{
								nn0 = 0;
								nn1 = n1;
							}
							else if ( Val == H )
							{
								nn0 = n0;
								nn1 = 0;
							}
							else
							{
								if ( n0 > n1 )
								{
									nn0 = n0;
									nn1 = n1;
								}
								else
								{
									nn0 = n1;
									nn1 = n0;
								}
							}

							if ( nn0 > 0 || nn1 > 0 )
							{

								// first find this fanout point,  add to
								// Fanout-Point Objectives set
								if ( n0_[ pFaninGate->id_ ] == 0 && n1_[ pFaninGate->id_ ] == 0 )
									fanoutObjective_.push_back( pFaninGate->id_ );

								// new value = old value(n0_(),
								// n1_()) + this branch's value
								// (nn0, nn1)
								// rule6: fanout point's n0, n1 = sum of it's
								// branch's no, n1
								unsigned NewZero = n0_[ pFaninGate->id_ ] + nn0;
								unsigned NewOne = n1_[ pFaninGate->id_ ] + nn1;

								// ADD n0 AND n1 TO THE CORRESPONDING
								// FANOUT-POINT OBJECTIVE BY THE RULE(6)
								setn0n1( pFaninGate->id_, NewZero, NewOne );
								backtraceResetList_.push_back( pFaninGate->id_ );
							}
						}
						else
						{ // NO

							// rule 1 ~ rule 5
							if ( pFaninGate == pEasiestInput )
							{
								nn0 = n0;
								nn1 = n1;
							}
							else if ( Val == L )
							{
								nn0 = 0;
								nn1 = n1;
							}
							else if ( Val == H )
							{
								nn0 = n0;
								nn1 = 0;
							}
							else
							{
								if ( n0 > n1 )
								{
									nn0 = n0;
									nn1 = n1;
								}
								else
								{
									nn0 = n1;
									nn1 = n0;
								}
							}

							if ( nn0 > 0 || nn1 > 0 )
							{

								// add gate into Current Objective set
								// BY THE RULES(1)-(5) DETERMINE NEXT OBJECTIVES
								// AND ADD THEM TO THE SET OF CURRENT OBJECTIVES
								setn0n1( pFaninGate->id_, nn0, nn1 );
								backtraceResetList_.push_back( pFaninGate->id_ );
								currentObject_.push_back( pFaninGate->id_ );
							}
						}
					}
				}
				atpgStatus = CHECK_AND_SELECT;
				break; // switch break

			case FANOBJ_DETERMINE:
				// TAKE OUT A FANOUT-POINT OBJECTIVE p CLOSEST TO A PRIMARY OUTPUT
				pCurrentObj = findCloseToOutput( fanoutObjective_, index );

				// ListDelete() defined in <atpg.h>, delete the element
				// specified by the index from FanObject
				listDelete( fanoutObjective_, index );

				// if value of pCurrent is not X
				// ignore the Fanout-Point Objective that already set value
				// (not unknown), back to CHECK_AND_SELECT state
				if ( pCurrentObj->v_ != X )
				{
					atpgStatus = CHECK_AND_SELECT;
					break; // switch break
				}

				// if fault reach of pCurrent is equal to 1 => reachable
				// IS p REACHABLE FROM THE FAULT LINE?
				if ( faultReach_[ pCurrentObj->id_ ] == 1 )
				{ // YES
					atpgStatus = CURROBJ_DETERMINE;
					break; // switch break
				}

				// if one of numOfZero or numOfOne is equal to 0
				if ( !( n0_[ pCurrentObj->id_ ] != 0 && n1_[ pCurrentObj->id_ ] != 0 ) )
				{
					atpgStatus = CURROBJ_DETERMINE;
					break; // switch break
				}

				// if three conditions are not set up, then push back pCurrentObj to finalObject
				// then terminate Multiple Backtrace procedure
				// when "a Fanout-Point objective is not reachable from the
				// fault line" and "both no, n1 of it are nonzero "
				// return CONTRADICTORY in this situation
				// Let the Fanout-Point objective to be a Final Objective
				finalObjectiveId = pCurrentObj->id_;
				return CONTRADICTORY;

			default:
				break;
		}
	}

	// after breaking while loop
	return NO_CONTRADICTORY;
}

// **************************************************************************
// Function   [ Atpg::findFinalObjective ]
// Commentor  [ WYH ]
// Synopsis   [ usage: Determination of final objective.
//                     Choose a value and a line such that the chosen value assigned
//                     to the chosen line can meet the initial objectives.
//              in:    BACKTRACE_STATUS& flag: The flag indicates the backtrace atpgStatus.
//                     bool FaultPropPO: The FaultPropPo indicates whether the
//                     fault signal can propagate to PO or not.
//                     Gate* pLastDFrontier: The pLastDFrontier indicates the cloest
//                     DFrontier to PO
//              out:   flag(BACKTRACE_STATUS reference)
//                     pLastDFrontier(Gate pointer reference)
//            ]
// Date       [ WYH Ver. 1.0 started 2013/08/15 ]
// **************************************************************************
void Atpg::findFinalObjective( BACKTRACE_STATUS &flag, bool FaultPropPO,
															 Gate *&pLastDFrontier )
{
	BACKTRACE_RESULT result;
	Gate *pGate;
	int finalObjectiveId = -1;
	int index;

	while ( true )
	{
		// IS BACKTRACE FLAG ON?
		if ( flag == INITIAL )
		{ // YES
			// RESET BACKTRACE FLAG
			flag = FANOBJ_DETERMINE;
			// set the times of objective 0 and objective 1 of the gate to be zero
			// AND LET ALL THE SETS OF OBJECTIVES BE EMPTY
			for ( unsigned i = 0; i < backtraceResetList_.size(); i++ )
				setn0n1( backtraceResetList_[ i ], 0, 0 );
			backtraceResetList_.clear();
			initialList( false );

			// IS THERE ANY UNJUSTIFIED LINE?
			if ( !unjustified_.empty() )
			{ // YES
				// LET ALL THE UNJUSTIFIED LINES BE THE SET OF INITIAL OBJECTIVES
				initObject_ = unjustified_;

				// FAULT SIGNAL PROPAGATED TO A PRIMARY OUTPUT?
				if ( FaultPropPO ) // YES
					// Not add any gates
					pLastDFrontier = NULL;
				else
				{ // NO
					// ADD A GATE IN D-FRONTIER TO THE SET OF INITIAL OBJECTIVES
					pLastDFrontier = findCloseToOutput( dFrontier_, index );
					initObject_.push_back( pLastDFrontier->id_ );
				}
			}
			else
			{ // NO
				// ADD A GATE IN D-FRONTIER TO THE SET OF INITIAL OBJECTIVES
				pLastDFrontier = findCloseToOutput( dFrontier_, index );
				initObject_.push_back( pLastDFrontier->id_ );
			}

			// A
			// MULTIPLE BACKTRACE FROM THE SET OF INITIAL OBJECTIVES
			result = multipleBacktrace( INITIAL, finalObjectiveId );
			// CONTRADICTORY REQUIREMENT AT A FANOUT-POINT OCCURRED?
			if ( result == CONTRADICTORY )
			{ // YES
				// LET THE FANOUT-POINT OBJECTIVE BE FINAL OBJECTIVE TO ASSIGNA VALUE
				finalObject_.push_back( finalObjectiveId );
				// EXIT
				return;
			}
		}
		else
		{ // NO
			// IS THE SET OF FANOUT-POINT OBJECTIVE EMPTY?
			if ( !fanoutObjective_.empty() )
			{ // NO
				// B
				// MULTIPLE BACKTRACE FROM A FANOUT-POINT OBJECTIVE
				result = multipleBacktrace( FANOBJ_DETERMINE, finalObjectiveId );
				// CONTRADICTORY REQUIREMENT AT A FANOUT-POINT OCCURRED?
				if ( result == CONTRADICTORY )
				{ // YES
					// LET THE FANOUT-POINT OBJECTIVE BE FINAL OBJECTIVE TO ASSIGNA VALUE
					finalObject_.push_back( finalObjectiveId );
					// EXIT
					return;
				}
			}
		}

		while ( true )
		{
			// IS THE SET OF HEAD OBJECTIVES EMPTY?
			if ( headObject_.empty() )
			{ // YES
				flag = INITIAL;
				break;
			}
			else
			{ // NO
				// TAKE OUT A HEAD OBJECTIVE
				pGate = &cir_->gates_[ listPop( headObject_ ) ];
				// IS THE HEAD LINE UNSPECIFIED?
				if ( pGate->v_ == X )
				{ // YES
					// LET THE HEAD OBJECTIVE BE FINAL OBJECTIVE
					finalObject_.push_back( pGate->id_ );
					// EXIT
					return;
				}
			}
		}
	}
}

// **************************************************************************
// Function   [ Atpg::xPathTracing ]
// Commentor  [ CLT ]
// Synopsis   [ usage: recursive call the fanout of pGate to PO and check if pGate has a X path
//              in:    Gate* pGate
//              out:   bool
//            ]
// Date       [ Ver. 1.0 started 2013/08/13 ]
// **************************************************************************
bool Atpg::xPathTracing( Gate *pGate )
{
	int i;

	if ( pGate->v_ != X || xPathStatus_[ pGate->id_ ] == NO_X_EXIST )
	{
		xPathStatus_[ pGate->id_ ] = NO_X_EXIST;
		return false;
	}

	if ( xPathStatus_[ pGate->id_ ] == X_EXIST )
	{
		// xPathStatus_[pGate->id_] = X_EXIST;
		return true;
	}

	if ( pGate->type_ == Gate::PO || pGate->type_ == Gate::PPO )
	{
		xPathStatus_[ pGate->id_ ] = X_EXIST;
		return true;
	}

	for ( i = 0; i < pGate->nfo_; i++ )
	{
		// TO-DO homework 03
		int foid = pGate->fos_[ i ];
		if ( xPathTracing( cir_->gates_ + foid ) )
		{
			return true;
		}
		// end of TO-DO
	}

	xPathStatus_[ pGate->id_ ] = NO_X_EXIST;

	return false;
}

// **************************************************************************
// Function   [ Atpg::assignBacktraceValue ]
// Commentor  [ CKY ]
// Synopsis   [ usage: help to get n0 n1 and Value depend on Gate's controlling value
//              in:    n0 (unsigned reference), n1 (unsigned reference), g (Gate reference)
//              out:   Value, n0, n1
//            ]
// Date       [ CKY Ver. 1.0 commented and finished 2013/08/17 ]
// **************************************************************************
Value Atpg::assignBacktraceValue( unsigned &n0, unsigned &n1, Gate &g )
{
	int v1;
	Value val;
	switch ( g.type_ )
	{
		// when gate is AND type,n0 = numOfZero,n1 = numOfOne
		case Gate::AND2:
		case Gate::AND3:
		case Gate::AND4:
			n0 = n0_[ g.id_ ];
			n1 = n1_[ g.id_ ];
			return L;

			// when gate is OR type,n0 = numOfZero,n1 = numOfOne
		case Gate::OR2:
		case Gate::OR3:
		case Gate::OR4:
			n0 = n0_[ g.id_ ];
			n1 = n1_[ g.id_ ];
			// TO-DO homework 04
			return H;
			// end of TO-DO

			// when gate is NAND type,n0 = numOfOne,n1 = numOfZero
		case Gate::NAND2:
		case Gate::NAND3:
		case Gate::NAND4:
			n0 = n1_[ g.id_ ];
			n1 = n0_[ g.id_ ];
			return L;

			// when gate is NOR type,n0 = numOfOne,n1 = numOfZero
		case Gate::NOR2:
		case Gate::NOR3:
		case Gate::NOR4:
			n0 = n1_[ g.id_ ];
			n1 = n0_[ g.id_ ];
			// TO-DO homework 04
			return H;
			// end of TO-DO

			// when gate is inverter,n0 = numOfOne,n1 = numOfZero
		case Gate::INV:
			n0 = n1_[ g.id_ ];
			n1 = n0_[ g.id_ ];
			return X;

			// when gate is XOR2 or XNOR2
		case Gate::XOR2:
		case Gate::XNOR2:
			val = cir_->gates_[ g.fis_[ 0 ] ].v_;
			if ( val == X )
				val = cir_->gates_[ g.fis_[ 1 ] ].v_;
			if ( val == H )
			{
				n0 = n1_[ g.id_ ];
				n1 = n0_[ g.id_ ];
			}
			else
			{
				n0 = n0_[ g.id_ ];
				n1 = n1_[ g.id_ ];
			}

			if ( g.type_ == Gate::XNOR2 )
			{
				unsigned temp = n0;
				n0 = n1;
				n1 = temp;
			}
			return X;

			// when gate is XOR3 or XNOR3
		case Gate::XOR3:
		case Gate::XNOR3:
			v1 = 0;
			for ( int i = 0; i < g.nfi_; i++ )
				if ( cir_->gates_[ g.fis_[ 0 ] ].v_ == H )
					v1++;
			if ( v1 == 2 )
			{
				n0 = n1_[ g.id_ ];
				n1 = n0_[ g.id_ ];
			}
			else
			{
				n0 = n0_[ g.id_ ];
				n1 = n1_[ g.id_ ];
			}

			if ( g.type_ == Gate::XNOR3 )
			{
				unsigned temp = n0;
				n0 = n1;
				n1 = temp;
			}
			return X;
		default:
			n0 = n0_[ g.id_ ];
			n1 = n1_[ g.id_ ];
			return X;
	}
}
