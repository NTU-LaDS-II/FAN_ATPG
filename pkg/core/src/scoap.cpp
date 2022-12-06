#include "atpg.h"
using namespace CoreNs;
template <typename T>
void DEBUG(const std::string &message, T var)
{
	std::cout << "[DEBUG]"
						<< "\t";
	std::cout << message << "\t" << var << "\n";
}
void Atpg::calSCOAP()
{
	// cc0, cc1 and co default is 0
	// check if is changed before
	for (int i = 0; i < cir_->tgate_; ++i)
	{
		Gate &g = cir_->gates_[i];
		if (g.cc0_ != 0)
		{
			std::cout << "cc0_ is not -1\n";
			std::cin.get();
		}
		if (g.cc1_ != 0)
		{
			std::cout << "cc1_ is not -1\n";
			std::cin.get();
		}
		if (g.co_ != 0)
		{
			std::cout << "co_ is not -1\n";
			std::cin.get();
		}
	}

	// array for xor2, xor3, xnor2, xnor3
	// xor2 xnor2 : {00,01,10,11}
	// xor3 xnor3 : {000, 001, 010, 011, 100, 101, 110, 111}
	int xorcc[8] = {0};
	Gate xorgin[2];

	// calculate cc0 and cc1 starting from PI and PPI
	for (int i = 0; i < cir_->tgate_; ++i)
	{
		Gate &g = cir_->gates_[i];
		switch (g.type_)
		{
			case Gate::PPI:
			case Gate::PI:
				g.cc0_ = 1;
				g.cc1_ = 1;
				break;
			case Gate::PO:
			case Gate::PPO:
			case Gate::BUF:
				g.cc0_ = cir_->gates_[g.fis_[0]].cc0_;
				g.cc1_ = cir_->gates_[g.fis_[0]].cc1_;
				break;
			case Gate::INV:
				g.cc0_ = cir_->gates_[g.fis_[0]].cc1_ + 1;
				g.cc1_ = cir_->gates_[g.fis_[0]].cc0_ + 1;
				break;
			case Gate::AND2:
			case Gate::AND3:
			case Gate::AND4:
				for (int j = 0; j < g.nfi_; j++)
				{
					Gate &gin = cir_->gates_[g.fis_[j]];
					if (j == 0 || (gin.cc0_ < g.cc0_))
					{
						g.cc0_ = gin.cc0_;
					}
					g.cc1_ += gin.cc1_;
				}
				g.cc1_++;
				g.cc0_++;
				break;

			case Gate::NAND2:
			case Gate::NAND3:
			case Gate::NAND4:
				for (int j = 0; j < g.nfi_; j++)
				{
					Gate &gin = cir_->gates_[g.fis_[j]];
					if (j == 0 || (gin.cc0_ < g.cc1_))
					{
						g.cc1_ = gin.cc0_;
					}
					g.cc0_ += gin.cc1_;
				}
				g.cc0_++;
				g.cc1_++;
				break;

			case Gate::OR2:
			case Gate::OR3:
			case Gate::OR4:
				for (int j = 0; j < g.nfi_; j++)
				{
					Gate &gin = cir_->gates_[g.fis_[j]];
					if (j == 0 || (gin.cc1_ < g.cc1_))
					{
						g.cc1_ = gin.cc1_;
					}
					g.cc0_ += gin.cc0_;
				}
				g.cc0_++;
				g.cc1_++;
				break;
			case Gate::NOR2:
			case Gate::NOR3:
			case Gate::NOR4:
				for (int j = 0; j < g.nfi_; j++)
				{
					Gate &gin = cir_->gates_[g.fis_[j]];
					if (j == 0 || (gin.cc1_ < g.cc0_))
					{
						g.cc0_ = gin.cc1_;
					}
					g.cc1_ += gin.cc0_;
				}
				g.cc0_++;
				g.cc1_++;
				break;
			case Gate::XOR2:
				xorgin[0] = cir_->gates_[g.fis_[0]];
				xorgin[1] = cir_->gates_[g.fis_[1]];
				xorcc[0] = xorgin[0].cc0_ + xorgin[1].cc0_;
				xorcc[1] = xorgin[0].cc0_ + xorgin[1].cc1_;
				xorcc[2] = xorgin[0].cc1_ + xorgin[1].cc0_;
				xorcc[3] = xorgin[0].cc1_ + xorgin[1].cc1_;
				g.cc0_ = std::min(xorcc[0], xorcc[3]);
				g.cc1_ = std::min(xorcc[1], xorcc[2]);
				g.cc0_++;
				g.cc1_++;
				break;
			case Gate::XOR3:
				xorgin[0] = cir_->gates_[g.fis_[0]];
				xorgin[1] = cir_->gates_[g.fis_[1]];
				xorgin[2] = cir_->gates_[g.fis_[2]];
				xorcc[0] = xorgin[0].cc0_ + xorgin[1].cc0_ + xorgin[2].cc0_;
				xorcc[1] = xorgin[0].cc0_ + xorgin[1].cc0_ + xorgin[2].cc1_;
				xorcc[2] = xorgin[0].cc0_ + xorgin[1].cc1_ + xorgin[2].cc0_;
				xorcc[3] = xorgin[0].cc0_ + xorgin[1].cc1_ + xorgin[2].cc1_;
				xorcc[4] = xorgin[0].cc1_ + xorgin[1].cc0_ + xorgin[2].cc0_;
				xorcc[5] = xorgin[0].cc1_ + xorgin[1].cc0_ + xorgin[2].cc1_;
				xorcc[6] = xorgin[0].cc1_ + xorgin[1].cc1_ + xorgin[2].cc0_;
				xorcc[7] = xorgin[0].cc1_ + xorgin[1].cc1_ + xorgin[2].cc1_;
				g.cc0_ = std::min(xorcc[0], xorcc[7]);
				for (int j = 1; j < 7; j++)
				{
					if (j == 1 || xorcc[j] < g.cc1_)
					{
						g.cc1_ = xorcc[j];
					}
				}
				g.cc0_++;
				g.cc1_++;
				break;
			case Gate::XNOR2:
				xorgin[0] = cir_->gates_[g.fis_[0]];
				xorgin[1] = cir_->gates_[g.fis_[1]];
				xorcc[0] = xorgin[0].cc0_ + xorgin[1].cc0_;
				xorcc[1] = xorgin[0].cc0_ + xorgin[1].cc1_;
				xorcc[2] = xorgin[0].cc1_ + xorgin[1].cc0_;
				xorcc[3] = xorgin[0].cc1_ + xorgin[1].cc1_;
				g.cc0_ = std::min(xorcc[1], xorcc[2]);
				g.cc1_ = std::min(xorcc[0], xorcc[3]);
				g.cc0_++;
				g.cc1_++;
				break;
			case Gate::XNOR3:
				xorgin[0] = cir_->gates_[g.fis_[0]];
				xorgin[1] = cir_->gates_[g.fis_[1]];
				xorgin[2] = cir_->gates_[g.fis_[2]];
				xorcc[0] = xorgin[0].cc0_ + xorgin[1].cc0_ + xorgin[2].cc0_;
				xorcc[1] = xorgin[0].cc0_ + xorgin[1].cc0_ + xorgin[2].cc1_;
				xorcc[2] = xorgin[0].cc0_ + xorgin[1].cc1_ + xorgin[2].cc0_;
				xorcc[3] = xorgin[0].cc0_ + xorgin[1].cc1_ + xorgin[2].cc1_;
				xorcc[4] = xorgin[0].cc1_ + xorgin[1].cc0_ + xorgin[2].cc0_;
				xorcc[5] = xorgin[0].cc1_ + xorgin[1].cc0_ + xorgin[2].cc1_;
				xorcc[6] = xorgin[0].cc1_ + xorgin[1].cc1_ + xorgin[2].cc0_;
				xorcc[7] = xorgin[0].cc1_ + xorgin[1].cc1_ + xorgin[2].cc1_;
				for (int j = 1; j < 7; j++)
				{
					if (j == 1 || xorcc[j] < g.cc1_)
					{
						g.cc0_ = xorcc[j];
					}
				}
				g.cc1_ = std::min(xorcc[0], xorcc[7]);
				g.cc0_++;
				g.cc1_++;
				break;
			default:
				DEBUG("default", "should not happen");
				break;
		}
	}

	// calculate co_ starting from PO and PP
	for (int i = 0; i < cir_->tgate_; ++i)
	{
		Gate &g = cir_->gates_[i];
		Gate &gout = cir_->gates_[0]; // for fanout free
		switch (g.type_)
		{
			case Gate::PO:
			case Gate::PPO:
				g.co_ = 0;
				break;
			case Gate::PPI:
			case Gate::PI:
			case Gate::BUF:
				for (int j = 0; j < g.nfo_; j++)
				{
					if (j == 0 || cir_->gates_[g.fos_[j]].co_ < g.co_)
					{
						g.co_ = cir_->gates_[g.fos_[j]].co_;
					}
				}
				break;
			case Gate::INV:
				g.co_ = gout.co_ + 1;
				break;
			case Gate::AND2:
			case Gate::AND3:
			case Gate::AND4:
			case Gate::NAND2:
			case Gate::NAND3:
			case Gate::NAND4:
				g.co_ = gout.co_ + 1;
				for (int j = 0; j < gout.nfi_; j++)
				{
					Gate &gin = cir_->gates_[g.fis_[j]];
					if (g.fis_[j] != i)
					{
						g.co_ += g.cc1_;
					}
				}
				break;
			case Gate::OR2:
			case Gate::OR3:
			case Gate::OR4:
			case Gate::NOR2:
			case Gate::NOR3:
			case Gate::NOR4:
				g.co_ = gout.co_ + 1;
				for (int j = 0; j < gout.nfi_; j++)
				{
					Gate &gin = cir_->gates_[g.fis_[j]];
					if (g.fis_[j] != i)
					{
						g.co_ += g.cc0_;
					}
				}
				break;
			case Gate::XOR2:
			case Gate::XNOR2:
			case Gate::XOR3:
			case Gate::XNOR3:
				int minccin;
				g.co_ = gout.co_ + 1;
				for (int j = 0; j < gout.nfo_; j++)
				{
					Gate &gin = cir_->gates_[g.fis_[j]];
					if (g.fis_[j] != i)
					{
						int temp = std::min(gin.cc0_, gin.cc1_);
						minccin = std::min(temp, minccin);
					}
				}
				g.co_ += minccin;
				break;
			default:
				DEBUG("default", "should not happen");
				break;
		}
	}

	return;
}