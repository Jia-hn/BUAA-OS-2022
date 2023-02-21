extern char _my_getchar();
extern void _myputchar(char ch);
extern void _my_exit();

void my_cal() {
	unsigned int a;
	while (1) {
        char c = _my_getchar();
        if (c == '\0') {
            continue;
        } 
        if (c >= '0' && c <= '9') {
            a = a * 10 + (c - '0');
        } else { // '\n'
            break; 
		}
	}
	char buf[64];
	int cnt = 0;
	while(a) {
		buf[cnt] = a % 2 + '0';
		a = a / 2;
		cnt ++;
	}
	int i;
	for (i = cnt - 1; i >= 0; i --) {
		_my_putchar(buf[i]);
	}
	if (cnt == 0) {
		_my_putchar('0');
	}
	return ;
}
