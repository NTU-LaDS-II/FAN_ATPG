// **************************************************************************
// File       [ pattern_rw.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/10/10 created ]
// **************************************************************************

#include "pattern_rw.h"
#include <fstream>
#include <map>
#include <algorithm>

using namespace IntfNs;
using namespace CoreNs;

// **************************************************************************
// Function   [ PatternReader::setPiOrder ]
// Commenter  [ CHT ]
// Synopsis   [ usage: Map the PI order to the circuit order.
//							description:
//								First traverse all PIs to calculate the number of PIs,
//								then set the order of PIs of the PatternProcessor
//								according to the gate id of the circuit. The result will be
//								stored in the vector pPIorder_ of the PatternProcessor.
//							arguments:
// 								[in] pPIs : A pointer to the linked structure of
//														primary inputs.
//						]
// Date       [ CHT started 2023/01/05 ]
// **************************************************************************
void PatternReader::setPiOrder(const PatNames *const pPIs)
{
	if (!success_)
	{
		return;
	}

	if (!pCircuit_ || !pCircuit_->pNetlist_)
	{
		success_ = false;
		return;
	}

	pPatternProcessor_->numPI_ = 0;
	PatNames *pPI = pPIs->head;
	while (pPI)
	{
		++pPatternProcessor_->numPI_;
		pPI = pPI->next;
	}

	pPatternProcessor_->pPIorder_.resize(pPatternProcessor_->numPI_);
	pPI = pPIs->head;
	int i = 0;
	while (pPI)
	{
		Port *pPort = pCircuit_->pNetlist_->getTop()->getPort(pPI->name);
		if (!pPort)
		{
			fprintf(stderr, "**ERROR PatternReader::setPiOrder(): port ");
			fprintf(stderr, "`%s' not found\n", pPI->name);
			success_ = false;
			pPatternProcessor_->pPIorder_.clear();
			return;
		}
		pPatternProcessor_->pPIorder_[i] = pCircuit_->portIndexToGateIndex_[pPort->id_];
		++i;
		pPI = pPI->next;
	}
}

// **************************************************************************
// Function   [ PatternReader::setPpiOrder ]
// Commenter  [ CHT ]
// Synopsis   [ usage: Map the PPI order to the circuit order.
//							description:
//								First traverse all PPIs to calculate the number of PPIs,
//								then set the order of PPIs of the PatternProcessor
//								according to the gate id of the circuit. The result will be
//								stored in the vector pPPIorder_ of the PatternProcessor.
//							arguments:
// 								[in] pPPIs : A pointer to the linked structure of
//														pseudo primary inputs.
//						]
// Date       [ CHT started 2023/01/05 ]
// **************************************************************************
void PatternReader::setPpiOrder(const PatNames *const pPPIs)
{
	if (!success_)
	{
		return;
	}

	if (!pCircuit_ || !pCircuit_->pNetlist_)
	{
		success_ = false;
		return;
	}

	pPatternProcessor_->numPPI_ = 0;
	PatNames *pPPI = pPPIs->head;
	while (pPPI)
	{
		++pPatternProcessor_->numPPI_;
		pPPI = pPPI->next;
	}
	pPatternProcessor_->pPPIorder_.resize(pPatternProcessor_->numPPI_);
	pPPI = pPPIs->head;
	int i = 0;
	while (pPPI)
	{
		Cell *pCell = pCircuit_->pNetlist_->getTop()->getCell(pPPI->name);
		if (!pCell)
		{
			fprintf(stderr, "**ERROR PatternReader::setPpiOrder(): gate ");
			fprintf(stderr, "`%s' not found\n", pPPI->name);
			success_ = false;
			pPatternProcessor_->pPPIorder_.clear();
			return;
		}
		pPatternProcessor_->pPPIorder_[i] = pCircuit_->cellIndexToGateIndex_[pCell->id_];
		++i;
		pPPI = pPPI->next;
	}
}

// **************************************************************************
// Function   [ PatternReader::setPoOrder ]
// Commenter  [ CHT ]
// Synopsis   [ usage: Map the PO order to the circuit order.
//							description:
//								First traverse all POs to calculate the number of POs,
//								then set the order of POs of the PatternProcessor
//								according to the gate id of the circuit. The result will be
//								stored in the vector pPOorder_ of the PatternProcessor.
//							arguments:
// 								[in] pPOs : A pointer to the linked structure of
//														primary onputs.
//						]
// Date       [ CHT started 2023/01/05 ]
// **************************************************************************
void PatternReader::setPoOrder(const PatNames *const pPOs)
{
	if (!success_)
	{
		return;
	}

	if (!pCircuit_ || !pCircuit_->pNetlist_)
	{
		success_ = false;
		return;
	}
	pPatternProcessor_->numPO_ = 0;
	PatNames *pPO = pPOs->head;
	while (pPO)
	{
		++pPatternProcessor_->numPO_;
		pPO = pPO->next;
	}
	pPatternProcessor_->pPOorder_.resize(pPatternProcessor_->numPO_);
	pPO = pPOs->head;
	int i = 0;
	while (pPO)
	{
		Port *pPort = pCircuit_->pNetlist_->getTop()->getPort(pPO->name);
		if (!pPort)
		{
			fprintf(stderr, "**ERROR PatternReader::setPoOrder(): port ");
			fprintf(stderr, "`%s' not found\n", pPO->name);
			success_ = false;
			pPatternProcessor_->pPOorder_.clear();
			return;
		}
		pPatternProcessor_->pPOorder_[i] = pCircuit_->portIndexToGateIndex_[pPort->id_];
		++i;
		pPO = pPO->next;
	}
}

// **************************************************************************
// Function   [ PatternReader::setPatternType ]
// Commenter  [ CHT ]
// Synopsis   [ usage: Set the type of the Pattern Processor.
//							description:
//								Set type_ of the Pattern Processor according to the input.
//								If type is LAUNCH_SHIFT, set numSI_ to be 1.
//							arguments:
// 								[in] patternType : Pattern Type to be set to.
//						]
// Date       [ CHT started 2023/01/05 ]
// **************************************************************************
void PatternReader::setPatternType(const PatType &patternType)
{
	if (!success_)
	{
		return;
	}

	switch (patternType)
	{
		case IntfNs::BASIC_SCAN:
			pPatternProcessor_->type_ = PatternProcessor::BASIC_SCAN;
			break;
		case IntfNs::LAUNCH_CAPTURE:
			pPatternProcessor_->type_ = PatternProcessor::LAUNCH_CAPTURE;
			break;
		case IntfNs::LAUNCH_SHIFT:
			pPatternProcessor_->type_ = PatternProcessor::LAUNCH_SHIFT;
			pPatternProcessor_->numSI_ = 1;
			break;
	}
}

