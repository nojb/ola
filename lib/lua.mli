type state

val newstate: unit -> state
val pushnil: state -> unit
val pushboolean: state -> bool -> unit
val pushnumber: state -> float -> unit
val pushinteger: state -> int -> unit
val pushstring: state -> string -> unit
val checkstack: state -> int -> bool
val isnumber: state -> int -> bool
val isstring: state -> int -> bool
val toboolean: state -> int -> bool
val tonumber: state -> int -> float
val tointeger: state -> int -> int
val tostring: state -> int -> string
val pushfunction: state -> (state -> int) -> unit
val pcall: state -> int -> unit
val pop: state -> int -> unit
val setglobal: state -> string -> unit
val getglobal: state -> string -> unit
val close: state -> unit
val loadstring: state -> string -> unit
