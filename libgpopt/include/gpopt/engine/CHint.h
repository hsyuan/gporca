//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software, Inc.
//
//	@filename:
//		CHint.h
//
//	@doc:
//		Hint configurations
//
//	@owner:
//		
//
//	@test:
//
//
//---------------------------------------------------------------------------
#ifndef GPOPT_CHint_H
#define GPOPT_CHint_H

#include "gpos/base.h"
#include "gpos/memory/IMemoryPool.h"
#include "gpos/common/CRefCount.h"

namespace gpopt
{
	using namespace gpos;

	//---------------------------------------------------------------------------
	//	@class:
	//		CHint
	//
	//	@doc:
	//		Hint configurations
	//
	//---------------------------------------------------------------------------
	class CHint : public CRefCount
	{

		private:

			// Minimum number of partitions required for sorting tuples during
			// insertion in an append only row-oriented partitioned table
			ULONG m_ulInsertSortPartitionNumber;

			// private copy ctor
			CHint(const CHint &);

		public:

			// ctor
			CHint
				(
				ULONG ulInsertSortPartitionNumber
				)
				:
				m_ulInsertSortPartitionNumber(ulInsertSortPartitionNumber)
			{}

			// Minimum number of partitions required for sorting tuples during
			// insertion in an append only row-oriented partitioned table
			ULONG UlInsertSortPartitionNumber() const
			{
				return m_ulInsertSortPartitionNumber;
			}

			// generate default optimizer configurations
			static
			CHint *PhintDefault(IMemoryPool *pmp)
			{
				return GPOS_NEW(pmp) CHint(0 /* ulInsertSortPartitionNumber */);
			}

	}; // class CHint
}

#endif // !GPOPT_CHint_H

// EOF