// **************************************************************************
// Function   [ PatternReader::setPatternNum ]
// Commenter  [ CHT ]
// Synopsis   [ usage: Set the pattern vector according to the given size.
//							description:
//								Set each element of patternvector_ of the Pattern Processor
//								 to be default Pattern() with given input size (patternNum).
//							arguments:
// 								[in] patternNum : Pattern number to be set.
//						]
// Date       [ CHT started 2023/01/05 ]
// **************************************************************************
void PatternReader::setPatternNum(const int &patternNum)
{
	if (!success_)
	{
		return;
	}
	pPatternProcessor_->patternVector_.resize(patternNum);
	for (Pattern pattern : pPatternProcessor_->patternVector_)
	{
		pattern = Pattern();
	}
	curPattern_ = 0;
}

// **************************************************************************
// Function   [ PatternReader::addPattern ]
// Commenter  [ CHT ]
// Synopsis   [ usage: Read in a pattern and assign according values.
//							description:
//								For each input argument, assign according values to the
//								corresponding vector of the Pattern Processor if exists.
//							arguments:
// 								[in] pPI1 : The pointer to the first primary input pattern.
// 								[in] pPI2 : The pointer to the second primary input pattern.
// 								[in] pPPI : The pointer to the pseudo primary input pattern.
// 								[in] pSI  : The pointer to the shift in pattern.
// 								[in] pPO1 : The pointer to the first primary output pattern.
// 								[in] pPO2 : The pointer to the second primary output pattern.
// 								[in] pPPO : The pointer to the pseudo primary output pattern.
//						]
// Date       [ CHT started 2023/01/05 ]
// **************************************************************************
void PatternReader::addPattern(const char *const pPI1,
															 const char *const pPI2,
															 const char *const pPPI,
															 const char *const pSI,
															 const char *const pPO1,
															 const char *const pPO2,
															 const char *const pPPO)
{
	if (!success_)
	{
		return;
	}
	if (pPI1 && curPattern_ < (int)pPatternProcessor_->patternVector_.size())
	{
		pPatternProcessor_->patternVector_[curPattern_].PI1_.resize(pPatternProcessor_->numPI_);
		assignValue(pPatternProcessor_->patternVector_[curPattern_].PI1_, pPI1, pPatternProcessor_->numPI_);
	}
	if (pPI2 && curPattern_ < (int)pPatternProcessor_->patternVector_.size())
	{
		pPatternProcessor_->patternVector_[curPattern_].PI2_.resize(pPatternProcessor_->numPI_);
		assignValue(pPatternProcessor_->patternVector_[curPattern_].PI2_, pPI2, pPatternProcessor_->numPI_);
	}
	if (pPPI && curPattern_ < (int)pPatternProcessor_->patternVector_.size())
	{
		pPatternProcessor_->patternVector_[curPattern_].PPI_.resize(pPatternProcessor_->numPPI_);
		assignValue(pPatternProcessor_->patternVector_[curPattern_].PPI_, pPPI, pPatternProcessor_->numPPI_);
	}
	if (pSI && curPattern_ < (int)pPatternProcessor_->patternVector_.size())
	{
		pPatternProcessor_->patternVector_[curPattern_].SI_.resize(pPatternProcessor_->numSI_);
		assignValue(pPatternProcessor_->patternVector_[curPattern_].SI_, pSI, pPatternProcessor_->numSI_);
	}
	if (pPO1 && curPattern_ < (int)pPatternProcessor_->patternVector_.size())
	{
		pPatternProcessor_->patternVector_[curPattern_].PO1_.resize(pPatternProcessor_->numPO_);
		assignValue(pPatternProcessor_->patternVector_[curPattern_].PO1_, pPO1, pPatternProcessor_->numPO_);
	}
	if (pPO2 && curPattern_ < (int)pPatternProcessor_->patternVector_.size())
	{
		pPatternProcessor_->patternVector_[curPattern_].PO2_.resize(pPatternProcessor_->numPO_);
		assignValue(pPatternProcessor_->patternVector_[curPattern_].PO2_, pPO2, pPatternProcessor_->numPO_);
	}
	if (pPPO && curPattern_ < (int)pPatternProcessor_->patternVector_.size())
	{
		pPatternProcessor_->patternVector_[curPattern_].PPO_.resize(pPatternProcessor_->numPPI_);
		assignValue(pPatternProcessor_->patternVector_[curPattern_].PPO_, pPPO, pPatternProcessor_->numPPI_);
	}
	++curPattern_;
}

// **************************************************************************
// Function   [ PatternReader::assignValue ]
// Commenter  [ CHT ]
// Synopsis   [ usage: Set the pattern vector according to given size.
//							description:
//								For each bit in the range of input size, assign value to
//								the Value vector according to the input pattern content.
//							arguments:
// 								[in, out] valueVector : The Value vector to be modified.
// 								[in] pattern : The pattern content to be assigned to.
// 								[in] size : The length of the input pattern.
//						]
// Date       [ CHT started 2023/01/05 ]
// **************************************************************************
void PatternReader::assignValue(std::vector<Value> &valueVector, const char *const pattern,
																const int &size)
{
	for (int i = 0; i < size; ++i)
	{
		switch (pattern[i])
		{
			case '0':
				valueVector[i] = L;
				break;
			case '1':
				valueVector[i] = H;
				break;
			default:
				valueVector[i] = X;
				break;
		}
	}
}

