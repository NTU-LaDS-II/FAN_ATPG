#include "atpg.h"

using namespace CoreNs;

void Atpg::calSCOAP()
{
	// cc0, cc1 and co default is 0
	// check if is changed before
	for (int gateID = 0; gateID < pCircuit_->tgate_; ++gateID)
	{
		Gate &gate = pCircuit_->gates_[gateID];
		if (gate.cc0_ != 0)
		{
			std::cerr << "cc0_ is not -1\n";
			std::cin.get();
		}
		if (gate.cc1_ != 0)
		{
			std::cerr << "cc1_ is not -1\n";
			std::cin.get();
		}
		if (gate.co_ != 0)
		{
			std::cerr << "co_ is not -1\n";
			std::cin.get();
		}
	}

	// array for xor2, xor3, xnor2, xnor3
	// xor2 xnor2 : {00,01,10,11}
	// xor3 xnor3 : {000, 001, 010, 011, 100, 101, 110, 111}
	int xorcc[8] = {0};
	Gate gateInputs[3];

	// calculate cc0 and cc1 starting from PI and PPI
	for (int gateID = 0; gateID < pCircuit_->tgate_; ++gateID)
	{
		Gate &gate = pCircuit_->gates_[gateID];
		switch (gate.type_)
		{
			case Gate::PPI:
			case Gate::PI:
				gate.cc0_ = 1;
				gate.cc1_ = 1;
				break;
			case Gate::PO:
			case Gate::PPO:
			case Gate::BUF:
				gate.cc0_ = pCircuit_->gates_[gate.fis_[0]].cc0_;
				gate.cc1_ = pCircuit_->gates_[gate.fis_[0]].cc1_;
				break;
			case Gate::INV:
				gate.cc0_ = pCircuit_->gates_[gate.fis_[0]].cc1_ + 1;
				gate.cc1_ = pCircuit_->gates_[gate.fis_[0]].cc0_ + 1;
				break;
			case Gate::AND2:
			case Gate::AND3:
			case Gate::AND4:
				for (int j = 0; j < gate.nfi_; j++)
				{
					Gate &gateInput = pCircuit_->gates_[gate.fis_[j]];
					if (j == 0 || (gateInput.cc0_ < gate.cc0_))
					{
						gate.cc0_ = gateInput.cc0_;
					}
					gate.cc1_ += gateInput.cc1_;
				}
				gate.cc1_++;
				gate.cc0_++;
				break;

			case Gate::NAND2:
			case Gate::NAND3:
			case Gate::NAND4:
				for (int j = 0; j < gate.nfi_; j++)
				{
					Gate &gateInput = pCircuit_->gates_[gate.fis_[j]];
					if (j == 0 || (gateInput.cc0_ < gate.cc1_))
					{
						gate.cc1_ = gateInput.cc0_;
					}
					gate.cc0_ += gateInput.cc1_;
				}
				gate.cc0_++;
				gate.cc1_++;
				break;

			case Gate::OR2:
			case Gate::OR3:
			case Gate::OR4:
				for (int j = 0; j < gate.nfi_; j++)
				{
					Gate &gateInput = pCircuit_->gates_[gate.fis_[j]];
					if (j == 0 || (gateInput.cc1_ < gate.cc1_))
					{
						gate.cc1_ = gateInput.cc1_;
					}
					gate.cc0_ += gateInput.cc0_;
				}
				gate.cc0_++;
				gate.cc1_++;
				break;
			case Gate::NOR2:
			case Gate::NOR3:
			case Gate::NOR4:
				for (int j = 0; j < gate.nfi_; j++)
				{
					Gate &gateInput = pCircuit_->gates_[gate.fis_[j]];
					if (j == 0 || (gateInput.cc1_ < gate.cc0_))
					{
						gate.cc0_ = gateInput.cc1_;
					}
					gate.cc1_ += gateInput.cc0_;
				}
				gate.cc0_++;
				gate.cc1_++;
				break;
			case Gate::XOR2:
				gateInputs[0] = pCircuit_->gates_[gate.fis_[0]];
				gateInputs[1] = pCircuit_->gates_[gate.fis_[1]];
				xorcc[0] = gateInputs[0].cc0_ + gateInputs[1].cc0_;
				xorcc[1] = gateInputs[0].cc0_ + gateInputs[1].cc1_;
				xorcc[2] = gateInputs[0].cc1_ + gateInputs[1].cc0_;
				xorcc[3] = gateInputs[0].cc1_ + gateInputs[1].cc1_;
				gate.cc0_ = std::min(xorcc[0], xorcc[3]);
				gate.cc1_ = std::min(xorcc[1], xorcc[2]);
				gate.cc0_++;
				gate.cc1_++;
				break;
			case Gate::XOR3:
				gateInputs[0] = pCircuit_->gates_[gate.fis_[0]];
				gateInputs[1] = pCircuit_->gates_[gate.fis_[1]];
				gateInputs[2] = pCircuit_->gates_[gate.fis_[2]];
				xorcc[0] = gateInputs[0].cc0_ + gateInputs[1].cc0_ + gateInputs[2].cc0_;
				xorcc[1] = gateInputs[0].cc0_ + gateInputs[1].cc0_ + gateInputs[2].cc1_;
				xorcc[2] = gateInputs[0].cc0_ + gateInputs[1].cc1_ + gateInputs[2].cc0_;
				xorcc[3] = gateInputs[0].cc0_ + gateInputs[1].cc1_ + gateInputs[2].cc1_;
				xorcc[4] = gateInputs[0].cc1_ + gateInputs[1].cc0_ + gateInputs[2].cc0_;
				xorcc[5] = gateInputs[0].cc1_ + gateInputs[1].cc0_ + gateInputs[2].cc1_;
				xorcc[6] = gateInputs[0].cc1_ + gateInputs[1].cc1_ + gateInputs[2].cc0_;
				xorcc[7] = gateInputs[0].cc1_ + gateInputs[1].cc1_ + gateInputs[2].cc1_;
				gate.cc0_ = std::min(xorcc[0], xorcc[7]);
				for (int j = 1; j < 7; j++)
				{
					if (j == 1 || xorcc[j] < gate.cc1_)
					{
						gate.cc1_ = xorcc[j];
					}
				}
				gate.cc0_++;
				gate.cc1_++;
				break;
			case Gate::XNOR2:
				gateInputs[0] = pCircuit_->gates_[gate.fis_[0]];
				gateInputs[1] = pCircuit_->gates_[gate.fis_[1]];
				xorcc[0] = gateInputs[0].cc0_ + gateInputs[1].cc0_;
				xorcc[1] = gateInputs[0].cc0_ + gateInputs[1].cc1_;
				xorcc[2] = gateInputs[0].cc1_ + gateInputs[1].cc0_;
				xorcc[3] = gateInputs[0].cc1_ + gateInputs[1].cc1_;
				gate.cc0_ = std::min(xorcc[1], xorcc[2]);
				gate.cc1_ = std::min(xorcc[0], xorcc[3]);
				gate.cc0_++;
				gate.cc1_++;
				break;
			case Gate::XNOR3:
				gateInputs[0] = pCircuit_->gates_[gate.fis_[0]];
				gateInputs[1] = pCircuit_->gates_[gate.fis_[1]];
				gateInputs[2] = pCircuit_->gates_[gate.fis_[2]];
				xorcc[0] = gateInputs[0].cc0_ + gateInputs[1].cc0_ + gateInputs[2].cc0_;
				xorcc[1] = gateInputs[0].cc0_ + gateInputs[1].cc0_ + gateInputs[2].cc1_;
				xorcc[2] = gateInputs[0].cc0_ + gateInputs[1].cc1_ + gateInputs[2].cc0_;
				xorcc[3] = gateInputs[0].cc0_ + gateInputs[1].cc1_ + gateInputs[2].cc1_;
				xorcc[4] = gateInputs[0].cc1_ + gateInputs[1].cc0_ + gateInputs[2].cc0_;
				xorcc[5] = gateInputs[0].cc1_ + gateInputs[1].cc0_ + gateInputs[2].cc1_;
				xorcc[6] = gateInputs[0].cc1_ + gateInputs[1].cc1_ + gateInputs[2].cc0_;
				xorcc[7] = gateInputs[0].cc1_ + gateInputs[1].cc1_ + gateInputs[2].cc1_;
				gate.cc1_ = std::min(xorcc[0], xorcc[7]);
				for (int j = 1; j < 7; j++)
				{
					if (j == 1 || xorcc[j] < gate.cc1_)
					{
						gate.cc0_ = xorcc[j];
					}
				}
				gate.cc0_++;
				gate.cc1_++;
				break;
			default:
				std::cerr << "Bug: reach switch case default while calculating cc0_, cc1_";
				std::cin.get();
				break;
		}
	}

	// calculate co_ starting from PO and PP
	for (int gateID = 0; gateID < pCircuit_->tgate_; ++gateID)
	{
		Gate &gate = pCircuit_->gates_[gateID];
		switch (gate.type_)
		{
			case Gate::PO:
			case Gate::PPO:
				gate.co_ = 0;
				break;
			case Gate::PPI:
			case Gate::PI:
			case Gate::BUF:
				for (int j = 0; j < gate.nfo_; j++)
				{
					if (j == 0 || pCircuit_->gates_[gate.fos_[j]].co_ < gate.co_)
					{
						gate.co_ = pCircuit_->gates_[gate.fos_[j]].co_;
					}
				}
				break;
			case Gate::INV:
				gate.co_ = pCircuit_->gates_[gate.fos_[0]].co_ + 1;
				break;
			case Gate::AND2:
			case Gate::AND3:
			case Gate::AND4:
			case Gate::NAND2:
			case Gate::NAND3:
			case Gate::NAND4:
				gate.co_ = pCircuit_->gates_[gate.fos_[0]].co_ + 1;
				for (int j = 0; j < pCircuit_->gates_[gate.fos_[0]].nfi_; j++)
				{
					if (pCircuit_->gates_[gate.fos_[0]].fis_[j] != gateID)
					{
						Gate &gateSibling = pCircuit_->gates_[pCircuit_->gates_[gate.fos_[0]].fis_[j]];
						gate.co_ += gateSibling.cc1_;
					}
				}
				break;
			case Gate::OR2:
			case Gate::OR3:
			case Gate::OR4:
			case Gate::NOR2:
			case Gate::NOR3:
			case Gate::NOR4:
				gate.co_ = pCircuit_->gates_[gate.fos_[0]].co_ + 1;
				for (int j = 0; j < pCircuit_->gates_[gate.fos_[0]].nfi_; j++)
				{
					if (pCircuit_->gates_[gate.fos_[0]].fis_[j] != gateID)
					{
						Gate &gateSibling = pCircuit_->gates_[pCircuit_->gates_[gate.fos_[0]].fis_[j]];
						gate.co_ += gateSibling.cc0_;
					}
				}
				break;
			case Gate::XOR2:
			case Gate::XNOR2:
			case Gate::XOR3:
			case Gate::XNOR3:
				gate.co_ = pCircuit_->gates_[gate.fos_[0]].co_ + 1;
				for (int j = 0; j < pCircuit_->gates_[gate.fos_[0]].nfi_; j++)
				{
					Gate &gateSibling = pCircuit_->gates_[pCircuit_->gates_[gate.fos_[0]].fis_[j]];
					if (pCircuit_->gates_[gate.fos_[0]].fis_[j] != gateID)
					{
						gate.co_ += std::min(gateSibling.cc0_, gateSibling.cc1_);
					}
				}
				break;
			default:
				std::cerr << "Bug: reach switch case default while calculating co_";
				std::cin.get();
				break;
		}
	}

	return;
}