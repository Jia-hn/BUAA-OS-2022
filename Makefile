all: os_hello
os_hello: hello_os.c
	gcc hello_os.c -o os_hello
clean: os_hello
	rm os_hello
