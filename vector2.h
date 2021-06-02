#ifndef VECTOR2_H
#define VECTOR2_H

typedef struct Vector2 Vector2;

struct Vector2 {
	int x, y;
};

Vector2 vector2_init(int x, int y) {
	Vector2 vector2;
	vector2.x = x;
	vector2.y = y;
	return vector2;
}

#endif
