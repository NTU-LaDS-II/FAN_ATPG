// *************************************************************************
// File       [ atpg.h ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/11/01 created ]
// **************************************************************************

#ifndef _CORE_ATPG_H_
#define _CORE_ATPG_H_

#include <cmath>
#include <cstdlib>
#include <string>
#include <vector> // added by wang
#include <stack>	// added by wang
#include <algorithm>
#include "decision_tree.h"
#include "simulator.h"

namespace CoreNs
{
	constexpr int BACKTRACK_LIMIT = 500;
	constexpr int INFINITE = 0x7fffffff;
	constexpr int MAX_LIST_SIZE = 1000;
	constexpr int NO_UNIQUE_PATH = -1;
	constexpr int UNIQUE_PATH_SENSITIZE_FAIL = -2;

	class Atpg
	{
	public:
		inline Atpg(Circuit *pCircuit, Simulator *pSimulator);

		enum SINGLE_PATTERN_GENERATION_STATUS 
		{
			PATTERN_FOUND = 0,
			FAULT_UNTESTABLE, 
			ABORT
		};
		enum GATE_LINE_TYPE
		{
			FREE_LINE = 0,
			HEAD_LINE,
			BOUND_LINE
		};
		enum XPATH_STATE
		{
			NO_XPATH_EXIST = 0,
			UNKNOWN,
			XPATH_EXIST
		};
		enum IMPLICATION_STATUS
		{
			FORWARD = 0,
			BACKWARD,
			CONFLICT
		};
		enum BACKTRACE_STATUS
		{
			INITIAL = 0,
			CHECK_AND_SELECT,
			CURRENT_OBJ_DETERMINE,
			FAN_OBJ_DETERMINE
		};
		enum BACKTRACE_RESULT
		{
			NO_CONTRADICTORY = 0,
			CONTRADICTORY
		};

		// class Atpg main method
		void generatePatternSet(PatternProcessor *pPatternProcessor, FaultListExtract *pFaultListExtracter);

	private:
		int numberOfHeadLine_;																		// number of headlines
		Circuit *pCircuit_;																				// cir_ => pCircuit_ by wang
		Simulator *pSimulator_;																		// sim_ => simulator by wang
		Fault currentTargetHeadLineFault_;												// Fault headLineFault_ => Fault currentTargetHeadLineFault_ by wang
		Fault currentTargetFault_;																// Fault currentFault_; => Fault currentTargetFault_ by wang
		std::vector<int> gateID_to_n0_;														// unsigned *n0_ => std::vector<int> gateID_to_n0_ by wang
		std::vector<int> gateID_to_n1_;														// unsigned *n1_ => std::vector<int> gateID_to_n1_ by wang
		std::vector<int> gateID_to_valModified_;									// indicate whether the gate has been backtraced or implied, true means the gate has been modified, new => vec by wang
		std::vector<int> headLineGateIDs_;												// array of headlines
		std::vector<int> gateID_to_reachableByTargetFault_;				// TRUE means this fanout is in fanout cone of target fault;
		std::vector<GATE_LINE_TYPE> gateID_to_lineType_;					// array of line types for all gates: FREE HEAD or BOUND
		std::vector<XPATH_STATE> gateID_to_xPathStatus_;					// changed by wang
		std::vector<std::vector<int>> gateID_to_uniquePath_;			// list of gates on the unique path associated with a D-forontier.  when there is only one gate in D-frontier, xPathTracing will update this information.
		std::vector<std::stack<int>> circuitLevel_to_EventStack_; // std::stack<int> *circuitLevel_to_EventStack_ => std::stack<int> *circuitLevel_to_EventStack_
		std::vector<int> backtrackList_;													// a vector for backtracking
		std::vector<int> backtraceResetList_;											// the gate that need to have gate.n0_ and gate.n1_ reset after backtrace
		std::vector<int> unjustified_;														// J-frontier list
		std::vector<int> initObject_;															// Initial fault activation objectives or D-frontier propagation objectives.
		std::vector<int> currentObject_;													// The objective that is selected from final objectives.
		std::vector<int> fanoutObjective_;												// the fanout objectives recorded in atpg
		std::vector<int> headObject_;															// objectives at the headlines
		std::vector<int> finalObject_;														// final objectives include fanout objectives and headline objectives.
		std::vector<int> dFrontier_;															// D-frontier list
		DecisionTree decisionTree_;
		std::vector<bool> isInEventStack_;

		Gate *firstTimeFrameHeadLine_;

		// ---------------private methods----------------- //

		// set up circuit parameters
		void setupCircuitParameter();
		void calGateDepthFromPO();
		void identifyLineParameter();
		void identifyDominator();
		void identifyUniquePath();

		void TransitionDelayFaultATPG(FaultPtrList &faultListToGen, PatternProcessor *pPatternProcessor, int &numOfAtpgUntestableFaults);
		void StuckAtFaultATPGWithDTC(FaultPtrList &faultListToGen, PatternProcessor *pPatternProcessor, int &numOfAtpgUntestableFaults);

		Gate *getWireForActivation(const Fault &fault);
		void setValueAndRunImp(Gate &gate, const Value &val);

		void resetPreValue();
		int storeCurrentGateValue();
		void clearAllFaultEffectBySimulation();
		void clearOneGateFaultEffect(Gate &gate);

