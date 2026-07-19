COMPILATEUR = avr-gcc
CIBLE = atmega328p
OPTIONS = -Wall -Wextra -g -O0 -Iinclude -std=c99 -mmcu=$(CIBLE) -DF_CPU=16000000UL
LIBRAIRIES = 
SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=build/%.o)
NOM = projet_3
ELF = $(NOM).elf
HEX = $(NOM).hex

.PHONY: all flash clean

flash: $(HEX)
	avrdude -p atmega328p -P /dev/ttyACM0 -b 115200 -U flash:w:$(NOM).$^

$(HEX): $(ELF)
	avr-objcopy -O ihex $^ $@

all: $(ELF)

$(ELF): $(OBJ)
	$(COMPILATEUR) $(OPTIONS) $^ -o $@ $(LIBRAIRIES)
	@# $^ pour appeler ce qui est appelé avant le lancement de la commande, ici $(OBj) soit tous les .o présents dans /build
	@# $@ pour appeler celui qui execute la commande, ici elf

build/%.o: src/%.c
	@mkdir -p build #@ avant une ligne pour cacher la commande dans le terminal
	$(COMPILATEUR) $(OPTIONS) -c $^ -o $@

clean:
	@$(RM) -r $(NOM).hex $(NOM).elf build
	clear


