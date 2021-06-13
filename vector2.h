#ifndef VECTOR2_H
#define VECTOR2_H

typedef struct Vector2 Vector2;

struct Vector2 {
	unsigned x, y;
};

Vector2 vector2_init(unsigned x, unsigned y) {
	Vector2 vector2;
	vector2.x = x;
	vector2.y = y;
	return vector2;
}

#endif
