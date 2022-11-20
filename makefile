all:projet

projet : Projet_XU_SIDIBEM.o
	gcc Projet_XU_SIDIBEM.o -o projet
	
projet.o : Projet_XU_SIDIBEM.c
	gcc -c Projet_XU_SIDIBEM.c
	
clean :
	@echo "Effacez les fichiers objets"
	-rm *.o
