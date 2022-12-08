// *************************************************************************
// File       [ atpg.h ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/11/01 created ]
// **************************************************************************

#ifndef _CORE_ATPG_H_
#define _CORE_ATPG_H_
// header file order modified by wang
#include <cstdlib>
#include <string>
#include <vector> // added by wang
#include <stack>	// added by wang
#include <algorithm>
// #include <cassert> removed by wang
#include "decision_tree.h"
#include "simulator.h"

namespace CoreNs
{

	const int MAX_LIST_SIZE = 1000;	 // unsigned => int by wang
	const int MAX_BACKTRACK = 500;	 // unsigned => int by wang
	const int INFINITE = 0x7fffffff; // unsigned => int by wang
	const int UNIQUE_PATH_SENSITIZE_FAIL = -2;
	const int NO_UNIQUE_PATH = -1; // added by wang

	class Atpg
	{
	public:
		Atpg(Circuit *pCircuit, Simulator *pSimulator); // cir => pCircuit, sim => pSmiulator by wang
		~Atpg();

		enum SINGLE_PATTERN_GENERATION_STATUS // GENERATION_STATUS => SINGLE_PATTERN_GENERATION_STATUS by wang
		{
			PATTERN_FOUND = 0, // TEST_FOUND => PATTERN_FOUND by wang
			FAULT_UNTESTABLE,	 // UNTESTABLE => FAULT_UNTESTABLE by wang
			ABORT
		};
		enum GATE_LINE_TYPE // LINE_TYPE => GATE_LINE_TYPE by wang
		{
			FREE_LINE = 0,
			HEAD_LINE,
			BOUND_LINE
		};
		enum XPATH_STATE
		{
			NO_XPATH_EXIST = 0, // NO_X_EXIST => NO_XPATH_EXIST by wang
			UNKNOWN,
			XPATH_EXIST // X_EXIST => NO_XPATH_EXIST by wang
		};
		enum IMPLICATION_STATUS // IMP_STATUS => IMPLICATION_STSTUS by wang
		{
			FORWARD = 0,
			BACKWARD,
			CONFLICT
		};
		enum BACKTRACE_STATUS
		{
			INITIAL = 0,
			CHECK_AND_SELECT,
			CURRENT_OBJ_DETERMINE, // CURROBJ_DETERMINE => CURRENT_OBJ_DETERMINE by wang
			FAN_OBJ_DETERMINE			 // FANOBJ_DETERMINE => FAN_OBJ_DETERMINE by wang
		};
		enum BACKTRACE_RESULT
		{
			NO_CONTRADICTORY = 0,
			CONTRADICTORY
		};
		// enum FIND_OBJECTIVE_STATUS { CHECK_UNJUSTIFIED = 0, BACKTRACE_CURR, BACKTRACE_FANOUT, CHECK_HEADOBJ }; removed by wang
		// enum MAIN_STATUS{ IMPLY_AND_CHECK = 0, DECISION, BACKTRACK, JUSTIFY_FREE }; removed by wang

		// class Atpg main method
		// void generation(PatternProcessor *pcoll, FaultListExtract *fListExtract); =>
		// void generatePatternSet(PatternProcessor *pPatternProcessor, FaultListExtract *pFaultListExtracted); by wang
		void generatePatternSet(PatternProcessor *pPatternProcessor, FaultListExtract *pFaultListExtracted);

		void TransitionDelayFaultATPG(FaultList &faultListToGen, PatternProcessor *pPatternProcessor, int &untest);
		void StuckAtFaultATPG(FaultList &faultListToGen, PatternProcessor *pPatternProcessor, int &untest);
		void XFill(PatternProcessor *pPatternProcessor);
		// GENERATION_STATUS patternGeneration(Fault fault, bool isDTC);
		// => SINGLE_PATTERN_GENERATION_STATUS generateSinglePatternOnTargetFault(Fault targetFault, bool isDTC) by wang
		SINGLE_PATTERN_GENERATION_STATUS generateSinglePatternOnTargetFault(Fault targetFault, bool isDTC);

		void setupCircuitParameter();
		void identifyLineParameter();
		void identifyDominator();
		void identifyUniquePath();
		void assignPatternPiValue(Pattern *pat); // write PI values to pattern
		void assignPatternPoValue(Pattern *pat); // write PO values to pattern