// PatternWriter
// **************************************************************************
// Function   [ PatternWriter::writePattern ]
// Commenter  [ CHT ]
// Synopsis   [ usage: Write to LaDS's own *.pat pattern format.
//							description:
//								Output the pattern to the given input file name
//								with LaDS's own *.pat pattern format.
// 								Support at most 2 time frames.
//							arguments:
// 								[in] fname : The file name to be written to.
//								[out] bool : Output written successfully or not.
//						]
// Date       [ CHT started 2023/01/05 ]
// **************************************************************************
bool PatternWriter::writePattern(const char *const fname)
{
	FILE *fout = fopen(fname, "w");
	if (!fout)
	{
		fprintf(stderr, "**ERROR PatternWriter::writePat(): file");
		fprintf(stderr, "`%s' cannot be opened\n", fname);
		return false;
	}

	for (int i = 0; i < pCircuit_->numPI_; ++i)
	{
		fprintf(fout, "%s ",
						pCircuit_->pNetlist_->getTop()->getPort(pCircuit_->circuitGates_[i].cellId_)->name_);
	}
	fprintf(fout, " |\n");

	for (int i = pCircuit_->numPI_; i < pCircuit_->numPI_ + pCircuit_->numPPI_; ++i)
	{
		fprintf(fout, "%s ",
						pCircuit_->pNetlist_->getTop()->getCell(pCircuit_->circuitGates_[i].cellId_)->name_);
	}
	fprintf(fout, " |\n");

	int start = pCircuit_->numPI_ + pCircuit_->numPPI_ + pCircuit_->numComb_;
	for (int i = start; i < start + pCircuit_->numPO_; ++i)
	{
		fprintf(fout, "%s ",
						pCircuit_->pNetlist_->getTop()->getPort(pCircuit_->circuitGates_[i].cellId_)->name_);
	}
	fprintf(fout, "\n");

	switch (pPatternProcessor_->type_)
	{
		case PatternProcessor::BASIC_SCAN:
			fprintf(fout, "BASIC_SCAN\n");
			break;
		case PatternProcessor::LAUNCH_CAPTURE:
			fprintf(fout, "LAUNCH_ON_CAPTURE\n");
			break;
		case PatternProcessor::LAUNCH_SHIFT:
			fprintf(fout, "LAUNCH_ON_SHIFT\n");
			break;
	}

	fprintf(fout, "_num_of_pattern_%d\n", (int)pPatternProcessor_->patternVector_.size());

	for (int i = 0; i < (int)pPatternProcessor_->patternVector_.size(); ++i)
	{
		fprintf(fout, "_pattern_%d ", i + 1);
		if (!pPatternProcessor_->patternVector_[i].PI1_.empty())
		{
			for (int j = 0; j < pPatternProcessor_->numPI_; ++j)
			{
				if (pPatternProcessor_->patternVector_[i].PI1_[j] == L)
					fprintf(fout, "0");
				else if (pPatternProcessor_->patternVector_[i].PI1_[j] == H)
					fprintf(fout, "1");
				else
					fprintf(fout, "X");
			}
		}
		fprintf(fout, " | ");
		if (!pPatternProcessor_->patternVector_[i].PI2_.empty())
		{
			for (int j = 0; j < pPatternProcessor_->numPI_; ++j)
			{
				if (pPatternProcessor_->patternVector_[i].PI2_[j] == L)
					fprintf(fout, "0");
				else if (pPatternProcessor_->patternVector_[i].PI2_[j] == H)
					fprintf(fout, "1");
				else
					fprintf(fout, "X");
			}
		}
		fprintf(fout, " | ");
		if (!pPatternProcessor_->patternVector_[i].PPI_.empty())
		{
			for (int j = 0; j < pPatternProcessor_->numPPI_; ++j)
			{
				if (pPatternProcessor_->patternVector_[i].PPI_[j] == L)
					fprintf(fout, "0");
				else if (pPatternProcessor_->patternVector_[i].PPI_[j] == H)
					fprintf(fout, "1");
				else
					fprintf(fout, "X");
			}
		}
		fprintf(fout, " | ");
		if (!pPatternProcessor_->patternVector_[i].SI_.empty())
		{
			for (int j = 0; j < pPatternProcessor_->numSI_; ++j)
			{
				if (pPatternProcessor_->patternVector_[i].SI_[j] == L)
					fprintf(fout, "0");
				else if (pPatternProcessor_->patternVector_[i].SI_[j] == H)
					fprintf(fout, "1");
				else
					fprintf(fout, "X");
			}
		}
		fprintf(fout, " | ");
		if (!pPatternProcessor_->patternVector_[i].PO1_.empty())
		{
			for (int j = 0; j < pPatternProcessor_->numPO_; ++j)
			{
				if (pPatternProcessor_->patternVector_[i].PO1_[j] == L)
					fprintf(fout, "0");
				else if (pPatternProcessor_->patternVector_[i].PO1_[j] == H)
					fprintf(fout, "1");
				else
					fprintf(fout, "X");
			}
		}
		fprintf(fout, " | ");
		if (!pPatternProcessor_->patternVector_[i].PO2_.empty())
		{
			for (int j = 0; j < pPatternProcessor_->numPO_; ++j)
			{
				if (pPatternProcessor_->patternVector_[i].PO2_[j] == L)
					fprintf(fout, "0");
				else if (pPatternProcessor_->patternVector_[i].PO2_[j] == H)
					fprintf(fout, "1");
				else
					fprintf(fout, "X");
			}
		}
		fprintf(fout, " | ");
		if (!pPatternProcessor_->patternVector_[i].PPI_.empty())
		{
			for (int j = 0; j < pPatternProcessor_->numPPI_; ++j)
			{
				if (pPatternProcessor_->patternVector_[i].PPO_[j] == L)
					fprintf(fout, "0");
				else if (pPatternProcessor_->patternVector_[i].PPO_[j] == H)
					fprintf(fout, "1");
				else
					fprintf(fout, "X");
			}
		}
		fprintf(fout, "\n");
	}
	fprintf(fout, "\n");
	fclose(fout);

	return true;
}

