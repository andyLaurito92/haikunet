[a,err]=file('open', pwd() + '/../bin/BackDoor/loader.sce',"old");
if err==240 then
  exec(pwd() + '/../bin/BackDoor/builder.sce','errcatch');
  exec(pwd() + '/../bin/BackDoor/builder.sce');
end
exec(pwd() + '/../bin/BackDoor/loader.sce');
//cd(pwd() + '/output');
