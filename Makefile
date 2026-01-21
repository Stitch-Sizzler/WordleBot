src/bot: src/ex11q1.c src/game.c
	gcc -Wall -Werror -std=c99 -g src/ex11q1.c src/game.c -o bot
clean:
	rm -f src/ex11q1
