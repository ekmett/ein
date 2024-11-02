/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense */

/// cond xmacro
X template float scalef(float, float) noexcept;
X template double scalef(double, double) noexcept;

#if 0
#define EIN_CMPINT_IMPL(Z,Y) \
X template bool cmpint<CMPINT::Z,Y>(Y,Y) noexcept;
#define EIN_CMPINT(Z) \
  EIN_CMPINT_IMPL(Z,uint8_t) \
  EIN_CMPINT_IMPL(Z,uint16_t) \
  EIN_CMPINT_IMPL(Z,uint32_t) \
  EIN_CMPINT_IMPL(Z,uint64_t) \
  EIN_CMPINT_IMPL(Z,int8_t) \
  EIN_CMPINT_IMPL(Z,int16_t) \
  EIN_CMPINT_IMPL(Z,int32_t) \
  EIN_CMPINT_IMPL(Z,int64_t)
EIN_CMPINT(TRUE)
EIN_CMPINT(FALSE)
EIN_CMPINT(LT)
EIN_CMPINT(NLT)
EIN_CMPINT(LE)
EIN_CMPINT(NLE)
EIN_CMPINT(EQ)
EIN_CMPINT(NE)
#undef EIN_CMPINT_IMPL
#undef EIN_CMPINT

#define EIN_CMP_IMPL(Z,Y) \
X template bool cmp<CMP::Z,Y>(Y,Y) noexcept;
#define EIN_CMP(Z) \
  EIN_CMP_IMPL(Z,float) \
  EIN_CMP_IMPL(Z,double)

EIN_CMP(EQ_OQ)
EIN_CMP(LT_OS)
EIN_CMP(LE_OS)
EIN_CMP(UNORD_Q)
EIN_CMP(NEQ_UQ)
EIN_CMP(NLT_US)
EIN_CMP(NLE_US)
EIN_CMP(ORD_Q)
EIN_CMP(EQ_UQ)
EIN_CMP(NGE_US)
EIN_CMP(NGT_US)
EIN_CMP(FALSE_OQ)
EIN_CMP(NEQ_OQ)
EIN_CMP(GE_OS)
EIN_CMP(GT_OS)
EIN_CMP(TRUE_UQ)
EIN_CMP(EQ_OS)
EIN_CMP(LT_OQ)
EIN_CMP(LE_OQ)
EIN_CMP(UNORD_S)
EIN_CMP(NEQ_US)
EIN_CMP(NLT_UQ)
EIN_CMP(NLE_UQ)
EIN_CMP(ORD_S)
EIN_CMP(EQ_US)
EIN_CMP(NGE_UQ)
EIN_CMP(NGT_UQ)
EIN_CMP(FALSE_OS)
EIN_CMP(NEQ_OS)
EIN_CMP(GE_OQ)
EIN_CMP(GT_OQ)
EIN_CMP(TRUE_US)

#undef EIN_CMP
#undef EIN_CMP_IMPL
#endif

///endcond
