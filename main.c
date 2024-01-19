#include "raylib.h"
#include "tinyexpr.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define SAMPLE_RATE 0.1f
#define ASPECT_RATIO (16.0f / 9.0f)

#define ARRAY_LEN(xs) sizeof(xs) / sizeof(*xs)

double slope(double x1, double y1, double x2, double y2) {
  return (y2 - y1) / (x2 - x1);
}

int main(void) {
  const int screenWidth = 1080;
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
  Vector2 *samples = calloc(bufsize, sizeof(Vector2));

  Color strip_color = GRAY;
  strip_color.a /= 2;

  char input[1000];
  size_t letter_count = 0;

  te_expr *expr = NULL;

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);

    int key;
    if (IsKeyPressed(KEY_BACKSPACE)) {
      letter_count--;
      if (letter_count < 0)
        letter_count = 0;
      input[letter_count] = '\0';
      printf("%s\n", input);
    }

    while ((key = GetCharPressed()) > 0) {
      if ((key >= 32) && (key <= 125) && (letter_count < 1000)) {
        input[letter_count] = (char)key;
        input[letter_count + 1] = '\0';
        letter_count++;
      }

      printf("%s\n", input);
    }

    double x;
    te_variable vars[] = {
        {"x", &x, TE_VARIABLE},
    };

    size_t num_vars = ARRAY_LEN(vars);

    if (IsKeyPressed(KEY_ENTER)) {
      printf("compiling %s\n", input);
      int err;
      expr = te_compile(input, vars, num_vars, &err);
    }
    if (IsKeyPressed(KEY_SPACE)) {
      screen_center =
          (Vector2){(float)screenWidth / 2, (float)screenHeight / 2};
    }

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
      Vector2 d = GetMouseDelta();
      screen_center.x += d.x;
      screen_center.y += d.y;
    }

    float f;
    if ((f = GetMouseWheelMove()) != 0) {
      zoom += f * 10;
      if (zoom < 1) {
        zoom = 1;
      }
    }

    // Main axes
    DrawRectangle(screen_center.x, 0, 2, screenHeight, WHITE);
    DrawRectangle(0, screen_center.y, screenWidth, 2, WHITE);

    int x_segments = screen_center.x / zoom + 1;
    int y_segments = screen_center.y / zoom + 1;

    // TODO: Draw more or less lines depending on zoom
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

    size_t i = 0;
    float prevY = 0;

    double start = screen_center.x - ((float)screenWidth / 2);
    DrawLine(start, 0, start, screenHeight, GREEN);
    double end = screen_center.x + ((float)screenWidth / 2);
    DrawLine(end, 0, end, screenHeight, RED);
    if (!expr) {
      EndDrawing();
      continue;
    }

    for (double cur_x = start; cur_x < end; cur_x += SAMPLE_RATE, i++) {
      double in = cur_x / zoom;
      x = in;

      double out = te_eval(expr);
      double y = out * zoom;

      // y is inverted because of the way the screen coordinates work
      Vector2 current_point =
          (Vector2){screen_center.x + cur_x, screen_center.y - y};
      samples[i] = current_point;

      if (i > 2) {
        DrawSplineBezierQuadratic(samples + i - 2, 3, 2.0, RED);
      }
    }

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
