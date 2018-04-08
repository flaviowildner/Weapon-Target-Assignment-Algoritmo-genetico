GPP=g++
FILE=WTA

all:
	$(GPP) $(FILE).cpp -o FILE
clean:
	rm -f $(FILE)
	rm -f Resultados.txt
	rm -f Instancia.txt
	rm -f Parametros.txt