// **************************************************************************
// Function   [ PatternWriter::writeLht ]
// Commenter  [ CHT ]
// Synopsis   [ usage: Write to Ling Hsio-Ting's pattern format.
//										 Not supported now!
//							description:
//								Output the pattern to the given input file name
//								with Ling Hsio-Ting's pattern format.
// 								Not supported now!
//							arguments:
// 								[in] fname : The file name to be written to.
//								[out] bool : Output written successfully or not.
//						]
// Date       [ CHT started 2023/01/05 ]
// **************************************************************************
bool PatternWriter::writeLht(const char *const fname)
{
	FILE *fout = fopen(fname, "w");
	if (!fout)
	{
		fprintf(stderr, "**ERROR PatternWriter::writePat(): file");
		fprintf(stderr, "`%s' cannot be opened\n", fname);
		return false;
	}

	for (size_t i = 0; i < pPatternProcessor_->patternVector_.size(); ++i)
	{
		fprintf(fout, "%d: ", (int)i + 1);
		if (!pPatternProcessor_->patternVector_[i].PI1_.empty())
		{
			for (int j = 0; j < pPatternProcessor_->numPI_; ++j)
			{
				if (pPatternProcessor_->patternVector_[i].PI1_[j] == L)
					fprintf(fout, "0");
				else if (pPatternProcessor_->patternVector_[i].PI1_[j] == H)
					fprintf(fout, "1");
				else
					fprintf(fout, "X");
			}
		}
		if (!pPatternProcessor_->patternVector_[i].PI2_.empty())
		{
			fprintf(fout, "->");
			for (int j = 0; j < pPatternProcessor_->numPI_; ++j)
			{
				if (pPatternProcessor_->patternVector_[i].PI2_[j] == L)
					fprintf(fout, "0");
				else if (pPatternProcessor_->patternVector_[i].PI2_[j] == H)
					fprintf(fout, "1");
				else
					fprintf(fout, "X");
			}
		}
		fprintf(fout, "_");
		if (!pPatternProcessor_->patternVector_[i].PPI_.empty())
		{
			fprintf(fout, "->");
			for (int j = 0; j < pPatternProcessor_->numPPI_; ++j)
			{
				if (pPatternProcessor_->patternVector_[i].PPI_[j] == L)
					fprintf(fout, "0");
				else if (pPatternProcessor_->patternVector_[i].PPI_[j] == H)
					fprintf(fout, "1");
				else
					fprintf(fout, "X");
			}
		}
		if (!pPatternProcessor_->patternVector_[i].SI_.empty())
		{
			fprintf(fout, "@");
			for (int j = 0; j < pPatternProcessor_->numSI_; ++j)
			{
				if (pPatternProcessor_->patternVector_[i].SI_[j] == L)
					fprintf(fout, "0");
				else if (pPatternProcessor_->patternVector_[i].SI_[j] == H)
					fprintf(fout, "1");
				else
					fprintf(fout, "X");
			}
		}
		fprintf(fout, " | ");

		if (!pPatternProcessor_->patternVector_[i].PO1_.empty() && pPatternProcessor_->type_ == PatternProcessor::BASIC_SCAN)
		{
			for (int j = 0; j < pPatternProcessor_->numPO_; ++j)
			{
				if (pPatternProcessor_->patternVector_[i].PO1_[j] == L)
					fprintf(fout, "0");
				else if (pPatternProcessor_->patternVector_[i].PO1_[j] == H)
					fprintf(fout, "1");
				else
					fprintf(fout, "X");
			}
		}

		if (!pPatternProcessor_->patternVector_[i].PO2_.empty() && (pPatternProcessor_->type_ == PatternProcessor::LAUNCH_CAPTURE || pPatternProcessor_->type_ == PatternProcessor::LAUNCH_SHIFT))
		{
			for (int j = 0; j < pPatternProcessor_->numPO_; ++j)
			{
				if (pPatternProcessor_->patternVector_[i].PO2_[j] == L)
					fprintf(fout, "0");
				else if (pPatternProcessor_->patternVector_[i].PO2_[j] == H)
					fprintf(fout, "1");
				else
					fprintf(fout, "X");
			}
		}
		fprintf(fout, "_");
		if (!pPatternProcessor_->patternVector_[i].PPO_.empty())
		{
			fprintf(fout, "->");
			for (int j = 0; j < pPatternProcessor_->numPPI_; ++j)
			{
				if (pPatternProcessor_->patternVector_[i].PPO_[j] == L)
					fprintf(fout, "0");
				else if (pPatternProcessor_->patternVector_[i].PPO_[j] == H)
					fprintf(fout, "1");
				else
					fprintf(fout, "X");
			}
		}
		fprintf(fout, "\n");
	}
	fprintf(fout, "\n");
	fclose(fout);

	return true;
}

