#TODO
CFLAGS = -fsanitize=address

<<<<<<< HEAD
sound_seg: sound_seg.c
<<<<<<< HEAD
	${CC} -o bin/sound_seg sound_seg.c ${CFLAGS}
=======
	${CC} -g sound_seg.c -o bin/sound_seg ${CFLAGS} -lm
>>>>>>> b80c02d (Completed the identify function)
=======
sound_seg.o: sound_seg.c
	${CC} -g sound_seg.c -o sound_seg.o ${CFLAGS}
>>>>>>> 6101dcf (removed .gitignore from the repo)

clean:
	rm bin/*