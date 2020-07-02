rooms: hollryan.buildrooms.c
	gcc -Wall hollryan.buildrooms.c -o buildrooms
	buildrooms

adv: hollryan.adventure.c
	gcc -Wall hollryan.adventure.c -o adventure
	adventure

clean: 
	rmdir hollryan.rooms*