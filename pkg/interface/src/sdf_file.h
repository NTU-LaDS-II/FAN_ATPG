// **************************************************************************
// File       [ sdf_file.h ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/07/19 created ]
// **************************************************************************

#ifndef _INTF_SDF_FILE_H_
#define _INTF_SDF_FILE_H_

#include "global.h"

namespace IntfNs
{

	enum SdfDelayType
	{
		DELAY_ABSOLUTE,
		DELAY_INCREMENT
	};

	struct SdfPortSpec
	{
		enum EdgeType
		{
			EDGE_NA,
			EDGE_01,
			EDGE_10,
			EDGE_0Z,
			EDGE_Z1,
			EDGE_1Z,
			EDGE_Z0
		};

		EdgeType type;
		char port[NAME_LEN];
	};

	struct SdfIoPath
	{
		char in[NAME_LEN];
		char out[NAME_LEN];
	};

	struct SdfValue
	{
		int n;
		float v[3];
	};

	struct SdfDelayValue
	{
		int n;
		SdfValue v[3];
	};

	struct SdfDelayValueList
	{
		int n;
		SdfDelayValue v[12];
	};

	struct SdfDelayDef
	{
		enum Type
		{
			IO_DELAY,
			IO_RETAIN,
			PORT_DELAY,
			INTER_DELAY,
			DEVICE_DELAY
		};
		Type type;
		SdfPortSpec pspec;
		char p1[NAME_LEN];
		char p2[NAME_LEN];
		SdfDelayValueList vlist;
		SdfDelayDef *next;
		SdfDelayDef *head;
	};

	class SdfFile
	{
	public:
		SdfFile();
		virtual ~SdfFile();
		virtual bool read(const char *const fname, const bool &verbose = false);

		virtual bool addVersion(const char *const version);
		virtual bool addDate(const char *const date);
		virtual bool addDesign(const char *const design);
		virtual bool addVendor(const char *const vendor);
		virtual bool addProgName(const char *const name);
		virtual bool addProgVersion(const char *const version);
		virtual bool addHierChar(const char &hier);
		virtual bool addVoltage(const SdfValue &volt);
		virtual bool addProcess(const char *const c);
		virtual bool addTemperature(const SdfValue &temp);
		virtual bool addTimeScale(const float &num, const char *const unit);
		virtual bool addCell(const char *const type, const char *const name);
		virtual bool addIoDelay(const SdfDelayType &type,
														const SdfPortSpec &spec,
														const char *const port,
														const SdfDelayValueList &v);
		virtual bool addIoRetain(const SdfDelayType &type,
														 const SdfPortSpec &spec,
														 const char *const port,
														 const SdfDelayValueList &v);
		virtual bool addPortDelay(const SdfDelayType &type,
															const char *const port,
															const SdfDelayValueList &v);
		virtual bool addInterconnectDelay(const SdfDelayType &type,
																			const char *const from,
																			const char *const to,
																			const SdfDelayValueList &v);
		virtual bool addDeviceDelay(const SdfDelayType &type,
																const char *const dev,
																const SdfDelayValueList &v);

	protected:
		bool success_;
		bool verbose_;
	};

	inline SdfFile::SdfFile() {}
	inline SdfFile::~SdfFile() {}

	inline void sdfValueCopy(SdfValue &v1, SdfValue &v2)
	{
		v1.n = v2.n;
		v1.v[0] = v2.v[0];
		v1.v[1] = v2.v[1];
		v1.v[2] = v2.v[2];
	}

	inline void sdfDelayValueCopy(SdfDelayValue &v1, SdfDelayValue &v2)
	{
		v1.n = v2.n;
		sdfValueCopy(v1.v[0], v2.v[0]);
		sdfValueCopy(v1.v[1], v2.v[1]);
		sdfValueCopy(v1.v[2], v2.v[2]);
	}

	inline void sdfDelayValueListCopy(SdfDelayValueList &v1,
																		SdfDelayValueList &v2)
	{
		v1.n = v2.n;
		for (int i = 0; i < 12; ++i)
		{
			sdfDelayValueCopy(v1.v[i], v2.v[i]);
		}
	}

};

#endif