// **************************************************************************
// Function   [ PatternWriter::writeAscii ]
// Commenter  [ CHT ]
// Synopsis   [ usage: Write to Mentor ASCii pattern format.
//										 Must be tested with mentor fastscan.
//							description:
//								Output the pattern to the given input file name
//								with Mentor ASCii pattern format.
// 								Should be tested with mentor fastscan.
//							arguments:
// 								[in] fname : The file name to be written to.
//								[out] bool : Output written successfully or not.
//						]
// Date       [ CHT started 2023/01/05 ]
// **************************************************************************
bool PatternWriter::writeAscii(const char *const fname)
{
	FILE *fout = fopen(fname, "w");
	int first_flag = 1;
	int seqCircuitCheck = 0;
	if (!fout)
	{
		fprintf(stderr, "**ERROR PatternWriter::writePat(): file");
		fprintf(stderr, "`%s' cannot be opened\n", fname);
		return false;
	}
	fprintf(fout, "ASCII_PATTERN_FILE_VERSION = 2;\n");
	fprintf(fout, "SETUP =\n");

	// input
	fprintf(fout, "declare input bus \"PI\" = ");
	for (size_t i = 0; i < pCircuit_->pNetlist_->getTop()->getNPort(); ++i)
	{
		Port *pPort = pCircuit_->pNetlist_->getTop()->getPort(i);
		if (pPort->type_ != Port::INPUT)
			continue;
		if (first_flag)
		{
			fprintf(fout, "\"/%s\"",
							pCircuit_->pNetlist_->getTop()->getPort(i)->name_);
			first_flag = 0;
		}
		else
		{
			fprintf(fout, ", \"/%s\"",
							pCircuit_->pNetlist_->getTop()->getPort(i)->name_);
		}
		if (!strcmp(pPort->name_, "CK"))
			seqCircuitCheck = 1;
	}
	fprintf(fout, ";\n");
	first_flag = 1;
	// output
	fprintf(fout, "declare output bus \"PO\" = ");
	for (size_t i = 0; i < pCircuit_->pNetlist_->getTop()->getNPort(); ++i)
	{
		Port *pPort = pCircuit_->pNetlist_->getTop()->getPort(i);
		if (pPort->type_ != Port::OUTPUT)
			continue;
		if (first_flag)
		{
			fprintf(fout, "\"/%s\"",
							pCircuit_->pNetlist_->getTop()->getPort(i)->name_);
			first_flag = 0;
		}
		else
		{
			fprintf(fout, ", \"/%s\"",
							pCircuit_->pNetlist_->getTop()->getPort(i)->name_);
		}
	}
	fprintf(fout, ";\n");

	// clock
	if (seqCircuitCheck)
	{
		fprintf(fout, "clock \"/CK\" =\n");
		fprintf(fout, "    off_state = 0;\n");
		fprintf(fout, "    pulse_width = 1;\n");
		fprintf(fout, "end;\n");

		// test setup
		fprintf(fout, "procedure test_setup \"test_setup\" =\n");
		fprintf(fout, "    force \"/CK\" 0 0;\n");
		fprintf(fout, "    force \"/test_si\" 0 0;\n");
		fprintf(fout, "    force \"/test_se\" 0 0;\n");
		fprintf(fout, "end;\n");

		// scan group
		fprintf(fout, "scan_group \"group1\" =\n");
		fprintf(fout, "    scan_chain \"chain1\" =\n");
		fprintf(fout, "    scan_in = \"/test_si\";\n");
		fprintf(fout, "    scan_out = \"/test_so\";\n");
		fprintf(fout, "    length = %d;\n", pCircuit_->numPPI_);
		fprintf(fout, "    end;\n");

		fprintf(fout, "    procedure shift \"group1_load_shift\" =\n");
		fprintf(fout, "    force_sci \"chain1\" 0;\n");
		fprintf(fout, "    force \"/CK\" 1 16;\n");
		fprintf(fout, "    force \"/CK\" 0 19;\n");
		fprintf(fout, "    period 32;\n");
		fprintf(fout, "    end;\n");

		fprintf(fout, "    procedure shift \"group1_unload_shift\" =\n");
		fprintf(fout, "    measure_sco \"chain1\" 0;\n");
		fprintf(fout, "    force \"/CK\" 1 16;\n");
		fprintf(fout, "    force \"/CK\" 0 19;\n");
		fprintf(fout, "    period 32;\n");
		fprintf(fout, "    end;\n");

		fprintf(fout, "    procedure load \"group1_load\" =\n");
		fprintf(fout, "    force \"/CK\" 0 0;\n");
		fprintf(fout, "    force \"/test_se\" 1 0;\n");
		fprintf(fout, "    force \"/test_si\" 0 0;\n");
		fprintf(fout, "    apply \"group1_load_shift\" %d 32;\n", pCircuit_->numPPI_);
		fprintf(fout, "    end;\n");

		fprintf(fout, "    procedure unload \"group1_unload\" =\n");
		fprintf(fout, "    force \"/CK\" 0 0;\n");
		fprintf(fout, "    force \"/test_se\" 1 0;\n");
		fprintf(fout, "    force \"/test_si\" 0 0;\n");
		fprintf(fout, "    apply \"group1_unload_shift\" %d 32;\n", pCircuit_->numPPI_);
		fprintf(fout, "    end;\n");

		fprintf(fout, "end;\n");
	}
	fprintf(fout, "end;\n\n");

	fprintf(fout, "SCAN_TEST =\n");
	// fprintf(fout, "apply \"test_setup\" 1 0;\n");

	for (int i = 0; i < (int)pPatternProcessor_->patternVector_.size(); ++i)
	{
		fprintf(fout, "pattern = %d", i);
		if (!pPatternProcessor_->patternVector_[i].PI2_.empty())
			fprintf(fout, " clock_sequential;\n");
		else
			fprintf(fout, ";\n");

		if (pPatternProcessor_->numPPI_)
		{
			fprintf(fout, "apply  \"group1_load\" 0 =\n");
			fprintf(fout, "chain \"chain1\" = \"");
			for (int j = pPatternProcessor_->numPPI_ - 1; j >= 0; --j)
			{
				if (pPatternProcessor_->patternVector_[i].PPI_[j] == L)
					fprintf(fout, "0");
				else if (pPatternProcessor_->patternVector_[i].PPI_[j] == H)
					fprintf(fout, "1");
				else
					fprintf(fout, "X");
			}
			fprintf(fout, "\";\n");
			fprintf(fout, "end;\n");
		}
		if (!pPatternProcessor_->patternVector_[i].PI1_.empty())
		{
			fprintf(fout, "force \"PI\" \"");
			if (seqCircuitCheck)
				fprintf(fout, "000");
			for (int j = 0; j < pPatternProcessor_->numPI_; ++j)
			{
				if (pPatternProcessor_->patternVector_[i].PI1_[j] == L)
					fprintf(fout, "0");
				else if (pPatternProcessor_->patternVector_[i].PI1_[j] == H)
					fprintf(fout, "1");
				else
					fprintf(fout, "X");
			}
			fprintf(fout, "\" 1;\n");
		}
		// fprintf(fout, "pulse \"/CK\" 2;\n");
		if (!pPatternProcessor_->patternVector_[i].PI2_.empty())
		{
			if (seqCircuitCheck)
				fprintf(fout, "pulse \"/CK\" 2;\n");
			fprintf(fout, "force \"PI\" \"");
			if (seqCircuitCheck)
				fprintf(fout, "000");
			for (int j = 0; j < pPatternProcessor_->numPI_; ++j)
			{
				if (pPatternProcessor_->patternVector_[i].PI2_[j] == L)
					fprintf(fout, "0");
				else if (pPatternProcessor_->patternVector_[i].PI2_[j] == H)
					fprintf(fout, "1");
				else
					fprintf(fout, "X");
			}
			fprintf(fout, "\" 3;\n");
		}
		if (!pPatternProcessor_->patternVector_[i].PO2_.empty())
		{
			fprintf(fout, "measure \"PO\" \"");
			if (seqCircuitCheck)
				fprintf(fout, "X");
			for (int j = 0; j < pPatternProcessor_->numPO_; ++j)
			{
				if (pPatternProcessor_->patternVector_[i].PO2_[j] == L)
					fprintf(fout, "0");
				else if (pPatternProcessor_->patternVector_[i].PO2_[j] == H)
					fprintf(fout, "1");
				else
					fprintf(fout, "X");
			}
			fprintf(fout, "\" 4;\n");
		}
		else if (!pPatternProcessor_->patternVector_[i].PO1_.empty())
		{
			fprintf(fout, "measure \"PO\" \"");
			if (seqCircuitCheck)
				fprintf(fout, "X");
			for (int j = 0; j < pPatternProcessor_->numPO_; ++j)
			{
				if (pPatternProcessor_->patternVector_[i].PO1_[j] == L)
					fprintf(fout, "0");
				else if (pPatternProcessor_->patternVector_[i].PO1_[j] == H)
					fprintf(fout, "1");
				else
					fprintf(fout, "X");
			}
			fprintf(fout, "\" 4;\n");
		}

		if (pPatternProcessor_->numPPI_)
		{
			fprintf(fout, "pulse \"/CK\" 5;\n");
			fprintf(fout, "apply  \"group1_unload\" 6 =\n");
			fprintf(fout, "chain \"chain1\" = \"");
			for (int j = pPatternProcessor_->numPPI_ - 1; j >= 0; --j)
			{
				if (pPatternProcessor_->patternVector_[i].PPO_[j] == L)
					fprintf(fout, "0");
				else if (pPatternProcessor_->patternVector_[i].PPO_[j] == H)
					fprintf(fout, "1");
				else
					fprintf(fout, "X");
			}
			fprintf(fout, "\";\n");
			fprintf(fout, "end;\n");
		}
	}
	fprintf(fout, "end;\n");

	// scan cells
	if (seqCircuitCheck)
	{
		fprintf(fout, "SCAN_CELLS =\n");
		fprintf(fout, "scan_group \"group1\" =\n");
		fprintf(fout, "scan_chain \"chain1\" =\n");
		for (int i = pCircuit_->numPI_ + pCircuit_->numPPI_ - 1; i >= pCircuit_->numPI_; --i)
		{
			fprintf(fout,
							"scan_cell = %d MASTER FFFF \"/%s\" \"I1\" \"SI\" \"Q\";\n",
							pCircuit_->numPI_ + pCircuit_->numPPI_ - 1 - i, pCircuit_->pNetlist_->getTop()->getCell(pCircuit_->circuitGates_[i].cellId_)->name_);
		}

		fprintf(fout, "end;\n");
		fprintf(fout, "end;\n");
		fprintf(fout, "end;\n");
	}
	fprintf(fout, "\n");
	fclose(fout);

	return true;
}

