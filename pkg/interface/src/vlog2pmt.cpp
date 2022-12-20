// **************************************************************************
// File       [ vlog2pmt.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ Flatten verilog netlist to primitives ]
// Date       [ 2011/04/19 created ]
// **************************************************************************

#include <cstdlib>
#include <iostream>
#include <fstream>

#include "netlist_builder.h"
#include "techlib_builder.h"

using namespace IntfNs;

void printCell(Cell *c, std::ofstream &fout);
void printPmt(Pmt *pmt, Cell *c, std::ofstream &fout);
void printInst(Cell *inst, Cell *c, std::ofstream &fout);
// void printMux(Pmt *pmt, Cell *c, std::ofstream &fout);
std::string getPmtNetName(Port *p, Pmt *pmt, Cell *c);
void printDff(Cell *c, std::ofstream &fout);

const char hier = '/';
// const std::string muxS0 = "S0";
// const std::string muxA = "A";
// const std::string muxB = "B";
// const std::string muxY = "Y";

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		std::cerr << "**ERROR main(): need netlist, MDT lib";
		std::cerr << "\n";
		exit(0);
	}

	// open output file
	/*
	std::ofstream fout(argv[3]);
	if (!fout.good()) {
			std::cerr << "**ERROR main(): file `" << argv[3] << "' cannot be opened";
			std::cerr << "\n";
			exit(0);
	}
	*/

	// read technology library
	Techlib lib;
	MdtFile *libBlder = new MdtLibBuilder(&lib);

	std::cout << "> Reading technology library...\n";
	if (!libBlder->read(argv[2], true))
	{
		std::cerr << "**ERROR main(): MDT lib builder error\n";
		delete libBlder;
		libBlder = NULL;
		exit(0);
	}
	std::cout << "> Checking technology library...\n";
	if (!lib.check(true))
	{
		std::cerr << "**ERROR main(): MDT lib error\n";
		delete libBlder;
		libBlder = NULL;
		exit(0);
	}

	// read netlist
	Netlist nl;
	nl.setTechlib(&lib);
	VlogFile *nlBlder = new VlogNlBuilder(&nl);

	std::cout << "> Reading netlist...\n";
	if (!nlBlder->read(argv[1], true))
	{
		std::cerr << "**ERROR main(): verilog builder error\n";
		delete nlBlder;
		libBlder = NULL;
		exit(0);
	}

	std::cout << "> Removing floating nets...\n";
	nl.removeFloatingNets();

	std::cout << "> Checking netlist...\n";
	if (!nl.check(true))
	{
		std::cerr << "**ERROR main(): netlist error\n";
		delete nlBlder;
		libBlder = NULL;
		exit(0);
	}

	/*
	std::cout << "> Writing netlist..." << "\n";
	// print module
	fout << "module " << nl.getTop()->name_ << "(";
	for (size_t i = 0; i < nl.getTop()->getNPort(); ++i) {
			Port *p = nl.getTop()->getPort(i);
			fout << p->name_;
			if (i + 1 != nl.getTop()->getNPort())
					fout << ", ";
	}
	fout << ");" << "\n" << "\n";

	// print ports
	for (size_t i = 0; i < nl.getTop()->getNPort(); ++i) {
			Port *p = nl.getTop()->getPort(i);
			switch (p->type_) {
					case Port::INPUT:
							fout << "input ";
							break;
					case Port::OUTPUT:
							fout << "output ";
							break;
					case Port::INOUT:
							fout << "inout ";
							break;
					default:
							fout << "NA ";
			}
			fout << p->name_ << ";" << "\n";
	}
	fout << "\n";


	// print cells
	for (size_t i = 0; i < nl.getTop()->getNCell(); ++i) {
			Cell *c = nl.getTop()->getCell(i);
			if (!lib.hasDff(c->libc_->id_) && !lib.hasDlat(c->libc_->id_))
					printCell(c, fout);
			else
					printDff(c, fout);
	}

	// print assigns
	for (size_t i = 0; i < nl.getTop()->getNNet(); ++i) {
			Net *n = nl.getTop()->getNet(i);
			if (n->getNEqv() == 0)
					continue;
			bool outNet = true;
			for (size_t j = 0; j < n->getNPort(); ++j) {
					Port *p = n->getPort(j);
					if (p->top_ == nl.getTop() && p->type_ == Port::INPUT
							|| p->top_ != nl.getTop() && p->type_ == Port::OUTPUT) {
							outNet = false;
							break;
					}
			}
			if (!outNet)
					continue;
			Net *nin = NULL;
			NetSet eqv = nl.getTop()->getEqvNets(i);
			NetSet::iterator it = eqv.begin();
			for ( ; it != eqv.end(); ++it) {
					for (size_t j = 0; j < (*it)->getNPort(); ++j) {
							Port *p = (*it)->getPort(j);
							if (p->top_ == nl.getTop() && p->type_ == Port::INPUT
									|| p->top_ != nl.getTop() && p->type_ == Port::OUTPUT) {
									nin = (*it);
									break;
							}
					}
			}
			if (!nin)
					continue;
			fout << "assign " << n->name_ << " = " << nin->name_ << ";" << "\n";
	}

	// print endmodule
	fout << "\n" << "endmodule" << "\n" << "\n";
	*/

	delete nlBlder;
	delete libBlder;
	nlBlder = NULL;
	libBlder = NULL;
	return 0;
}

