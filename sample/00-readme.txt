## Sample Server

server-enhanced is a valid server with these special cases to stress-test your
client:

* If you ask for "pi", you get "314159265358979323846264\n".

* If you ask for "Ramanujan", you get many, many digits.

solarflares.dat: A customer file. It has:

Name                            Sunspots
----                            --------
Dennis Ritchie                  1926
Archimedes                      290
Johnny Ronald. Reuel. Tolkien   61234
Alan Turing                     14530


## Sample Clients

All take 2 arguments as in the handout.

client-good: A correct client with a relaxation: You can enter a name up to
200ish bytes, this client will obey and send to the server even if too long,
so you can test how a server handles that.

client-skipper: Automated client that sends these two messages:
  "Alan Turing\n"
  "Johnny Ronald. Reuel. Tolkien\n"
However! The chunking is misaligned. This is not wrong behaviour. We expect the
server to give two answers as normal. The client will then print the two
answers.

client-hit-and-run: Bad automated client. Sends "Vincent\n" and immediately
exits.

client-4ever: Bad automated client. Sends a very, very long name.  A correct
server would disconnect this client; this client would then terminate with an
error message.
