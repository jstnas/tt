tt: tt.c config.h node.h words.h window.h twindow.h typing.h
	gcc -o tt tt.c -lncurses

old: tt-old.c config.h node.h words.h
	gcc -o tt-old tt-old.c -lncurses
