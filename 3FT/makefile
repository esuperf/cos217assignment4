# Dependency rules for non-file targets

all: ft
clobber: clean 
	rm -f *~\#*\#
clean:
	rm -f dynarray.o path.o checkerFT.o nodeFT.o ft.o 

# Dependency rules for file targets
ft: dynarray.o path.o nodeFT.o ft.o ft_client.o
	gcc217 dynarray.o path.o nodeFT.o ft.o ft_client.o –o ft
dynarray.o: dynarray.c dynarray.h
	gcc217 -c dynarray.c 
path.o: path.c dynarray.h path.h a4def.h
	gcc217 -c path.c
ft_client.o: ft_client.c ft.h a4def.h
	gcc217 -c ft_client.c
nodeFT.o: nodeFT.c dynarray.h nodeFT.h path.h a4def.h
	gcc217 -c nodeFT.c
ft.o: ft.c dynarray.h nodeft.h ft.h path.h a4def.h
	gcc217 -c ft.c

