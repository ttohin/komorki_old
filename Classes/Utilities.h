//
//  Utilities.h
//  prsv
//
//  Created by Anton Simakov on 06.06.15.
//
//

#ifndef prsv_Utilities_h
#define prsv_Utilities_h

#ifdef _WIN32

struct timezone;

int gettimeofday(struct timeval * tp, struct timezone * tzp);

#endif

#endif // #ifndef prsv_Utilities_h
