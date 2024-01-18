#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define SAMPLE_RATE 1.0
#define ASPECT_RATIO (16.0f / 9.0f)

Vector2 FlipY(Vector2 v, int height) {
  return (Vector2){
      .x = v.x,
      .y = height - v.y,
  };
}

int main(void) {
  const int screenWidth = 1920;
  // const int screenWidth = 960;
  const int screenHeight = screenWidth / ASPECT_RATIO;

  InitWindow(screenWidth, screenHeight, "y = x^2 Plot");

  SetTraceLogLevel(LOG_DEBUG);
  SetTargetFPS(60);

  SetConfigFlags(FLAG_MSAA_4X_HINT);

  double zoom = 100;

  Vector2 screen_center = {
      .x = (double)screenWidth / 2,
      .y = (double)screenHeight / 2,
  };

  size_t bufsize = ceil((screenWidth * 2) / SAMPLE_RATE);

  Color strip_color = GRAY;
  strip_color.a /= 2;

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
      Vector2 d = GetMouseDelta();
      screen_center = Vector2Add(screen_center, d);
    }

    float f;
    if ((f = GetMouseWheelMove()) != 0) {
      zoom += f * 10;
      if (zoom < 1) {
        zoom = 1;
      }
    }

    DrawRectangle(screen_center.x, 0, 2, screenHeight, WHITE);
    DrawRectangle(0, screen_center.y, screenWidth, 2, WHITE);

    int x_segments = screen_center.x / zoom + 1;
    int y_segments = screen_center.y / zoom + 1;

    // TODO: Draw more or less lines depending on zoom
    // TODO: Move lines with the camera
    for (int i = 0; i < x_segments; i++) {
      int x1 = i * zoom + screen_center.x;
      DrawLine(x1, 0, x1, screenHeight, strip_color);

      int x2 = screen_center.x - i * zoom;
      DrawLine(x2, 0, x2, screenHeight, strip_color);

      DrawText(TextFormat("%d", i), x1 + 5, screen_center.y + 5, 12, WHITE);
      if (i != 0) {
        DrawText(TextFormat("-%d", i), x2 + 5, screen_center.y + 5, 12, WHITE);
      }
    }

    for (int i = 0; i < y_segments; i++) {
      int y1 = i * zoom + screen_center.y;
      DrawLine(0, y1, screenWidth, y1, strip_color);

      int y2 = screen_center.y - i * zoom;
      DrawLine(0, y2, screenWidth, y2, strip_color);

      if (i != 0) {
        float offset = 5;
        DrawText(TextFormat("%d", i), screen_center.x + offset, y2 - offset, 12,
                 WHITE);
        DrawText(TextFormat("-%d", i), screen_center.x + offset, y1 - offset,
                 12, WHITE);
      }
    }

    Vector2 *samples = calloc(bufsize, sizeof(Vector2));

    int i = 0;
    for (double x = -screen_center.x; x < screen_center.x;
         x += SAMPLE_RATE, i++) {
      double y = powf(x, 2) / zoom;

      // y is inverted because of the way the screen coordinates work
      Vector2 current_point = Vector2Add(screen_center, (Vector2){x, -y});
      samples[i] = current_point;

      // Linear interpolation between sample points
      if (i > 0) {
        Vector2 prev_point = samples[i - 1];
        DrawLineEx(current_point, prev_point, 2.0f, RED);
      }
    }

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