void printCell(Cell *c, std::ofstream &fout)
{
	Cell *libc = c->libc_;
	for (size_t i = 0; i < libc->getNCell(); ++i)
	{
		Cell *in = libc->getCell(i);
		if (in->isPmt_)
		{
			printPmt((Pmt *)in, c, fout);
		}
		else
		{
			printInst(in, c, fout);
		}
	}
}

void printPmt(Pmt *pmt, Cell *c, std::ofstream &fout)
{
	int inSize = (int)pmt->getNPort() - 1;
	char buf[8];
	switch (pmt->type_)
	{
		case Pmt::BUF:
			fout << "BUFX1";
			break;
		case Pmt::INV:
		case Pmt::INVF:
			fout << "INVX1";
			break;
		case Pmt::AND:
			fout << "AND";
			sprintf(buf, "%d", inSize);
			fout << buf << "X1";
			break;
		case Pmt::NAND:
			fout << "NAND";
			sprintf(buf, "%d", inSize);
			fout << buf << "X1";
			break;
		case Pmt::OR:
			fout << "OR";
			sprintf(buf, "%d", inSize);
			fout << buf << "X1";
			break;
		case Pmt::NOR:
			fout << "NOR";
			sprintf(buf, "%d", inSize);
			fout << buf << "X1";
			break;
		case Pmt::XOR:
			fout << "XOR";
			sprintf(buf, "%d", inSize);
			fout << buf << "X1";
			break;
		case Pmt::XNOR:
			fout << "XNOR";
			sprintf(buf, "%d", inSize);
			fout << buf << "X1";
			break;
		default:
			fout << "NA";
			break;
	}
	fout << " " << c->name_ << hier << pmt->name_ << " (";

	for (size_t j = 0; j < pmt->getNPort(); ++j)
	{
		fout << "." << pmt->getPort(j)->name_ << "(";
		fout << getPmtNetName(pmt->getPort(j), pmt, c) << ")";
		if (j + 1 != pmt->getNPort())
		{
			fout << ", ";
		}
	}
	fout << ");"
			 << "\n";
}

void printInst(Cell *inst, Cell *c, std::ofstream &fout) {}

std::string getPmtNetName(Port *p, Pmt *pmt, Cell *c)
{
	Cell *libc = c->libc_;
	Net *n = p->exNet_;
	if (!n)
	{
		return "";
	}
	bool intern = true;
	std::string netName = "";
	for (size_t k = 0; k < n->getNPort(); ++k)
	{
		Port *p = n->getPort(k);
		if (p->top_ == libc)
		{
			if (c->getPort(p->name_)->exNet_)
			{
				netName = c->getPort(p->name_)->exNet_->name_;
			}
			intern = false;
			break;
		}
	}

	if (intern)
	{
		netName = std::string(c->name_) + hier + std::string(n->name_);
	}
	return netName;
}

void printDff(Cell *c, std::ofstream &fout)
{
	fout << c->libc_->name_ << " " << c->name_ << " (";
	bool hasPort = false;
	for (size_t i = 0; i < c->getNPort(); ++i)
	{
		Net *n = c->getPort(i)->exNet_;
		if (!n)
		{
			continue;
		}
		if (hasPort)
		{
			fout << ", " << std::flush;
		}
		fout << "." << c->getPort(i)->name_ << "(";
		fout << n->name_ << ")";
		hasPort = true;
	}
	fout << ");"
			 << "\n";
}

/*
void printMux(Pmt *pmt, Cell *c, std::ofstream &fout) {
		std::string internInv = c->name_ + hier + pmt->name_ + "_inv";
		std::string internAnd1 = c->name_ + hier + pmt->name_ + "_and1";
		std::string internAnd2 = c->name_ + hier + pmt->name_ + "_and2";

		fout << "INVX1 " << c->name_ << hier << pmt->name_ << hier;
		fout << "INV (";
		fout << ".A(" << getPmtNetName(pmt->getPort(muxS0), pmt, c);
		fout << "), ";
		fout << ".Y(" << internInv << ") );" << "\n";

		fout << "AND2X1 " << c->name_ << hier << pmt->name_ << hier;
		fout << "AND1 (";
		fout << ".A(" << internInv << "), ";
		fout << ".B(" << getPmtNetName(pmt->getPort(muxA), pmt, c);
		fout << "), ";
		fout << ".Y(" << internAnd1 << ") );" << "\n";

		fout << "AND2X1 " << c->name_ << hier << pmt->name_ << hier;
		fout << "AND2 (";
		fout << ".A(" << getPmtNetName(pmt->getPort(muxS0), pmt, c);
		fout << "), ";
		fout << ".B(" << getPmtNetName(pmt->getPort(muxB), pmt, c);
		fout << "), ";
		fout << ".Y(" << internAnd2 << ") );" << "\n";

		fout << "OR2X1 " << c->name_ << hier << pmt->name_ << hier;
		fout << "OR (";
		fout << ".A(" << internAnd1 << "), ";
		fout << ".B(" << internAnd2 << "), ";
		fout << ".Y(" << getPmtNetName(pmt->getPort(muxY), pmt, c);
		fout << ") );" << "\n";
} */