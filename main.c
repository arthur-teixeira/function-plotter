#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SAMPLE_RATE 1.0

Vector2 FlipY(Vector2 v, int height) {
  return (Vector2){
      .x = v.x,
      .y = height - v.y,
  };
}

int main(void) {
  const int screenWidth = 1920;
  const int screenHeight = 1080;

  InitWindow(screenWidth, screenHeight, "y = x^2 Plot");

  SetTraceLogLevel(LOG_DEBUG);
  SetTargetFPS(60);

  SetConfigFlags(FLAG_MSAA_4X_HINT);

  double zoom = 100;

  Vector2 center = {
      .x = (double)screenWidth / 2,
      .y = (double)screenHeight / 2,
  };

  size_t bufsize = ceil((screenWidth * 2) / SAMPLE_RATE);

  Color strip_color = GRAY;
  strip_color.a /= 2;

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);

    float f;
    if ((f = GetMouseWheelMove()) != 0) {
      zoom += f * 10;
      if (zoom < 1) {
        zoom = 1;
      }
    }

    DrawRectangle(screenWidth / 2, 0, 2, screenHeight, WHITE);
    DrawRectangle(0, screenHeight / 2, screenWidth, 2, WHITE);

    int x_segments = screenWidth / zoom / 2 + 1;
    int y_segments = screenHeight / zoom / 2 + 1;

    for (int i = 0; i < x_segments; i++) {
      int x1 = i * zoom + center.x;
      DrawLine(x1, 0, x1, screenHeight, strip_color);

      int x2 = center.x - i * zoom;
      DrawLine(x2, 0, x2, screenHeight, strip_color);

      DrawText(TextFormat("%d", i), x1 + 5, screenHeight / 2 + 5, 12, WHITE);
      if (i != 0) {
        DrawText(TextFormat("-%d", i), x2 + 5, screenHeight / 2 + 5, 12, WHITE);
      }
    }

    for (int i = 0; i < y_segments; i++) {
      int y1 = i * zoom + center.y;
      DrawLine(0, y1, screenWidth, y1, strip_color);

      int y2 = center.y - i * zoom;
      DrawLine(0, y2, screenWidth, y2, strip_color);

      DrawText(TextFormat("%d", i), screenWidth / 2, y1, 12, WHITE);
      if (i != 0) {
        DrawText(TextFormat("-%d", i), screenWidth / 2 + 5, y2 + 5, 12, WHITE);
      }
    }

    Vector2 *samples = calloc(bufsize, sizeof(Vector2));

    int i = 0;
    for (double x = (float)-screenWidth / 2; x < (float)screenWidth / 2;
         x += SAMPLE_RATE, i++) {
      double y = -powf(x, 2) / zoom;
      Vector2 current_point = Vector2Add(center, (Vector2){x, y});
      samples[i] = FlipY(current_point, screenHeight);
    }

    DrawSplineBezierQuadratic(samples, bufsize, 2, RED);

    DrawText(TextFormat("%f", zoom), 0, 0, 16, RED);

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
