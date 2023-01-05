// **************************************************************************
// File       [ pattern_rw.h ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/10/10 created ]
// **************************************************************************

#ifndef _CORE_PATTERN_RW_H_
#define _CORE_PATTERN_RW_H_

#include "interface/pat_file.h"

#include "circuit.h"
#include "pattern.h"

namespace CoreNs
{

	class PatternReader : public IntfNs::PatFile
	{
	public:
		PatternReader(PatternProcessor *pPatternProcessor, Circuit *pCircuit);
		~PatternReader();
		virtual void setPiOrder(const IntfNs::PatNames *const pPIs);
		virtual void setPpiOrder(const IntfNs::PatNames *const pPPIs);
		virtual void setPoOrder(const IntfNs::PatNames *const pPOs);
		virtual void setPatternType(const IntfNs::PatType &patternType);
		virtual void setPatternNum(const int &num);
		virtual void addPattern(const char *const pPI1, const char *const pPI2,
														const char *const pPPI, const char *const pSI,
														const char *const pPO1, const char *const pPO2,
														const char *const pPPO);

	protected:
		void assignValue(std::vector<Value> &valueVector, const char *const pPattern,
										 const int &size);
		int curPattern_;
		PatternProcessor *pPatternProcessor_;
		Circuit *pCircuit_;
	};

	inline PatternReader::PatternReader(PatternProcessor *pPatternProcessor, Circuit *pCircuit)
	{
		curPattern_ = 0;
		pPatternProcessor_ = pPatternProcessor;
		pCircuit_ = pCircuit;
	}

	inline PatternReader::~PatternReader() {}

	class PatternWriter
	{
	public:
		PatternWriter(PatternProcessor *pPatternProcessor, Circuit *pCircuit);
		~PatternWriter();

		bool writePattern(const char *const fname);
		bool writeLht(const char *const fname); // no longer supported
		bool writeAscii(const char *const fname);
		bool writeSTIL(const char *const fname); // 2016 summer train

	protected:
		PatternProcessor *pPatternProcessor_;
		Circuit *pCircuit_;
	};

	inline PatternWriter::PatternWriter(PatternProcessor *pPatternProcessor, Circuit *pCircuit)
	{
		pPatternProcessor_ = pPatternProcessor;
		pCircuit_ = pCircuit;
	}

	inline PatternWriter::~PatternWriter() {}

	class ProcedureWriter
	{
	public:
		ProcedureWriter(Circuit *pCircuit);
		~ProcedureWriter();
		bool writeProcedure(const char *const fname);

	protected:
		Circuit *pCircuit_;
	};

	inline ProcedureWriter::ProcedureWriter(Circuit *pCircuit)
	{
		pCircuit_ = pCircuit;
	}

	inline ProcedureWriter::~ProcedureWriter() {}
};

#endif
