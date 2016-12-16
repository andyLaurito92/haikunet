#ifdef __cplusplus
extern "C" {
#endif
#include <mex.h> 
#include <sci_gateway.h>
#include <api_scilab.h>
#include <MALLOC.h>
static int direct_gateway(char *fname,void F(void)) { F();return 0;};
extern Gatefunc sci_openBackDoor;
extern Gatefunc sci_closeBackDoor;
static GenericTable Tab[]={
  {(Myinterfun)sci_gateway_without_putlhsvar,sci_openBackDoor,"openBackDoor"},
  {(Myinterfun)sci_gateway_without_putlhsvar,sci_closeBackDoor,"closeBackDoor"},
};
 
int C2F(libbackdoor_c)()
{
  Rhs = Max(0, Rhs);
  if (*(Tab[Fin-1].f) != NULL) 
  {
     if(pvApiCtx == NULL)
     {
       pvApiCtx = (StrCtx*)MALLOC(sizeof(StrCtx));
     }
     pvApiCtx->pstName = (char*)Tab[Fin-1].name;
    (*(Tab[Fin-1].f))(Tab[Fin-1].name,Tab[Fin-1].F);
  }
  return 0;
}
#ifdef __cplusplus
}
#endif