		SINGLE_PATTERN_GENERATION_STATUS generateSinglePatternOnTargetFault(Fault targetFault, bool isDTC); // name changed by wang

		// initialization at the start of single pattern generation
		Gate *initialize(Fault &targetFault, int &BackImpLevel, IMPLICATION_STATUS &implyStatus, bool isDTC);
		void initialList(bool initFlag);
		void initialNetlist(Gate &gFaultyLine, bool isDTC);
		void clearEventStack(bool isDebug);

		bool Implication(IMPLICATION_STATUS atpgStatus, int StartLevel);
		IMPLICATION_STATUS backwardImplication(Gate *pGate);

		bool backtrack(int &BackImpLevel);
		bool continuationMeaningful(Gate *pLastDFrontier);
		void updateUnjustifiedLines();
		void updateDFrontier();
		bool checkFaultPropToPO(bool &faultPropToPO);
		bool checkUnjustifiedBoundLines();
		void findFinalObjective(BACKTRACE_STATUS &flag, bool FaultPropPO, Gate *&pLastDFrontier);
		void assignValueToFinalObject();
		void justifyFreeLines(Fault &fOriginalFault);
		void restoreFault(Fault &fOriginalFault);
		int countNumGatesInDFrontier(Gate *pFaultyLine);
		int uniquePathSensitize(Gate &gate);

		bool isExistXPath(Gate *pGate);
		bool xPathTracing(Gate *pGate);

		int setFaultyGate(Fault &fault);
		Fault setFreeFaultyGate(Gate &gate);

		void fanoutFreeBacktrace(Gate *pObjectGate);
		BACKTRACE_RESULT multipleBacktrace(BACKTRACE_STATUS atpgStatus, int &finalObjectiveId);
		Value assignBacktraceValue(int &n0, int &n1, Gate &gate);
		void initialObjectives();
		Gate *findEasiestInput(Gate *pGate, Value Val);
		Gate *findCloseToOutput(std::vector<int> &list, int &index);

		IMPLICATION_STATUS evaluation(Gate *pGate);
		IMPLICATION_STATUS faultyGateEvaluation(Gate *pGate);

		// statuc test compression
		void staticTestCompressionByReverseFaultSimulation(PatternProcessor *pPatternProcessor, FaultPtrList &originalFaultList);

		int firstTimeFrameSetUp(Fault &fault); // this function is for multiple time frame

		inline Value evaluateGoodVal(Gate &gate);
		inline Value evaluateFaultyVal(Gate &gate);

		inline void setGaten0n1(const int &gateID, const int &n0, const int &n1);

		inline void writeGateValToPatternPI(Pattern &pattern);		// write PI values to pattern
		inline void writeGoodSimValToPatternPO(Pattern &pattern); // write PO values to pattern

		inline void pushGateToEventStack(const int &gateID); // push events to the event list of corresponding level
		inline int popEventStack(const int &level);
		inline int pushGateFanoutsToEventStack(const int &gateID); // push all the gate's output to eventStack, return pushed gatecount

		inline void clearAllEvent();

		inline int vecPop(std::vector<int> &vec);
		inline void vecDelete(std::vector<int> &list, const int &index);

		// 5-Value logic evaluation functions
		inline Value cINV(const Value &i1);
		inline Value cAND2(const Value &i1, const Value &i2);
		inline Value cAND3(const Value &i1, const Value &i2, const Value &i3);
		inline Value cAND4(const Value &i1, const Value &i2, const Value &i3, const Value &i4);
		inline Value cNAND2(const Value &i1, const Value &i2);
		inline Value cNAND3(const Value &i1, const Value &i2, const Value &i3);
		inline Value cNAND4(const Value &i1, const Value &i2, const Value &i3, const Value &i4);
		inline Value cOR2(const Value &i1, const Value &i2);
		inline Value cOR3(const Value &i1, const Value &i2, const Value &i3);
		inline Value cOR4(const Value &i1, const Value &i2, const Value &i3, const Value &i4);
		inline Value cNOR2(const Value &i1, const Value &i2);
		inline Value cNOR3(const Value &i1, const Value &i2, const Value &i3);
		inline Value cNOR4(const Value &i1, const Value &i2, const Value &i3, const Value &i4);
		inline Value cXOR2(const Value &i1, const Value &i2);
		inline Value cXOR3(const Value &i1, const Value &i2, const Value &i3);
		inline Value cXNOR2(const Value &i1, const Value &i2);
		inline Value cXNOR3(const Value &i1, const Value &i2, const Value &i3);

		// should be moved to pattern.h
		inline void randomFill(Pattern &pattern);
		inline void adjacentFill(Pattern &pattern); // added by wang, currently not used

		// function not used or removed
		void checkLevelInfo();																		// for debug use
		std::string getValStr(Value val);													// for debug use
		void calSCOAP();																					// heuristic not effective, currently not used, added by Wei-Shen Wang
		void testClearFaultEffect(FaultPtrList &faultListToTest); // removed from generatePatternSet() for now seems like debug usage
		void resetIsInEventStack();																// not used
		void XFill(PatternProcessor *pPatternProcessor);					// redundant function, removed by wang

