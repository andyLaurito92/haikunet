// This file is released under the 3-clause BSD license. See COPYING-BSD.
// Generated by builder.sce : Please, do not edit this file
// ----------------------------------------------------------------------------
//
libbackdoor_c_path = get_absolute_file_path('loader.sce');
//
// ulink previous function with same name
[bOK, ilib] = c_link('libbackdoor_c');
if bOK then
  ulink(ilib);
end
//
link(libbackdoor_c_path + filesep() + '../../src/c/libbackdoor' + getdynlibext());
list_functions = [ 'openBackDoor';
                   'closeBackDoor';
];
addinter(libbackdoor_c_path + filesep() + 'libbackdoor_c' + getdynlibext(), 'libbackdoor_c', list_functions);
// remove temp. variables on stack
clear libbackdoor_c_path;
clear bOK;
clear ilib;
clear list_functions;
// ----------------------------------------------------------------------------
