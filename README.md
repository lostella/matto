# Matto - a chess engine written in C

Matto is a very simple chess engine I started writing ages ago when learning [the C programming language](http://s3-us-west-2.amazonaws.com/belllabs-microsite-dritchie/cbook/index.html). Hopefully I'll be able to start again working on it, fix its bugs, make it stronger and more usable.

## Build and run

	$ cd <path>
	$ make
	$ ./matto

## Commands

command | description
--- | ---
`d` | display the chessboard
`quit` or `exit` | shut down the program
`force` | turns off the automatic player
`go` | turns on the automatic player
`?` | (while thinking) stops the search
`getback` | gets back one move in the game
`sd <n>` | sets the search depth to `<n>` moves
`st <n>` | sets the search time to `<n>` seconds
`perft <n>` | run the move generation test up to depth `<n>`

## Known bugs

* When you stop the search by issuing the `?` command, the input mechanism stops working

## To do

* Fix bugs :-)
* Use dynamically allocated structures (e.g. for the board)
* Make 100% [UCI](http://www.shredderchess.com/chess-info/features/uci-universal-chess-interface.html) compliant
* Better evaluation function
* Opening books
* Implement a timer instead of checking time at every node