	protected:
		Circuit *pCircuit_;			// cir_ => pCircuit_ by wang
		Simulator *pSimulator_; // sim_ => simulator by wang

		Fault currentTargetHeadLineFault_; // Fault headLineFault_ => Fault currentTargetHeadLineFault_ by wang
		Fault currentTargetFault_;				 // Fault currentFault_; => Fault currentTargetFault_ by wang
		int backtrackLimit_;							 // unsigned => int by wang

		std::vector<bool> gateValModified_;				 // indicate whether the gate has been backtraced or implied, true means the gate has been modified.
		std::vector<int> gateID_to_n0_Vec_;				 // unsigned *n0_ => std::vector<int> gateID_to_n0_Vec_ by wang
		std::vector<int> gateID_to_n1_Vec_;				 // unsigned *n1_ => std::vector<int> gateID_to_n1_Vec_ by wang
		int *headLines_;													 // array of headlines
		int nHeadLine_;														 // number of headlines
		int *faultReach_;													 // TRUE means this fanout is in fanout cone of target fault;
		std::vector<GATE_LINE_TYPE> gateLineType_; // array of line types for all gates: FREE HEAD or BOUND
		XPATH_STATE *xPathStatus_;
		std::vector<int> *uniquePath_; // list of gates on the unique path associated with a D-forontier.  when there is only one gate in D-frontier, xPathTracing will update this information.

		std::vector<std::stack<int>> eventStack_Vec_; // std::stack<int> *eventStack_Vec_ => std::stack<int> *eventStack_Vec_
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

		int firstTimeFrameSetUp(Fault &fault); // for multiple time frame
		Gate *firstTimeFrameHeadLine_;

		bool coninuationMeaningful(Gate *pLastDFrontier);
		int countNumGatesInDFrontier(Gate *pFaultyLine);
		bool checkFaultPropToPO(bool &faultPropToPO);
		bool checkUnjustifiedBoundLines();
		void assignValueToFinalObject();
		Gate *initialize(Fault &targetFault, int &BackImpLevel, IMPLICATION_STATUS &implyStatus, bool isDTC);
		void initialList(bool initFlag);
		void initialNetlist(Gate &gFaultyLine, bool isDTC);
		Fault setFreeFaultyGate(Gate &gate);
		int setFaultyGate(Fault &fault);
		int uniquePathSensitize(Gate &gate);
		void justifyFreeLines(Fault &fOriginalFault);
		void restoreFault(Fault &fOriginalFault);
		void fanoutFreeBacktrace(Gate *pObjectGate);
		void updateUnjustifiedLines();
		void updateDFrontier();
		bool backtrack(int &BackImpLevel);
		void initialObjectives();
		void findFinalObjective(BACKTRACE_STATUS &flag,
														bool FaultPropPO, Gate *&pLastDFrontier);
		Gate *findEasiestInput(Gate *pGate, Value Val);
		Gate *findCloseToOutput(std::vector<int> &list, int &index);
		BACKTRACE_RESULT multipleBacktrace(BACKTRACE_STATUS atpgStatus, int &finalObjectiveId); // WRONG should be multiBacktrace or multipleBacktrace
		bool xPathTracing(Gate *pGate);																													// WRONG should be XPathTracing or UnknownPathTracing
		bool Implication(IMPLICATION_STATUS atpgStatus, int StartLevel);
		IMPLICATION_STATUS faultyGateEvaluation(Gate *pGate);
		IMPLICATION_STATUS evaluation(Gate *pGate);
		IMPLICATION_STATUS backwardImplication(Gate *pGate);

		Value evaluationGood(Gate &g);
		Value evaluationFaulty(Gate &g);
		Value assignBacktraceValue(unsigned &n0, unsigned &n1, Gate &g);

		void randomFill(Pattern *pat);

		// void setn0n1(int gate, int n0, int n1) => void setGaten0n1(int gateID, int n0, int n1) by wang
		void setGaten0n1(const int &gateID, const int &n0, const int &n1);

		//
		void pushEvent(const int &gateID); // push events to the event list of corresponding level
		int popEvent(int depth);
		void pushOutputEvents(int gateID);								 // WRONG NAME; T.14 ; should be called pushOutputEvents ;  push all the gate output events to event list of the corresponding level
		void pushOutputEvents(int gateID, int &gateCount); // record how many gates are pushed
		void pushInputEvents(int gateID, int index);
		int listPop(std::vector<int> &list);
		void listDelete(std::vector<int> &list, int index);
		void clearAllEvent();