// **************************************************************************
// Function   [ PatternWriter::writeSTIL ]
// Commenter  [ CHT ]
// Synopsis   [ usage: Write to STIL pattern format.
//										 Must be tested with tetramax.
//							description:
//								Output the pattern to the given input file name
//								with STIL pattern format
// 								Should be tested with tetramax.
//							arguments:
// 								[in] fname : The file name to be written to.
//								[out] bool : Output written successfully or not.
//						]
// Date       [ CHT started 2023/01/05 ]
// **************************************************************************
bool PatternWriter::writeSTIL(const char *const fname)
{
	std::vector<std::string> PI_Order;
	std::vector<std::string> SCAN_Order;
	std::vector<std::string> PO_Order;

	std::cout << "==========\n"; //

	PI_Order.push_back("CK");
	PI_Order.push_back("test_si");
	PI_Order.push_back("test_se");
	PO_Order.push_back("test_so");

	for (int i = 0; i < pCircuit_->numPI_; ++i)
	{
		PI_Order.push_back(pCircuit_->pNetlist_->getTop()->getPort(pCircuit_->circuitGates_[i].cellId_)->name_);
	}

	for (int i = pCircuit_->numPI_; i < pCircuit_->numPI_ + pCircuit_->numPPI_; ++i)
	{
		SCAN_Order.push_back(pCircuit_->pNetlist_->getTop()->getCell(pCircuit_->circuitGates_[i].cellId_)->name_);
	}

	int start = pCircuit_->numPI_ + pCircuit_->numPPI_ + pCircuit_->numComb_;
	for (int i = start; i < start + pCircuit_->numPO_; ++i)
	{
		PO_Order.push_back(pCircuit_->pNetlist_->getTop()->getPort(pCircuit_->circuitGates_[i].cellId_)->name_);
	}

	std::cout << "PI_ORDER ";
	for (std::string pi : PI_Order)
	{
		std::cout << pi << " ";
	}
	std::cout << "\n";

	std::cout << "SCAN_ORDER ";
	for (std::string scan : SCAN_Order)
	{
		std::cout << scan << " ";
	}
	std::cout << "\n";
	std::cout << "PO_ORDER ";
	for (std::string po : PO_Order)
	{
		std::cout << po << " ";
	}
	std::cout << "\n";

	std::string Processor_Mode = "";
	int pattern_size = (int)pPatternProcessor_->patternVector_.size();

	switch (pPatternProcessor_->type_)
	{
		case PatternProcessor::BASIC_SCAN:
			Processor_Mode = "BASIC_SCAN";
			break;
		case PatternProcessor::LAUNCH_CAPTURE:
			Processor_Mode = "LAUNCH_ON_CAPTURE";
			break;
		case PatternProcessor::LAUNCH_SHIFT:
			Processor_Mode = "LAUNCH_ON_SHIFT";
			break;
	}

	std::cout << Processor_Mode << " " << pattern_size << "\n"; //
	std::vector<std::map<std::string, std::string>> patternList;
	for (Pattern pattern : pPatternProcessor_->patternVector_)
	{
		std::map<std::string, std::string> map_pattern;

		if (!pattern.PI1_.empty())
		{
			map_pattern["pPI1"] = "";
			for (int j = 0; j < pPatternProcessor_->numPI_; ++j)
			{
				if (pattern.PI1_[j] == L)
					map_pattern["pPI1"] += "0";
				else if (pattern.PI1_[j] == H)
					map_pattern["pPI1"] += "1";
				else
					map_pattern["pPI1"] += "N";
			}
		}

		if (!pattern.PPI_.empty())
		{
			map_pattern["pPPI1"] = "";
			for (int j = 0; j < pPatternProcessor_->numPPI_; ++j)
			{
				if (pattern.PPI_[j] == L)
					map_pattern["pPPI1"] += "0";
				else if (pattern.PPI_[j] == H)
					map_pattern["pPPI1"] += "1";
				else
					map_pattern["pPPI1"] += "N";
			}
			reverse(map_pattern["pPPI1"].begin(), map_pattern["pPPI1"].end());
		}
		if (!pattern.PO1_.empty())
		{
			map_pattern["pPO1"] = "";
			for (int j = 0; j < pPatternProcessor_->numPO_; ++j)
			{
				if (pattern.PO1_[j] == L)
					map_pattern["pPO1"] += "L";
				else if (pattern.PO1_[j] == H)
					map_pattern["pPO1"] += "H";
				else
					map_pattern["pPO1"] += "N";
			}
		}

		if (!pattern.PPI_.empty())
		{
			map_pattern["pPPO"] = "";
			for (int j = 0; j < pPatternProcessor_->numPPI_; ++j)
			{
				if (pattern.PPO_[j] == L)
					map_pattern["pPPO"] += "L";
				else if (pattern.PPO_[j] == H)
					map_pattern["pPPO"] += "H";
				else
					map_pattern["pPPO"] += "N";
			}
			std::reverse(map_pattern["pPPO"].begin(), map_pattern["pPPO"].end());
		}
		patternList.push_back(map_pattern);
	}
	for (size_t i = 0; i < patternList.size(); ++i)
	{
		std::cout << "\npattern_" << i + 1 << "\n";
		std::map<std::string, std::string>::iterator iter;
		for (iter = patternList[i].begin(); iter != patternList[i].end(); iter++)
		{
			std::cout << iter->first << " " << iter->second << "\n";
		}
	} //

	std::cout << "==========\n";

	std::ofstream os(fname);
	os << "STIL 1.0;\n\n";

	/////signals/////
	os << "Signals {\n";
	for (std::string pi : PI_Order)
	{
		os << "   \"" + pi + "\" " + "In";
		if (pi == "test_si")
		{
			os << " { ScanIn; }\n";
		}
		else
		{
			os << ";\n";
		}
	}

	for (std::string po : PO_Order)
	{
		os << "   \"" + po + "\" " + "Out";
		if (po == "test_so")
		{
			os << " { ScanOut; }\n";
		}
		else
		{
			os << ";\n";
		}
	}
	os << "}\n\n";
	/////SignalGroups/////
	os << "SignalGroups {\n";
	std::string _pi_in = "";
	std::string _po_out = "";
	std::string _in_timing = "";
	for (size_t i = 0; i < PI_Order.size(); ++i)
	{
		_pi_in = _pi_in + "\"" + PI_Order[i] + "\"";
		if (i != PI_Order.size() - 1)
		{
			_pi_in += " + ";
		}

		if (PI_Order[i] != "CK")
		{
			_in_timing = _in_timing + "\"" + PI_Order[i] + "\"";
			if (i != PI_Order.size() - 1)
			{
				_in_timing += " + ";
			}
		}
	}
	for (size_t i = 0; i < PO_Order.size(); ++i)
	{
		_po_out = _po_out + "\"" + PO_Order[i] + "\"";
		if (i != PO_Order.size() - 1)
		{
			_po_out += " + ";
		}
	}
	os << "   \"_pi\" = \'" + _pi_in + "\';\n";
	os << "   \"_in\" = \'" + _pi_in + "\';\n";
	os << "   \"_si\" = \'\"test_si\"\' { ScanIn; }\n";
	os << "   \"_po\" = \'" + _po_out + "\';\n";
	os << "   \"_out\" = \'" + _po_out + "\';\n";
	os << "   \"_so\" = \'\"test_so\"\' { ScanOut; }\n";
	os << "   \"_default_In_Timing_\" = \'" + _in_timing + "\';\n";
	os << "   \"_default_Out_Timing_\" = \'" + _po_out + "\';\n";
	os << "}\n\n";
	/////Timing/////
	os << "Timing {\n";
	os << "   WaveformTable \"_default_WFT_\" {\n";
	os << "       Period \'100ns\';\n";
	os << "       Waveforms {\n";
	os << "           \"CK\" { 0 { \'0ns\' D; } }\n";
	os << "           \"CK\" { P { \'0ns\' D; \'50ns\' U; \'75ns\' D; } }\n";
	os << "           \"CK\" { 1 { \'0ns\' U; } }\n";
	os << "           \"CK\" { Z { \'0ns\' Z; } }\n";
	os << "           \"_default_In_Timing_\" { 0 { \'0ns\' D; } }\n";
	os << "           \"_default_In_Timing_\" { 1 { \'0ns\' U; } }\n";
	os << "           \"_default_In_Timing_\" { Z { \'0ns\' Z; } }\n";
	os << "           \"_default_In_Timing_\" { N { \'0ns\' N; } }\n";
	os << "           \"_default_Out_Timing_\" { X { \'0ns\' X; } }\n";
	os << "           \"_default_Out_Timing_\" { H { \'0ns\' X; \' 90 ns\' H; } }\n";
	os << "           \"_default_Out_Timing_\" { T { \'0ns\' X; \' 90 ns\' T; } }\n";
	os << "           \"_default_Out_Timing_\" { L { \'0ns\' X; \' 90 ns\' L; } }\n";
	os << "       }\n";
	os << "   }\n";
	os << "}\n\n";

	/////ScanStructures/////
	os << "ScanStructures {\n";
	os << "   ScanChain \"chain1\" {\n"; //
	os << "       ScanLength " << SCAN_Order.size() << ";\n";
	os << "       ScanIn \"test_si\";\n";
	os << "       ScanOut \"test_so\";\n";
	os << "       ScanInversion 0;\n";
	os << "       ScanCells";

	for (std::string scan : SCAN_Order)
	{
		os << " \"TOP." << scan << ".SI\"";
	}
	os << ";\n";
	os << "       ScanMasterClock \"CK\" ;\n";
	os << "   }\n";
	os << "}\n\n";

	/////PatternBurst PatternExec/////
	os << "PatternBurst \"_burst_\" {\n";
	os << "   PatList { \"_pattern_\" { } }\n";
	os << "}\n\n";
	os << "PatternExec {\n";
	os << "   PatternBurst \"_burst_\";\n";
	os << "}\n\n";

	/////Procedures/////
	os << "Procedures {\n";
	os << "   \"load_unload\" {\n";
	os << "       W \"_default_WFT_\";\n";
	os << "       C { \"test_si\"=0; \"CK\"=0; \"test_se\"=1; }\n";
	os << "       V { \"_so\"=#; }\n";
	os << "       Shift {\n";
	os << "           W \"_default_WFT_\";\n";
	os << "           V { \"_si\"=#; \"_so\"=#; \"CK\"=P; }\n";
	os << "       }\n";
	os << "   }\n";
	os << "   \"capture_CK\" {\n";
	os << "       W \"_default_WFT_\";\n";
	os << "       F { \"test_se\"=0; }\n";
	os << "       C { \"_po\"=\\r" << PO_Order.size() << " X ; }\n";
	os << "       \"forcePI\": V { \"_pi\"=\\r" << PI_Order.size() << " # ; }\n";
	os << "       \"measurePO\": V { \"_po\"=\\r" << PO_Order.size() << " # ; }\n";
	os << "       C { \"_po\"=\\r" << PO_Order.size() << " X ; }\n";
	os << "       \"pulse\": V { \"CK\"=P; }\n";
	os << "   }\n";
	os << "   \"capture\" {\n";
	os << "       W \"_default_WFT_\";\n";
	os << "       F { \"test_se\"=0; }\n";
	os << "       C { \"_po\"=\\r" << PO_Order.size() << " X ; }\n";
	os << "       \"forcePI\": V { \"_pi\"=\\r" << PI_Order.size() << " # ; }\n";
	os << "       \"measurePO\": V { \"_po\"=\\r" << PO_Order.size() << " # ; }\n";
	os << "   }\n";
	os << "}\n\n";

	/////MacroDefs/////
	os << "MacroDefs {\n";
	os << "   \"test_setup\" {\n";
	os << "       W \"_default_WFT_\";\n";
	os << "       V { \"test_se\"=0; \"CK\"=0; }\n";
	os << "   }\n";
	os << "}\n\n";

	/////Pattern/////
	os << "Pattern \"_pattern_\" {\n";
	os << "   W \"_default_WFT_\";\n";
	os << "   \"precondition all Signals\": C { \"_pi\"=\\r" << PI_Order.size() << " 0 ; \"_po\"=\\r" << PO_Order.size() << " X ; }\n";
	os << "   Macro \"test_setup\";\n";

	for (int i = 0; i < pattern_size; ++i)
	{
		os << "   \"pattern " << i << "\":\n";
		os << "       Call \"load_unload\" {\n";
		if (i > 0)
		{
			os << "           \"test_so\"=" << patternList[i - 1]["pPPO"] << ";\n";
		}
		os << "           \"test_si\"=" << patternList[i]["pPPI1"] << ";\n";
		os << "       }\n";
		os << "       Call \"capture_CK\" {\n";
		os << "           \"_pi\"=";
		for (size_t j = patternList[i]["pPI1"].length(); j < PI_Order.size(); ++j)
		{
			os << "0";
		}
		os << patternList[i]["pPI1"] << ";\n";
		os << "           \"_po\"=";
		for (size_t j = patternList[i]["pPO1"].length(); j < PO_Order.size(); ++j)
		{
			os << "L";
		}
		os << patternList[i]["pPO1"] << ";\n";
		os << "       }\n";
	}

	os << "   \"end " << pattern_size - 1 << " unload\":\n";
	os << "       Call \"load_unload\" {\n";
	os << "           \"test_so\"=" << patternList[pattern_size - 1]["pPPO"] << ";\n";
	os << "       }\n";
	os << "}";

	os.close();

	return true;
}

