# Matto - a chess player written in C

Matto is a very simple chess player I started writing ages ago when learning [the C programming language](http://s3-us-west-2.amazonaws.com/belllabs-microsite-dritchie/cbook/index.html). Hopefully I will be able to fix it a bit, make it stronger and more usable.

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

## To do

* Translate all comments in english
* Comment, comment, comment
* Fix bugs :-)
* Use dynamically allocated structures (e.g. for the board)
* Make 100% [UCI](http://www.shredderchess.com/chess-info/features/uci-universal-chess-interface.html) compliant
* Better evaluation function
* Opening books