		// 5-Value logic evaluation functions
		Value cINV(Value i1);
		Value cAND2(Value i1, Value i2);
		Value cAND3(Value i1, Value i2, Value i3);
		Value cAND4(Value i1, Value i2, Value i3, Value i4);
		Value cNAND2(Value i1, Value i2);
		Value cNAND3(Value i1, Value i2, Value i3);
		Value cNAND4(Value i1, Value i2, Value i3, Value i4);
		Value cOR2(Value i1, Value i2);
		Value cOR3(Value i1, Value i2, Value i3);
		Value cOR4(Value i1, Value i2, Value i3, Value i4);
		Value cNOR2(Value i1, Value i2);
		Value cNOR3(Value i1, Value i2, Value i3);
		Value cNOR4(Value i1, Value i2, Value i3, Value i4);
		Value cXOR2(Value i1, Value i2);
		Value cXOR3(Value i1, Value i2, Value i3);
		Value cXNOR2(Value i1, Value i2);
		Value cXNOR3(Value i1, Value i2, Value i3);

		// added by Wei-Shen Wang
		void calSCOAP();

		/* Added by Shi-Tang Liu */
		std::string getValStr(Value val);
		void clearOneGateFaultEffect(Gate &g);
		void clearAllFaultEffectBySimulation();
		void testClearFaultEffect(FaultList &faultListToTest);
		int storeCurrentGateValue();
		void calDepthFromPo();
		void resetPreValue();
		bool isExistXPath(Gate *pGate);
		std::vector<bool> isInEventList_;
		void clearEventList_(bool isDebug);
		void resetIsInEventList();
		void setValueAndRunImp(Gate &g, Value val);
		void checkLevelInfo();
		void StuckAtFaultATPGWithDTC(FaultList &faultListToGen, PatternProcessor *pPatternProcessor, int &untest);
		Gate *getWireForActivation(Fault &fault);
		void reverseFaultSimulation(PatternProcessor *pPatternProcessor, FaultList &originalFaultList);
	};

	inline Atpg::Atpg(Circuit *pCircuit, Simulator *pSimulator)
			: gateID_to_n0_Vec_(pCircuit->tgate_, 0),
				gateID_to_n1_Vec_(pCircuit->tgate_, 0),
				eventStack_Vec_(pCircuit->lvl_),
				gateValModified_(pCircuit->tgate_, false),
				gateLineType_(pCircuit_->tgate_)
	{
		pCircuit_ = pCircuit;
		pSimulator_ = pSimulator;

		// gateID_to_n0_Vec_.resize(pCircuit->tgate_); removed by wang
		// gateID_to_n1_Vec_.resize(pCircuit->tgate_); removed by wang
		// gateLineType_ = new GATE_LINE_TYPE[pCircuit->tgate_];
		xPathStatus_ = new XPATH_STATE[pCircuit->tgate_];
		faultReach_ = new int[pCircuit->tgate_];
		uniquePath_ = new std::vector<int>[pCircuit->tgate_];
		// gateValModified_ = new bool[pCircuit->tgate_];
		// eventStack_Vec_ = new std::stack<int>[pCircuit->tlvl_]; removed by wang
		headLines_ = NULL;
		firstTimeFrameHeadLine_ = NULL;

		backtrackList_.reserve(pCircuit->tgate_);
		backtrackLimit_ = MAX_BACKTRACK;

		dFrontier_.reserve(MAX_LIST_SIZE);
		fanoutObjective_.reserve(MAX_LIST_SIZE);
		initObject_.reserve(MAX_LIST_SIZE);
		headObject_.reserve(MAX_LIST_SIZE);
		finalObject_.reserve(MAX_LIST_SIZE);
		unjustified_.reserve(MAX_LIST_SIZE);
		currentObject_.reserve(MAX_LIST_SIZE);
		isInEventList_.resize(pCircuit->tgate_);
		std::fill(isInEventList_.begin(), isInEventList_.end(), false);
	}
	inline Atpg::~Atpg()
	{
		// delete[] eventStack_Vec_; removed by wang
		delete[] headLines_;
		// delete[] gateID_to_n0_Vec_; removed by wang
		// delete[] gateID_to_n1_Vec_; removed by wang
		// delete[] gateLineType_;
		delete[] xPathStatus_;
		delete[] faultReach_;
		delete[] uniquePath_;
		// delete[] gateValModified_;
	}

