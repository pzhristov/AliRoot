// $Id$

///
/// Tries to read all motif types for stations 3, 4, 5
///
/// 

#if !defined(__CINT__) || defined(__MAKECINT__)

#include "AliMpStation12Type.h"
#include "AliMpPlaneType.h"
#include "AliMpDataProcessor.h"
#include "AliMpDataMap.h"
#include "AliMpDataStreams.h"
#include "AliMpMotifReader.h"
#include "AliMpMotifType.h"

#include <Riostream.h>

#endif

Int_t test(AliMpDataStreams& dataStreams, AliMpMotifReader& r, const char letter, Int_t from, Int_t to)
{
  char m[256];
  Int_t n = 0;
  for ( Int_t i = from; i <= to; ++i ) 
  {
    snprintf(m,256,"%c%d",letter,i);
    AliMpMotifType* motifType = r.BuildMotifType(dataStreams,m);
    if ( motifType ) 
    {
      motifType->Print("G");
      ++n;
    }
    else
    {
      cout << "Cannot read motifType " << m << endl;
    }
  }
  return n;
}

void testSt345ReadMotifType()
{
  AliMpDataProcessor mp;
  AliMpDataMap* dataMap = mp.CreateDataMap("data");
  AliMpDataStreams dataStreams(dataMap);

  AliMpMotifReader r(AliMp::kStation345, AliMq::kNotSt12, AliMp::kNonBendingPlane);
  // note that second parameter is not used for station345.

  Int_t n = 0;
  
  n += test(dataStreams,r,'I',1,1);
  n += test(dataStreams,r,'L',1,25);
  n += test(dataStreams,r,'O',1,20);
  n += test(dataStreams,r,'P',1,4);
  n += test(dataStreams,r,'Q',1,4);
  n += test(dataStreams,r,'R',1,45);
  n += test(dataStreams,r,'Z',1,8);
  
  cout << "Successfully read in " << n << " motifTypes" << endl;
}  

