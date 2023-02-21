INCLUDES := -I../include
  
%.o: %.c
	$(CC) $(DEFS) $(CFLAGS) $(INCLUDES) -c $<
%.o: %.S
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

.PHONY: clean

all: my_cal.o my_driver.o

clean:
	rm -rf *~ *.o


include ../include.mk
