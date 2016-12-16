function s=simpd(tf)
  curdir=pwd();
  out= sprintf('%s/powerdevs/output/',getenv('HOME'));
  cd(out);
  s=host(sprintf('./model -tf %g',tf));
  cd(curdir);
endfunction