		// considered unneccesary function
		// void pushGateFanoutsToEventStack(const int &gateID); overloaded function without readability, removed by wang
		// void pushInputEvents(const int &gateID, int index);, bad readability and performance, removed by wang
	};

	// --------------inline methods----------------- //
	inline Atpg::Atpg(Circuit *pCircuit, Simulator *pSimulator)
			: pCircuit_(pCircuit),
				pSimulator_(pSimulator),
				gateID_to_n0_(pCircuit->totalGate_, 0),
				gateID_to_n1_(pCircuit->totalGate_, 0),
				gateID_to_valModified_(pCircuit->totalGate_, 0),
				gateID_to_reachableByTargetFault_(pCircuit->totalGate_),
				gateID_to_lineType_(pCircuit->totalGate_, FREE_LINE),
				gateID_to_xPathStatus_(pCircuit->totalGate_),
				gateID_to_uniquePath_(pCircuit->totalGate_, std::vector<int>()),
				circuitLevel_to_EventStack_(pCircuit->circuitLvl_)
	{

		firstTimeFrameHeadLine_ = NULL;

		backtrackList_.reserve(pCircuit->totalGate_);

		dFrontier_.reserve(MAX_LIST_SIZE);
		fanoutObjective_.reserve(MAX_LIST_SIZE);
		initObject_.reserve(MAX_LIST_SIZE);
		headObject_.reserve(MAX_LIST_SIZE);
		finalObject_.reserve(MAX_LIST_SIZE);
		unjustified_.reserve(MAX_LIST_SIZE);
		currentObject_.reserve(MAX_LIST_SIZE);
		isInEventStack_.resize(pCircuit->totalGate_, false);
	}