	inline void Atpg::setGaten0n1(const int &gateID, const int &n0, const int &n1)
	{
		gateID_to_n0_Vec_[gateID] = n0;
		gateID_to_n1_Vec_[gateID] = n1;
	}
	inline void Atpg::pushEvent(const int &gateID)
	{
		Gate &gate = pCircuit_->gates_[gateID];
		if (!gateValModified_[gateID])
		{
			eventStack_Vec_[gate.lvl_].push(gateID);
			gateValModified_[gateID] = true;
		}
	}
	inline int Atpg::popEvent(int depth)
	{
		int gateID = eventStack_Vec_[depth].top();
		eventStack_Vec_[depth].pop();
		return gateID;
	}
	inline int Atpg::listPop(std::vector<int> &list)
	{
		int num = list.back();
		list.pop_back();
		return num;
	}
	inline void Atpg::pushOutputEvents(int gateID)
	{
		Gate &g = pCircuit_->gates_[gateID];
		for (int i = 0; i < g.nfo_; i++)
			pushEvent(g.fos_[i]);
	}
	inline void Atpg::pushOutputEvents(int gateID, int &gateCount)
	{
		Gate &g = pCircuit_->gates_[gateID];
		for (int i = 0; i < g.nfo_; i++)
		{
			Gate &gOut = pCircuit_->gates_[g.fos_[i]];
			if (!gateValModified_[g.fos_[i]])
			{
				eventStack_Vec_[gOut.lvl_].push(g.fos_[i]);
				gateValModified_[g.fos_[i]] = true;
				gateCount++;
			}
		}
	}
	inline void Atpg::pushInputEvents(int gateID, int index)
	{
		Gate &g = pCircuit_->gates_[gateID];
		pushEvent(g.fis_[index]);
		pushOutputEvents(g.fis_[index]);
	}
	inline void Atpg::listDelete(std::vector<int> &list, int index)
	{
		list[index] = list.back();
		list.pop_back();
	}
	inline void Atpg::clearAllEvent()
	{
		int gateID;
		for (int i = 0; i < pCircuit_->tlvl_; i++)
			while (!eventStack_Vec_[i].empty())
			{
				gateID = popEvent(i);
				gateValModified_[gateID] = false;
			}
	}
	//}}}
	//{{{ 5-value logic evaluation functions
	inline Value Atpg::cINV(Value i1)
	{
		const static Value map[5] = {H, L, X, B, D};
		if (i1 >= Z)
			return Z;
		return map[i1];
	}
	inline Value Atpg::cAND2(Value i1, Value i2)
	{
		const static Value map[5][5] = {
				{L, L, L, L, L},
				{L, H, X, D, B},
				{L, X, X, X, X},
				{L, D, X, D, L},
				{L, B, X, L, B}};
		if (i1 >= Z || i2 >= Z)
			return Z;
		return map[i1][i2];
	}
	inline Value Atpg::cAND3(Value i1, Value i2, Value i3)
	{
		return cAND2(i1, cAND2(i2, i3));
	}
	inline Value Atpg::cAND4(Value i1, Value i2, Value i3, Value i4)
	{
		return cAND2(cAND2(i1, i2), cAND2(i3, i4));
	}
	inline Value Atpg::cNAND2(Value i1, Value i2)
	{
		return cINV(cAND2(i1, i2));
	}
	inline Value Atpg::cNAND3(Value i1, Value i2, Value i3)
	{
		return cINV(cAND3(i1, i2, i3));
	}
	inline Value Atpg::cNAND4(Value i1, Value i2, Value i3, Value i4)
	{
		return cINV(cAND4(i1, i2, i3, i4));
	}
	inline Value Atpg::cOR2(Value i1, Value i2)
	{
		const static Value map[5][5] = {
				{L, H, X, D, B},
				{H, H, H, H, H},
				{X, H, X, X, X},
				{D, H, X, D, H},
				{B, H, X, H, B}};
		if (i1 >= Z || i2 >= Z)
			return Z;
		return map[i1][i2];
	}
	inline Value Atpg::cOR3(Value i1, Value i2, Value i3)
	{
		return cOR2(i1, cOR2(i2, i3));
	}
	inline Value Atpg::cOR4(Value i1, Value i2, Value i3, Value i4)
	{
		return cOR2(cOR2(i1, i2), cOR2(i3, i4));
	}
	inline Value Atpg::cNOR2(Value i1, Value i2)
	{
		return cINV(cOR2(i1, i2));
	}
	inline Value Atpg::cNOR3(Value i1, Value i2, Value i3)
	{
		return cINV(cOR3(i1, i2, i3));
	}
	inline Value Atpg::cNOR4(Value i1, Value i2, Value i3, Value i4)
	{
		return cINV(cOR4(i1, i2, i3, i4));
	}
	inline Value Atpg::cXOR2(Value i1, Value i2)
	{
		const static Value map[5][5] = {
				{L, H, X, D, B},
				{H, L, X, B, D},
				{X, X, X, X, X},
				{D, B, X, L, H},
				{B, D, X, H, L}};
		if (i1 >= Z || i2 >= Z)
			return Z;
		return map[i1][i2];
	}
	inline Value Atpg::cXOR3(Value i1, Value i2, Value i3)
	{
		return cXOR2(i1, cXOR2(i2, i3));
	}
	inline Value Atpg::cXNOR2(Value i1, Value i2)
	{
		return cINV(cXOR2(i1, i2));
	}
	inline Value Atpg::cXNOR3(Value i1, Value i2, Value i3)
	{
		return cINV(cXOR3(i1, i2, i3));
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
	inline Value Atpg::evaluationGood(Gate &g)
	{
		if (g.type_ == Gate::PI || g.type_ == Gate::PPI)
			return g.v_;
		Value v[4];
		for (int i = 0; i < g.nfi_; i++)
			v[i] = pCircuit_->gates_[g.fis_[i]].v_;

		switch (g.type_)
		{
			case Gate::PO:
			case Gate::PPO:
			case Gate::BUF:
				return v[0];
			case Gate::INV:
				return cINV(v[0]);
			case Gate::AND2:
				return cAND2(v[0], v[1]);
			case Gate::AND3:
				return cAND3(v[0], v[1], v[2]);
			case Gate::AND4:
				return cAND4(v[0], v[1], v[2], v[3]);

			case Gate::NAND2:
				return cNAND2(v[0], v[1]);
			case Gate::NAND3:
				return cNAND3(v[0], v[1], v[2]);
			case Gate::NAND4:
				return cNAND4(v[0], v[1], v[2], v[3]);

			case Gate::OR2:
				return cOR2(v[0], v[1]);
			case Gate::OR3:
				return cOR3(v[0], v[1], v[2]);
			case Gate::OR4:
				return cOR4(v[0], v[1], v[2], v[3]);

			case Gate::NOR2:
				return cNOR2(v[0], v[1]);
			case Gate::NOR3:
				return cNOR3(v[0], v[1], v[2]);
			case Gate::NOR4:
				return cNOR4(v[0], v[1], v[2], v[3]);

			case Gate::XOR2:
				return cXOR2(v[0], v[1]);
			case Gate::XOR3:
				return cXOR3(v[0], v[1], v[2]);

			case Gate::XNOR2:
				return cXNOR2(v[0], v[1]);
			case Gate::XNOR3:
				return cXNOR3(v[0], v[1], v[2]);
			default:
				return v[0];
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
	inline Value Atpg::evaluationFaulty(Gate &g)
	{
		Value val;
		int i, FaultyLine;
		switch (g.type_)
		{
			case Gate::PI:
			case Gate::PPI:
				return g.v_;
			case Gate::PO:
			case Gate::PPO:
			case Gate::BUF:
				val = pCircuit_->gates_[g.fis_[0]].v_;
				if (val == L && (currentTargetFault_.type_ == Fault::SA1 || currentTargetFault_.type_ == Fault::STF))
					val = B; // logic D' (0/1)
				if (val == H && (currentTargetFault_.type_ == Fault::SA0 || currentTargetFault_.type_ == Fault::STR))
					val = D; // logic D  (1/0)
				return val;
			case Gate::INV:
				val = pCircuit_->gates_[g.fis_[0]].v_;
				if (currentTargetFault_.line_ == 0)
				{
					val = cINV(val);
					if (val == L && (currentTargetFault_.type_ == Fault::SA1 || currentTargetFault_.type_ == Fault::STF))
						val = B;
					if (val == H && (currentTargetFault_.type_ == Fault::SA0 || currentTargetFault_.type_ == Fault::STR))
						val = D;
					return val;
				}
				else
				{
					if (val == L && (currentTargetFault_.type_ == Fault::SA1 || currentTargetFault_.type_ == Fault::STF))
						val = B;
					if (val == H && (currentTargetFault_.type_ == Fault::SA0 || currentTargetFault_.type_ == Fault::STR))
						val = D;
					return cINV(val);
				}
			case Gate::AND2:
			case Gate::AND3:
			case Gate::AND4:
				FaultyLine = currentTargetFault_.line_;
				if (currentTargetFault_.line_ == 0)
				{
					val = pCircuit_->gates_[g.fis_[0]].v_;
					for (i = 1; i < g.nfi_; i++)
						val = cAND2(val, pCircuit_->gates_[g.fis_[i]].v_);
					if (val == L && (currentTargetFault_.type_ == Fault::SA1 || currentTargetFault_.type_ == Fault::STF))
						val = B;
					if (val == H && (currentTargetFault_.type_ == Fault::SA0 || currentTargetFault_.type_ == Fault::STR))
						val = D;
				}
				else
				{
					val = pCircuit_->gates_[g.fis_[FaultyLine - 1]].v_;
					if (val == L && (currentTargetFault_.type_ == Fault::SA1 || currentTargetFault_.type_ == Fault::STF))
						val = B;
					if (val == H && (currentTargetFault_.type_ == Fault::SA0 || currentTargetFault_.type_ == Fault::STR))
						val = D;
					for (i = 0; i < g.nfi_; i++)
						if (i != FaultyLine - 1)
							val = cAND2(val, pCircuit_->gates_[g.fis_[i]].v_);
				}
				return val;
			case Gate::NAND2:
			case Gate::NAND3:
			case Gate::NAND4:
				FaultyLine = currentTargetFault_.line_;
				if (currentTargetFault_.line_ == 0)
				{
					val = pCircuit_->gates_[g.fis_[0]].v_;
					for (i = 1; i < g.nfi_; i++)
						val = cAND2(val, pCircuit_->gates_[g.fis_[i]].v_);

					val = cINV(val);

					if (val == L && (currentTargetFault_.type_ == Fault::SA1 || currentTargetFault_.type_ == Fault::STF))
						val = B;
					if (val == H && (currentTargetFault_.type_ == Fault::SA0 || currentTargetFault_.type_ == Fault::STR))
						val = D;
				}
				else
				{
					val = pCircuit_->gates_[g.fis_[FaultyLine - 1]].v_;
					if (val == L && (currentTargetFault_.type_ == Fault::SA1 || currentTargetFault_.type_ == Fault::STF))
						val = B;
					if (val == H && (currentTargetFault_.type_ == Fault::SA0 || currentTargetFault_.type_ == Fault::STR))
						val = D;
					for (i = 0; i < g.nfi_; i++)
						if (i != FaultyLine - 1)
							val = cAND2(val, pCircuit_->gates_[g.fis_[i]].v_);
					val = cINV(val);
				}
				return val;
			case Gate::OR2:
			case Gate::OR3:
			case Gate::OR4:
				FaultyLine = currentTargetFault_.line_;
				if (currentTargetFault_.line_ == 0)
				{
					val = pCircuit_->gates_[g.fis_[0]].v_;
					for (i = 1; i < g.nfi_; i++)
						val = cOR2(val, pCircuit_->gates_[g.fis_[i]].v_);
					if (val == L && (currentTargetFault_.type_ == Fault::SA1 || currentTargetFault_.type_ == Fault::STF))
						val = B;
					if (val == H && (currentTargetFault_.type_ == Fault::SA0 || currentTargetFault_.type_ == Fault::STR))
						val = D;
				}
				else
				{
					val = pCircuit_->gates_[g.fis_[FaultyLine - 1]].v_;
					if (val == L && (currentTargetFault_.type_ == Fault::SA1 || currentTargetFault_.type_ == Fault::STF))
						val = B;
					if (val == H && (currentTargetFault_.type_ == Fault::SA0 || currentTargetFault_.type_ == Fault::STR))
						val = D;
					for (i = 0; i < g.nfi_; i++)
						if (i != FaultyLine - 1)
							val = cOR2(val, pCircuit_->gates_[g.fis_[i]].v_);
				}
				return val;
			case Gate::NOR2:
			case Gate::NOR3:
			case Gate::NOR4:
				FaultyLine = currentTargetFault_.line_;
				if (currentTargetFault_.line_ == 0)
				{
					val = pCircuit_->gates_[g.fis_[0]].v_;
					for (i = 1; i < g.nfi_; i++)
						val = cOR2(val, pCircuit_->gates_[g.fis_[i]].v_);

					val = cINV(val);

					if (val == L && (currentTargetFault_.type_ == Fault::SA1 || currentTargetFault_.type_ == Fault::STF))
						val = B;
					if (val == H && (currentTargetFault_.type_ == Fault::SA0 || currentTargetFault_.type_ == Fault::STR))
						val = D;
				}
				else
				{
					val = pCircuit_->gates_[g.fis_[FaultyLine - 1]].v_;
					if (val == L && (currentTargetFault_.type_ == Fault::SA1 || currentTargetFault_.type_ == Fault::STF))
						val = B;
					if (val == H && (currentTargetFault_.type_ == Fault::SA0 || currentTargetFault_.type_ == Fault::STR))
						val = D;
					for (i = 0; i < g.nfi_; i++)
						if (i != FaultyLine - 1)
							val = cOR2(val, pCircuit_->gates_[g.fis_[i]].v_);
					val = cINV(val);
				}
				return val;
			case Gate::XOR2:
			case Gate::XOR3:
				FaultyLine = currentTargetFault_.line_;
				if (currentTargetFault_.line_ == 0)
				{

					if (g.type_ == Gate::XOR2)
						val = cXOR2(pCircuit_->gates_[g.fis_[0]].v_, pCircuit_->gates_[g.fis_[1]].v_);
					else
						val = cXOR3(pCircuit_->gates_[g.fis_[0]].v_, pCircuit_->gates_[g.fis_[1]].v_, pCircuit_->gates_[g.fis_[2]].v_);

					if (val == L && (currentTargetFault_.type_ == Fault::SA1 || currentTargetFault_.type_ == Fault::STF))
						val = B;
					if (val == H && (currentTargetFault_.type_ == Fault::SA0 || currentTargetFault_.type_ == Fault::STR))
						val = D;
				}
				else
				{
					val = pCircuit_->gates_[g.fis_[FaultyLine - 1]].v_;
					if (val == L && (currentTargetFault_.type_ == Fault::SA1 || currentTargetFault_.type_ == Fault::STF))
						val = B;
					if (val == H && (currentTargetFault_.type_ == Fault::SA0 || currentTargetFault_.type_ == Fault::STR))
						val = D;

					if (g.type_ == Gate::XOR2)
					{
						if (FaultyLine - 1 == 0)
							val = cXOR2(pCircuit_->gates_[g.fis_[1]].v_, val);
						else
							val = cXOR2(pCircuit_->gates_[g.fis_[0]].v_, val);
					}
					else
					{ // XOR3
						if (FaultyLine - 1 == 0)
							val = cXOR3(pCircuit_->gates_[g.fis_[1]].v_, pCircuit_->gates_[g.fis_[2]].v_, val);
						else if (FaultyLine - 1 == 1)
							val = cXOR3(pCircuit_->gates_[g.fis_[0]].v_, pCircuit_->gates_[g.fis_[2]].v_, val);
						else
							val = cXOR3(pCircuit_->gates_[g.fis_[1]].v_, pCircuit_->gates_[g.fis_[0]].v_, val);
					}
				}
				return val;
			case Gate::XNOR2:
			case Gate::XNOR3:
				FaultyLine = currentTargetFault_.line_;
				if (currentTargetFault_.line_ == 0)
				{

					if (g.type_ == Gate::XNOR2)
						val = cXNOR2(pCircuit_->gates_[g.fis_[0]].v_, pCircuit_->gates_[g.fis_[1]].v_);
					else
						val = cXNOR3(pCircuit_->gates_[g.fis_[0]].v_, pCircuit_->gates_[g.fis_[1]].v_, pCircuit_->gates_[g.fis_[2]].v_);

					if (val == L && (currentTargetFault_.type_ == Fault::SA1 || currentTargetFault_.type_ == Fault::STF))
						val = B;
					if (val == H && (currentTargetFault_.type_ == Fault::SA0 || currentTargetFault_.type_ == Fault::STR))
						val = D;
				}
				else
				{
					val = pCircuit_->gates_[g.fis_[FaultyLine - 1]].v_;
					if (val == L && (currentTargetFault_.type_ == Fault::SA1 || currentTargetFault_.type_ == Fault::STF))
						val = B;
					if (val == H && (currentTargetFault_.type_ == Fault::SA0 || currentTargetFault_.type_ == Fault::STR))
						val = D;

					if (g.type_ == Gate::XNOR2)
					{
						if (FaultyLine - 1 == 0)
							val = cXNOR2(pCircuit_->gates_[g.fis_[1]].v_, val);
						else
							val = cXNOR2(pCircuit_->gates_[g.fis_[0]].v_, val);
					}
					else
					{ // XOR3
						if (FaultyLine - 1 == 0)
							val = cXNOR3(pCircuit_->gates_[g.fis_[1]].v_, pCircuit_->gates_[g.fis_[2]].v_, val);
						else if (FaultyLine - 1 == 1)
							val = cXNOR3(pCircuit_->gates_[g.fis_[0]].v_, pCircuit_->gates_[g.fis_[2]].v_, val);
						else
							val = cXNOR3(pCircuit_->gates_[g.fis_[1]].v_, pCircuit_->gates_[g.fis_[0]].v_, val);
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
	inline void Atpg::assignPatternPiValue(Pattern *pat)
	{
		for (int i = 0; i < pCircuit_->npi_; i++)
			pat->pi1_[i] = pCircuit_->gates_[i].v_;
		if (pat->pi2_ != NULL && pCircuit_->nframe_ > 1)
			for (int i = 0; i < pCircuit_->npi_; i++)
				pat->pi2_[i] = pCircuit_->gates_[i + pCircuit_->ngate_].v_;
		for (int i = 0; i < pCircuit_->nppi_; i++)
			pat->ppi_[i] = pCircuit_->gates_[pCircuit_->npi_ + i].v_;
		if (pat->si_ != NULL && pCircuit_->nframe_ > 1)
			pat->si_[0] = pCircuit_->connType_ == Circuit::SHIFT ? pCircuit_->gates_[pCircuit_->ngate_ + pCircuit_->npi_].v_ : X;
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
	inline void Atpg::assignPatternPoValue(Pattern *pat)
	{
		pSimulator_->goodSim();
		int offset = pCircuit_->ngate_ - pCircuit_->npo_ - pCircuit_->nppi_;
		for (int i = 0; i < pCircuit_->npo_; i++)
		{
			if (pCircuit_->gates_[offset + i].gl_ == PARA_H)
				pat->po1_[i] = L;
			else if (pCircuit_->gates_[offset + i].gh_ == PARA_H)
				pat->po1_[i] = H;
			else
				pat->po1_[i] = X;
		}
		if (pat->po2_ != NULL && pCircuit_->nframe_ > 1)
			for (int i = 0; i < pCircuit_->npo_; i++)
			{
				if (pCircuit_->gates_[offset + i + pCircuit_->ngate_].gl_ == PARA_H)
					pat->po2_[i] = L;
				else if (pCircuit_->gates_[offset + i + pCircuit_->ngate_].gh_ == PARA_H)
					pat->po2_[i] = H;
				else
					pat->po2_[i] = X;
			}

		offset = pCircuit_->ngate_ - pCircuit_->nppi_;
		if (pCircuit_->nframe_ > 1)
			offset += pCircuit_->ngate_;
		for (int i = 0; i < pCircuit_->nppi_; i++)
		{
			if (pCircuit_->gates_[offset + i].gl_ == PARA_H)
				pat->ppo_[i] = L;
			else if (pCircuit_->gates_[offset + i].gh_ == PARA_H)
				pat->ppo_[i] = H;
			else
				pat->ppo_[i] = X;
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
	inline void Atpg::randomFill(Pattern *pat)
	{
		srand(0);
		for (int i = 0; i < pCircuit_->npi_; i++)
			if (pat->pi1_[i] == X)
				pat->pi1_[i] = rand() % 2;
		for (int i = 0; i < pCircuit_->nppi_; i++)
			if (pat->ppi_[i] == X)
				pat->ppi_[i] = rand() % 2;
		if (pat->pi2_ != NULL)
			for (int i = 0; i < pCircuit_->npi_; i++)
				if (pat->pi2_[i] == X)
					pat->pi2_[i] = rand() % 2;
		if (pat->si_ != NULL)
			if (pat->si_[0] == X)
				pat->si_[0] = rand() % 2;
	}

};

#endif
