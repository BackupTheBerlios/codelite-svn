# Superspecial makefile by Sverre Rabbelier!

# Maaaagic variables
cc=g++
filesuffix=
file=variable
infix_lex=yy
infix_bison=tab
# the rules.
$(file):				main.o $(file).$(infix_bison).o $(file).$(infix_lex).o
					$(cc) -g -o $(file) main.o $(file).$(infix_lex).o $(file).$(infix_bison).o -lfl

main.o:					$(file).$(infix_bison).h main.cpp
					$(cc) -g -c main.cpp -o main.o

$(file).$(infix_bison).o:		$(file).$(infix_bison).c$(filesuffix)
					$(cc) -g -c $(file).$(infix_bison).c$(filesuffix) -o $(file).$(infix_bison).o

$(file).$(infix_bison).h:		$(file).y
					bison -d $(file).y -o $(file).$(infix_bison).c$(filesuffix)
$(file).$(infix_bison).c$(filesuffix):	$(file).y
					bison -d $(file).y -o $(file).$(infix_bison).c$(filesuffix)

$(file).$(infix_lex).o:			$(file).$(infix_lex).c $(file).$(infix_bison).h
					$(cc) -g -c $(file).$(infix_lex).c -o $(file).$(infix_lex).o

$(file).$(infix_lex).c:			$(file).l
					flex -o $(file).$(infix_lex).c $(file).l

clean:
					rm -f $(file) $(file).$(infix_lex).* $(file).$(infix_bison).* main.o
					rm *~