	// **************************************************************************
	// Function   [ Atpg::evaluateGoodVal ]
	// Commentor  [ WYH ]
	// Synopsis   [ usage: Given the gate without falut, and generate the output,
	//                     and return.
	//              in:    Gate& gate
	//              out:   Value
	//            ]
	// Date       [ WYH Ver. 1.0 started 2013/08/15]
	// **************************************************************************
	inline Value Atpg::evaluateGoodVal(Gate &gate)
	{
		if (gate.gateType_ == Gate::PI || gate.gateType_ == Gate::PPI)
		{
			return gate.atpgVal_;
		}

		static Value v[4];
		int index = 0;
		for (const int &faninID : gate.faninVector_)
		{
			v[index++] = pCircuit_->circuitGates_[faninID].atpgVal_;
		}

		switch (gate.gateType_)
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

	// **************************************************************************
	// Function   [ Atpg::evaluateFaultyVal ]
	// Commentor  [ CAL ]
	// Synopsis   [ usage: deal with 2 frame PPI, check it's D or D' logic
	//              in:    gate
	//              out:   value
	//            ]
	// Date       [ Ver. 1.0 started 2013/08/13 ]
	// **************************************************************************
	inline Value Atpg::evaluateFaultyVal(Gate &gate)
	{
		Value val;
		const int &faultyLine = currentTargetFault_.faultyLine_;
		switch (gate.gateType_)
		{
			case Gate::PI:
			case Gate::PPI:
				return gate.atpgVal_;
			case Gate::PO:
			case Gate::PPO:
			case Gate::BUF:
				val = pCircuit_->circuitGates_[gate.faninVector_[0]].atpgVal_;
				if (val == L && (currentTargetFault_.faultType_ == Fault::SA1 || currentTargetFault_.faultType_ == Fault::STF))
				{
					val = B; // logic D' (0/1)
				}
				if (val == H && (currentTargetFault_.faultType_ == Fault::SA0 || currentTargetFault_.faultType_ == Fault::STR))
				{
					val = D; // logic D  (1/0)
				}
				return val;
			case Gate::INV:
				val = pCircuit_->circuitGates_[gate.faninVector_[0]].atpgVal_;
				if (faultyLine == 0)
				{
					val = cINV(val);
					if (val == L && (currentTargetFault_.faultType_ == Fault::SA1 || currentTargetFault_.faultType_ == Fault::STF))
					{
						val = B;
					}
					if (val == H && (currentTargetFault_.faultType_ == Fault::SA0 || currentTargetFault_.faultType_ == Fault::STR))
					{
						val = D;
					}
					return val;
				}
				else
				{
					if (val == L && (currentTargetFault_.faultType_ == Fault::SA1 || currentTargetFault_.faultType_ == Fault::STF))
					{
						val = B;
					}
					if (val == H && (currentTargetFault_.faultType_ == Fault::SA0 || currentTargetFault_.faultType_ == Fault::STR))
					{
						val = D;
					}
					return cINV(val);
				}
			case Gate::AND2:
			case Gate::AND3:
			case Gate::AND4:
				if (faultyLine == 0)
				{
					val = pCircuit_->circuitGates_[gate.faninVector_[0]].atpgVal_;
					for (int i = 1; i < gate.numFI_; ++i)
					{
						val = cAND2(val, pCircuit_->circuitGates_[gate.faninVector_[i]].atpgVal_);
					}

					if (val == L && (currentTargetFault_.faultType_ == Fault::SA1 || currentTargetFault_.faultType_ == Fault::STF))
					{
						val = B;
					}

					if (val == H && (currentTargetFault_.faultType_ == Fault::SA0 || currentTargetFault_.faultType_ == Fault::STR))
					{
						val = D;
					}
				}
				else
				{
					val = pCircuit_->circuitGates_[gate.faninVector_[faultyLine - 1]].atpgVal_;
					if (val == L && (currentTargetFault_.faultType_ == Fault::SA1 || currentTargetFault_.faultType_ == Fault::STF))
					{
						val = B;
					}

					if (val == H && (currentTargetFault_.faultType_ == Fault::SA0 || currentTargetFault_.faultType_ == Fault::STR))
					{
						val = D;
					}

					for (int i = 0; i < gate.numFI_; ++i)
					{
						if (i != faultyLine - 1)
						{
							val = cAND2(val, pCircuit_->circuitGates_[gate.faninVector_[i]].atpgVal_);
						}
					}
				}
				return val;
			case Gate::NAND2:
			case Gate::NAND3:
			case Gate::NAND4:
				if (faultyLine == 0)
				{
					val = pCircuit_->circuitGates_[gate.faninVector_[0]].atpgVal_;
					for (int i = 1; i < gate.numFI_; ++i)
					{
						val = cAND2(val, pCircuit_->circuitGates_[gate.faninVector_[i]].atpgVal_);
					}

					val = cINV(val);
					if (val == L && (currentTargetFault_.faultType_ == Fault::SA1 || currentTargetFault_.faultType_ == Fault::STF))
					{
						val = B;
					}

					if (val == H && (currentTargetFault_.faultType_ == Fault::SA0 || currentTargetFault_.faultType_ == Fault::STR))
					{
						val = D;
					}
				}
				else
				{
					val = pCircuit_->circuitGates_[gate.faninVector_[faultyLine - 1]].atpgVal_;
					if (val == L && (currentTargetFault_.faultType_ == Fault::SA1 || currentTargetFault_.faultType_ == Fault::STF))
					{
						val = B;
					}

					if (val == H && (currentTargetFault_.faultType_ == Fault::SA0 || currentTargetFault_.faultType_ == Fault::STR))
					{
						val = D;
					}

					for (int i = 0; i < gate.numFI_; ++i)
					{
						if (i != faultyLine - 1)
						{
							val = cAND2(val, pCircuit_->circuitGates_[gate.faninVector_[i]].atpgVal_);
						}
					}
					val = cINV(val);
				}
				return val;
			case Gate::OR2:
			case Gate::OR3:
			case Gate::OR4:
				if (faultyLine == 0)
				{
					val = pCircuit_->circuitGates_[gate.faninVector_[0]].atpgVal_;
					for (int i = 1; i < gate.numFI_; ++i)
					{
						val = cOR2(val, pCircuit_->circuitGates_[gate.faninVector_[i]].atpgVal_);
					}

					if (val == L && (currentTargetFault_.faultType_ == Fault::SA1 || currentTargetFault_.faultType_ == Fault::STF))
					{
						val = B;
					}

					if (val == H && (currentTargetFault_.faultType_ == Fault::SA0 || currentTargetFault_.faultType_ == Fault::STR))
					{
						val = D;
					}
				}
				else
				{
					val = pCircuit_->circuitGates_[gate.faninVector_[faultyLine - 1]].atpgVal_;
					if (val == L && (currentTargetFault_.faultType_ == Fault::SA1 || currentTargetFault_.faultType_ == Fault::STF))
					{
						val = B;
					}

					if (val == H && (currentTargetFault_.faultType_ == Fault::SA0 || currentTargetFault_.faultType_ == Fault::STR))
					{
						val = D;
					}

					for (int i = 0; i < gate.numFI_; ++i)
					{
						if (i != faultyLine - 1)
						{
							val = cOR2(val, pCircuit_->circuitGates_[gate.faninVector_[i]].atpgVal_);
						}
					}
				}
				return val;
			case Gate::NOR2:
			case Gate::NOR3:
			case Gate::NOR4:
				if (faultyLine == 0)
				{
					val = pCircuit_->circuitGates_[gate.faninVector_[0]].atpgVal_;
					for (int i = 1; i < gate.numFI_; ++i)
					{
						val = cOR2(val, pCircuit_->circuitGates_[gate.faninVector_[i]].atpgVal_);
					}

					val = cINV(val);

					if (val == L && (currentTargetFault_.faultType_ == Fault::SA1 || currentTargetFault_.faultType_ == Fault::STF))
					{
						val = B;
					}
					if (val == H && (currentTargetFault_.faultType_ == Fault::SA0 || currentTargetFault_.faultType_ == Fault::STR))
					{
						val = D;
					}
				}
				else
				{
					val = pCircuit_->circuitGates_[gate.faninVector_[faultyLine - 1]].atpgVal_;
					if (val == L && (currentTargetFault_.faultType_ == Fault::SA1 || currentTargetFault_.faultType_ == Fault::STF))
					{
						val = B;
					}
					if (val == H && (currentTargetFault_.faultType_ == Fault::SA0 || currentTargetFault_.faultType_ == Fault::STR))
					{
						val = D;
					}
					for (int i = 0; i < gate.numFI_; ++i)
					{
						if (i != faultyLine - 1)
						{
							val = cOR2(val, pCircuit_->circuitGates_[gate.faninVector_[i]].atpgVal_);
						}
					}
					val = cINV(val);
				}
				return val;
			case Gate::XOR2:
			case Gate::XOR3:
				if (faultyLine == 0)
				{
					if (gate.gateType_ == Gate::XOR2)
					{
						val = cXOR2(pCircuit_->circuitGates_[gate.faninVector_[0]].atpgVal_, pCircuit_->circuitGates_[gate.faninVector_[1]].atpgVal_);
					}
					else
					{
						val = cXOR3(pCircuit_->circuitGates_[gate.faninVector_[0]].atpgVal_, pCircuit_->circuitGates_[gate.faninVector_[1]].atpgVal_, pCircuit_->circuitGates_[gate.faninVector_[2]].atpgVal_);
					}

					if (val == L && (currentTargetFault_.faultType_ == Fault::SA1 || currentTargetFault_.faultType_ == Fault::STF))
					{
						val = B;
					}
					if (val == H && (currentTargetFault_.faultType_ == Fault::SA0 || currentTargetFault_.faultType_ == Fault::STR))
					{
						val = D;
					}
				}
				else
				{
					val = pCircuit_->circuitGates_[gate.faninVector_[faultyLine - 1]].atpgVal_;
					if (val == L && (currentTargetFault_.faultType_ == Fault::SA1 || currentTargetFault_.faultType_ == Fault::STF))
					{
						val = B;
					}
					if (val == H && (currentTargetFault_.faultType_ == Fault::SA0 || currentTargetFault_.faultType_ == Fault::STR))
					{
						val = D;
					}

					if (gate.gateType_ == Gate::XOR2)
					{
						// if (faultyLine - 1 == 0)
						// 	val = cXOR2(pCircuit_->circuitGates_[gate.faninVector_[1]].atpgVal_, val);
						// else
						// 	val = cXOR2(pCircuit_->circuitGates_[gate.faninVector_[0]].atpgVal_, val);
						switch (faultyLine - 1)
						{
							case 0:
								val = cXOR2(pCircuit_->circuitGates_[gate.faninVector_[1]].atpgVal_, val);
								break;
							case 1:
								val = cXOR2(pCircuit_->circuitGates_[gate.faninVector_[0]].atpgVal_, val);
								break;
							default:
								std::cerr << "switch case default, should not happen\n";
								break;
						}
					}
					else
					{ // XOR3
						// if (faultyLine - 1 == 0)
						// 	val = cXOR3(pCircuit_->circuitGates_[gate.faninVector_[1]].atpgVal_, pCircuit_->circuitGates_[gate.faninVector_[2]].atpgVal_, val);
						// else if (faultyLine - 1 == 1)
						// 	val = cXOR3(pCircuit_->circuitGates_[gate.faninVector_[0]].atpgVal_, pCircuit_->circuitGates_[gate.faninVector_[2]].atpgVal_, val);
						// else
						// 	val = cXOR3(pCircuit_->circuitGates_[gate.faninVector_[1]].atpgVal_, pCircuit_->circuitGates_[gate.faninVector_[0]].atpgVal_, val);
						switch (faultyLine - 1)
						{
							case 0:
								val = cXOR3(pCircuit_->circuitGates_[gate.faninVector_[1]].atpgVal_, pCircuit_->circuitGates_[gate.faninVector_[2]].atpgVal_, val);
								break;
							case 1:
								val = cXOR3(pCircuit_->circuitGates_[gate.faninVector_[0]].atpgVal_, pCircuit_->circuitGates_[gate.faninVector_[2]].atpgVal_, val);
								break;
							case 2:
								val = cXOR3(pCircuit_->circuitGates_[gate.faninVector_[1]].atpgVal_, pCircuit_->circuitGates_[gate.faninVector_[0]].atpgVal_, val);
								break;
							default:
								std::cerr << "switch case default, should not happen\n";
								break;
						}
					}
				}
				return val;
			case Gate::XNOR2:
			case Gate::XNOR3:
				if (faultyLine == 0)
				{
					if (gate.gateType_ == Gate::XNOR2)
					{
						val = cXNOR2(pCircuit_->circuitGates_[gate.faninVector_[0]].atpgVal_, pCircuit_->circuitGates_[gate.faninVector_[1]].atpgVal_);
					}
					else
					{
						val = cXNOR3(pCircuit_->circuitGates_[gate.faninVector_[0]].atpgVal_, pCircuit_->circuitGates_[gate.faninVector_[1]].atpgVal_, pCircuit_->circuitGates_[gate.faninVector_[2]].atpgVal_);
					}

					if (val == L && (currentTargetFault_.faultType_ == Fault::SA1 || currentTargetFault_.faultType_ == Fault::STF))
					{
						val = B;
					}
					if (val == H && (currentTargetFault_.faultType_ == Fault::SA0 || currentTargetFault_.faultType_ == Fault::STR))
					{
						val = D;
					}
				}
				else
				{
					val = pCircuit_->circuitGates_[gate.faninVector_[faultyLine - 1]].atpgVal_;
					if (val == L && (currentTargetFault_.faultType_ == Fault::SA1 || currentTargetFault_.faultType_ == Fault::STF))
					{
						val = B;
					}
					if (val == H && (currentTargetFault_.faultType_ == Fault::SA0 || currentTargetFault_.faultType_ == Fault::STR))
					{
						val = D;
					}

					if (gate.gateType_ == Gate::XNOR2)
					{
						// if (faultyLine - 1 == 0)
						// 	val = cXNOR2(pCircuit_->circuitGates_[gate.faninVector_[1]].atpgVal_, val);
						// else
						// 	val = cXNOR2(pCircuit_->circuitGates_[gate.faninVector_[0]].atpgVal_, val);
						switch (faultyLine - 1)
						{
							case 0:
								val = cXNOR2(pCircuit_->circuitGates_[gate.faninVector_[1]].atpgVal_, val);
								break;
							case 1:
								val = cXNOR2(pCircuit_->circuitGates_[gate.faninVector_[0]].atpgVal_, val);
								break;
							default:
								std::cerr << "switch case default, should not happen\n";
								break;
						}
					}
					else
					{ // XOR3
						// change from if else to switch by wang
						// if (faultyLine - 1 == 0)
						// 	val = cXNOR3(pCircuit_->circuitGates_[gate.faninVector_[1]].atpgVal_, pCircuit_->circuitGates_[gate.faninVector_[2]].atpgVal_, val);
						// else if (faultyLine - 1 == 1)
						// 	val = cXNOR3(pCircuit_->circuitGates_[gate.faninVector_[0]].atpgVal_, pCircuit_->circuitGates_[gate.faninVector_[2]].atpgVal_, val);
						// else
						// 	val = cXNOR3(pCircuit_->circuitGates_[gate.faninVector_[1]].atpgVal_, pCircuit_->circuitGates_[gate.faninVector_[0]].atpgVal_, val);
						switch (faultyLine - 1)
						{
							case 0:
								val = cXNOR3(pCircuit_->circuitGates_[gate.faninVector_[1]].atpgVal_, pCircuit_->circuitGates_[gate.faninVector_[2]].atpgVal_, val);
								break;
							case 1:
								val = cXNOR3(pCircuit_->circuitGates_[gate.faninVector_[0]].atpgVal_, pCircuit_->circuitGates_[gate.faninVector_[2]].atpgVal_, val);
								break;
							case 2:
								val = cXNOR3(pCircuit_->circuitGates_[gate.faninVector_[1]].atpgVal_, pCircuit_->circuitGates_[gate.faninVector_[0]].atpgVal_, val);
								break;
							default:
								std::cerr << "switch case default, should not happen\n";
								break;
						}
					}
				}
				return val;
			default:
				return gate.atpgVal_;
		}
	}

	inline void Atpg::setGaten0n1(const int &gateID, const int &n0, const int &n1)
	{
		gateID_to_n0_[gateID] = n0;
		gateID_to_n1_[gateID] = n1;
	}

	// **************************************************************************
	// Function   [ Atpg::writeGateValToPatternPI ]
	// Commentor  [ CAL ]
	// Synopsis   [ usage: assign primary input pattern value
	//              in:    Pattern list
	//              out:   void
	//              pointer modification: Pattern
	//            ]
	// Date       [ Ver. 1.0 started 2013/08/13 ]
	// **************************************************************************
	inline void Atpg::writeGateValToPatternPI(Pattern &pattern)
	{
		for (int i = 0; i < pCircuit_->numPI_; ++i)
		{
			pattern.primaryInputs1st_[i] = pCircuit_->circuitGates_[i].atpgVal_;
		}
		// if (pattern.primaryInputs2nd_ != NULL && pCircuit_->numFrame_ > 1)
		if (!(pattern.primaryInputs2nd_.empty()) && pCircuit_->numFrame_ > 1)
		{
			for (int i = 0; i < pCircuit_->numPI_; ++i)
			{
				pattern.primaryInputs2nd_[i] = pCircuit_->circuitGates_[i + pCircuit_->numGate_].atpgVal_;
			}
		}
		for (int i = 0; i < pCircuit_->numPPI_; ++i)
		{
			pattern.pseudoPrimaryInputs_[i] = pCircuit_->circuitGates_[pCircuit_->numPI_ + i].atpgVal_;
		}
		// if (pattern.shiftIn_ != NULL && pCircuit_->numFrame_ > 1)
		if (!(pattern.shiftIn_.empty()) && pCircuit_->numFrame_ > 1)
		{
			pattern.shiftIn_[0] = (pCircuit_->timeFrameConnectType_ == Circuit::SHIFT) ? pCircuit_->circuitGates_[pCircuit_->numGate_ + pCircuit_->numPI_].atpgVal_ : X;
		}
	}

	// **************************************************************************
	// Function   [ Atpg::writeGoodSimValToPatternPO ]
	// Commentor  [ CAL ]
	// Synopsis   [ usage: assign primary output pattern value
	//              in:    Pattern list
	//              out:   void
	//              pointer modification: Pattern
	//            ]
	// Date       [ Ver. 1.0 started 2013/08/13 ]
	// **************************************************************************
	inline void Atpg::writeGoodSimValToPatternPO(Pattern &pattern)
	{
		// pSimulator_->goodSim();call externally instead, removed by wang
		int offset = pCircuit_->numGate_ - pCircuit_->numPO_ - pCircuit_->numPPI_;
		for (int i = 0; i < pCircuit_->numPO_; ++i)
		{
			if (pCircuit_->circuitGates_[offset + i].goodSimLow_ == PARA_H)
			{
				pattern.primaryOutputs1st_[i] = L;
			}
			else if (pCircuit_->circuitGates_[offset + i].goodSimHigh_ == PARA_H)
			{
				pattern.primaryOutputs1st_[i] = H;
			}
			else
			{
				pattern.primaryOutputs1st_[i] = X;
			}
		}

		if (!(pattern.primaryOutputs2nd_.empty()) && pCircuit_->numFrame_ > 1)
		{
			for (int i = 0; i < pCircuit_->numPO_; ++i)
			{
				if (pCircuit_->circuitGates_[offset + i + pCircuit_->numGate_].goodSimLow_ == PARA_H)
				{
					pattern.primaryOutputs2nd_[i] = L;
				}
				else if (pCircuit_->circuitGates_[offset + i + pCircuit_->numGate_].goodSimHigh_ == PARA_H)
				{
					pattern.primaryOutputs2nd_[i] = H;
				}
				else
				{
					pattern.primaryOutputs2nd_[i] = X;
				}
			}
		}

		offset = pCircuit_->numGate_ - pCircuit_->numPPI_;
		if (pCircuit_->numFrame_ > 1)
		{
			offset += pCircuit_->numGate_;
		}

		for (int i = 0; i < pCircuit_->numPPI_; ++i)
		{
			if (pCircuit_->circuitGates_[offset + i].goodSimLow_ == PARA_H)
			{
				pattern.pseudoPrimaryOutputs_[i] = L;
			}
			else if (pCircuit_->circuitGates_[offset + i].goodSimHigh_ == PARA_H)
			{
				pattern.pseudoPrimaryOutputs_[i] = H;
			}
			else
			{
				pattern.pseudoPrimaryOutputs_[i] = X;
			}
		}
	}

	inline void Atpg::pushGateToEventStack(const int &gateID)
	{
		const Gate &gate = pCircuit_->circuitGates_[gateID];
		if (!gateID_to_valModified_[gateID])
		{
			circuitLevel_to_EventStack_[gate.numLevel_].push(gateID);
			gateID_to_valModified_[gateID] = 1;
		}
	}

	inline int Atpg::popEventStack(const int &level)
	{
		const int &gateID = circuitLevel_to_EventStack_[level].top();
		circuitLevel_to_EventStack_[level].pop();
		return gateID;
	}

	inline int Atpg::pushGateFanoutsToEventStack(const int &gateID)
	{
		int pushedGateCount = 0;
		const Gate &gate = pCircuit_->circuitGates_[gateID];
		for (const int &fanoutID : gate.fanoutVector_)
		{
			const int &outputGateLevel = pCircuit_->circuitGates_[fanoutID].numLevel_;
			if (!gateID_to_valModified_[fanoutID])
			{
				circuitLevel_to_EventStack_[outputGateLevel].push(fanoutID);
				gateID_to_valModified_[fanoutID] = 1;
				++pushedGateCount;
			}
		}
		return pushedGateCount;
	}

	inline void Atpg::clearAllEvent()
	{
		int gateID;
		// change old for loop to range-based for loop
		for (std::stack<int> &eventStack : circuitLevel_to_EventStack_)
		{
			while (!eventStack.empty())
			{
				gateID = eventStack.top();
				eventStack.pop();
				gateID_to_valModified_[gateID] = 0;
			}
		}
	}

	inline int Atpg::vecPop(std::vector<int> &vec) // listPop => vecPop by wang
	{
		int lastElement = vec.back();
		vec.pop_back();
		return lastElement;
	}

	inline void Atpg::vecDelete(std::vector<int> &list, const int &index)
	{
		list[index] = list.back();
		list.pop_back();
	}

	// 5-value logic evaluation functions
	inline Value Atpg::cINV(const Value &i1)
	{
		constexpr Value map[5] = {H, L, X, B, D};
		if (i1 >= Z)
		{
			return Z;
		}
		return map[i1];
	}
	inline Value Atpg::cAND2(const Value &i1, const Value &i2)
	{
		constexpr Value map[5][5] = {
				{L, L, L, L, L},
				{L, H, X, D, B},
				{L, X, X, X, X},
				{L, D, X, D, L},
				{L, B, X, L, B}};
		if (i1 >= Z || i2 >= Z)
		{
			return Z;
		}
		return map[i1][i2];
	}
	inline Value Atpg::cAND3(const Value &i1, const Value &i2, const Value &i3)
	{
		return cAND2(i1, cAND2(i2, i3));
	}
	inline Value Atpg::cAND4(const Value &i1, const Value &i2, const Value &i3, const Value &i4)
	{
		return cAND2(cAND2(i1, i2), cAND2(i3, i4));
	}
	inline Value Atpg::cNAND2(const Value &i1, const Value &i2)
	{
		return cINV(cAND2(i1, i2));
	}
	inline Value Atpg::cNAND3(const Value &i1, const Value &i2, const Value &i3)
	{
		return cINV(cAND3(i1, i2, i3));
	}
	inline Value Atpg::cNAND4(const Value &i1, const Value &i2, const Value &i3, const Value &i4)
	{
		return cINV(cAND4(i1, i2, i3, i4));
	}
	inline Value Atpg::cOR2(const Value &i1, const Value &i2)
	{
		constexpr Value map[5][5] = {
				{L, H, X, D, B},
				{H, H, H, H, H},
				{X, H, X, X, X},
				{D, H, X, D, H},
				{B, H, X, H, B}};
		if (i1 >= Z || i2 >= Z)
		{
			return Z;
		}
		return map[i1][i2];
	}
	inline Value Atpg::cOR3(const Value &i1, const Value &i2, const Value &i3)
	{
		return cOR2(i1, cOR2(i2, i3));
	}
	inline Value Atpg::cOR4(const Value &i1, const Value &i2, const Value &i3, const Value &i4)
	{
		return cOR2(cOR2(i1, i2), cOR2(i3, i4));
	}
	inline Value Atpg::cNOR2(const Value &i1, const Value &i2)
	{
		return cINV(cOR2(i1, i2));
	}
	inline Value Atpg::cNOR3(const Value &i1, const Value &i2, const Value &i3)
	{
		return cINV(cOR3(i1, i2, i3));
	}
	inline Value Atpg::cNOR4(const Value &i1, const Value &i2, const Value &i3, const Value &i4)
	{
		return cINV(cOR4(i1, i2, i3, i4));
	}
	inline Value Atpg::cXOR2(const Value &i1, const Value &i2)
	{
		constexpr Value map[5][5] = {
				{L, H, X, D, B},
				{H, L, X, B, D},
				{X, X, X, X, X},
				{D, B, X, L, H},
				{B, D, X, H, L}};
		if (i1 >= Z || i2 >= Z)
		{
			return Z;
		}
		return map[i1][i2];
	}
	inline Value Atpg::cXOR3(const Value &i1, const Value &i2, const Value &i3)
	{
		return cXOR2(i1, cXOR2(i2, i3));
	}
	inline Value Atpg::cXNOR2(const Value &i1, const Value &i2)
	{
		return cINV(cXOR2(i1, i2));
	}
	inline Value Atpg::cXNOR3(const Value &i1, const Value &i2, const Value &i3)
	{
		return cINV(cXOR3(i1, i2, i3));
	}

	// should be moved to pattern.h
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
	inline void Atpg::randomFill(Pattern &pattern)
	{
		srand(0);
		for (int i = 0; i < pCircuit_->numPI_; ++i)
		{
			if (pattern.primaryInputs1st_[i] == X)
			{
				pattern.primaryInputs1st_[i] = rand() % 2;
			}
		}
		for (int i = 0; i < pCircuit_->numPPI_; ++i)
		{
			if (pattern.pseudoPrimaryInputs_[i] == X)
			{
				pattern.pseudoPrimaryInputs_[i] = rand() % 2;
			}
		}
		if (!pattern.primaryInputs2nd_.empty())
		{
			for (int i = 0; i < pCircuit_->numPI_; ++i)
			{
				if (pattern.primaryInputs2nd_[i] == X)
				{
					pattern.primaryInputs2nd_[i] = rand() % 2;
				}
			}
		}
		if (!pattern.shiftIn_.empty())
		{
			if (pattern.shiftIn_[0] == X)
			{
				pattern.shiftIn_[0] = rand() % 2;
			}
		}
	}

	inline void Atpg::adjacentFill(Pattern &pattern)
	{
		if (pattern.primaryInputs1st_[0] == X)
		{
			pattern.primaryInputs1st_[0] = L;
		}
		for (int i = 1; i < pCircuit_->numPI_; ++i)
		{
			if (pattern.primaryInputs1st_[i] == X)
			{
				pattern.primaryInputs1st_[i] = pattern.primaryInputs1st_[i - 1];
			}
		}

		if (pattern.pseudoPrimaryInputs_[0] == X)
		{
			pattern.pseudoPrimaryInputs_[0] = pattern.primaryInputs1st_[pCircuit_->numPI_ - 1];
		}
		for (int i = 1; i < pCircuit_->numPPI_; ++i)
		{
			if (pattern.pseudoPrimaryInputs_[i] == X)
			{
				pattern.pseudoPrimaryInputs_[i] = pattern.pseudoPrimaryInputs_[i - 1];
			}
		}

		if (!pattern.primaryInputs2nd_.empty())
		{
			for (int i = 0; i < pCircuit_->numPI_; ++i)
			{
				if (pattern.primaryInputs2nd_[i] == X)
				{
					pattern.primaryInputs2nd_[i] = pattern.primaryInputs1st_[i];
				}
			}
		}
		if (!pattern.shiftIn_.empty())
		{
			if (pattern.shiftIn_[0] == X)
			{
				pattern.shiftIn_[0] = L;
			}
		}
	}

	// inline void Atpg::pushGateFanoutsToEventStack(const int &gateID)
	// {
	// 	Gate &gate = pCircuit_->circuitGates_[gateID];
	// 	for (int i = 0; i < gate.numFO_; ++i)
	// 	{
	// 		pushGateToEventStack(gate.fanoutVector_[i]);
	// 	}
	// } originally duplicate function and implemented for performance but bad readability, removed by wang

	// inline void Atpg::pushInputEvents(const int &gateID, int index)
	// {
	// 	Gate &gate = pCircuit_->circuitGates_[gateID];
	// 	pushGateToEventStack(gate.faninVector_[index]);
	// 	pushGateFanoutsToEventStack(gate.faninVector_[index]);
	// }
};

#endif
