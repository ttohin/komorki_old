//
//  CellShapes.cpp
//  prsv
//
//  Created by Anton Simakov on 05.05.15.
//
//

#include "CellShapes.h"
#include "PixelDescriptor.h"
#include "b2Utilites.h"

using namespace komorki;

void SingleCell::ForEach(const PerPixelFunc& op) const
{
  bool stop = false;
  op(m_pd, stop);
}

void SingleCell::Around(const PerPixelFunc& op) const
{
  m_pd->Around(op);
}

void SingleCell::AroundRandom(const PerPixelFunc& op) const
{
  m_pd->AroundRandom(op);
}

void SingleCell::SetPosition(PixelDescriptor* pd)
{
  m_pd = pd;
}

PixelDescriptor* SingleCell::GetOpposite(PixelDescriptor* target) const
{
  return m_pd->GetOpposite(target);
}

void BigCell::ForEach(const PerPixelFunc& op) const
{
  bool stop = false;
  if (false == stop) op(m_pd, stop);
  if (false == stop) op(m_pd->rc, stop);
  if (false == stop) op(m_pd->rt, stop);
  if (false == stop) op(m_pd->ct, stop);
}

void BigCell::Around(const PerPixelFunc& op) const
{
  bool stop = false;
  if (false == stop) op(m_pd->lc, stop);
  if (false == stop) op(m_pd->lb, stop);
  if (false == stop) op(m_pd->cb, stop);
  if (false == stop) op(m_pd->rc->cb, stop);
  if (false == stop) op(m_pd->rc->rb, stop);
  if (false == stop) op(m_pd->rc->rc, stop);
  if (false == stop) op(m_pd->rt->rt, stop);
  if (false == stop) op(m_pd->rt->rc, stop);
  if (false == stop) op(m_pd->rt->ct, stop);
  if (false == stop) op(m_pd->ct->ct, stop);
  if (false == stop) op(m_pd->ct->lt, stop);
  if (false == stop) op(m_pd->ct->lc, stop);
}

void BigCell::AroundRandom(const PerPixelFunc& op) const
{
  const int start = cRandABInt(0, 12);
  const int dir = rand()%2 ? 1 : 1;
  bool stop = false;
  for (int ri = start; (ri - dir*start) != 12; ri += dir)
  {
    int i = ri%12;
    if (i == 0) op(m_pd->lc, stop); if (stop) break;
    if (i == 1) op(m_pd->lb, stop); if (stop) break;
    if (i == 2) op(m_pd->cb, stop); if (stop) break;
    if (i == 3) op(m_pd->rc->cb, stop); if (stop) break;
    if (i == 4) op(m_pd->rc->rb, stop); if (stop) break;
    if (i == 5) op(m_pd->rc->rc, stop); if (stop) break;
    if (i == 6) op(m_pd->rt->rt, stop); if (stop) break;
    if (i == 7) op(m_pd->rt->rc, stop); if (stop) break;
    if (i == 8) op(m_pd->rt->ct, stop); if (stop) break;
    if (i == 9) op(m_pd->ct->ct, stop); if (stop) break;
    if (i == 10) op(m_pd->ct->lt, stop); if (stop) break;
    if (i == 11) op(m_pd->ct->lc, stop); if (stop) break;
  }
}

void BigCell::SetPosition(PixelDescriptor* pd)
{
  m_pd = pd;
}

PixelDescriptor* BigCell::GetOpposite(PixelDescriptor* target) const
{
  return nullptr;
}

