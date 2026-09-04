#pragma once

float soma(float primeiro, float segundo);

inline float somar(float primeiro, float segundo) {
    return soma(primeiro, segundo);
}
