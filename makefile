rooms: hollryan.buildrooms.c
	gcc -g -Wall hollryan.buildrooms.c -o buildrooms
	buildrooms

adv: hollryan.adventure.c
	gcc -g -Wall hollryan.adventure.c -o adventure
	adventure

clean: 
	rm -rf hollryan.rooms*

re: 
	make rooms
	make adv