// **************************************************************************
// Function   [ PatternWriter::writeProcedure ]
// Commenter  [ CHT ]
// Synopsis   [ usage: Write the procedure setup.
//							description:
//								Output the procedure setup to the given input file name,
//								including time scale, strobe_window time, timeplate _default_WFT_ etc.
//							arguments:
// 								[in] fname : The file name to be written to.
//								[out] bool : Output written successfully or not.
//						]
// Date       [ CHT started 2023/01/05 ]
// **************************************************************************
bool ProcedureWriter::writeProcedure(const char *const fname)
{
	FILE *fout = fopen(fname, "w");
	if (!fout)
	{
		fprintf(stderr, "**ERROR PatternWriter::writePat(): file");
		fprintf(stderr, "`%s' cannot be opened\n", fname);
		return false;
	}

	fprintf(fout, "set time scale 1.000000 ns ;\n");
	fprintf(fout, "set strobe_window time 3 ;\n\n");
	fprintf(fout, "timeplate _default_WFT_ =\n");
	fprintf(fout, "force_pi 0 ;\n");
	fprintf(fout, "measure_po 29 ;\n");
	fprintf(fout, "pulse CK 16 3;\n");
	fprintf(fout, "period 32 ;\n");
	fprintf(fout, "end;\n\n");
	fprintf(fout, "procedure shift =\n");
	fprintf(fout, "scan_group group1 ;\n");
	fprintf(fout, "timeplate _default_WFT_ ;\n");
	fprintf(fout, "cycle =\n");
	fprintf(fout, "force_sci ;\n");
	fprintf(fout, "pulse CK ;\n");
	fprintf(fout, "measure_sco ;\n");
	fprintf(fout, "end;\n");
	fprintf(fout, "end;\n\n");
	fprintf(fout, "procedure load_unload =\n");
	fprintf(fout, "scan_group group1 ;\n");
	fprintf(fout, "timeplate _default_WFT_ ;\n");
	fprintf(fout, "cycle =\n");
	fprintf(fout, "force CK 0 ;\n");
	fprintf(fout, "force test_se 1 ;\n");
	fprintf(fout, "force test_si 0 ;\n");
	fprintf(fout, "measure_sco ;\n");
	fprintf(fout, "end ;\n");
	fprintf(fout, "apply shift %d;\n", pCircuit_->numPPI_);
	fprintf(fout, "end;\n");
	fprintf(fout, "procedure test_setup =\n");
	fprintf(fout, "timeplate _default_WFT_ ;\n");
	fprintf(fout, "cycle =\n");
	fprintf(fout, "force CK 0 ;\n");
	fprintf(fout, "force test_se 0 ;\n");
	fprintf(fout, "end;\n");
	fprintf(fout, "end;\n");
	fclose(fout);
	return true;
}
