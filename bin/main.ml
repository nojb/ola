let () =
  let state = Lua.newstate () in
  let r = ref 0 in
  let f state = incr r; Lua.pushinteger state !r; 1 in
  Lua.pushfunction state f;
  Lua.setglobal state "f";
  Lua.pushfunction state (fun _L -> print_endline (Lua.tostring _L (-1)); 0);
  Lua.setglobal state "print";
  Lua.loadstring state {|
    print(f())
    print(f())
    print(f())
|};
  Lua.pcall state 0;
  (* for _ = 1 to 10 do *)
  (*   Lua.getglobal state "f"; *)
  (*   Lua.pcall state 0; *)
  (*   let n = Lua.tointeger state (-1) in *)
  (*   Lua.pop state 1; *)
  (*   print_int n; *)
  (*   print_newline () *)
  (* done; *)
  Lua.close state;
  print_endline "bye"
