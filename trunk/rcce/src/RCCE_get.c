//***************************************************************************************
// Get data from communication buffer. 
//***************************************************************************************
//
// Author: Rob F. Van der Wijngaart
//         Intel Corporation
// Date:   12/22/2010
//
//***************************************************************************************
// 
// Copyright 2010 Intel Corporation
// 
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
// 
//        http://www.apache.org/licenses/LICENSE-2.0
// 
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
// 
#include "RCCE_lib.h"
#ifdef SCC
// include the next file instead of linking to facilitate easy inlining
#include "RCCE_memcpy.c"
#endif

//--------------------------------------------------------------------------------------
// FUNCTION: RCCE_get
//--------------------------------------------------------------------------------------
// copy data from address "source" in the remote MPB to address "target" in either the
// local MPB, or in the calling UE's private memory. We do not test to see if a move
// into the calling UE's private memory stays within allocated memory                     *
//--------------------------------------------------------------------------------------
int RCCE_get(
  t_vcharp target, // target buffer, MPB or private memory
  t_vcharp source, // source buffer, MPB
  int num_bytes,   // number of bytes to copy (must be multiple of cache line size
  int ID           // rank of source UE
  ) {

#ifdef GORY
  // we only need to do tests in GORY mode; in non-GORY mode ths function is never 
  // called by the user, but only be the library
  int copy_mode;

  // check validity of parameters                                        
  if (!target) return(RCCE_error_return(RCCE_debug_comm,RCCE_ERROR_TARGET));
  if (!source) return(RCCE_error_return(RCCE_debug_comm,RCCE_ERROR_SOURCE));

  if (ID<0 || ID>=RCCE_NP) return(RCCE_error_return(RCCE_debug_comm,RCCE_ERROR_ID));

  if (num_bytes <0 || num_bytes%RCCE_LINE_SIZE!=0) 
      return(RCCE_error_return(RCCE_debug_comm,RCCE_ERROR_MESSAGE_LENGTH));

  // determine if source data is in MPB; check using local buffer boundaries 
  if (source - RCCE_comm_buffer[RCCE_IAM] >=0 &&
      source+num_bytes - (RCCE_comm_buffer[RCCE_IAM] + RCCE_BUFF_SIZE)<=0)
    // shift source address to point to remote MPB                
    source = RCCE_comm_buffer[ID]+(source-RCCE_comm_buffer[RCCE_IAM]);
  else  return(RCCE_error_return(RCCE_debug_comm,RCCE_ERROR_SOURCE));

  // target can be either local MPB or private memory             
  if (target -RCCE_comm_buffer[RCCE_IAM] >= 0 &&
      target+num_bytes - (RCCE_comm_buffer[RCCE_IAM] + RCCE_BUFF_SIZE)<=0)
    copy_mode = BOTH_IN_COMM_BUFFER;
  else 
    copy_mode = TARGET_IN_PRIVATE_MEMORY;

  // make sure that if the copy is between locations within the same MPB
  // there is no overlap between source and target address ranges  
  if ( copy_mode == BOTH_IN_COMM_BUFFER) {
    if (((source-target)>0 && (source+num_bytes-target)<0) ||
        ((target-source)>0 && (target+num_bytes-source)<0)) {
      return(RCCE_error_return(RCCE_debug_comm,RCCE_ERROR_DATA_OVERLAP));
    }
  }

  // ascertain that the start of the buffer is  cache line aligned  
  int start_index = source-RCCE_comm_buffer[ID];
  if (start_index%RCCE_LINE_SIZE!=0) 
      return(RCCE_error_return(RCCE_debug_comm,RCCE_ERROR_ALIGNMENT));

  // only verify alignment of the target if it is in the MPB 
  if (copy_mode == BOTH_IN_COMM_BUFFER) {
    start_index = target-RCCE_comm_buffer[ID];
    if (start_index%RCCE_LINE_SIZE!=0) 
      return(RCCE_error_return(RCCE_debug_comm,RCCE_ERROR_ALIGNMENT));
  }
#else
    // in non-GORY mode we only need to retain the MPB source shift; we
    // already know the source is in the MPB, not private memory
    source = RCCE_comm_buffer[ID]+(source-RCCE_comm_buffer[RCCE_IAM]);
#endif


#ifdef _OPENMP
  // make sure that any data that has been put in our MPB by another UE is visible 
  #pragma omp flush
#endif

  // do the actual copy 
  RC_cache_invalidate();
#ifdef SCC
  memcpy_get((void *)target, (void *)source, num_bytes);
#else
  memcpy((void *)target, (void *)source, num_bytes);
#endif

#ifdef _OPENMP
  // flush data to make sure it is visible to all threads; cannot use a flush list 
  // because it concerns malloced space                     
  #pragma omp flush
#endif
  return(RCCE_SUCCESS);
}


