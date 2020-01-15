# TemperatureForecast-ACSE-5
Homework 1 for ACSE-5

## Baseline - Exponential Smoothing

### Basic Exponential Smoothing

$$
s_i = \alpha x_i +(1-\alpha)s_{i-1}
$$

### Double Exponential Smoothing

$$
s_t^1 = \alpha x_t + (1-\alpha)s_{t-1}^1 \\
s_t^2 = \alpha s_t^1 + (1-\alpha) s_{t-1}^2 \\
x_{t+T} = (2s_t^1 - s_t^2) + \frac{T\alpha}{1-\alpha}(s_t^1-s_t^2)
$$

### Triple Exponential SmoothingExponential smoothing

$$
s_t^1 = \alpha x_t + (1-\alpha)s_{t-1}^1 \\
s_t^2 = \alpha s_t^1 + (1-\alpha) s_{t-1}^2 \\
s_t^3 = \alpha s_t^2 + (1-\alpha) s_{t-1}^3 \\
x_{t+T} = A_1 + B_1T + C_1T^2 \\
A_t = 3s_t^1 - 3s_t^2 + s_t^3 \\
B_t = \frac{\alpha}{2(1-\alpha)^2}[(6-5\alpha)s_t^1-2(5-4\alpha)s_t^2+(4-3\alpha)s_t^3] \\
C_t = \frac{\alpha^2}{2(1-\alpha)^2}[s_t^1-2s_t^2+s_t^3]
$$