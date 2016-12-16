/* ==================================================================== */
/* This file is released into the public domain */
/* ==================================================================== */
#define __USE_DEPRECATED_STACK_FUNCTIONS__
#include "stack-c.h" 
#include "api_scilab.h"
#include "Scierror.h"
#include "MALLOC.h"
/* ==================================================================== */
void sci_openBackDoor()
{
	openBackDoor();
	PutLhsVar();
  return 0;
}
void sci_closeBackDoor()
{
	closeBackDoor();
	PutLhsVar();
  return 0;
}