//--------------------------------------------------------------------------------------
// FUNCTION: RCCE_get_char
//--------------------------------------------------------------------------------------
// copy one byte from address "source" in the remote MPB to address "target" in either 
// the local MPB, or in the calling UE's private memory. 
//--------------------------------------------------------------------------------------
int RCCE_get_char(
  t_vcharp target, // target buffer, MPB or private memory
  t_vcharp source, // source buffer, MPB
  int ID           // rank of source UE
  ) {

#ifdef GORY
  // we only need to do tests in GORY mode; in non-GORY mode ths function is never 
  // called by the user, but only be the library
  int copy_mode;

  // check validity of parameters                                        
  if (!target) return(RCCE_error_return(RCCE_debug_comm,RCCE_ERROR_TARGET));
  if (!source) return(RCCE_error_return(RCCE_debug_comm,RCCE_ERROR_SOURCE));
  if (ID<0 || ID>=RCCE_NP) return(RCCE_error_return(RCCE_debug_comm,RCCE_ERROR_ID));

  // determine if source data is in MPB; check using local buffer boundaries 
  if (source - RCCE_comm_buffer[RCCE_IAM] >=0 &&
      source - (RCCE_comm_buffer[RCCE_IAM] + RCCE_BUFF_SIZE)<0)
    // shift source address to point to remote MPB                
    source = RCCE_comm_buffer[ID]+(source-RCCE_comm_buffer[RCCE_IAM]);
  else  return(RCCE_error_return(RCCE_debug_comm,RCCE_ERROR_SOURCE));

  // target can be either local MPB or private memory             
  if (target -RCCE_comm_buffer[RCCE_IAM] >= 0 &&
      target - (RCCE_comm_buffer[RCCE_IAM] + RCCE_BUFF_SIZE)<0)
    copy_mode = BOTH_IN_COMM_BUFFER;
  else 
    copy_mode = TARGET_IN_PRIVATE_MEMORY;

  // make sure that if the copy is between locations within the same MPB
  // there is no overlap between source and target address ranges  
  if ( copy_mode == BOTH_IN_COMM_BUFFER) {
    if (source == target) {
      return(RCCE_error_return(RCCE_debug_comm,RCCE_ERROR_DATA_OVERLAP));
    }
  }

#else
    // in non-GORY mode we only need to retain the MPB source shift; we
    // already know the source is in the MPB, not private memory
    source = RCCE_comm_buffer[ID]+(source-RCCE_comm_buffer[RCCE_IAM]);
#endif

#ifdef _OPENMP
  // make sure that any data that has been put in our MPB by another UE is visible 
  #pragma omp flush
#endif

  // do the actual copy 
  RC_cache_invalidate();
  *target = *source;
#ifdef _OPENMP
  // flush data to make sure it is visible to all threads; cannot use a flush list 
  // because it concerns malloced space                     
  #pragma omp flush
#endif
  return(RCCE_SUCCESS);
}

