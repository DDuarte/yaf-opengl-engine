:-use_module(library(sockets)).

port(5555).

% launch me in sockets mode
server:-
    port(Port),
    socket_server_open(Port, Socket, [reuseaddr(true)]),
    socket_server_accept(Socket, _Client, Stream, [type(text)]),
    write('Accepted connection'), nl,
    serverLoop(Stream),
    socket_server_close(Socket).

% wait for commands
serverLoop(Stream) :-
    repeat,
    read(Stream, ClientMsg),
    write('Received: '), write(ClientMsg), nl,
    parse_input(ClientMsg, MyReply),
    format(Stream, '~q.~n', [MyReply]),
    write('Wrote: '), write(MyReply), nl,
    flush_output(Stream),
    (ClientMsg == quit; ClientMsg == end_of_file), !.

parse_input(move(Arg1, Arg2), Answer) :-
    move(Arg1, Arg2, Answer).

parse_input(quit, ok-bye) :- !.

move(Arg1, Arg2, Answer) :-
    write(Arg1), nl, write(Arg2), nl,
    Answer is Arg1 + Arg2.
