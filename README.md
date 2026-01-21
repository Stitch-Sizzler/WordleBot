# WordleBot
Based on Wordle by The New York Times.  Given set of words and a random seed, this bot will attempt to guess the correct word using as few guesses as possible.

How to Use:

Download the bot. In case there are any issues, download the Makefile and src folder and run ````make```` in the project directory to compile the program again. 

Use one of the provided example input files or create your own one. The first line is a random seed in the range [0, 2<sup>32</sup> - 1]. Each n line after that is a word in the set of n words you would like the bot to use.

Use the following command to run the program with two arguments:

````./bot <inputfile> <outputfile>````

Whenever the bot makes a guess, it will output the guessed word to the output file. The game will color-code these letters as green, yellow, or gray, just like Wordle.
