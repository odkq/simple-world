#include <input.h>
#include <string.h>

#define NUMKEYS 256

struct key_callback {
	int set;
	int data;
	void (*callback)(int);
};

struct key_callback key[NUMKEYS];

/* opengl initialization boilerplate */
int input_start(void)
{
	memset(key, 0, sizeof(struct key_callback) * NUMKEYS);
	return 0;
}

void input_register(int k, void (*callback)(int), int data)
{
	key[k].callback = callback;
	key[k].data = data;
	key[k].set = 0;
}

void input_poll(void)
{
	int i;

	for (i = 0; i < 256; i++) {
		if (key[i].set == 1) {
			key[i].callback(key[i].data);
			key[i].set = 0;
		}
	}
}

void input_set(int k)
{
	key[k].set = 1;
}

void input_stop(void)
{
	return;
}

