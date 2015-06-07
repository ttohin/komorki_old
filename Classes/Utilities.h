//
//  Utilities.h
//  prsv
//
//  Created by Anton Simakov on 06.06.15.
//
//

#ifndef prsv_Utilities_h
#define prsv_Utilities_h

#define PEDANTIC_MODE

#ifdef PEDANTIC_MODE
#define PEDANTIC(blockBody) \
{\
auto pendantic_block = [&]() {blockBody}; \
bool pedantic_result = pendantic_block(); \
assert (pedantic_result); \
}
#else
#define PEDANTIC(blockBody)
#endif // PEDANTIC_MODE

#endif // #ifndef prsv_Utilities_h
