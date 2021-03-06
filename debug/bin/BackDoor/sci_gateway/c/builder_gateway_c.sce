// ====================================================================
// Allan CORNET
// Simon LIPP
// INRIA 2008
// This file is released into the public domain
// ====================================================================

MSDOS = getos() == 'Windows'; 
if MSDOS then
  // to manage long pathname
  includes_src_c = '-I""' + get_absolute_file_path('builder_gateway_c.sce') + '../../src/c""';
else
  includes_src_c = '-I' + get_absolute_file_path('builder_gateway_c.sce') + '../../src/c';
end

// PutLhsVar managed by user in sci_sum and in sci_sub
// if you do not this variable, PutLhsVar is added
// in gateway generated (default mode in scilab 4.x and 5.x)
WITHOUT_AUTO_PUTLHSVAR = %t;

tbx_build_gateway('backdoor_c', ['openBackDoor','sci_openBackDoor';'closeBackDoor','sci_closeBackDoor'], ['sci_backdoor.c'], ..
                  get_absolute_file_path('builder_gateway_c.sce'), ..
                  ['../../src/c/libbackdoor'],'',includes_src_c);

clear WITHOUT_AUTO_PUTLHSVAR;

clear tbx_build_gateway;
