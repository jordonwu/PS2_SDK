#*---------------------------------------------------------------------*/
#*    Customize here for your prefered compiler...                     */
#*    Hopefully this makefile will change one day :P                   */
#*---------------------------------------------------------------------*/
CC=gcc
LD=gcc
CFLAGS=-c

PRG=ps2img
FILES=main mkimg ximg common

all: $(PRG)

$(PRG): $(FILES:%=%.o)
	$(LD) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o *~ $(PRG)